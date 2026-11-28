#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <ubusmsg.h>

#include <sys/socket.h>
#include <sys/time.h>
#include <netlink/msg.h>
#include <netlink/attr.h>

#include "sf_genl.h"
#include "compiler.h"
#include "hashtable.h"
#include "private.h"
#include "genl.h"

#define DEFAULT_RSP_TIMEOUT_MS                  (3 * 1000)

enum {
        GENL_NOT_REGISTERED = 0,
        GENL_REGISTERED,
};

struct task_node {
        struct updown_info *info;
        struct list_head list;
};

struct updown_info {
        char mac[20];
        char updown[2];
        char port[3];
        char vlan_id[5];
        char is_wifi[2];
        char ifname[IFNAMSIZ];
};

static struct nl_cb *nlcb = NULL;
static struct nl_sock *nlsock = NULL;
static uint32_t nlsock_ref = 0;
static int nlstatus = GENL_NOT_REGISTERED;
static int nlfamily_id = -1;
static pthread_t tid_genl;
static pthread_t tid_rsp_expire;
static pthread_t updown_event_thread;
struct list_head task_queue;
pthread_mutex_t task_mutex;
pthread_cond_t task_cond;

#define HASH_BUCKET_SHIFT               (7)

struct rsp_info_q {
        pthread_spinlock_t bkt_lock[1 << HASH_BUCKET_SHIFT];
        DECLARE_HASHTABLE(queue, HASH_BUCKET_SHIFT);
};

struct rsp_info {
        struct hlist_node       node;
        uint32_t                seq;
        void                    *arg;
        rsp_recv_cb_t           recv_cb;
        rsp_expire_cb_t         exp_cb;
        uint64_t                ts;
        uint64_t                lifetime;
};

struct rsp_wait_ctx {
        void                    *rbuf;
        size_t                  rlen;
        sem_t                   wait;
};

static struct rsp_info_q rsp_info_q;

