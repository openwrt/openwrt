#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/skbuff.h>
#include <linux/netlink.h>
#include <linux/smp.h>
#include <linux/sched.h>

#include <net/genetlink.h>

#include "sf_genl.h"
#include "sf_genl_msg.h"
#include "dpns_common.h"

static struct genl_family sf_genl_family;
static u32 sf_genl_inited = 0;
static struct net *genl_userns = &init_net;
static u32 genl_userport = 0;
static atomic_t genl_seqnum = ATOMIC_INIT(0);

static __rcu struct sfgenl_msg_ops *sf_msg_ops[NUM_SF_GENL_COMPS] = { 0 };
static DEFINE_SPINLOCK(msg_ops_lock);

unsigned char g_dbg_log[DPNS_MAX] = {5,5,5,5,5,5,5,5,5};
EXPORT_SYMBOL(g_dbg_log);

int sfgenl_ops_register(uint32_t comp_id, struct sfgenl_msg_ops *ops)
{
        if (comp_id >= NUM_SF_GENL_COMPS)
                return -EINVAL;

        spin_lock(&msg_ops_lock);
        rcu_assign_pointer(sf_msg_ops[comp_id], ops);
        spin_unlock(&msg_ops_lock);
        synchronize_rcu();

        return 0;
}

EXPORT_SYMBOL(sfgenl_ops_register);

int sfgenl_msg_ops_unregister(uint32_t comp_id)
{
        if (comp_id >= NUM_SF_GENL_COMPS)
                return -EINVAL;

        spin_lock(&msg_ops_lock);
        rcu_assign_pointer(sf_msg_ops[comp_id], NULL);
        spin_unlock(&msg_ops_lock);
        synchronize_rcu();

        return 0;
}

EXPORT_SYMBOL(sfgenl_msg_ops_unregister);

static struct sk_buff *nlmsg_alloc(u8 cmd, u32 port_id, u32 seq, size_t msglen)
{
        struct sk_buff *skb;
        void *usrhdr;

        skb = genlmsg_new(msglen, GFP_ATOMIC);
        if (!skb) {
                GENL_DBG(ERR_LV, "failed to allocate %zu bytes\n", msglen);
                return NULL;
        }

        usrhdr = genlmsg_put(skb, port_id, seq, &sf_genl_family, 0, cmd);
        if (!usrhdr) {
                GENL_DBG(ERR_LV, "genlmsg_put() failed\n");
                nlmsg_free(skb);

                return NULL;
        }

        return skb;
}

static void *nlmsg_mkdata(struct sk_buff *skb, int attr, size_t sz)
{
        struct nlattr *nlattr;

        nlattr = nla_reserve(skb, attr, sz);
        if (!nlattr)
                return NULL;

        return nla_data(nlattr);
}

static int nlmsg_send(struct sk_buff *skb, u32 port_id)
{
        struct genlmsghdr *genlhdr = nlmsg_data((struct nlmsghdr *)skb->data);
        void *usrhdr = genlmsg_data(genlhdr);

        genlmsg_end(skb, usrhdr);

        return genlmsg_unicast(genl_userns, skb, port_id);
}

static int _sendto_user(u8 cmd, int attr, u32 port_id, u32 seq, void *buf, size_t buflen)
{
        struct sk_buff *skb;
        size_t msglen = nla_total_size(buflen);
        void *nldata = NULL;
        int err;

        if (cmd > NUM_SF_GENL_CMDS)
                return -EINVAL;

        if (attr < 0 || attr > SF_GENL_ATTR_MAX_VALID)
                return -EINVAL;

        if (buflen > sf_genl_policy[attr].len)
                return -ENOSPC;

        skb = nlmsg_alloc(cmd, port_id, seq, msglen + 2);
        if (!skb)
                return -ENOMEM;

        // XXX: nla string must be NULL terminated
        if (sf_genl_policy[attr].type == NLA_STRING) {
                if (nla_put_string(skb, attr, buf) < 0) {
                        err = -EINVAL;
                        goto err_free;
                }
        } else if (sf_genl_policy[attr].type == NLA_BINARY) {
                nldata = nlmsg_mkdata(skb, attr, buflen);
                if (!nldata) {
                        err = -EINVAL;
                        goto err_free;
                }

                memcpy(nldata, buf, buflen);
        } else {
                err = -ENOTSUPP;
                goto err_free;
        }

        return nlmsg_send(skb, port_id);

err_free:
        nlmsg_free(nldata);

        return err;
}

static int sendto_user(u8 cmd, int attr, void *buf, size_t buflen)
{
        u32 port_id = genl_userport;

        if (!port_id)
                return -ENOENT;

        return _sendto_user(cmd, attr, port_id,
                            atomic_fetch_inc(&genl_seqnum),
                            buf, buflen);
}

static int nlmsg_reply(struct genl_info *info, u8 cmd, int attr, void *buf, size_t buflen)
{
        return _sendto_user(cmd, attr, info->snd_portid, info->snd_seq, buf, buflen);
}

