#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <libubus.h>

#include "utils.h"
#include "genl.h"
#include "private.h"
#include "sf_genl_msg.h"
#include "sf_tmu_genl.h"
#include "sf_tmu_regs.h"

static struct tmu_info g_tmu_info;

static inline int has_all_ubus_args(struct blob_attr **tb,
                                    const int req_args[],
                                    int args_cnt)
{
        for (int i = 0; i < args_cnt; i++) {
                if (!tb[req_args[i]])
                        return 0;
        }

        return 1;
}

static inline void tmu_hdr_set(struct tmu_msg *msg,
                               uint32_t cmd,
                               uint32_t comp,
                               uint32_t port,
                               uint32_t comp_idx,
                               size_t buflen)
{
        msg->cmd = cmd;
        msg->port = port;
        msg->comp = comp;
        msg->u.nr_comp = comp_idx;
        msg->buflen = buflen;
}

static inline int tmu_send_recv(struct tmu_msg *sbuf, size_t slen, struct tmu_msg **rbuf, size_t *rlen)
{
        int err;
        err = sfgenl_msg_send_recv(SF_GENL_COMP_TMU,
                                   sbuf, slen,
                                   (void **)rbuf, rlen,
                                   1000);

        // unlikely
        if (!err && *rlen < sizeof(struct tmu_msg)) {
                err = -EINVAL;

                if (*rbuf)
                        free(*rbuf);

                *rbuf = 0;
                *rlen = 0;
        }

        return err;
}

static void tmu_errmsg_ubus_reply(struct ubus_context *ctx,
                                  struct ubus_request_data *req,
                                  struct tmu_msg *err_msg)
{
        struct blob_buf b = { };

        blobmsg_buf_init(&b);
        blobmsg_add_u32(&b, "error", *(int32_t *)err_msg->buf);

        ubus_send_reply(ctx, req, b.head);
}

static void ubus_err_reply(struct ubus_context *ctx,
                           struct ubus_request_data *req,
                           int err, char *msg)
{
        struct blob_buf b = { };

        blobmsg_buf_init(&b);
        blobmsg_add_u32(&b, "error", err);
        blobmsg_add_string(&b, "msg", msg);

        ubus_send_reply(ctx, req, b.head);
}

enum {
        BITRATE_PKT_LEN = 0,
        BITRATE_PKT_CNT,
        NUM_BITRATE_MODES,
};

static const char *bitrate_mode_strs[] = {
        [BITRATE_PKT_LEN] = "pkt_len",
        [BITRATE_PKT_CNT] = "pkt_cnt",
};

enum shaper_rate_blob_id {
        SHAPER_RATE_PID = 0,
        SHAPER_RATE_SHP,
        SHAPER_RATE_CLKDIV,
        SHAPER_RATE_BPS,
        SHAPER_RATE_MBPS,
        SHAPER_RATE_BURST,
        NUM_SHAPER_RATE_BLOB_ID,
};

static const struct blobmsg_policy portrate_blob_policy[] = {
        [SHAPER_RATE_PID]    = { .name = "port",        .type = BLOBMSG_TYPE_INT32 },
        [SHAPER_RATE_SHP]    = { .name = "shaper",      .type = BLOBMSG_TYPE_INT32 },
        [SHAPER_RATE_CLKDIV] = { .name = "clk_div",     .type = BLOBMSG_TYPE_INT32 },
        [SHAPER_RATE_BPS]    = { .name = "bps",         .type = BLOBMSG_TYPE_INT32 },
        [SHAPER_RATE_MBPS]   = { .name = "Mbps",        .type = BLOBMSG_TYPE_INT32 },
        [SHAPER_RATE_BURST]  = { .name = "allow_burst", .type = BLOBMSG_TYPE_INT32 },
};

static int port_rate_limit(struct ubus_context *ctx, struct ubus_object *obj,
                           struct ubus_request_data *req, const char *method,
                           struct blob_attr *msg)
{
        static const int req_args[] = {
                SHAPER_RATE_PID,
                SHAPER_RATE_SHP,
        };
        struct blob_attr *tb[NUM_SHAPER_RATE_BLOB_ID] = { };
        uint32_t port, shaper;
        uint8_t buf_req[tmu_msg_newlen(sizeof(struct tmu_shaper_set))] = { };
        struct tmu_msg *msg_req = (void *)buf_req;
        struct tmu_shaper_set *set = (void *)(msg_req->buf);
        struct tmu_shaper_rate_info *rate_info = &set->rate_info;
        struct tmu_msg *msg_resp = NULL; size_t resp_len;
        int err;

        rate_info->bps = 0;
        rate_info->mbps = 0;
        rate_info->allow_burst = -1;
        rate_info->clk_div = -1;

        blobmsg_parse(portrate_blob_policy,
                      ARRAY_SIZE(portrate_blob_policy),
                      tb, blob_data(msg), blob_len(msg));

        if (!has_all_ubus_args(tb, req_args, ARRAY_SIZE(req_args)))
                return UBUS_STATUS_INVALID_ARGUMENT;

        port = blobmsg_get_u32(tb[SHAPER_RATE_PID]);
        shaper = blobmsg_get_u32(tb[SHAPER_RATE_SHP]);

        if (port >= g_tmu_info.port_cnt)
                return UBUS_STATUS_INVALID_ARGUMENT;

        if (shaper >= g_tmu_info.shaper_per_port)
                return UBUS_STATUS_INVALID_ARGUMENT;

        if (!tb[SHAPER_RATE_BPS] && !tb[SHAPER_RATE_MBPS])
                return UBUS_STATUS_INVALID_ARGUMENT;

        tmu_hdr_set(msg_req,
                    TMU_MSG_CMD_SHAPER_RATE_SET,
                    TMU_COMP_SHAPER,
                    port,
                    shaper,
                    sizeof(struct tmu_shaper_set));

        if (tb[SHAPER_RATE_BPS])
                rate_info->bps = blobmsg_get_u32(tb[SHAPER_RATE_BPS]);

        if (tb[SHAPER_RATE_MBPS])
                rate_info->mbps = blobmsg_get_u32(tb[SHAPER_RATE_MBPS]);

