#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <libubus.h>

#include "utils.h"
#include "genl.h"
#include "private.h"
#include "se_mcast_cfg.h"
#include "sf_genl_mcast.h"

enum l3_mcast_blob_id {
        BLOB_L3_SRC_IP = 0,
        BLOB_L3_DST_IP,
        BLOB_L3_IIF,        // in interface
        BLOB_L3_OIFS,       // out interface(s)
        BLOB_L3_RULE_MARK,
        NUM_L3_MCAST_BLOB_IDS,
};

static const struct blobmsg_policy l3_mcast_blob_policy[] = {
        [BLOB_L3_SRC_IP]    = { .name = "src",  .type = BLOBMSG_TYPE_STRING },
        [BLOB_L3_DST_IP]    = { .name = "dst",  .type = BLOBMSG_TYPE_STRING },
        [BLOB_L3_IIF]       = { .name = "iif",  .type = BLOBMSG_TYPE_STRING },
        [BLOB_L3_OIFS]      = { .name = "oif",  .type = BLOBMSG_TYPE_ARRAY  },
        [BLOB_L3_RULE_MARK] = { .name = "mark", .type = BLOBMSG_TYPE_STRING },
};

static const struct blobmsg_policy l3_mcast_del_marked_policy[] = {
        [0] = { .name = "mark", .type = BLOBMSG_TYPE_STRING },
};

static int l3_mcast_cfg_make(struct blob_attr **tb, se_l3_mcast_cfg_t **cfg)
{
        se_l3_mcast_cfg_t *mcast = NULL;
        struct sockaddr_storage sip = { }, dip = { };
        char *sip_s, *dip_s, *iif, *mark = NULL;
        struct blobmsg_policy *oif_policy = NULL;
        struct blob_attr **oif_arr = NULL;
        size_t n_oif;
        int err = 0;

        if (!cfg)
                return -EINVAL;

        *cfg = NULL;

        sip_s = blobmsg_get_string(tb[BLOB_L3_SRC_IP]);
        if (ipstr_aton(sip_s, &sip))
                return -EINVAL;

        dip_s = blobmsg_get_string(tb[BLOB_L3_DST_IP]);
        if (ipstr_aton(dip_s, &dip))
                return -EINVAL;

        if (sip.ss_family != dip.ss_family)
                return -EINVAL;

        iif = blobmsg_get_string(tb[BLOB_L3_IIF]);

        if (tb[BLOB_L3_RULE_MARK])
                mark = blobmsg_get_string(tb[BLOB_L3_RULE_MARK]);

        n_oif = blobmsg_check_array(tb[BLOB_L3_OIFS], BLOB_ATTR_STRING);
        if (n_oif == 0)
                return -EINVAL;

        if (n_oif > SE_MCAST_OIF_MAX)
                return -EINVAL;

        oif_policy = calloc(n_oif, sizeof(struct blobmsg_policy));
        if (!oif_policy) {
                err = -ENOMEM;
                goto out_free;
        }

        oif_arr = calloc(n_oif, sizeof(struct blob_attr *));
        if (!oif_arr) {
                err = -ENOMEM;
                goto out_free;
        }

        mcast = calloc(1, se_l3_mcast_cfg_newsz());
        if (!mcast) {
                err = -ENOMEM;
                goto out_free;
        }

        for (size_t i = 0; i < n_oif; i++) {
                oif_policy[i].type = BLOBMSG_TYPE_STRING;
        }

        blobmsg_parse_array(oif_policy, (int)n_oif, oif_arr,
                            blobmsg_data(tb[BLOB_L3_OIFS]),
                            blobmsg_data_len(tb[BLOB_L3_OIFS]));

        for (size_t i = 0; i < n_oif; i++) {
                char *ifname = blobmsg_get_string(oif_arr[i]);
                uint32_t ifidx = if_nametoindex(ifname);

                if (ifidx == SE_INVALID_IF_IDX) {
                        pr_err("failed to get if index from name %s\n", ifname);
                        err = -errno;
                        goto out_free;
                }

                mcast->oif[i] = ifidx;
        }

        mcast->oif_cnt = n_oif;

        mcast->iif = if_nametoindex(iif);
        if (mcast->iif == SE_INVALID_IF_IDX) {
                pr_err("failed to get if index from name %s\n", iif);
                err = -errno;
                goto out_free;
        }

        if (sip.ss_family == AF_INET) {
                memcpy(&mcast->sip.ip4.d,
                       &((struct sockaddr_in *)&sip)->sin_addr,
                       sizeof(mcast->sip.ip4.d));
                memcpy(&mcast->dip.ip4.d,
                       &((struct sockaddr_in *)&dip)->sin_addr,
                       sizeof(mcast->dip.ip4.d));

                mcast->sip.ip4.d = __bswap32(mcast->sip.ip4.d);
                mcast->dip.ip4.d = __bswap32(mcast->dip.ip4.d);

                mcast->is_mcsg = mcast->sip.ip4.d != 0 ? 1 : 0;
                mcast->is_ipv6 = 0;
        } else if (sip.ss_family == AF_INET6) {
                // TODO
                err = -EINVAL;
                goto out_free;
        }

        if (mark)
                strncpy(mcast->mark, mark, sizeof(mcast->mark));

out_free:
        if (oif_policy)
                free(oif_policy);

        if (oif_arr)
                free(oif_arr);

        if (err && mcast) {
                free(mcast);
                mcast = NULL;
        }

        *cfg = mcast;

        return err;
}