int parse_mac(uint8_t *buf, const char *str)
{
	const char *fmt0 = "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx";
	const char *fmt1 = "%02hhx-%02hhx-%02hhx-%02hhx-%02hhx-%02hhx";

	if (sscanf(str, fmt0, &buf[0], &buf[1], &buf[2], &buf[3], &buf[4], &buf[5]) != 6 &&
	    sscanf(str, fmt1, &buf[0], &buf[1], &buf[2], &buf[3], &buf[4], &buf[5]) != 6) {
		pr_err("invalid MAC address: %s\n", str);
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	return 0;
}

static struct rsp_info *rsp_info_alloc(uint32_t seq,
                                       rsp_recv_cb_t recv_cb,
                                       rsp_expire_cb_t exp_cb,
                                       void *cb_arg,
                                       uint32_t expire_ms)
{
        struct rsp_info *info = calloc(1, sizeof(struct rsp_info));
        if (!info)
                return NULL;

        info->seq = seq;
        info->arg = cb_arg;
        info->exp_cb = exp_cb;
        info->recv_cb = recv_cb;
        info->ts = milli_time_get();
        info->lifetime = expire_ms ? expire_ms : DEFAULT_RSP_TIMEOUT_MS;

        return info;
}

static struct rsp_info *rsp_info_deq(struct rsp_info_q *q, uint32_t seq)
{
        struct rsp_info *info = NULL, *p;
        struct hlist_node *n;
        uint32_t bkt = hash_bkt(q->queue, seq);

        if (pthread_spin_lock(&q->bkt_lock[bkt]))
                return NULL;

        hash_for_each_possible_safe(q->queue, p, n, node, seq) {
                if (p->seq == seq) {
                        hash_del(&p->node);
                        info = p;
                        break;
                }
        }

        pthread_spin_unlock(&q->bkt_lock[bkt]);

        return info;
}

static int rsp_info_enq(struct rsp_info_q *q, struct rsp_info *info)
{
        uint32_t bkt = hash_bkt(q->queue, info->seq);
        int err;

        if ((err = pthread_spin_lock(&q->bkt_lock[bkt])))
                return err;

        hash_add(q->queue, &info->node, info->seq);

        pthread_spin_unlock(&q->bkt_lock[bkt]);

        return 0;
}

static int rsp_info_queue_expire(struct rsp_info_q *q)
{
        uint64_t curr_time = milli_time_get();
        struct hlist_node *n;
        struct rsp_info *p;
        uint32_t bkt;

        hash_for_bkt(q->queue, bkt, p, node) {
                pthread_spin_lock(&q->bkt_lock[bkt]);

                hash_for_bkt_each_safe(q->queue, bkt, n, p, node) {
                        if (curr_time - p->ts < p->lifetime)
                                continue;

                        pr_info("%s(): seq %u expired!\n", __func__, p->seq);

                        if (p->exp_cb)
                                p->exp_cb(p->arg);

                        hash_del(&p->node);
                        free(p);
                }

                pthread_spin_unlock(&q->bkt_lock[bkt]);
        }

        return 0;
}

static void *rsp_expire_worker(void *arg)
{
        struct rsp_info_q *q = arg;

        while (!is_gonna_exit()) {
                rsp_info_queue_expire(q);

                // will be interrupted by signal
                sleep(1);
        }

        pthread_exit(NULL);
}

static void rsp_info_queue_init(struct rsp_info_q *q)
{
        for(unsigned i = 0; i < ARRAY_SIZE(q->bkt_lock); i++)
                pthread_spin_init(&q->bkt_lock[i], 0);

        hash_init(q->queue);
}

static void rsp_info_queue_deinit(struct rsp_info_q *q)
{
        struct hlist_node *n;
        struct rsp_info *p;
        uint32_t bkt;

        hash_for_bkt(q->queue, bkt, p, node) {
                pthread_spin_lock(&q->bkt_lock[bkt]);

                hash_for_bkt_each_safe(q->queue, bkt, n, p, node) {
                        hash_del(&p->node);
                        free(p);
                }

                pthread_spin_unlock(&q->bkt_lock[bkt]);
        }

        for(unsigned i = 0; i < ARRAY_SIZE(q->bkt_lock); i++)
                pthread_spin_destroy(&q->bkt_lock[i]);
}

static struct nl_sock *nlsock_open(int *family_id)
{
        struct nl_sock *sock;
        int fid, sock_fd;
        struct timeval ti;

        sock = nl_socket_alloc();
        if (!sock) {
                pr_err("nl_socket_alloc() failed\n");
                return NULL;
        }

        nl_socket_disable_seq_check(sock);
        nl_socket_disable_auto_ack(sock);

        if (genl_connect(sock)) {
                pr_err("genl_connect() failed\n");
                goto err_free;
        }

        fid = genl_ctrl_resolve(sock, SF_GENL_FAMILY_NAME);
        if (fid < 0) {
                pr_err("genl_ctrl_resolve() failed to resolve family name %s\n",
                       SF_GENL_FAMILY_NAME);
                goto err_free;
        }

        sock_fd = nl_socket_get_fd(sock);
        if (sock_fd < 0) {
                pr_err("failed to get socket fd\n");
                goto err_free;
        }

        ti.tv_sec = 0;
        ti.tv_usec = 100000;
        if (setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &ti, sizeof(ti)) < 0) {
                pr_err("setsockopt() failed to set timeout: %s\n", strerror(errno));
                goto err_free;
        }

        if (family_id)
                *family_id = fid;

        return sock;

err_free:
        if (family_id)
                *family_id = -1;

        nl_socket_free(sock);

        return NULL;
}

static uint32_t nlseq_num_inc(void)
{
        static uint32_t nlseq_num = UINT32_MAX - 1000;
        uint32_t seq;

        if (likely((seq = __sync_add_and_fetch(&nlseq_num, 1))))
                return seq;

        // seq can't be 0, which means NL_AUTO_SEQ
        return __sync_add_and_fetch(&nlseq_num, 1);
}