        if (tb[SHAPER_RATE_BURST])
                rate_info->allow_burst = blobmsg_get_bool(tb[SHAPER_RATE_BURST]);

        if (tb[SHAPER_RATE_CLKDIV])
                rate_info->clk_div = (int)blobmsg_get_u32(tb[SHAPER_RATE_CLKDIV]);

        pr_info("%s bps:%d mbps:%d clk_div:%d\n", __func__, rate_info->bps, rate_info->mbps, rate_info->clk_div);
        err = tmu_send_recv(msg_req, sizeof(buf_req), &msg_resp, &resp_len);
        if (err)
                return UBUS_STATUS_NO_DATA;

        if (!msg_resp)
                return UBUS_STATUS_NO_DATA;

        err = UBUS_STATUS_UNKNOWN_ERROR;

        if (msg_resp->buflen == sizeof(int32_t)) {
                tmu_errmsg_ubus_reply(ctx, req, msg_resp);
                err = UBUS_STATUS_OK;
        }

        free(msg_resp);

        return err;
}

static int lif_rate_limit(struct ubus_context *ctx, struct ubus_object *obj,
                           struct ubus_request_data *req, const char *method,
                           struct blob_attr *msg)
{
        static const int req_args[] = {
                SHAPER_RATE_MBPS,
		};
        struct blob_attr *tb[NUM_SHAPER_RATE_BLOB_ID] = { };
        uint8_t buf_req[tmu_msg_newlen(sizeof(struct tmu_shaper_set))] = { };
        struct tmu_msg *msg_req = (void *)buf_req;
        struct tmu_shaper_set *set = (void *)(msg_req->buf);
        struct tmu_shaper_rate_info *rate_info = &set->rate_info;
        struct tmu_msg *msg_resp = NULL; size_t resp_len;
        int err;

        blobmsg_parse(portrate_blob_policy,
                      ARRAY_SIZE(portrate_blob_policy),
                      tb, blob_data(msg), blob_len(msg));

        if (!has_all_ubus_args(tb, req_args, ARRAY_SIZE(req_args)))
                return UBUS_STATUS_INVALID_ARGUMENT;

        if (!tb[SHAPER_RATE_MBPS])
                return UBUS_STATUS_INVALID_ARGUMENT;

        tmu_hdr_set(msg_req,
                    TMU_MSG_CMD_LIF_SHAPER_RATE_SET,
                    TMU_COMP_SHAPER,
                    0,
                    0,
                    sizeof(struct tmu_shaper_set));

		rate_info->mbps = blobmsg_get_u32(tb[SHAPER_RATE_MBPS]);

        err = tmu_send_recv(msg_req, sizeof(buf_req), &msg_resp, &resp_len);
        if (err)
                return UBUS_STATUS_NO_DATA;

        if (!msg_resp)
                return UBUS_STATUS_NO_DATA;

        err = UBUS_STATUS_UNKNOWN_ERROR;

        if (msg_resp->buflen == sizeof(int32_t)) {
                tmu_errmsg_ubus_reply(ctx, req, msg_resp);
                err = UBUS_STATUS_OK;
        }

        free(msg_resp);

        return err;
}

enum shaper_get_blob_id {
        SHAPER_GET_PID,
        SHAPER_GET_SHP,
        NUM_SHAPER_GET_BLOB_ID,
};

static const struct blobmsg_policy shp_get_blob_policy[] = {
        [SHAPER_GET_PID] = { .name = "port",   .type = BLOBMSG_TYPE_INT32 },
        [SHAPER_GET_SHP] = { .name = "shaper", .type = BLOBMSG_TYPE_INT32 },
};

static int tmu_shaper_get(struct ubus_context *ctx, struct ubus_object *obj,
                          struct ubus_request_data *req, const char *method,
                          struct blob_attr *msg)
{
        static const int req_args[] = {
                SHAPER_GET_PID,
                SHAPER_GET_SHP,
        };
        struct blob_attr *tb[NUM_SHAPER_GET_BLOB_ID] = { };
        struct tmu_msg msg_req = { };
        struct tmu_shaper_info *info;
        struct tmu_msg *msg_resp = NULL; size_t resp_len;
        struct blob_buf b = { };
        uint32_t port, shaper;
        int err = UBUS_STATUS_OK;

        blobmsg_parse(shp_get_blob_policy,
                      ARRAY_SIZE(shp_get_blob_policy),
                      tb, blob_data(msg), blob_len(msg));

        if (!has_all_ubus_args(tb, req_args, ARRAY_SIZE(req_args)))
                return UBUS_STATUS_INVALID_ARGUMENT;

        port = blobmsg_get_u32(tb[SHAPER_GET_PID]);
        shaper = blobmsg_get_u32(tb[SHAPER_GET_SHP]);

        if (port >= g_tmu_info.port_cnt)
                return UBUS_STATUS_INVALID_ARGUMENT;

        if (shaper >= g_tmu_info.shaper_per_port)
                return UBUS_STATUS_INVALID_ARGUMENT;

        tmu_hdr_set(&msg_req,
                    TMU_MSG_CMD_SHAPER_GET,
                    TMU_COMP_SHAPER,
                    port,
                    shaper,
                    0);

        if (tmu_send_recv(&msg_req, sizeof(msg_req), &msg_resp, &resp_len))
                return UBUS_STATUS_NO_DATA;

        if (!msg_resp)
                return UBUS_STATUS_NO_DATA;

        if (msg_resp->buflen != sizeof(struct tmu_shaper_info)) {
                err = UBUS_STATUS_SYSTEM_ERROR;

                if (msg_resp->buflen == sizeof(int32_t)) {
                        tmu_errmsg_ubus_reply(ctx, req, msg_resp);
                }

                goto out;
        }

        info = (void *)msg_resp->buf;

        blobmsg_buf_init(&b);

