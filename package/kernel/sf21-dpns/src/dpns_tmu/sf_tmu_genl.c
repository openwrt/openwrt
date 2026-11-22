#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/netlink.h>
#include <linux/genetlink.h>

#include <net/genetlink.h>

#include "sf_genl_msg.h"

#include "sf_tmu.h"
#include "sf_tmu_genl.h"

extern TMU_t *g_tmu;
static int tmu_errmsg_reply(struct genl_info *genl_info, struct tmu_msg *req, int err)
{
        u8 _resp[tmu_msg_newlen(sizeof(s32))] = { };
        struct tmu_msg *resp = (void *)_resp;

        // copy header
        memcpy(resp, req, sizeof(struct tmu_msg));

        resp->buflen = sizeof(s32);
        *((s32 *)resp->buf) = err;

        return sfgenl_msg_reply(genl_info, _resp, sizeof(_resp));
}

static int genl_tmu_reset(struct genl_info *genl_info, struct tmu_msg *req)
{
        tmu_reset(tmu_get());

        return tmu_errmsg_reply(genl_info, req, 0);;
}

static int genl_tmu_port_reset(struct genl_info *genl_info, struct tmu_msg *req)
{
        _tmu_reset(tmu_get(), req->port);

        return tmu_errmsg_reply(genl_info, req, 0);
}

static int genl_tmu_info_get(struct genl_info *genl_info)
{
        u8 buf[tmu_msg_newlen(sizeof(struct tmu_info))] = { };
        struct tmu_msg *msg = (void *)buf;
        struct tmu_info *info = (void *)msg->buf;

        msg->cmd = TMU_MSG_CMD_INFO_GET;
        msg->buflen = sizeof(struct tmu_info);

        info->port_cnt = TMU_MAX_PORT_CNT;
        info->queue_per_port = QUE_MAX_NUM_PER_PORT;
        info->shaper_per_port = QUE_SHAPER_NUM_PER_PORT;
        info->scheduler_per_port = QUE_SCH_NUM_PER_PORT;

        return sfgenl_msg_reply(genl_info, buf, sizeof(buf));
}

static uint64_t shaper_bps_compute(uint32_t clk_div, uint32_t wght_int, uint32_t wght_frac)
{
        uint64_t Bps_int = 0, Bps_frac = 0;
        u64 sysclk = clk_get_rate(g_tmu->cpriv->clk);	//get NPU clock Hz

        Bps_int = (uint64_t)(sysclk * wght_int * 8) / (1 << (clk_div + 1));
        Bps_frac = (uint64_t)(sysclk * wght_frac * 8) / (1 << (clk_div + 1 + 12));
        return Bps_int + Bps_frac;
}

static int shaper_info_get(struct genl_info *genl_info, struct tmu_msg *req)
{
        u8 buf[tmu_msg_newlen(sizeof(struct tmu_shaper_info))] = { };
        struct tmu_msg *resp = (void *)buf;
        struct tmu_shaper_info *info = (void *)resp->buf;
        TMU_t *tmu = g_tmu;
        u32 port, shaper;

        port = req->port;
        shaper = req->u.shaper;

        resp->cmd = TMU_MSG_CMD_SHAPER_GET;
        resp->buflen = sizeof(struct tmu_shaper_info);

        info->enabled = tmu_shaper_is_enabled(tmu, port, shaper);
        info->is_working = tmu_shaper_is_working(tmu, port, shaper);
        tmu_shaper_credit_rate_get(tmu, port, shaper, &info->credit_rate);
        tmu_shaper_credit_int_weight_get(tmu, port, shaper, &info->credit_weight_int);
        tmu_shaper_credit_frac_weight_get(tmu, port, shaper, &info->credit_weight_frac);
        tmu_shaper_max_credit_get(tmu, port, shaper, &info->credit_max);
        tmu_shaper_min_credit_get(tmu, port, shaper, &info->credit_min);
        tmu_shaper_credit_get(tmu, port, shaper, &info->credit_avail);
        tmu_shaper_credit_clear_get(tmu, port, shaper, &info->credit_clear);
        tmu_shaper_pos_get(tmu, port, shaper, &info->location);
        tmu_shaper_bitrate_mode_get(tmu, port, shaper, &info->bitrate_mode);
        info->bitrate = shaper_bps_compute(info->credit_rate, info->credit_weight_int, info->credit_weight_frac);

        return sfgenl_msg_reply(genl_info, buf, sizeof(buf));
}