static int __sendto_kernel(struct nl_sock *sock,
                           uint8_t cmd, int attr, uint32_t seq,
                           void *buf, size_t buflen)
{
        struct nlmsghdr *nlmsghdr;
        struct nl_msg *nlmsg;
        void *user_hdr;
        int err = 0;

        if (!sock)
                return -ENOENT;

        if (nlfamily_id < 0)
                return -ENODATA;

        if (cmd > NUM_SF_GENL_CMDS)
                return -EINVAL;

        if (attr < 0 || attr > NUM_SF_GENL_ATTRS)
                return -EINVAL;

        if (buflen > sf_genl_policy[attr].maxlen)
                return -ENOSPC;

        nlmsg = nlmsg_alloc();
        if (!nlmsg) {
                pr_err("nlmsg_alloc() failed\n");
                return -ENOMEM;
        }

        nlmsghdr = nlmsg_hdr(nlmsg);

        // NL_AUTO_SEQ: require nl_send_auto()
        // (struct nl_msg *msg, uint32_t port, uint32_t seq, int family,
        // int hdrlen, int flags, uint8_t cmd, uint8_t version)
        user_hdr = genlmsg_put(nlmsg, NL_AUTO_PORT, seq,
                               nlfamily_id, 0, NLM_F_REQUEST,
                               cmd, 0);
        if (!user_hdr) {
                pr_err("genlmsg_put() failed\n");
                err = -EINVAL;
                goto out;
        }

        // XXX: NLA_STRING must be NULL terminated
        if (sf_genl_policy[attr].type == NLA_STRING) {
                nla_put_string(nlmsg, attr, buf);
        } else if (sf_genl_policy[attr].type == NLA_BINARY) {
                struct nlattr *nlattr = nla_reserve(nlmsg, attr, buflen);
                if (!nlattr) {
                        err = -EINVAL;
                        goto out;
                }

                char *data = nla_data(nlattr);
                memcpy(data, buf, buflen);
        }

        err = nl_send_auto(sock, nlmsg);
        if (err < 0) {
                pr_err("nl_send() failed\n");
                goto out;
        }

        // err will be sent count
        err = 0;

        {
                struct genlmsghdr *genlmsghdr = genlmsg_hdr(nlmsghdr);
                size_t genlmsglen = genlmsg_len(genlmsghdr);

                pr_dbg("%s(): genlmsg_len: %zu\n", __func__, genlmsglen);
                pr_dbg("%s(): nlmsg_seq: %u\n", __func__, nlmsghdr->nlmsg_seq);
        }

out:
        nlmsg_free(nlmsg);

        return err;
}

static int sendto_kernel(uint8_t cmd, int attr, uint32_t seq, void *buf, size_t buflen)
{
        int err;

        if (nlstatus != GENL_REGISTERED)
                return -ENOENT;

        __sync_fetch_and_add(&nlsock_ref, 1);

        err = __sendto_kernel(nlsock, cmd, attr, seq, buf, buflen);

        __sync_fetch_and_sub(&nlsock_ref, 1);

        return err;
}

/**
 * sfgenl_msg_send_cb - send message to kernel and receive reply (non-blocking)
 * @param comp_id: component id
 * @param buf: buffer to send
 * @param buflen: length of buffer
 * @param recv_cb: cb to process reply, NULL to ignore response
 * @param exp_cb: cb when timed out to get response
 * @param cb_arg: private data will be passed to @recv_cb and @exp_cb
 * @param expired_ms: lifetime of waiting for response
 * @return 0 on success otherwise error code
 */
int sfgenl_msg_send_cb(uint32_t comp_id, void *buf, size_t buflen,
                       rsp_recv_cb_t recv_cb, rsp_expire_cb_t exp_cb,
                       void *cb_arg, uint32_t expired_ms)
{
        struct sfgenl_msghdr *msg = NULL;
        struct rsp_info *rsp_info = NULL;
        uint32_t seq = nlseq_num_inc();
        int err;

        if (comp_id >= NUM_SF_GENL_COMPS)
                return -EINVAL;

        msg = malloc(sizeof(struct sfgenl_msghdr) + buflen);
        if (!msg)
                return -ENOMEM;

        msg->comp_id = comp_id;
        msg->buflen = buflen;
        memcpy(msg->buf, buf, buflen);

        if (recv_cb) {
                rsp_info = rsp_info_alloc(seq, recv_cb, exp_cb, cb_arg, expired_ms);
                if (!rsp_info) {
                        err = -ENOMEM;
                        goto err_free;
                }

                if ((err = rsp_info_enq(&rsp_info_q, rsp_info))) {
                        pr_err("%s(): rsp_info_enq() failed\n", __func__);
                        goto err_free;
                }
        }

        err = sendto_kernel(SF_GENL_CMD_MSG,
                             SF_GENL_ATTR_MSG,
                             seq, msg, sfgenl_msglen(msg));
        if (err) {
                pr_dbg("%s(): sendto_kernel() failed, err = %d\n", __func__, err);
                goto err_deq;
        }

        free(msg);

        return 0;

err_deq:
        if (rsp_info && (rsp_info_deq(&rsp_info_q, rsp_info->seq) == NULL))
                pr_err("%s(): failed to dequeue, unexpected\n", __func__);

err_free:
        free(msg);

        if (rsp_info)
                free(rsp_info);

        return err;
}