        blobmsg_add_u8(&b,     "enabled",      info->enabled);
        blobmsg_add_u32(&b,    "credit_rate",  info->credit_rate);
        blobmsg_add_u32(&b,    "credit_max",   info->credit_max);
        blobmsg_add_u32(&b,    "credit_min",   info->credit_min);
        blobmsg_add_u32(&b,    "credit_avail", info->credit_avail);
        blobmsg_add_u32(&b,    "weight_int",   info->credit_weight_int);
        blobmsg_add_u32(&b,    "weight_frac",  info->credit_weight_frac);
        blobmsg_add_u8(&b,     "allow_burst",  !info->credit_clear);
        blobmsg_add_string(&b, "sched_on",
                           info->bitrate_mode >= NUM_BITRATE_MODES ?
                           "unknown" : bitrate_mode_strs[info->bitrate_mode]);
        blobmsg_add_u8(&b,     "is_working",   info->is_working);
        blobmsg_add_u32(&b,    "location",     info->location);
        blobmsg_add_u64(&b,    "bps_limit",    info->bitrate);

        ubus_send_reply(ctx, req, b.head);

out:
        if (msg_resp)
                free(msg_resp);

        return err;
}

enum shaper_set_blob_id {
        SHAPER_SET_PID,
        SHAPER_SET_SHP,
        SHAPER_SET_ENABLED,
        SHAPER_SET_CLKDIV,
        SHAPER_SET_CRDMAX,
        SHAPER_SET_CRDMIN,
        SHAPER_SET_WGHT_INT,
        SHAPER_SET_WGHT_FRAC,
        SHAPER_SET_BURST_EN,
        SHAPER_SET_BITRATE_MODE,
        SHAPER_SET_LOCATION,
        NUM_SHAPER_SET_BLOB_ID,
};

static const struct blobmsg_policy shp_set_blob_policy[] = {
        [SHAPER_SET_PID]          = { .name = "port",         .type = BLOBMSG_TYPE_INT32 },
        [SHAPER_SET_SHP]          = { .name = "shaper",       .type = BLOBMSG_TYPE_INT32 },
        [SHAPER_SET_ENABLED]      = { .name = "enabled",      .type = BLOBMSG_TYPE_BOOL  },
        [SHAPER_SET_CLKDIV]       = { .name = "credit_rate",  .type = BLOBMSG_TYPE_INT32 },
        [SHAPER_SET_CRDMAX]       = { .name = "credit_max",   .type = BLOBMSG_TYPE_INT32 },
        [SHAPER_SET_CRDMIN]       = { .name = "credit_min",   .type = BLOBMSG_TYPE_INT32 },
        [SHAPER_SET_WGHT_INT]     = { .name = "weight_int",   .type = BLOBMSG_TYPE_INT32 },
        [SHAPER_SET_WGHT_FRAC]    = { .name = "weight_frac",  .type = BLOBMSG_TYPE_INT32 },
        [SHAPER_SET_BURST_EN]     = { .name = "allow_burst",  .type = BLOBMSG_TYPE_BOOL  },
        [SHAPER_SET_BITRATE_MODE] = { .name = "sched_on",     .type = BLOBMSG_TYPE_STRING },
        [SHAPER_SET_LOCATION]     = { .name = "location",     .type = BLOBMSG_TYPE_INT32 },
};

static int tmu_shaper_set(struct ubus_context *ctx, struct ubus_object *obj,
                          struct ubus_request_data *req, const char *method,
                          struct blob_attr *msg)
{
        static const int req_args[] = {
                SHAPER_SET_PID,
                SHAPER_SET_SHP,
        };
        struct blob_attr *tb[NUM_SHAPER_SET_BLOB_ID] = { };
        uint8_t buf_req[tmu_msg_newlen(sizeof(struct tmu_shaper_set))] = { };
        struct tmu_msg *msg_req = (void *)buf_req;
        struct tmu_shaper_set *set = (void *)(msg_req->buf);
        struct tmu_shaper_info *info = &set->info;
        struct tmu_msg *msg_resp = NULL; size_t resp_len;
        uint32_t port, shaper;

        int err;

        blobmsg_parse(shp_set_blob_policy,
                      ARRAY_SIZE(shp_set_blob_policy),
                      tb, blob_data(msg), blob_len(msg));

        if (!has_all_ubus_args(tb, req_args, ARRAY_SIZE(req_args)))
                return UBUS_STATUS_INVALID_ARGUMENT;

        port = blobmsg_get_u32(tb[SHAPER_GET_PID]);
        shaper = blobmsg_get_u32(tb[SHAPER_GET_SHP]);

        if (port >= g_tmu_info.port_cnt)
                return UBUS_STATUS_INVALID_ARGUMENT;

        if (shaper >= g_tmu_info.shaper_per_port)
                return UBUS_STATUS_INVALID_ARGUMENT;

        if (tb[SHAPER_SET_ENABLED]) {
                set->set |= BIT(SHAPER_ENABLED);
                info->enabled = blobmsg_get_bool(tb[SHAPER_SET_ENABLED]);
        }

        if (tb[SHAPER_SET_CLKDIV]) {
                set->set |= BIT(SHAPER_CREDIT_RATE);
                info->credit_rate = blobmsg_get_u32(tb[SHAPER_SET_CLKDIV]);
        }

        if (tb[SHAPER_SET_CRDMAX]) {
                set->set |= BIT(SHAPER_CREDIT_MAX);
                info->credit_max = blobmsg_get_u32(tb[SHAPER_SET_CRDMAX]);
        }

        if (tb[SHAPER_SET_CRDMIN]) {
                set->set |= BIT(SHAPER_CREDIT_MIN);
                info->credit_min = blobmsg_get_u32(tb[SHAPER_SET_CRDMIN]);
        }

        if (tb[SHAPER_SET_WGHT_INT]) {
                set->set |= BIT(SHAPER_CREDIT_WEIGHT_INT);
                info->credit_weight_int = blobmsg_get_u32(tb[SHAPER_SET_WGHT_INT]);
        }

        if (tb[SHAPER_SET_WGHT_FRAC]) {
                set->set |= BIT(SHAPER_CREDIT_WEIGHT_FRAC);
                info->credit_weight_frac = blobmsg_get_u32(tb[SHAPER_SET_WGHT_FRAC]);
        }

        if (tb[SHAPER_SET_BURST_EN]) {
                set->set |= BIT(SHAPER_ALLOW_BURST);
                info->credit_clear = !blobmsg_get_bool(tb[SHAPER_SET_BURST_EN]);
        }