static struct mcast_genl_msg *l3_mcast_genl_msg_make(int cmd, void *buf, size_t buflen)
{
        struct mcast_genl_msg *msg;

        if (cmd >= NUM_MC_MSG_CMDS)
                return NULL;

        msg = calloc(1, mcast_genl_newsz(buf ? buflen : 0));
        if (!msg)
                return NULL;

        msg->cmd = cmd;
        msg->mc_type = MCAST_L3;

        if (buf) {
                msg->buflen = buflen;
                memcpy(msg->buf, buf, buflen);
        }

        return msg;
}

static void ubus_err_code_reply(struct ubus_context *ctx, struct ubus_request_data *req, int err)
{
        struct blob_buf b = { };

        blob_buf_init(&b, 0);
        blobmsg_add_u32(&b, "error", err);
        ubus_send_reply(ctx, req, b.head);
}

static int __l3_mcast_add_del(struct ubus_context *ctx, struct ubus_object *obj,
                              struct ubus_request_data *req, const char *method,
                              struct blob_attr *msg, int is_add)
{
        static const int req_args[] = {
                BLOB_L3_SRC_IP,
                BLOB_L3_DST_IP,
                BLOB_L3_IIF,
                BLOB_L3_OIFS,
        };
        struct blob_attr *tb[NUM_L3_MCAST_BLOB_IDS] = { };
        se_l3_mcast_cfg_t *cfg = NULL;
        struct mcast_genl_msg *genl_msg = NULL;
        int *rsp = NULL; size_t rsp_len;
        int err;

        blobmsg_parse(l3_mcast_blob_policy,
                      ARRAY_SIZE(l3_mcast_blob_policy),
                      tb, blob_data(msg), blob_len(msg));

	if (!tb[BLOB_L3_SRC_IP])
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (!tb[BLOB_L3_DST_IP])
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (!tb[BLOB_L3_IIF])
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (!tb[BLOB_L3_OIFS])
		return UBUS_STATUS_INVALID_ARGUMENT;

        err = l3_mcast_cfg_make(tb, &cfg);
        if (err) {
                switch (err) {
                case -EINVAL:
                        return UBUS_STATUS_INVALID_ARGUMENT;

                case -ENOMEM:
                        return UBUS_STATUS_NO_MEMORY;

                default:
                        return UBUS_STATUS_UNKNOWN_ERROR;
                }
        }