/**
 * sfgenl_msg_send - send message to kernel, does not care reply
 * @comp_id: component id
 * @buf: buffer to send
 * @buflen: length of buffer
 * return 0 on success, otherwise error
 */
int sfgenl_msg_send(uint32_t comp_id, void *buf, size_t buflen)
{
        return sfgenl_msg_send_cb(comp_id, buf, buflen,
                                  NULL, NULL, NULL,
                                  0);
}

static int blocked_recv_cb(void *buf, size_t buflen, void *arg)
{
        struct rsp_wait_ctx *ctx = arg;

        ctx->rbuf = malloc(buflen);
        ctx->rlen = buflen;

        memcpy(ctx->rbuf, buf, buflen);

        sem_post(&ctx->wait);

        return 0;
}

static void blocked_recv_expired(void *arg)
{
        struct rsp_wait_ctx *ctx = arg;

        sem_post(&ctx->wait);
}

/**
 * sfgenl_msg_send_recv - send and blocking waiting for reply
 * @param comp_id: component id
 * @param sbuf: message payload to send
 * @param slen: length of message payload to send
 * @param rbuf: message payload received, allocated memory, need to free outside
 * @param rlen: length of message payload received
 * @param expired_ms: time to wait for response, 0 for default value
 * @return 0 on success, -ETIMEOUT on timed out, otherwise error
 */
int sfgenl_msg_send_recv(uint32_t comp_id, void *sbuf, size_t slen,
                         void **rbuf, size_t *rlen, uint32_t expired_ms)
{
        struct rsp_wait_ctx ctx = { 0 };

        if (!rbuf || !rlen)
                return -EINVAL;

        *rbuf = NULL;

        sem_init(&ctx.wait, 0, 0);

        int err = sfgenl_msg_send_cb(comp_id, sbuf, slen,
                                     blocked_recv_cb, blocked_recv_expired,
                                     &ctx, expired_ms);
        if (err)
                return err;

        sem_wait(&ctx.wait);
        sem_destroy(&ctx.wait);

        if (ctx.rbuf == NULL)
                return -ETIMEDOUT;

        *rbuf = ctx.rbuf;
        *rlen = ctx.rlen;

        return 0;
}

static int sfgenl_hello_cmd_recv(struct nl_msg *nlmsg, struct nlattr *attrs[])
{
        struct genlmsghdr *genlmsghdr = genlmsg_hdr(nlmsg_hdr(nlmsg));
        uint32_t *cmd;

        if (!attrs[SF_GENL_ATTR_HELLO]) {
                pr_err("%s(): attribute is not found\n", __func__);
                return -EINVAL;
        }

        if ((uint32_t)genlmsg_len(genlmsghdr) < sizeof(uint32_t)) {
                pr_err("%s(): invalid message length\n", __func__);
                return -EINVAL;
        }

        cmd = nla_data(attrs[SF_GENL_ATTR_HELLO]);

        switch (*cmd) {
        case SF_GENL_HELLO:
                pr_info("%s(): received hello from kernel\n", __func__);

                if (nlstatus != GENL_NOT_REGISTERED)
                        pr_err("%s(): state is out of sync with kernel\n", __func__);

                nlstatus = GENL_REGISTERED;

                break;

        case SF_GENL_BYE:
                pr_info("%s(): received bye from kernel\n", __func__);

                if (nlstatus != GENL_REGISTERED)
                        pr_err("%s(): state is out of sync with kernel\n", __func__);

                nlstatus = GENL_NOT_REGISTERED;

                break;

        default:
                pr_err("%s(): invalid hello cmd\n", __func__);
                return -EINVAL;
        }

        return 0;
}