        if (tb[SHAPER_SET_BITRATE_MODE]) {
                char *param = blobmsg_get_string(tb[SHAPER_SET_BITRATE_MODE]);
                int match = -1;

                for (uint32_t i = 0; i < ARRAY_SIZE(bitrate_mode_strs); i++) {
                        const char *s = bitrate_mode_strs[i];

                        if (0 == strncmp(s, param, strlen(s))) {
                                match = (int)i;
                                break;
                        }
                }

                if (match < 0)
                        return UBUS_STATUS_INVALID_ARGUMENT;

                set->set |= BIT(SHAPER_BITRATE_MODE);
                info->bitrate_mode = match;
        }

        if (tb[SHAPER_SET_LOCATION]) {
                set->set |= BIT(SHAPER_LOCATION);
                info->location = blobmsg_get_u32(tb[SHAPER_SET_LOCATION]);
        }

        if (set->set == 0)
                return UBUS_STATUS_INVALID_ARGUMENT;

        tmu_hdr_set(msg_req,
                    TMU_MSG_CMD_SHAPER_SET,
                    TMU_COMP_SHAPER,
                    port,
                    shaper,
                    sizeof(struct tmu_shaper_set));

        err = tmu_send_recv(msg_req, sizeof(buf_req), &msg_resp, &resp_len);
        if (err)
                return UBUS_STATUS_NO_DATA;

        if (!msg_resp)
                return UBUS_STATUS_NO_DATA;

        err = UBUS_STATUS_UNKNOWN_ERROR;

        if (msg_resp->buflen == sizeof(int32_t)) {
                tmu_errmsg_ubus_reply(ctx, req, msg_resp);
                err = UBUS_STATUS_OK;
        }

        free(msg_resp);

        return err;
}

static const struct ubus_method shaper_methods[] = {
        UBUS_METHOD("get", tmu_shaper_get, shp_get_blob_policy),
        UBUS_METHOD("set", tmu_shaper_set, shp_set_blob_policy),
        UBUS_METHOD("rate_limit", port_rate_limit, portrate_blob_policy),
        UBUS_METHOD("lif_rate_limit", lif_rate_limit, portrate_blob_policy),
};

static struct ubus_object_type shaper_object_type =
        UBUS_OBJECT_TYPE("tmu_port_shaper", shaper_methods);

static struct ubus_object port_shaper_object = {
        .name = "dpns.tmu.port.shaper",
        .type = &shaper_object_type,
        .methods = shaper_methods,
        .n_methods = ARRAY_SIZE(shaper_methods),
};

enum queue_get_blob_id {
        QUEUE_GET_PID,
        QUEUE_GET_Q,
        NUM_QUEUE_GET_BLOB_ID,
};

static const struct blobmsg_policy queue_get_blob_policy[] = {
        [QUEUE_GET_PID] = { .name = "port",  .type = BLOBMSG_TYPE_INT32 },
        [QUEUE_GET_Q]   = { .name = "queue", .type = BLOBMSG_TYPE_INT32 },
};

enum queue_drop_types {
        DROP_MIX_TAIL = 0,
        DROP_TAIL,
        DROP_WRED,
        DROP_BUF_CNT_TAIL,
        NUM_DROP_TYPES,
};

static const char *drop_type_strs[NUM_DROP_TYPES] = {
        [DROP_MIX_TAIL] = "mix-tail-drop",
        [DROP_TAIL] = "tail-drop",
        [DROP_WRED] = "wred",
        [DROP_BUF_CNT_TAIL] = "buf-cnt-tail-drop",
};

static int tmu_queue_get(struct ubus_context *ctx, struct ubus_object *obj,
                         struct ubus_request_data *req, const char *method,
                         struct blob_attr *msg)
{
        static const int req_args[] = {
                QUEUE_GET_PID,
                QUEUE_GET_Q,
        };
        struct blob_attr *tb[NUM_QUEUE_GET_BLOB_ID] = { };
        struct tmu_queue_info *info = NULL;
        struct tmu_msg msg_req = { };
        struct tmu_msg *msg_resp = NULL;
        size_t resp_len;
        struct blob_buf b = { };
        uint32_t port, queue;
        int err = UBUS_STATUS_OK;

        blobmsg_parse(queue_get_blob_policy,
                      ARRAY_SIZE(queue_get_blob_policy),
                      tb, blob_data(msg), blob_len(msg));

        if (!has_all_ubus_args(tb, req_args, ARRAY_SIZE(req_args)))
                return UBUS_STATUS_INVALID_ARGUMENT;

        port = blobmsg_get_u32(tb[QUEUE_GET_PID]);
        queue = blobmsg_get_u32(tb[QUEUE_GET_Q]);

        if (port >= g_tmu_info.port_cnt)
                return UBUS_STATUS_INVALID_ARGUMENT;

        if (queue >= g_tmu_info.queue_per_port)
                return UBUS_STATUS_INVALID_ARGUMENT;

        tmu_hdr_set(&msg_req,
                    TMU_MSG_CMD_QUEUE_GET,
                    TMU_COMP_QUEUE,
                    port,
                    queue,
                    0);

        if (tmu_send_recv(&msg_req, sizeof(msg_req), &msg_resp, &resp_len))
                return UBUS_STATUS_NO_DATA;

        if (!msg_resp)
                return UBUS_STATUS_NO_DATA;

        if (msg_resp->buflen != sizeof(struct tmu_queue_info)) {
                err = UBUS_STATUS_SYSTEM_ERROR;

                if (msg_resp->buflen == sizeof(int32_t))
                        tmu_errmsg_ubus_reply(ctx, req, msg_resp);

                goto out;
        }

        info = (void *)msg_resp->buf;

        blobmsg_buf_init(&b);