static int shaper_info_set(struct genl_info *genl_info, struct tmu_msg *req)
{
        struct tmu_shaper_set *set = (void *)req->buf;
        struct tmu_shaper_info *info = &set->info;
        TMU_t *tmu = g_tmu;
        u32 port, shaper;

        port = req->port;
        shaper = req->u.shaper;

        if (set->set == 0)
                return -EINVAL;

        if (set->set & BIT(SHAPER_ENABLED)) {
                // if disable, disable first
                if (!info->enabled)
                        tmu_shaper_enable(tmu, port, shaper, info->enabled);
        }

        if (set->set & BIT(SHAPER_CREDIT_RATE))
                tmu_shaper_credit_rate_set(tmu, port, shaper, info->credit_rate);

        if (set->set & BIT(SHAPER_CREDIT_MAX))
                tmu_shaper_max_credit_set(tmu, port, shaper, info->credit_max);

        if (set->set & BIT(SHAPER_CREDIT_MIN))
                tmu_shaper_min_credit_set(tmu, port, shaper, info->credit_min);

        if (set->set & BIT(SHAPER_CREDIT_WEIGHT_INT))
                tmu_shaper_credit_int_weight_set(tmu, port, shaper, info->credit_weight_int);

        if (set->set & BIT(SHAPER_CREDIT_WEIGHT_FRAC))
                tmu_shaper_credit_frac_weight_set(tmu, port, shaper, info->credit_weight_frac);

        if (set->set & BIT(SHAPER_ALLOW_BURST))
                tmu_shaper_credit_clear_set(tmu, port, shaper, info->credit_clear);

        if (set->set & BIT(SHAPER_BITRATE_MODE))
                tmu_shaper_bitrate_mode_set(tmu, port, shaper, info->bitrate_mode);

        if (set->set & BIT(SHAPER_LOCATION))
                tmu_shaper_pos_set(tmu, port, shaper, info->location);

        if (set->set & BIT(SHAPER_ENABLED)) {
                tmu_shaper_enable(tmu, port, shaper, info->enabled);
        }

        return tmu_errmsg_reply(genl_info, req, 0);
}

/*
clock_div calculate way:
weight = (throughput*(2^(clock_div+1))/sysclk)*(2^9)/(2^12)
weight range：less than 256
throughput unit：Mbps
sysclk unit：Mhz
clock_div range: 0-15
*/
static int calc_clk_div(u64 sysclk, int mbps)
{
    int clk_div = 0;
    int weight = 0;

    while(1) {
        weight = ((mbps*(1 << (clk_div + 1)) / (sysclk / (1000 * 1000))) * (1 << 9)) / (1 << 12);
        if ((weight > (TMU_WEIGHT_MAX - 1)) || (clk_div > TMU_SHP_CLKDIV_MAX)) {
            break;
        }
        clk_div++;
    }

    if ((clk_div-1) < 0) {
        return TMU_SHP_CLKDIV_DEF;
    }

    return (clk_div-1);
}