static int sfgenl_msg_cmd_recv(struct nl_msg *nlmsg, struct nlattr *attrs[])
{
        struct nlmsghdr *nlmsghdr = nlmsg_hdr(nlmsg);
        struct genlmsghdr *genlmsghdr = genlmsg_hdr(nlmsghdr);
        struct nlattr *nlattr = attrs[SF_GENL_ATTR_MSG];
        struct sfgenl_msghdr *sfmsghdr;
        struct rsp_info *rsp_info;
        uint32_t comp_id;
        int err = 0;


        if (!nlattr) {
                pr_err("%s(): attribute is not found\n", __func__);
                return -EINVAL;
        }

        sfmsghdr = nla_data(nlattr);
        comp_id = sfmsghdr->comp_id;

        if (comp_id >= NUM_SF_GENL_COMPS) {
                pr_err("%s(): invalid component id\n", __func__);
                return -EINVAL;
        }

        if (sfgenl_msglen(sfmsghdr) > (uint32_t)genlmsg_len(genlmsghdr)) {
                pr_err("%s(): invalid message length\n", __func__);
                return -EINVAL;
        }

        rsp_info = rsp_info_deq(&rsp_info_q, nlmsghdr->nlmsg_seq);
        if (!rsp_info) {
                pr_dbg("%s(): handler for response message not found, seq: %u\n", __func__, nlmsghdr->nlmsg_seq);
                goto out;
        }

        if (rsp_info->recv_cb) {
                err = rsp_info->recv_cb(sfmsghdr->buf, sfmsghdr->buflen, rsp_info->arg);
        }

        free(rsp_info);

out:
        return err;
}

static int sfgenl_event_recv(int event_id, uint8_t *buf, size_t buflen)
{
        pr_info("event: %d bytes: %zu content: \"%s\"\n", event_id, buflen, (char *)buf);

        return 0;
}

static int updown_handler(struct updown_info *info)
{
        pid_t pid;

	pid = fork();

	if (pid == -1) {
		pr_err("%s %d fork failed\n", __func__, __LINE__);
		exit(EXIT_FAILURE);
	}

	if (pid == 0) {
		execl("/bin/sh", "sh", "/usr/sbin/dev.sh", info->updown, info->mac, info->is_wifi, info->ifname, info->port, info->vlan_id ,NULL);

		perror("execl");
		pr_err("%s %d fork failed\n", __func__, __LINE__);
		exit(EXIT_FAILURE);
	} else {
		wait(NULL);
		pr_dbg("Child process completed.\n");
	}

        free(info);

        return 0;
}

void* thread_function(void* arg) {
    while (1) {
        pthread_mutex_lock(&task_mutex);
        while (list_empty(&task_queue)) {
            pthread_cond_wait(&task_cond, &task_mutex);
        }

        struct task_node *node = list_first_entry(&task_queue, struct task_node, list);
        list_del(&node->list);

        pthread_mutex_unlock(&task_mutex);

        updown_handler(node->info);
        free(node);
    }
    return NULL;
}

static void add_task(struct updown_info *info) {
        pthread_mutex_lock(&task_mutex);

        struct task_node *task = malloc(sizeof(struct task_node));
        task->info = info;

        list_add_tail(&task->list, &task_queue);

        pthread_cond_signal(&task_cond);
        pthread_mutex_unlock(&task_mutex);
}