        blobmsg_add_u32(&b, "buf_cnt", info->buf_cnt);
        blobmsg_add_u32(&b, "buf_max", info->buf_max);
        blobmsg_add_u32(&b, "pkt_cnt", info->pkt_cnt);
        blobmsg_add_u32(&b, "qlen_max", info->qlen_max);
        blobmsg_add_u32(&b, "qlen_min", info->qlen_min);
        blobmsg_add_u32(&b, "q_head", info->q_head);
        blobmsg_add_u32(&b, "q_tail", info->q_tail);
        blobmsg_add_string(&b, "drop_type",
                           info->drop_type >= NUM_DROP_TYPES ? "unknown" : drop_type_strs[info->drop_type]);
        blobmsg_printf(&b, "wred_drop_probs",
                       "%hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu",
                       info->wred_drop_probs[0],
                       info->wred_drop_probs[1],
                       info->wred_drop_probs[2],
                       info->wred_drop_probs[3],
                       info->wred_drop_probs[4],
                       info->wred_drop_probs[5],
                       info->wred_drop_probs[6],
                       info->wred_drop_probs[7]);

        ubus_send_reply(ctx, req, b.head);

out:
        if (msg_resp)
                free(msg_resp);

        return err;
}

enum queue_set_blob_id {
        QUEUE_SET_PID,
        QUEUE_SET_Q,
        QUEUE_SET_BUF_MAX,
        QUEUE_SET_DROP_TYPE,
        QUEUE_SET_LEN_MAX,
        QUEUE_SET_LEN_MIN,
        QUEUE_SET_WRED_PROBS,
        NUM_QUEUE_SET_BLOB_ID,
};

static const struct blobmsg_policy queue_set_blob_policy[] = {
        [QUEUE_SET_PID]        = { .name = "port",            .type = BLOBMSG_TYPE_INT32  },
        [QUEUE_SET_Q]          = { .name = "queue",           .type = BLOBMSG_TYPE_INT32  },
        [QUEUE_SET_BUF_MAX]    = { .name = "buf_max",         .type = BLOBMSG_TYPE_INT32  },
        [QUEUE_SET_DROP_TYPE]  = { .name = "drop_type",       .type = BLOBMSG_TYPE_STRING },
        [QUEUE_SET_LEN_MAX]    = { .name = "qlen_max",        .type = BLOBMSG_TYPE_INT32  },
        [QUEUE_SET_LEN_MIN]    = { .name = "qlen_min",        .type = BLOBMSG_TYPE_INT32  },
        [QUEUE_SET_WRED_PROBS] = { .name = "wred_drop_probs", .type = BLOBMSG_TYPE_STRING },
};

static int tmu_queue_set(struct ubus_context *ctx, struct ubus_object *obj,
                         struct ubus_request_data *req, const char *method,
                         struct blob_attr *msg)
{
        static const int req_args[] = {
                QUEUE_SET_PID,
                QUEUE_SET_Q,
        };
        struct blob_attr *tb[NUM_QUEUE_SET_BLOB_ID] = { };
        uint8_t buf_req[tmu_msg_newlen(sizeof(struct tmu_queue_set))] = { };
        struct tmu_msg *msg_req = (void *)buf_req;
        struct tmu_queue_set *set = (void *)(msg_req->buf);
        struct tmu_queue_info *info = &set->info;
        struct tmu_msg *msg_resp = NULL;
        size_t resp_len;
        uint32_t port, queue;
        int err;

        blobmsg_parse(queue_set_blob_policy,
                      ARRAY_SIZE(queue_set_blob_policy),
                      tb, blob_data(msg), blob_len(msg));

        if (!has_all_ubus_args(tb, req_args, ARRAY_SIZE(req_args)))
                return UBUS_STATUS_INVALID_ARGUMENT;

        port = blobmsg_get_u32(tb[QUEUE_SET_PID]);
        queue = blobmsg_get_u32(tb[QUEUE_SET_Q]);

        if (port >= g_tmu_info.port_cnt)
                return UBUS_STATUS_INVALID_ARGUMENT;

        if (queue >= g_tmu_info.queue_per_port)
                return UBUS_STATUS_INVALID_ARGUMENT;

        if (tb[QUEUE_SET_BUF_MAX]) {
                set->set |= BIT(QUEUE_BUF_MAX);
                info->buf_max = blobmsg_get_u32(tb[QUEUE_SET_BUF_MAX]);
        }

        if (tb[QUEUE_SET_LEN_MAX]) {
                set->set |= BIT(QUEUE_LEN_MAX);
                info->qlen_max = blobmsg_get_u32(tb[QUEUE_SET_LEN_MAX]);
        }

        if (tb[QUEUE_SET_LEN_MIN]) {
                set->set |= BIT(QUEUE_LEN_MIN);
                info->qlen_min = blobmsg_get_u32(tb[QUEUE_SET_LEN_MIN]);
        }

        if (tb[QUEUE_SET_DROP_TYPE]) {
                char *param = blobmsg_get_string(tb[QUEUE_SET_DROP_TYPE]);
                int match = -1;

                for (uint32_t i = 0; i < ARRAY_SIZE(drop_type_strs); i++) {
                        const char *s = drop_type_strs[i];

                        if (0 == strncmp(s, param, strlen(s))) {
                                match = (int)i;
                                break;
                        }
                }

                if (match < 0)
                        return UBUS_STATUS_INVALID_ARGUMENT;

                set->set |= BIT(QUEUE_DROP_TYPE);
                info->drop_type = match;
        }

        if (tb[QUEUE_SET_WRED_PROBS]) {
                char *param = blobmsg_get_string(tb[QUEUE_SET_WRED_PROBS]);
                uint8_t *p = info->wred_drop_probs;

                if (8 != sscanf(param,
                                "%hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu",
                                &p[0], &p[1], &p[2], &p[3], &p[4], &p[5], &p[6], &p[7])) {
                        return UBUS_STATUS_INVALID_ARGUMENT;
                }

                set->set |= BIT(QUEUE_WRED_PROBS);
        }

        tmu_hdr_set(msg_req,
                    TMU_MSG_CMD_QUEUE_SET,
                    TMU_COMP_QUEUE,
                    port,
                    queue,
                    sizeof(struct tmu_queue_set));

        err = tmu_send_recv(msg_req, sizeof(buf_req), &msg_resp, &resp_len);
        if (err)
                return UBUS_STATUS_NO_DATA;