        genl_msg = l3_mcast_genl_msg_make(is_add ? MC_MSG_CMD_ADD : MC_MSG_CMD_DEL,
                                          cfg, se_l3_mcast_cfg_sz());
        if (!genl_msg) {
                err = UBUS_STATUS_NO_MEMORY;
                goto out_free;
        }

        err = sfgenl_msg_send_recv(SF_GENL_COMP_MCAST, genl_msg,
                                   mcast_genl_msglen(genl_msg),
                                   (void **)&rsp, &rsp_len, 1000);
        if (err) {
                err = UBUS_STATUS_UNKNOWN_ERROR;
        }

        if (rsp) {
                if (rsp_len == sizeof(int)) {
                        ubus_err_code_reply(ctx, req, *(int *)rsp);
                } else {
                        err = UBUS_STATUS_NO_DATA;
                }

                free(rsp);
        }

        free(genl_msg);

out_free:
        free(cfg);

        return err;
}

static int l3_mcast_add(struct ubus_context *ctx, struct ubus_object *obj,
                        struct ubus_request_data *req, const char *method,
                        struct blob_attr *msg)
{
        return __l3_mcast_add_del(ctx, obj, req, method, msg, 1);
}

static int l3_mcast_del(struct ubus_context *ctx, struct ubus_object *obj,
                        struct ubus_request_data *req, const char *method,
                        struct blob_attr *msg)
{
        return __l3_mcast_add_del(ctx, obj, req, method, msg, 0);
}

static int l3_mcast_del_marked(struct ubus_context *ctx, struct ubus_object *obj,
                               struct ubus_request_data *req, const char *method,
                               struct blob_attr *msg)
{
        struct blob_attr *tb[ARRAY_SIZE(l3_mcast_del_marked_policy)] = { };
        struct mcast_genl_msg *genl_msg = NULL;
        char *mark;
        int *rsp = NULL; size_t rsp_len;
        int err;

        blobmsg_parse(l3_mcast_del_marked_policy,
                      ARRAY_SIZE(l3_mcast_del_marked_policy),
                      tb, blob_data(msg), blob_len(msg));

        if (!tb[0])
                return UBUS_STATUS_INVALID_ARGUMENT;

        mark = blobmsg_get_string(tb[0]);

        genl_msg = l3_mcast_genl_msg_make(MC_MSG_CMD_DEL_MARKED, mark, strlen(mark));
        if (!genl_msg)
                return UBUS_STATUS_NO_MEMORY;

        err = sfgenl_msg_send_recv(SF_GENL_COMP_MCAST, genl_msg,
                                   mcast_genl_msglen(genl_msg),
                                   (void **)&rsp, &rsp_len, 0);
        if (err) {
                err = UBUS_STATUS_UNKNOWN_ERROR;
        }

        if (rsp) {
                if (rsp_len == sizeof(int)) {
                        ubus_err_code_reply(ctx, req, *(int *)rsp);
                } else {
                        err = UBUS_STATUS_NO_DATA;
                }

                free(rsp);
        }

        free(genl_msg);

        return err;
}

static int l3_mcast_list_reply(struct ubus_context *ctx, struct ubus_request_data *req,
                               struct blob_buf *b, struct mcast_genl_msg *rsp, size_t rsp_len)
{
        se_l3_mcast_cfg_t *list = (void *)rsp->buf;
        void *arr;

        if (mcast_genl_msglen(rsp) != rsp_len)
                return UBUS_STATUS_SYSTEM_ERROR;

        if (rsp->cmd != MC_MSG_CMD_LIST)
                return UBUS_STATUS_SYSTEM_ERROR;

        if (rsp->mc_type != MCAST_L3)
                return UBUS_STATUS_SYSTEM_ERROR;

        blob_buf_init(b, 0);
        arr = blobmsg_open_array(b, "rules");