static int sfgenl_event_updown_recv(int event_id, uint8_t *buf, size_t buflen)
{
	struct updown_info *info;
	struct sf_mac_updown *cxt = (struct sf_mac_updown *)buf;
        info = malloc(sizeof(struct updown_info));
        if (!info)
                return -ENOMEM;
        pr_dbg("sfgenl_event_updown_recv() mac:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n", cxt->dsmac[0], cxt->dsmac[1], cxt->dsmac[2], cxt->dsmac[3], cxt->dsmac[4], cxt->dsmac[5]);

	pr_dbg("event: %d bytes: %zu \n", event_id, buflen);
	pr_dbg("mac:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx vlan_id:%d ifname:%s port:%d link:%d is_wifi:%d\n",
				cxt->dsmac[0], cxt->dsmac[1], cxt->dsmac[2], cxt->dsmac[3], cxt->dsmac[4], cxt->dsmac[5],
		cxt->vlan_id, cxt->ifname, cxt->port, cxt->updown, cxt->is_wifi);

	sprintf(info->mac, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
		cxt->dsmac[0], cxt->dsmac[1], cxt->dsmac[2], cxt->dsmac[3], cxt->dsmac[4], cxt->dsmac[5]);

	sprintf(info->updown, "%d", cxt->updown);
	sprintf(info->port, "%d", cxt->port);
	sprintf(info->vlan_id, "%d", cxt->vlan_id);
	sprintf(info->is_wifi, "%d", cxt->is_wifi);
	sprintf(info->ifname, "%s", cxt->ifname);

        add_task(info);
	return 0;
}


static struct sfgenl_evt_ops sf_event_ops[NUM_SF_GENL_COMPS] = {
        [SF_GENL_COMP_NL]  = { .event_recv = sfgenl_event_recv },
        [SF_GENL_COMP_L2_MAC]  = { .event_recv = sfgenl_event_updown_recv},
};

int sfgenl_event_ops_register(int comp_id, struct sfgenl_evt_ops *ops)
{
        if (!ops)
                return -EINVAL;

        if (comp_id >= NUM_SF_GENL_COMPS)
                return -EINVAL;

        memcpy(&sf_event_ops[comp_id], ops, sizeof(struct sfgenl_evt_ops));

        return 0;
}

static int sfgenl_event_cmd_recv(struct nl_msg *nlmsg, struct nlattr *attrs[])
{
        struct nlmsghdr *nlmsghdr = nlmsg_hdr(nlmsg);
        struct genlmsghdr *genlmsghdr = genlmsg_hdr(nlmsghdr);
        struct nlattr *nlattr = attrs[SF_GENL_ATTR_EVENT];
        struct sfgenl_evthdr *sfevthdr;
        uint32_t comp_id, event_id;
        int err = 0;

        if (!nlattr) {
                pr_err("%s(): attribute is not found\n", __func__);
                return -EINVAL;
        }

        sfevthdr = nla_data(nlattr);
        comp_id = sfevthdr->comp_id;
        event_id = sfevthdr->event_id;

        if (comp_id >= NUM_SF_GENL_COMPS) {
                pr_err("%s(): invalid component id\n", __func__);
                return -EINVAL;
        }

        if (event_id >= NUM_SF_GENL_EVENTS) {
                pr_err("%s(): invalid event id\n", __func__);
                return -EINVAL;
        }

        if (sfgenl_evtlen(sfevthdr) > (uint32_t)genlmsg_len(genlmsghdr)) {
                pr_err("%s(): invalid message length\n", __func__);
                return -EINVAL;
        }

        if (sf_event_ops[comp_id].event_recv) {
                err = sf_event_ops[comp_id].event_recv(sfevthdr->event_id,
                                                       sfevthdr->buf,
                                                       sfevthdr->buflen);
        }

        return err;
}

static int genl_msg_recv(struct nl_msg *nlmsg, void *arg)
{
        struct nlmsghdr *nlhdr = nlmsg_hdr(nlmsg);
        struct genlmsghdr *genlhdr = genlmsg_hdr(nlhdr);
        struct nlattr *attr[NUM_SF_GENL_ATTRS] = {0 };
        int cmd = genlhdr->cmd;
        int err;

        pr_dbg("%s(): seq: %u cmd: %hhu\n",
               __func__, nlhdr->nlmsg_seq, genlhdr->cmd);

        err = genlmsg_parse(nlmsg_hdr(nlmsg), 0, attr,
                            SF_GENL_ATTR_MAX_VALID,
                            sf_genl_policy);
        if (err) {
                pr_err("%s(): genlmsg_parse() failed: %s\n",
                       __func__, strerror(err * -1));
                return NL_OK;
        }

        switch (cmd) {
        case SF_GENL_CMD_HELLO:
                err = sfgenl_hello_cmd_recv(nlmsg, attr);
                break;

        case SF_GENL_CMD_MSG:
                err = sfgenl_msg_cmd_recv(nlmsg, attr);
                break;

        case SF_GENL_CMD_EVENT:
                err = sfgenl_event_cmd_recv(nlmsg, attr);
                break;

        default:
                pr_err("%s(): invalid cmd: %d\n", __func__, cmd);
                break;
        }

        return NL_OK;
}