static int nlmsg_echo(struct genl_info *info, int attr)
{
        struct genlmsghdr *genlmsghdr = info->genlhdr;
        struct nlattr *nlattr = info->attrs[attr];
        size_t buflen = genlmsg_len(info->genlhdr);
        void *buf = nla_data(nlattr);

        return nlmsg_reply(info, genlmsghdr->cmd, attr, buf, buflen);
}

/**
 * sfgenl_msg_reply - reply sf genl message
 * @info: struct genl_info from original message
 * @buf: buffer to reply
 * @buflen: length of buffer
 * return 0 on success, otherwise error
 */
int sfgenl_msg_reply(struct genl_info *info, void *buf, size_t buflen)
{
        struct nlattr *nlattr = info->attrs[SF_GENL_ATTR_MSG];
        struct sfgenl_msghdr *orig, *hdr;
        int err;

        if (!nlattr)
                return -ENODATA;

        orig = nla_data(nlattr);

        hdr = kzalloc(sizeof(struct sfgenl_msghdr) + buflen, GFP_ATOMIC);
        if (!hdr)
                return -ENOMEM;

        hdr->comp_id = orig->comp_id;
        hdr->buflen = buflen;
        memcpy(hdr->buf, buf, buflen);

        err = nlmsg_reply(info, info->genlhdr->cmd,
                          SF_GENL_ATTR_MSG,
                          hdr, sfgenl_msglen(hdr));

        kfree(hdr);

        return err;
}

EXPORT_SYMBOL(sfgenl_msg_reply);

static int sfgenl_hello_cmd(struct sk_buff *skb, struct genl_info *info)
{
        struct nlattr *attr = info->attrs[SF_GENL_ATTR_HELLO];
        u32 hello_cmd;

        if (!attr) {
                GENL_DBG(ERR_LV, "attribute is not found\n");
                return -ENODATA;
        }

        hello_cmd = *(u32 *)nla_data(attr);

        switch (hello_cmd) {
        case SF_GENL_HELLO:
                if (genl_userport)
                        GENL_DBG(WARN_LV, "state may be out "
                                "of sync with userspace\n");

                genl_userport = info->snd_portid;
                genl_userns = genl_info_net(info);

                if (nlmsg_echo(info, SF_GENL_ATTR_HELLO))
                        GENL_DBG(ERR_LV, "failed to echo message\n");

                break;

        case SF_GENL_BYE:
                if (!genl_userport) {
                        GENL_DBG(INFO_LV, "user port_id is not set\n");
                        return 0;
                }

                if (info->snd_portid != genl_userport) {
                        GENL_DBG(WARN_LV, "user port_id mismatched, "
                                "request from pord_id %u "
                                "but we have %u\n",
                                info->snd_portid,
                                genl_userport);
                }

                genl_userport = 0;
                genl_userns = &init_net;

                if (nlmsg_echo(info, SF_GENL_ATTR_HELLO))
                        GENL_DBG(ERR_LV, "failed to echo message\n");

                break;

        default:
                GENL_DBG(ERR_LV, "invalid cmd value: %d\n", hello_cmd);
                return -EINVAL;
        }

        return 0;
}

static int sfgenl_msg_cmd(struct sk_buff *skb, struct genl_info *info)
{
        struct nlattr *attr = info->attrs[SF_GENL_ATTR_MSG];
        size_t msglen = genlmsg_len(info->genlhdr);
        struct sfgenl_msghdr *sf_nlhdr;
        struct sfgenl_msg_ops *op;
        u32 comp_id;
        int err = -ENOENT;

        if (!attr) {
                GENL_DBG(ERR_LV, "attribute is not found\n");
                return -ENODATA;
        }

        sf_nlhdr = nla_data(attr);
        comp_id = sf_nlhdr->comp_id;

        if (comp_id >= NUM_SF_GENL_COMPS) {
                GENL_DBG(ERR_LV, "invalid comp_id\n");
                return -EINVAL;
        }

        GENL_DBG(DBG_LV, "buflen: %u msglen: %lu\n", sf_nlhdr->buflen, msglen);

        if (sfgenl_msglen(sf_nlhdr) > msglen) {
                GENL_DBG(ERR_LV, "invalid buffer length %u > msglen %lu\n",
			sf_nlhdr->buflen, msglen);
                return -EINVAL;
        }

        rcu_read_lock();
        op = rcu_dereference(sf_msg_ops[comp_id]);
        rcu_read_unlock();

        if (op && op->msg_recv)
                err = op->msg_recv(info, sf_nlhdr->buf, sf_nlhdr->buflen);

        return err;
}

// NOTE: return < 0 will cause userspace recv() an error
static int sf_nlmsg_recv(struct sk_buff *skb, struct genl_info *info)
{
        struct nlmsghdr *nlhdr = info->nlhdr;
        struct genlmsghdr *genlhdr = info->genlhdr;
        u8 cmd = genlhdr->cmd;

        GENL_DBG(DBG_LV, "received from port_id: %u seq: %u\n",
                 info->snd_portid, nlhdr->nlmsg_seq);

        if (cmd >= NUM_SF_GENL_CMDS) {
                GENL_DBG(DBG_LV, "invalid cmd %hhu\n", cmd);
        }

        switch (cmd) {
        case SF_GENL_CMD_HELLO:
                return sfgenl_hello_cmd(skb, info);
        case SF_GENL_CMD_MSG:
                return sfgenl_msg_cmd(skb, info);
        default:
                break;
        }

        return -EINVAL;
}