        for (size_t i = 0; i < rsp_len / sizeof(se_l3_mcast_cfg_t); i++) {
                se_l3_mcast_cfg_t *c = &list[i];

                // TODO
                if (c->is_ipv6)
                        continue;

                void *tbl = blobmsg_open_table(b, NULL);
                char sip[IPV4_STRLEN_MAX] = { };
                char dip[IPV4_STRLEN_MAX] = { };
                char iif[IF_NAMESIZE] = { };

                inet_ntop(AF_INET, &(uint32_t){ __bswap32(c->sip.ip4.d) }, sip, sizeof(sip));
                inet_ntop(AF_INET, &(uint32_t){ __bswap32(c->dip.ip4.d) }, dip, sizeof(dip));

                blobmsg_add_string(b, "src", sip);
                blobmsg_add_string(b, "dst", dip);
                blobmsg_add_string(b, "iif", c->iif ? if_indextoname(c->iif, iif) : "invalid");

                void *oif = blobmsg_open_array(b, "oif");
                for (size_t j = 0; j < c->oif_cnt && j < ARRAY_SIZE(c->oif); j++) {
                        char ifname[IF_NAMESIZE] = { };
                        blobmsg_add_string(b, NULL, c->oif[j] ? if_indextoname(c->oif[j], ifname) : "invalid");
                }
                blobmsg_close_array(b, oif);

                blobmsg_add_string(b, "mark", c->mark);

                blobmsg_close_table(b, tbl);
        }

        blobmsg_close_array(b, arr);

        return ubus_send_reply(ctx, req, b->head);
}

static int l3_mcast_list(struct ubus_context *ctx, struct ubus_object *obj,
                         struct ubus_request_data *req, const char *method,
                         struct blob_attr *msg)
{
        struct mcast_genl_msg *genl_msg;
        void *rsp = NULL; size_t rsp_len;
        int err = UBUS_STATUS_OK;

        genl_msg = l3_mcast_genl_msg_make(MC_MSG_CMD_LIST, NULL, 0);
        if (!genl_msg)
                return UBUS_STATUS_NO_MEMORY;

        err = sfgenl_msg_send_recv(SF_GENL_COMP_MCAST, genl_msg,
                                   mcast_genl_msglen(genl_msg),
                                   (void **)&rsp, &rsp_len, 0);
        if (err) {
                err = UBUS_STATUS_UNKNOWN_ERROR;
                goto free_msg;
        }

        if (rsp) {
                struct blob_buf b = { };

                if (rsp_len == sizeof(int)) {
                        blob_buf_init(&b, 0);
                        blobmsg_add_u32(&b, "error", *(int *)rsp);
                        ubus_send_reply(ctx, req, b.head);
                } else {
                        err = l3_mcast_list_reply(ctx, req, &b, rsp, rsp_len);
                }

                free(rsp);
        }

free_msg:
        free(genl_msg);

        return err;
}

static const struct ubus_method l3_mcast_methods[] = {
        UBUS_METHOD("add", l3_mcast_add, l3_mcast_blob_policy),
        UBUS_METHOD("del", l3_mcast_del, l3_mcast_blob_policy),
        UBUS_METHOD("del_marked", l3_mcast_del_marked, l3_mcast_del_marked_policy),
        UBUS_METHOD_NOARG("list", l3_mcast_list),
};

static struct ubus_object_type l3_mcast_object_type =
        UBUS_OBJECT_TYPE("l3_mcast", l3_mcast_methods);

static struct ubus_object l3_mcast_object = {
        .name = "dpns.l3_mcast",
        .type = &l3_mcast_object_type,
        .methods = l3_mcast_methods,
        .n_methods = ARRAY_SIZE(l3_mcast_methods),
};

int mcast_init(void)
{
        ubus_add_object(ubus_ctx_get(), &l3_mcast_object);

        return 0;
}

int mcast_exit(void)
{
        ubus_remove_object(ubus_ctx_get(), &l3_mcast_object);

        return 0;
}