static int shaper_rate_info_set(struct genl_info *genl_info, struct tmu_msg *req)
{
        struct tmu_shaper_set *set = (void *)req->buf;
        struct tmu_shaper_rate_info *rate_info = &set->rate_info;
        uint64_t bps = 0, mbps = 0; // bits per sec
        uint64_t Bps = 0; // bytes per sec
        int clk_div = -1;
        uint32_t weight = 0;
        u64 sysclk = clk_get_rate(g_tmu->cpriv->clk);	//get NPU clock Hz

        TMU_t *tmu = g_tmu;
        u32 port, shaper;

        port = req->port;
        shaper = req->u.shaper;
        TMU_DBG(INFO_LV, "rate_info:bps:%d Mbps:%d clk_div:%d\n", rate_info->bps, rate_info->mbps, rate_info->clk_div);

        bps = rate_info->mbps * 1000ULL * 1000ULL + rate_info->bps;
        mbps = bps / 1000ULL / 1000ULL;
        Bps = bps / 8;

        // say -1 for default value
        if (rate_info->clk_div < 0) {
            // calibrate low speed
            clk_div = calc_clk_div(sysclk, mbps);
        } else {
            clk_div = rate_info->clk_div;
        }

        weight = Bps * (1 << (clk_div + 1 + 12)) / (sysclk);
        TMU_DBG(INFO_LV, "get sysclk:%llu weight:0x%x bps:%llu Mbps:%llu Bps:%llu clk_div:%d\n", sysclk, weight, bps, mbps, Bps, clk_div);

        // TODO: DISABLE SHAPER, SET, ENABLE SHAPER AGAIN
        tmu_shaper_credit_rate_set(tmu, port, shaper, clk_div);
        tmu_shaper_rmw32(tmu, port, shaper, TMU_SHP_WEIGHT, TMU_SHP_WEIGHT_INT_MASK | TMU_SHP_WEIGHT_FRAC_MASK, 0, weight); //[19:0]

        if ((set->info.credit_weight_int > TMU_SHP_INT_WGHT_MAX) ||
                (set->info.credit_weight_frac > TMU_SHP_FRAC_WGHT_MAX)) {
                printk("computed weight is too big, try to decrease clk_div\n");
        }

        if (rate_info->allow_burst >= 0)
            tmu_shaper_credit_clear_set(tmu, port, shaper, !rate_info->allow_burst);

        return tmu_errmsg_reply(genl_info, req, 0);
}

/*
 * clock_div calculate way:
 * weight = (throughput*(2^(clock_div+1))/sysclk)*(2^5)
 * weight range：0 ~ 0x7ff
 * throughput unit：Mbps
 * sysclk unit：Mhz
 * clock_div range: 0-15
 * */
static int lif_calc_clk_div(u64 sysclk, u32 mbps)
{
	u64 clk_div = 0, weight = 0;

	while(1) {
		weight = (u64)mbps*(1 << (clk_div + 1))*(1 << 5)*1000000/sysclk;
		if ((weight > LIF_WEIGHT_MAX) || (clk_div > TMU_SHP_CLKDIV_MAX))
			break;

		clk_div++;
	}

	if ((clk_div-1) < 0) {
		TMU_DBG(INFO_LV, "clk_div:%lld out of range\n", clk_div);
		return LIF_SHP_CLKDIV_DEF;
	}

	return (clk_div-1);
}

int lif_speed_limit(u32 mbps)
{
	u64 sysclk = clk_get_rate(g_tmu->cpriv->clk);
	u64 clk_div, weight;
	u32 ctrl;

	clk_div = lif_calc_clk_div(sysclk, mbps);
	weight = (u64)mbps * (1 << (clk_div + 1)) * (1 << 5) * 1000000 / sysclk;
	TMU_DBG(INFO_LV, "get sysclk:%llu weight:0x%llx Mbps:%u clk_div:%lld\n",
			sysclk, weight, mbps, clk_div);

	/* default use shaper0 */
	ctrl = FIELD_PREP(ETHSYS_SHP0_MAX_CRDT, 0xf00) |
		FIELD_PREP(ETHSYS_SHP0_CLK_DIV, clk_div) |
		ETHSYS_SHP0_EN;
	regmap_write(g_tmu->cpriv->ethsys, ETHSYS_SHP0_MIN_CRDT, 0x3ff00);
	regmap_write(g_tmu->cpriv->ethsys, ETHSYS_SHP0_WGHT, weight);
	regmap_write(g_tmu->cpriv->ethsys, ETHSYS_SHP0_CTRL, ctrl);
	return 0;
}