static const struct genl_ops sf_genl_ops[] = {
        {
                .cmd    = SF_GENL_CMD_HELLO,
                .policy = sf_genl_policy,
                .doit   = sf_nlmsg_recv,
                .dumpit = NULL,
        },
        {
                .cmd    = SF_GENL_CMD_MSG,
                .policy = sf_genl_policy,
                .doit   = sf_nlmsg_recv,
                .dumpit = NULL,
        },
};

static struct genl_family sf_genl_family = {
        .name           = SF_GENL_FAMILY_NAME,
        .version        = 1,
        .maxattr        = SF_GENL_ATTR_MAX_VALID,
        .netnsok        = false,
        .module         = THIS_MODULE,
        .ops            = sf_genl_ops,
        .n_ops          = ARRAY_SIZE(sf_genl_ops),
};

int sfgenl_self_msg_recv(struct genl_info *info, void *buf, size_t buflen)
{
        char reply[] = "ACK";

        GENL_DBG(INFO_LV, "received: %lu bytes, \"%s\"\n", buflen, (char *)buf);

        sfgenl_msg_reply(info, reply, 4);

        return 0;
}

static struct sfgenl_msg_ops sfgenl_self_msg_ops = {
        .msg_recv = sfgenl_self_msg_recv,
};

static int say_goodbye(void)
{
        return sendto_user(SF_GENL_CMD_HELLO,
                          SF_GENL_ATTR_HELLO,
                          &(u32){ SF_GENL_BYE }, sizeof(u32));
}

static int goodbye_set(const char *buf, const struct kernel_param *kp)
{
        int err = say_goodbye();
        if (err)
                GENL_DBG(INFO_LV, "sendto_user() failed, err= %d\n", err);

        return 0;
}

static int goodbye_get(char *buf, const struct kernel_param *kp)
{
        return 0;
}

static const struct kernel_param_ops goodbye_param_ops = {
        .set = goodbye_set,
        .get = goodbye_get,
};

module_param_cb(goodbye, &goodbye_param_ops, NULL, 0644);

/**
 * sfgenl_event_send - send event to userspace
 * @comp_id: enum sfgenl_comp
 * @event: enum sfgenl_event
 * @buf: buffer to send
 * @buflen: length of buffer
 * return 0 on success, otherwise error
 */
int sfgenl_event_send(u32 comp_id, u32 event, void *buf, u32 buflen)
{
        struct sfgenl_evthdr *hdr;
        int err;

        if (comp_id >= NUM_SF_GENL_COMPS || event >= NUM_SF_GENL_EVENTS)
                return -EINVAL;

        hdr = kzalloc(sizeof(struct sfgenl_evthdr) + buflen, GFP_ATOMIC);
        if (!hdr)
                return -ENOMEM;

        hdr->comp_id = comp_id;
        hdr->event_id = event;
        hdr->buflen = buflen;
        memcpy(hdr->buf, buf, buflen);

        err = sendto_user(SF_GENL_CMD_EVENT,
                          SF_GENL_ATTR_EVENT,
                          hdr, sfgenl_evtlen(hdr));

        kfree(hdr);

        return err;
}

EXPORT_SYMBOL(sfgenl_event_send);

static int send_event_set(const char *buf, const struct kernel_param *kp)
{
        char *ctx = "tmu tmu tmu";
        return sfgenl_event_send(SF_GENL_COMP_TMU,
                                 SF_GENL_EVT_TEST,
                                 ctx, strlen(ctx));
}

static int send_event_get(char *buf, const struct kernel_param *kp)
{
        return 0;
}

static const struct kernel_param_ops send_event_param_ops = {
        .set = send_event_set,
        .get = send_event_get,
};

module_param_cb(send_event, &send_event_param_ops, NULL, 0644);

// TODO: confirm init stage

static int __init sf_genl_init(void)
{
        int err;

        GENL_DBG(INFO_LV, "register generic netlink family: %s\n",
               SF_GENL_FAMILY_NAME);

        err = genl_register_family(&sf_genl_family);
        if (err) {
                GENL_DBG(ERR_LV, "genl_register_family() failed, err=%d\n", err);
                return err;
        }

        sfgenl_ops_register(SF_GENL_COMP_NL, &sfgenl_self_msg_ops);

        sf_genl_inited = 1;

        return 0;
}

static void __exit sf_genl_exit(void)
{
        if (!sf_genl_inited)
                return;

        GENL_DBG(INFO_LV, "unregister generic netlink family\n");

        if (genl_userport) {
                say_goodbye();
        }

        genl_unregister_family(&sf_genl_family);
}

module_init(sf_genl_init);
module_exit(sf_genl_exit);

MODULE_AUTHOR("0xc0cafe");
MODULE_LICENSE("GPL v2");