        if (!msg_resp)
                return UBUS_STATUS_NO_DATA;

        err = UBUS_STATUS_UNKNOWN_ERROR;

        if (msg_resp->buflen == sizeof(int32_t)) {
                tmu_errmsg_ubus_reply(ctx, req, msg_resp);
                err = UBUS_STATUS_OK;
        }

        free(msg_resp);

        return err;
}

static const struct ubus_method queue_methods[] = {
        UBUS_METHOD("get", tmu_queue_get, queue_get_blob_policy),
        UBUS_METHOD("set", tmu_queue_set, queue_set_blob_policy),
};

static struct ubus_object_type queue_object_type =
        UBUS_OBJECT_TYPE("tmu_port_queue", queue_methods);

static struct ubus_object port_queue_object = {
        .name = "dpns.tmu.port.queue",
        .type = &queue_object_type,
        .methods = queue_methods,
        .n_methods = ARRAY_SIZE(queue_methods),
};

enum sched_get_blob_id {
        SCHED_GET_PID,
        SCHED_GET_SCH,
        NUM_SCHED_GET_BLOB_ID,
};

static const struct blobmsg_policy sched_get_blob_policy[] = {
        [SCHED_GET_PID] = { .name = "port",  .type = BLOBMSG_TYPE_INT32 },
        [SCHED_GET_SCH] = { .name = "sched", .type = BLOBMSG_TYPE_INT32 },
};

enum sched_algo {
        SCH_PQ = 0,
        SCH_WFQ,
        SCH_DWRR,
        SCH_RR,
        SCH_WRR,
        NUM_SCH_ALGOS,
};

static const char *sched_algo_strs[] = {
        [SCH_PQ]   = "pq",
        [SCH_WFQ]  = "wfq",
        [SCH_DWRR] = "dwrr",
        [SCH_RR]   = "rr",
        [SCH_WRR]  = "wrr",
};

static int tmu_sched_get(struct ubus_context *ctx, struct ubus_object *obj,
                         struct ubus_request_data *req, const char *method,
                         struct blob_attr *msg)
{
        static const int req_args[] = {
                SCHED_GET_PID,
                SCHED_GET_SCH,
        };
        struct blob_attr *tb[NUM_SCHED_GET_BLOB_ID] = { };
        struct tmu_sched_info *info = NULL;
        struct tmu_msg msg_req = { };
        struct tmu_msg *msg_resp = NULL;
        size_t resp_len;
        struct blob_buf b = { };
        uint32_t port, sched;
        int err = UBUS_STATUS_OK;

        blobmsg_parse(sched_get_blob_policy,
                      ARRAY_SIZE(sched_get_blob_policy),
                      tb, blob_data(msg), blob_len(msg));

        if (!has_all_ubus_args(tb, req_args, ARRAY_SIZE(req_args)))
                return UBUS_STATUS_INVALID_ARGUMENT;

        port = blobmsg_get_u32(tb[SCHED_GET_PID]);
        sched = blobmsg_get_u32(tb[SCHED_GET_SCH]);

        if (port >= g_tmu_info.port_cnt)
                return UBUS_STATUS_INVALID_ARGUMENT;

        if (sched >= g_tmu_info.scheduler_per_port)
                return UBUS_STATUS_INVALID_ARGUMENT;

        tmu_hdr_set(&msg_req,
                    TMU_MSG_CMD_SCHED_GET,
                    TMU_COMP_SCHED,
                    port,
                    sched,
                    0);

        if (tmu_send_recv(&msg_req, sizeof(msg_req), &msg_resp, &resp_len))
                return UBUS_STATUS_NO_DATA;

        if (!msg_resp)
                return UBUS_STATUS_NO_DATA;

        if (msg_resp->buflen != sizeof(struct tmu_sched_info)) {
                err = UBUS_STATUS_SYSTEM_ERROR;

                if (msg_resp->buflen == sizeof(int32_t))
                        tmu_errmsg_ubus_reply(ctx, req, msg_resp);

                goto out;
        }

        info = (void *)msg_resp->buf;

        blobmsg_buf_init(&b);

        blobmsg_add_string(&b, "deq_algo",
                           info->deq_algo >= NUM_SCH_ALGOS ?
                           "unknown" : sched_algo_strs[info->deq_algo]);
        blobmsg_add_string(&b, "sched_on",
                           info->bitrate_mode >= NUM_BITRATE_MODES ?
                           "unknown" : bitrate_mode_strs[info->bitrate_mode]);

        if (sched == 0)
                blobmsg_add_u32(&b, "location", info->location);

        blobmsg_printf(&b, "queue_map", "%u %u %u %u %u %u %u %u",
                       info->q_map[0], info->q_map[1], info->q_map[2], info->q_map[3],
                       info->q_map[4], info->q_map[5], info->q_map[6], info->q_map[7]);
        blobmsg_printf(&b, "queue_weight", "%u %u %u %u %u %u %u %u",
                       info->q_weight[0], info->q_weight[1], info->q_weight[2], info->q_weight[3],
                       info->q_weight[4], info->q_weight[5], info->q_weight[6], info->q_weight[7]);

        ubus_send_reply(ctx, req, b.head);

out:
        if (msg_resp)
                free(msg_resp);

        return err;
}

enum sched_set_blob_id {
        SCHED_SET_PID,
        SCHED_SET_SCH,
        SCHED_SET_DEQ_ALGO,
        SCHED_SET_Q_MAP,
        SCHED_SET_Q_WGHT,
        SCHED_SET_BITRATE,
        SCHED_SET_LOCATION,
        NUM_SCHED_SET_BLOB_ID,
};

static const struct blobmsg_policy sched_set_blob_policy[] = {
        [SCHED_SET_PID]      = { .name = "port",         .type = BLOBMSG_TYPE_INT32  },
        [SCHED_SET_SCH]      = { .name = "sched",        .type = BLOBMSG_TYPE_INT32  },
        [SCHED_SET_DEQ_ALGO] = { .name = "deq_algo",     .type = BLOBMSG_TYPE_STRING },
        [SCHED_SET_Q_MAP]    = { .name = "queue_map",    .type = BLOBMSG_TYPE_STRING },
        [SCHED_SET_Q_WGHT]   = { .name = "queue_weight", .type = BLOBMSG_TYPE_STRING },
        [SCHED_SET_BITRATE]  = { .name = "sched_on",     .type = BLOBMSG_TYPE_STRING },
        [SCHED_SET_LOCATION] = { .name = "location",     .type = BLOBMSG_TYPE_INT32  },
};