int is_genl_registered(void)
{
        return nlstatus == GENL_REGISTERED ? 1 : 0;
}

static int nl_say_hello(struct nl_sock *sock)
{
        return __sendto_kernel(sock,
                               SF_GENL_CMD_HELLO,
                               SF_GENL_ATTR_HELLO,
                               nlseq_num_inc(),
                               &(uint32_t){ SF_GENL_HELLO },
                               sizeof(uint32_t));
}

static void nl_recv_loop(struct nl_sock *sock, struct nl_cb *cb)
{
        while (!is_gonna_exit()) {
                int err = nl_recvmsgs(sock, cb);

                if (nlstatus == GENL_NOT_REGISTERED)
                        break;

                if (err < 0) {
                        if (err == -EINTR)
                                continue;

                        pr_err("nl_recvmsgs() failed: %s\n", strerror(-err));
                        usleep(500 * 1000); // prevent spamming
                }
        }
}

static int nlsock_set(struct nl_sock *sock)
{
        // wait for all sendto_kernel()
        while (READ_ONCE(nlsock_ref))
                ;

        nlsock = sock;

        return 0;
}

static void *genl_worker(void *arg)
{
        struct nl_cb *cb = arg;
        struct nl_sock *sock;

        pr_dbg("%s(): start\n", __func__);

        while (!is_gonna_exit()) {
                sock = nlsock_open(&nlfamily_id);
                if (!sock) {
                        sleep(3);
                        continue;
                }

                if (nl_say_hello(sock))
                        pr_err("nl_say_hello() failed\n");

                for (int i = 0; i < 3; i++) {
                        if (nl_recvmsgs(sock, cb) >= 0)
                                break;
                }

                if (nlstatus == GENL_REGISTERED) {
                        nlsock_set(sock);
                        nl_recv_loop(sock, cb);
                }

                // gonna exit or re-register again
                nlsock_set(NULL);
                nl_socket_free(sock);
        }

        pr_dbg("%s(): exit\n", __func__);

        pthread_exit(NULL);
}


int sf_user_genl_init(void)
{
        INIT_LIST_HEAD(&task_queue);
        pthread_mutex_init(&task_mutex, NULL);
        pthread_cond_init(&task_cond, NULL);

        pthread_create(&updown_event_thread, NULL, thread_function, NULL);

        rsp_info_queue_init(&rsp_info_q);

        nlcb = nl_cb_alloc(NL_CB_DEFAULT);
        nl_cb_set(nlcb, NL_CB_VALID, NL_CB_CUSTOM, genl_msg_recv, NULL);

        if (pthread_create(&tid_genl, NULL, genl_worker, nlcb)) {
                pr_err("failed to create heartbeat thread\n");
                goto err_free;
        }

        if (pthread_create(&tid_rsp_expire, NULL, rsp_expire_worker, &rsp_info_q)) {
                pr_err("failed to create response gc thread\n");
                goto err_free;
        }

        return 0;

err_free:
        nl_cb_put(nlcb);
        rsp_info_queue_deinit(&rsp_info_q);

        return -EINVAL;
}

int sf_user_genl_exit(void)
{
        pthread_join(tid_rsp_expire, NULL);
        pthread_join(tid_genl, NULL);
        pthread_join(updown_event_thread, NULL);

        pthread_mutex_destroy(&task_mutex);
        pthread_cond_destroy(&task_cond);

        nl_cb_put(nlcb);

        rsp_info_queue_deinit(&rsp_info_q);

        return 0;
}