static int lif_shaper_rate_info_set(struct genl_info *genl_info,
		struct tmu_msg *req)
{
        struct tmu_shaper_set *set = (void *)req->buf;
        struct tmu_shaper_rate_info *rate_info = &set->rate_info;

        lif_speed_limit(rate_info->mbps);
        return tmu_errmsg_reply(genl_info, req, 0);
}

static int queue_info_get(struct genl_info *genl_info, struct tmu_msg *req)
{
        u8 buf[tmu_msg_newlen(sizeof(struct tmu_queue_info))] = { };
        struct tmu_msg *resp = (void *)buf;
        struct tmu_queue_info *info = (void *)resp->buf;
        u32 port, queue;
        TMU_t *tmu = g_tmu;

        port = req->port;
        queue = req->u.queue;

        resp->cmd = TMU_MSG_CMD_QUEUE_GET;
        resp->buflen = sizeof(struct tmu_queue_info);

        tmu_queue_buf_cnt_get(tmu, port, queue, &info->buf_cnt);
        tmu_queue_buf_max_get(tmu, port, queue, &info->buf_max);
        tmu_queue_pkt_cnt_get(tmu, port, queue, &info->pkt_cnt);
        tmu_queue_ptr_get(tmu, port, queue, &info->q_head, &info->q_tail);
        tmu_queue_type_get(tmu, port, queue, &info->drop_type);
        tmu_qlen_max_get(tmu, port, queue, &info->qlen_max);
        tmu_qlen_min_get(tmu, port, queue, &info->qlen_min);
        tmu_wred_drop_probs_get(tmu, port, queue, info->wred_drop_probs);

        return sfgenl_msg_reply(genl_info, buf, sizeof(buf));
}

static int queue_info_set(struct genl_info *genl_info, struct tmu_msg *req)
{
        struct tmu_queue_set *set = (void *)req->buf;
        struct tmu_queue_info *info = &set->info;
        u32 port, queue;
        TMU_t *tmu = g_tmu;

        port = req->port;
        queue = req->u.queue;

        if (set->set == 0)
                return -EINVAL;

        if (set->set & BIT(QUEUE_BUF_MAX))
                tmu_queue_buf_max_set(tmu, port, queue, info->buf_max);

        if (set->set & BIT(QUEUE_DROP_TYPE))
                tmu_queue_type_set(tmu, port, queue, info->drop_type);

        if (set->set & BIT(QUEUE_LEN_MAX))
                tmu_qlen_max_set(tmu, port, queue, info->qlen_max);

        if (set->set & BIT(QUEUE_LEN_MIN))
                tmu_qlen_min_set(tmu, port, queue, info->qlen_min);

        if (set->set & BIT(QUEUE_WRED_PROBS))
                tmu_wred_drop_probs_set(tmu, port, queue, info->wred_drop_probs);

        return tmu_errmsg_reply(genl_info, req, 0);
}

static int sched_info_get(struct genl_info *genl_info, struct tmu_msg *req)
{
        u8 buf[tmu_msg_newlen(sizeof(struct tmu_sched_info))] = { };
        struct tmu_msg *resp = (void *)buf;
        struct tmu_sched_info *info = (void *)resp->buf;
        u32 port, sched;
        TMU_t *tmu = g_tmu;

        port = req->port;
        sched = req->u.sched;

        resp->cmd = TMU_MSG_CMD_SCHED_GET;
        resp->buflen = sizeof(struct tmu_sched_info);

        tmu_sched_type_get(tmu, port, sched, &info->deq_algo);
        tmu_sched_queue_map_get(tmu, port, sched, info->q_map);
        tmu_sched_queue_weight_get(tmu, port, sched, info->q_weight);
        tmu_sched_bitrate_mode_get(tmu, port, sched, &info->bitrate_mode);

        if (sched == 0)
                tmu_sched0_pos_get(tmu, port, &info->location);

        return sfgenl_msg_reply(genl_info, buf, sizeof(buf));
}