static int tmu_sched_set(struct ubus_context *ctx, struct ubus_object *obj,
                         struct ubus_request_data *req, const char *method,
                         struct blob_attr *msg)
{
        static const int req_args[] = {
                SCHED_SET_PID,
                SCHED_SET_SCH,
        };
        struct blob_attr *tb[NUM_SCHED_SET_BLOB_ID] = { };
        uint8_t buf_req[tmu_msg_newlen(sizeof(struct tmu_sched_set))] = { };
        struct tmu_msg *msg_req = (void *)buf_req;
        struct tmu_sched_set *set = (void *)(msg_req->buf);
        struct tmu_sched_info *info = &set->info;
        struct tmu_msg *msg_resp = NULL;
        size_t resp_len;
        uint32_t port, sched;
        int err;

        blobmsg_parse(sched_set_blob_policy,
                      ARRAY_SIZE(sched_set_blob_policy),
                      tb, blob_data(msg), blob_len(msg));

        if (!has_all_ubus_args(tb, req_args, ARRAY_SIZE(req_args)))
                return UBUS_STATUS_INVALID_ARGUMENT;

        port = blobmsg_get_u32(tb[SCHED_SET_PID]);
        sched = blobmsg_get_u32(tb[SCHED_SET_SCH]);

        if (port >= g_tmu_info.port_cnt)
                return UBUS_STATUS_INVALID_ARGUMENT;

        if (sched >= g_tmu_info.scheduler_per_port)
                return UBUS_STATUS_INVALID_ARGUMENT;

        if (tb[SCHED_SET_DEQ_ALGO]) {
                char *param = blobmsg_get_string(tb[SCHED_SET_DEQ_ALGO]);
                int match = -1;

                for (uint32_t i = 0; i < ARRAY_SIZE(sched_algo_strs); i++) {
                        const char *s = sched_algo_strs[i];

                        if (0 == strncmp(s, param, strlen(s))) {
                                match = (int)i;
                                break;
                        }
                }

                if (match < 0)
                        return UBUS_STATUS_INVALID_ARGUMENT;

                set->set |= BIT(SCHED_DEQ_ALGO);
                info->deq_algo = match;
        }

        if (tb[SCHED_SET_Q_MAP]) {
                char *param = blobmsg_get_string(tb[SCHED_SET_Q_MAP]);
                uint32_t *p = info->q_map;

                if (8 != sscanf(param,
                                "%u %u %u %u %u %u %u %u",
                                &p[0], &p[1], &p[2], &p[3], &p[4], &p[5], &p[6], &p[7])) {
                        return UBUS_STATUS_INVALID_ARGUMENT;
                }

                set->set |= BIT(SCHED_Q_MAP);
        }

        if (tb[SCHED_SET_Q_WGHT]) {
                char *param = blobmsg_get_string(tb[SCHED_SET_Q_WGHT]);
                uint32_t *p = info->q_weight;

                if (8 != sscanf(param,
                                "%u %u %u %u %u %u %u %u",
                                &p[0], &p[1], &p[2], &p[3], &p[4], &p[5], &p[6], &p[7])) {
                        return UBUS_STATUS_INVALID_ARGUMENT;
                }

                set->set |= BIT(SCHED_Q_WGHT);
        }

        if (tb[SCHED_SET_BITRATE]) {
                char *param = blobmsg_get_string(tb[SCHED_SET_BITRATE]);
                int match = -1;

                for (uint32_t i = 0; i < ARRAY_SIZE(bitrate_mode_strs); i++) {
                        const char *s = bitrate_mode_strs[i];

                        if (0 == strncmp(s, param, strlen(s))) {
                                match = (int)i;
                                break;
                        }
                }

                if (match < 0)
                        return UBUS_STATUS_INVALID_ARGUMENT;

                set->set |= BIT(SCHED_BITRATE_MODE);
                info->bitrate_mode = match;
        }

        if (sched == 0 && tb[SCHED_SET_LOCATION]) {
                set->set |= BIT(SCHED_LOCATION);
                info->location = blobmsg_get_u32(tb[SCHED_SET_LOCATION]);
        }

        tmu_hdr_set(msg_req,
                    TMU_MSG_CMD_SCHED_SET,
                    TMU_COMP_SCHED,
                    port,
                    sched,
                    sizeof(struct tmu_sched_set));

        err = tmu_send_recv(msg_req, sizeof(buf_req), &msg_resp, &resp_len);
        if (err)
                return UBUS_STATUS_NO_DATA;

        if (!msg_resp)
                return UBUS_STATUS_NO_DATA;

        err = UBUS_STATUS_UNKNOWN_ERROR;

        if (msg_resp->buflen == sizeof(int32_t)) {
                tmu_errmsg_ubus_reply(ctx, req, msg_resp);
                err = UBUS_STATUS_OK;
        }

        free(msg_resp);

        return err;
}

static const struct ubus_method sched_methods[] = {
        UBUS_METHOD("get", tmu_sched_get, sched_get_blob_policy),
        UBUS_METHOD("set", tmu_sched_set, sched_set_blob_policy),
};

static struct ubus_object_type sched_object_type =
        UBUS_OBJECT_TYPE("tmu_port_sched", sched_methods);

static struct ubus_object port_sched_object = {
        .name = "dpns.tmu.port.sched",
        .type = &sched_object_type,
        .methods = sched_methods,
        .n_methods = ARRAY_SIZE(sched_methods),
};

enum port_blob_id {
        PORT_PID = 0,
        NUM_PORT_BLOB_ID,
};

static const struct blobmsg_policy port_blob_policy[] = {
        [PORT_PID] = { .name = "port", .type = BLOBMSG_TYPE_INT32 },
};

static int tmu_port_reset(struct ubus_context *ctx, struct ubus_object *obj,
                          struct ubus_request_data *req, const char *method,
                          struct blob_attr *msg)
{
        struct tmu_msg msg_req = { .cmd = TMU_MSG_CMD_PORT_RST };
        struct tmu_msg *msg_resp = NULL; size_t resp_len;
        struct blob_attr *tb[NUM_PORT_BLOB_ID] = { };
        static const int req_args[] = {
                PORT_PID,
        };
        uint32_t port;

        blobmsg_parse(port_blob_policy,
                      ARRAY_SIZE(port_blob_policy),
                      tb, blob_data(msg), blob_len(msg));

        if (!has_all_ubus_args(tb, req_args, ARRAY_SIZE(req_args)))
                return UBUS_STATUS_INVALID_ARGUMENT;

        port = blobmsg_get_u32(tb[PORT_PID]);

        msg_req.port = port;

        if (tmu_send_recv(&msg_req, sizeof(msg_req), &msg_resp, &resp_len))
                return UBUS_STATUS_NO_DATA;

        if (!msg_resp)
                return UBUS_STATUS_NO_DATA;

        if (msg_resp->buflen != sizeof(int32_t)) {
                free(msg_resp);
                return UBUS_STATUS_SYSTEM_ERROR;
        }

        tmu_errmsg_ubus_reply(ctx, req, msg_resp);

        free(msg_resp);

        return UBUS_STATUS_OK;
}

static const struct ubus_method tmu_port_methods[] = {
        UBUS_METHOD("reset", tmu_port_reset, port_blob_policy),
};

static struct ubus_object_type tmu_port_object_type =
        UBUS_OBJECT_TYPE("tmu_port", tmu_port_methods);

static struct ubus_object tmu_port_object = {
        .name = "dpns.tmu.port",
        .type = &tmu_port_object_type,
        .methods = tmu_port_methods,
        .n_methods = ARRAY_SIZE(tmu_port_methods),
};

static int ubus_tmu_reset(struct ubus_context *ctx, struct ubus_object *obj,
                          struct ubus_request_data *req, const char *method,
                          struct blob_attr *msg)
{
        struct tmu_msg buf = { .cmd = TMU_MSG_CMD_RST };
        int err;

        err = sfgenl_msg_send(SF_GENL_COMP_TMU, &buf, sizeof(buf));
        if (err)
                return UBUS_STATUS_UNKNOWN_ERROR;

        return UBUS_STATUS_OK;
}

static const struct ubus_method tmu_methods[] = {
        UBUS_METHOD_NOARG("reset", ubus_tmu_reset),
};

static struct ubus_object_type tmu_object_type =
        UBUS_OBJECT_TYPE("tmu", tmu_methods);

static struct ubus_object tmu_object = {
        .name = "tmu",
        .type = &tmu_object_type,
        .methods = tmu_methods,
        .n_methods = ARRAY_SIZE(tmu_methods),
};

static struct ubus_object *obj_list[] = {
        &tmu_object,
        &tmu_port_object,
        &port_queue_object,
        &port_sched_object,
        &port_shaper_object,
};

int tmu_ubus_object_add(struct ubus_context *ctx)
{
        int err;

        for (unsigned i = 0; i < ARRAY_SIZE(obj_list); i++) {
                struct ubus_object *obj = obj_list[i];

                pr_info("%s(): obj: %s\n", __func__, obj->name);

                if ((err = ubus_add_object(ctx, obj))) {
                        pr_err("%s(): failed to add ubus object \"%s\"\n",
                               __func__, obj->name);
                        return err;
                }
        }

        return 0;
}

int tmu_ubus_object_del(struct ubus_context *ctx)
{
        int err;

        for (unsigned i = 0; i < ARRAY_SIZE(obj_list); i++) {
                struct ubus_object *obj = obj_list[i];

                if ((err = ubus_remove_object(ctx, obj))) {
                        pr_err("%s(): failed to remove ubus object \"%s\"\n",
                               __func__, obj->name);
                        return err;
                }
        }

        return 0;
}

static int tmu_init_info_set(struct tmu_msg *msg)
{
        if (msg->cmd != TMU_MSG_CMD_INFO_GET) {
                pr_err("invalid cmd\n");
                return -EINVAL;
        }

        if (msg->buflen != sizeof(struct tmu_info)) {
                pr_err("invalid buffer length\n");
                return -EINVAL;
        }

        memcpy(&g_tmu_info, msg->buf, sizeof(g_tmu_info));

        return 0;
}

void *tmu_init_worker(void *data)
{
        do {
                struct tmu_msg init_msg = { .cmd = TMU_MSG_CMD_INFO_GET };
                struct tmu_msg *rsp_msg = NULL;
                size_t rsp_len = 0;

                if (!is_genl_registered())
                        goto retry;

                int err = sfgenl_msg_send_recv(SF_GENL_COMP_TMU,
                                               &init_msg, sizeof(init_msg),
                                               (void *)&rsp_msg, &rsp_len,
                                               0);
                if (err) {
                        if (err == -ETIMEDOUT) {
                                pr_dbg("tmu kernel module is not loaded\n");
                        } else {
                                pr_dbg("failed to send tmu init message, err=%d\n", err);
                        }

                        goto retry;
                }

                if (is_invalid_tmu_msg(rsp_msg, rsp_len)) {
                        pr_err("invalid tmu msg responded from kernel\n");
                        goto retry;
                }

                if (tmu_init_info_set(rsp_msg)) {
                        pr_err("invalid tmu info responded from kernel\n");
                        goto retry;
                }

                break;

retry:
                if (rsp_msg)
                        free(rsp_msg);

                sleep(3); // prevent spamming
        } while (!is_gonna_exit());

        pthread_exit(NULL);
}

int tmu_init(void)
{
        pthread_t init_worker;

        if (pthread_create(&init_worker, NULL, tmu_init_worker, NULL))
                return -EINVAL;

        // ubus_object_add() works in main thread only
        if (tmu_ubus_object_add(ubus_ctx_get()))
                pr_err("failed to add ubus object\n");

        return 0;
}

int tmu_exit(void)
{
        tmu_ubus_object_del(ubus_ctx_get());

        return 0;
}