static int sched_info_set(struct genl_info *genl_info, struct tmu_msg *req)
{
        struct tmu_sched_set *set = (void *)req->buf;
        struct tmu_sched_info *info = &set->info;
        u32 port, sched;
        TMU_t *tmu = g_tmu;

        port = req->port;
        sched = req->u.sched;

        if (set->set == 0)
                return -EINVAL;

        if (set->set & BIT(SCHED_DEQ_ALGO))
                tmu_sched_type_set(tmu, port, sched, info->deq_algo);

        if (set->set & BIT(SCHED_Q_MAP))
                tmu_sched_queue_map_set(tmu, port, sched, info->q_map);

        if (set->set & BIT(SCHED_Q_WGHT))
                tmu_sched_queue_weight_set(tmu, port, sched, info->q_weight);

        if (set->set & BIT(SCHED_BITRATE_MODE))
                tmu_sched_bitrate_mode_set(tmu, port, sched, info->bitrate_mode);

        if (sched == 0 && (set->set & BIT(SCHED_LOCATION)))
                tmu_sched0_pos_set(tmu, port, info->location);

        return tmu_errmsg_reply(genl_info, req, 0);
}

static int is_valid_tmu_msg(struct tmu_msg *msg, size_t buflen)
{
        u32 comp_max;

        if (tmu_msglen(msg) != buflen) {
                return 0;
        }

        if (msg->port >= TMU_MAX_PORT_CNT)
                return 0;

        switch (msg->comp) {
        case TMU_COMP_QUEUE:
                comp_max = QUE_MAX_NUM_PER_PORT;
                break;

        case TMU_COMP_SCHED:
                comp_max = QUE_SCH_NUM_PER_PORT;
                break;

        case TMU_COMP_SHAPER:
                comp_max = QUE_SHAPER_NUM_PER_PORT;
                break;

        case TMU_COMP_PORT:
                goto out;

        default:
                return 0;
        }

        if (msg->u.nr_comp >= comp_max)
                return 0;

out:
        return 1;
}

static int tmu_msg_recv(struct genl_info *info, void *buf, size_t buflen)
{
        struct tmu_msg *msg = buf;
        int err = 0;

        if (unlikely(!tmu_get()))
                return -ENODEV;

        if (!is_valid_tmu_msg(msg, buflen)) {
                return -EINVAL;
        }

        switch (msg->cmd) {
        case TMU_MSG_CMD_INFO_GET:
                err = genl_tmu_info_get(info);
                break;

        case TMU_MSG_CMD_RST:
                err = genl_tmu_reset(info, msg);
                break;

        case TMU_MSG_CMD_PORT_RST:
                err = genl_tmu_port_reset(info, msg);
                break;

        case TMU_MSG_CMD_QUEUE_GET:
                err = queue_info_get(info, msg);
                break;

        case TMU_MSG_CMD_QUEUE_SET:
                err = queue_info_set(info, msg);
                break;

        case TMU_MSG_CMD_SCHED_GET:
                err = sched_info_get(info, msg);
                break;

        case TMU_MSG_CMD_SCHED_SET:
                err = sched_info_set(info, msg);
                break;

        case TMU_MSG_CMD_SHAPER_GET:
                err = shaper_info_get(info, msg);
                break;

        case TMU_MSG_CMD_SHAPER_SET:
                err = shaper_info_set(info, msg);
                break;

        case TMU_MSG_CMD_SHAPER_RATE_SET:
                err = shaper_rate_info_set(info, msg);
                break;

        case TMU_MSG_CMD_LIF_SHAPER_RATE_SET:
                err = lif_shaper_rate_info_set(info, msg);
                break;

        default:
                err = -ENOTSUPP;
                break;
        }

        if (err)
                tmu_errmsg_reply(info, msg, err);

        return err;
}

static struct sfgenl_msg_ops tmu_msg_ops = {
        .msg_recv = tmu_msg_recv,
};

int tmu_genl_init(void)
{
        return sfgenl_ops_register(SF_GENL_COMP_TMU, &tmu_msg_ops);
}

int tmu_genl_deinit(void)
{
        return sfgenl_msg_ops_unregister(SF_GENL_COMP_TMU);
}
