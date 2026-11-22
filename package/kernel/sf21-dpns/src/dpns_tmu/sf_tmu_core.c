#include <linux/of_device.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/debugfs.h>
#include <linux/io.h>

#include "dpns_common.h"
#include "sf_tmu_genl.h"
#include "sf_tmu.h"

#define DRV_NAME 			"sf_tmu"

TMU_t *g_tmu = NULL;
/*
                   SCHEDULER
                   +------+
                   |      |
                   |      |
                   |      |
                   |      |
   PORT QUEUES     |      |
     +----+        |      |
     | Q0 +------->|      |
     +----+        |      |
     +----+        | SCH0 |
     | Q1 +------->|      |
     +----+        |      |
     +----+        |      |
     | Q2 +------->|      |      SHAPER     +------+
     +----+        |      |      +----+     |      |
     +----+        |      +----->|SHP1+---->|      |
     | Q3 +------->|      |      +----+     |      |
     +----+        |      |                 |      |
                   +------+                 |      |
                                            |      |
     +----+                      +----+     |      |
     | Q4 +--------------------->|SHP2+---->|      |      +----+
     +----+                      +----+     | SCH1 +----->|SHP0+-----> OUTPUT
     +----+                      +----+     |      |      +----+
     | Q5 +--------------------->|SHP3+---->|      |
     +----+                      +----+     |      |
     +----+                      +----+     |      |
     | Q6 +--------------------->|SHP4+---->|      |
     +----+                      +----+     |      |
     +----+                      +----+     |      |
     | Q7 +--------------------->|SHP5+---->|      |
     +----+                      +----+     |      |
                                            +------+
*/

/*
 * ETH WAN/LAN port 0 ~ 5
 *
 * WLAN port 6 ~ 9
 *      port 6: host
 *      port 7: WLAN 2_4G
 *      port 8: WLAN 5G
 *      port 9: WLAN 5G
 */
TMU_t *tmu_get(void)
{
	return g_tmu;
}

u32 tmu_rm32(TMU_t *tmu, u32 reg, u32 mask, u32 shift)
{
	u32 t;

	t = sf_readl(tmu, reg);
	t &= mask;
	t >>= shift;

	return t;
}

void tmu_rmw32(TMU_t *tmu, u32 reg, u32 mask, u32 shift, u32 val)
{
	u32 t;

	val <<= shift;
	val &= mask;
	t = sf_readl(tmu, reg);
	t &= ~mask;
	t |= val;

	sf_writel(tmu, reg, t);
}

static int is_valid_port_idx(TMU_t *tmu, u32 port)
{
	if (!tmu)
		return 0;

	if (port >= TMU_MAX_PORT_CNT)
		return 0;

	return 1;
}

static int is_valid_queue_idx(u32 q)
{
	if (q >= QUE_MAX_NUM_PER_PORT)
		return 0;

	return 1;
}

static int is_valid_sched_idx(TMU_t *tmu, u32 sched)
{
	if (!tmu)
		return 0;

	if (sched >= QUE_SCH_NUM_PER_PORT)
		return 0;

	return 1;
}

static int is_valid_shaper_idx(TMU_t *tmu, u32 shaper)
{
	if (!tmu)
		return 0;

	if (shaper >= QUE_SHAPER_NUM_PER_PORT)
		return 0;

	return 1;
}

int tmu_port_readl(TMU_t *tmu, u32 port, u32 reg, u32 *val)
{
	if (!is_valid_port_idx(tmu, port))
		return -EINVAL;

	*val = sf_readl(tmu, TMU_PORT_BASE(port) + reg);

	return 0;
}

int tmu_port_writel(TMU_t *tmu, u32 port, u32 reg, u32 val)
{
	if (!is_valid_port_idx(tmu, port))
		return -EINVAL;

	sf_writel(tmu, TMU_PORT_BASE(port) + reg, val);

	return 0;
}

int tmu_port_rm32(TMU_t *tmu, u32 port, u32 reg, u32 mask, u32 shift, u32 *val)
{
	if (!is_valid_port_idx(g_tmu, port))
		return -EINVAL;

	*val = tmu_rm32(tmu, TMU_PORT_BASE(port) + reg, mask, shift);

	return 0;
}

int tmu_port_rmw32(TMU_t *tmu, u32 port, u32 reg, u32 mask, u32 shift, u32 val)
{
	if (!is_valid_port_idx(tmu, port))
		return -EINVAL;

	tmu_rmw32(tmu, TMU_PORT_BASE(port) + reg, mask, shift, val);

	return 0;
}

int tmu_queue_readl(TMU_t *tmu, u32 port, u32 queue, u32 reg, u32 *val)
{
	if (!is_valid_queue_idx(queue))
		return -EINVAL;

	return tmu_port_readl(tmu, port, TMU_QUEUE_BASE(queue) + reg, val);
}

int tmu_queue_writel(TMU_t *tmu, u32 port, u32 queue, u32 reg, u32 val)
{
	if (!is_valid_queue_idx(queue))
		return -EINVAL;

	return tmu_port_writel(tmu, port, TMU_QUEUE_BASE(queue) + reg, val);
}

int tmu_queue_rm32(TMU_t *tmu, u32 port, u32 queue, u32 reg, u32 mask, u32 shift, u32 *val)
{
	if (!is_valid_queue_idx(queue))
		return -EINVAL;

	return tmu_port_rm32(tmu, port, TMU_QUEUE_BASE(queue) + reg, mask, shift, val);
}

int tmu_queue_rmw32(TMU_t *tmu, u32 port, u32 queue, u32 reg, u32 mask, u32 shift, u32 val)
{
	if (!is_valid_queue_idx(queue))
		return -EINVAL;

	return tmu_port_rmw32(tmu, port, TMU_QUEUE_BASE(queue) + reg, mask, shift, val);
}

int tmu_sched_readl(TMU_t *tmu, u32 port, u32 sched, u32 reg, u32 *val)
{
	if (!is_valid_sched_idx(g_tmu, sched))
		return -EINVAL;

	return tmu_port_readl(tmu, port, TMU_SCHED_BASE(sched) + reg, val);
}

int tmu_sched_writel(TMU_t *tmu, u32 port, u32 sched, u32 reg, u32 val)
{
	if (!is_valid_sched_idx(g_tmu, sched))
		return -EINVAL;

	return tmu_port_writel(tmu, port, TMU_SCHED_BASE(sched) + reg, val);
}

int tmu_sched_rm32(TMU_t *tmu, u32 port, u32 sched, u32 reg, u32 mask, u32 shift, u32 *val)
{
	if (!is_valid_sched_idx(g_tmu, sched))
		return -EINVAL;

	return tmu_port_rm32(tmu, port, TMU_SCHED_BASE(sched) + reg, mask, shift, val);
}

int tmu_sched_rmw32(TMU_t *tmu, u32 port, u32 sched, u32 reg, u32 mask, u32 shift, u32 val)
{
	if (!is_valid_sched_idx(g_tmu, sched))
		return -EINVAL;

	return tmu_port_rmw32(tmu, port, TMU_SCHED_BASE(sched) + reg, mask, shift, val);
}

int tmu_shaper_readl(TMU_t *tmu, u32 port, u32 shaper, u32 reg, u32 *val)
{
	if (!is_valid_shaper_idx(g_tmu, shaper))
		return -EINVAL;

	return tmu_port_readl(tmu, port, TMU_SHAPER_BASE(shaper) + reg, val);
}

int tmu_shaper_writel(TMU_t *tmu, u32 port, u32 shaper, u32 reg, u32 val)
{
	if (!is_valid_shaper_idx(g_tmu, shaper))
		return -EINVAL;

	return tmu_port_writel(tmu, port, TMU_SHAPER_BASE(shaper) + reg, val);
}

int tmu_shaper_rm32(TMU_t *tmu, u32 port, u32 shaper, u32 reg, u32 mask, u32 shift, u32 *val)
{
	if (!is_valid_shaper_idx(g_tmu, shaper))
		return -EINVAL;

	return tmu_port_rm32(tmu, port, TMU_SHAPER_BASE(shaper) + reg, mask, shift, val);
}

int tmu_shaper_rmw32(TMU_t *tmu, u32 port, u32 shaper, u32 reg, u32 mask, u32 shift, u32 val)
{
	if (!is_valid_shaper_idx(g_tmu, shaper))
		return -EINVAL;

	return tmu_port_rmw32(tmu, port, TMU_SHAPER_BASE(shaper) + reg, mask, shift, val);
}

int tmu_qlen_max_get(TMU_t *tmu, u32 port, u32 queue, u32 *val)
{
	return tmu_queue_rm32(tmu, port, queue,
	                      TMU_PORT_QUEUE_CFG0,
	                      TMU_QUEUE_MAX,
	                      TMU_QUEUE_MAX_SHIFT,
	                      val);
}

int tmu_qlen_max_set(TMU_t *tmu, u32 port, u32 queue, u32 val)
{
	return tmu_queue_rmw32(tmu, port, queue,
	                       TMU_PORT_QUEUE_CFG0,
	                       TMU_QUEUE_MAX,
	                       TMU_QUEUE_MAX_SHIFT,
	                       val);
}

int tmu_qlen_min_get(TMU_t *tmu, u32 port, u32 queue, u32 *val)
{
	return tmu_queue_rm32(tmu, port, queue,
	                      TMU_PORT_QUEUE_CFG0,
	                      TMU_QUEUE_MIN,
	                      TMU_QUEUE_MIN_SHIFT,
	                      val);
}

int tmu_qlen_min_set(TMU_t *tmu, u32 port, u32 queue, u32 val)
{
	return tmu_queue_rmw32(tmu, port, queue,
	                       TMU_PORT_QUEUE_CFG0,
	                       TMU_QUEUE_MIN,
	                       TMU_QUEUE_MIN_SHIFT,
	                       val);
}

/* enums are mapped to register value, must keep them the same */
int tmu_queue_type_get(TMU_t *tmu, u32 port, u32 queue, u32 *type)
{
	return tmu_queue_rm32(tmu, port, queue,
	                      TMU_PORT_QUEUE_CFG0,
	                      TMU_DROP_TYPE,
	                      TMU_DROP_TYPE_SHIFT,
	                      type);
}

int tmu_queue_type_set(TMU_t *tmu, u32 port, u32 queue, u32 type)
{
	return tmu_queue_rmw32(tmu, port, queue,
	                       TMU_PORT_QUEUE_CFG0,
	                       TMU_DROP_TYPE,
	                       TMU_DROP_TYPE_SHIFT,
	                       type);
}

int tmu_wred_drop_probs_get(TMU_t *tmu, u32 port, u32 queue, u8 probs[8])
{
	u32 val[2];
	int err;

	if ((err = tmu_queue_readl(tmu, port, queue, TMU_PORT_QUEUE_CFG1, &val[0])))
		return err;

	if ((err = tmu_queue_readl(tmu, port, queue, TMU_PORT_QUEUE_CFG2, &val[1])))
		return err;


	probs[0] = (val[0] & TMU_WRED_HW_PROB_STG0) >> TMU_WRED_HW_PROB_STG0_SHIFT;
	probs[1] = (val[0] & TMU_WRED_HW_PROB_STG1) >> TMU_WRED_HW_PROB_STG1_SHIFT;
	probs[2] = (val[0] & TMU_WRED_HW_PROB_STG2) >> TMU_WRED_HW_PROB_STG2_SHIFT;
	probs[3] = (val[0] & TMU_WRED_HW_PROB_STG3) >> TMU_WRED_HW_PROB_STG3_SHIFT;
	probs[4] = (val[0] & TMU_WRED_HW_PROB_STG4) >> TMU_WRED_HW_PROB_STG4_SHIFT;
	probs[5] = (val[0] & TMU_WRED_HW_PROB_STG5) >> TMU_WRED_HW_PROB_STG5_SHIFT;
	probs[6] = (val[1] & TMU_WRED_HW_PROB_STG6) >> TMU_WRED_HW_PROB_STG6_SHIFT;
	probs[7] = (val[1] & TMU_WRED_HW_PROB_STG7) >> TMU_WRED_HW_PROB_STG7_SHIFT;

	return 0;
}

int tmu_wred_drop_probs_set(TMU_t *tmu, u32 port, u32 queue, u8 probs[8])
{
	u32 val[2] = { 0 };
	int err;

	val[0] |= (probs[0] << TMU_WRED_HW_PROB_STG0_SHIFT) & TMU_WRED_HW_PROB_STG0;
	val[0] |= (probs[1] << TMU_WRED_HW_PROB_STG1_SHIFT) & TMU_WRED_HW_PROB_STG1;
	val[0] |= (probs[2] << TMU_WRED_HW_PROB_STG2_SHIFT) & TMU_WRED_HW_PROB_STG2;
	val[0] |= (probs[3] << TMU_WRED_HW_PROB_STG3_SHIFT) & TMU_WRED_HW_PROB_STG3;
	val[0] |= (probs[4] << TMU_WRED_HW_PROB_STG4_SHIFT) & TMU_WRED_HW_PROB_STG4;
	val[0] |= (probs[5] << TMU_WRED_HW_PROB_STG5_SHIFT) & TMU_WRED_HW_PROB_STG5;
	val[1] |= (probs[6] << TMU_WRED_HW_PROB_STG6_SHIFT) & TMU_WRED_HW_PROB_STG6;
	val[1] |= (probs[7] << TMU_WRED_HW_PROB_STG7_SHIFT) & TMU_WRED_HW_PROB_STG7;

	if ((err = tmu_queue_writel(g_tmu, port, queue, TMU_PORT_QUEUE_CFG1, val[0])))
		return err;

	if ((err = tmu_queue_writel(g_tmu, port, queue, TMU_PORT_QUEUE_CFG2, val[1])))
		return err;

	return 0;
}

int tmu_queue_ptr_get(TMU_t *tmu, u32 port, u32 queue, u32 *head, u32 *tail)
{
	u32 val;
	int err;

	if ((err = tmu_queue_readl(tmu, port, queue, TMU_PORT_QUEUE_STS0, &val)))
		return err;

	if (head)
		*head = (val & TMU_QUEUE_HEAD_PTR) >> TMU_QUEUE_HEAD_PTR_SHIFT;

	if (tail)
		*tail = (val & TMU_QUEUE_TAIL_PTR) >> TMU_QUEUE_TAIL_PTR_SHIFT;

	return 0;
}

int tmu_queue_pkt_cnt_get(TMU_t *tmu, u32 port, u32 queue, u32 *cnt)
{
	return tmu_queue_rm32(tmu, port, queue,
	                      TMU_PORT_QUEUE_STS1,
	                      TMU_QUEUE_PKT_CNT,
	                      TMU_QUEUE_PKT_CNT_SHIFT,
	                      cnt);
}

int tmu_queue_buf_cnt_get(TMU_t *tmu, u32 port, u32 queue, u32 *cnt)
{
	return tmu_queue_rm32(tmu, port, queue,
	                      TMU_PORT_QUEUE_STS2,
	                      TMU_QUEUE_BUF_CNT,
	                      TMU_QUEUE_BUF_CNT_SHIFT,
	                      cnt);
}

int tmu_queue_buf_max_get(TMU_t *tmu, u32 port, u32 queue, u32 *cnt)
{
	return tmu_queue_rm32(tmu, port, queue,
	                      TMU_PORT_QUEUE_CFG3,
	                      TMU_QUEUE_BUF_MAX,
	                      TMU_QUEUE_BUF_MAX_SHIFT,
	                      cnt);
}

int tmu_queue_buf_max_set(TMU_t *tmu, u32 port, u32 queue, u32 cnt)
{
	return tmu_queue_rmw32(tmu, port, queue,
	                       TMU_PORT_QUEUE_CFG3,
	                       TMU_QUEUE_BUF_MAX,
	                       TMU_QUEUE_BUF_MAX_SHIFT,
	                       cnt);
}

int tmu_sched_type_get(TMU_t *tmu, u32 port, u32 sched, u32 *type)
{
	return tmu_sched_rm32(tmu, port, sched,
	                      TMU_SCH_CTRL,
	                      TMU_SCH_ALGO,
	                      TMU_SCH_ALGO_SHIFT,
	                      type);
}

int tmu_sched_type_set(TMU_t *tmu, u32 port, u32 sched, u32 type)
{
	return tmu_sched_rmw32(tmu, port, sched,
	                       TMU_SCH_CTRL,
	                       TMU_SCH_ALGO,
	                       TMU_SCH_ALGO_SHIFT,
	                       type);
}

int tmu_sched_weight_get(TMU_t *tmu, u32 port, u32 sched, u32 queue, u32 *weight)
{
	if (queue >= ARRAY_SIZE(sched_q_weight_regs))
		return -EINVAL;

	return tmu_sched_readl(tmu, port, sched, sched_q_weight_regs[queue], weight);
}

int tmu_sched_weight_set(TMU_t *tmu, u32 port, u32 sched, u32 queue, u32 weight)
{
	if (queue >= ARRAY_SIZE(sched_q_weight_regs))
		return -EINVAL;

	return tmu_sched_writel(tmu, port, sched, sched_q_weight_regs[queue], weight);
}

int tmu_sched_queue_weight_get(TMU_t *tmu, u32 port, u32 sched, u32 weight[8])
{
        int i;

        for (i = 0; i < TMU_SCH_Q_WEIGHT_CNT; i++) {
                tmu_sched_weight_get(tmu, port, sched, i, &weight[i]);
        }

        return 0;
}

int tmu_sched_queue_weight_set(TMU_t *tmu, u32 port, u32 sched, u32 weight[8])
{
        int i;

        for (i = 0; i < TMU_SCH_Q_WEIGHT_CNT; i++) {
                tmu_sched_weight_set(tmu, port, sched, i, weight[i]);
        }

        return 0;
}

int tmu_sched_queue_map_get(TMU_t *tmu, u32 port, u32 sched, u32 maps[8])
{
	u32 val[2] = { 0 };
	int err;

	if ((err = tmu_sched_readl(tmu, port, sched, TMU_SCH_QUEUE_ALLOC0, &val[0])))
		return err;

	if ((err = tmu_sched_readl(tmu, port, sched, TMU_SCH_QUEUE_ALLOC1, &val[1])))
		return err;

	maps[0] = (val[0] & TMU_SCH_Q0_ALLOC) >> TMU_SCH_Q0_ALLOC_SHIFT;
	maps[1] = (val[0] & TMU_SCH_Q1_ALLOC) >> TMU_SCH_Q1_ALLOC_SHIFT;
	maps[2] = (val[0] & TMU_SCH_Q2_ALLOC) >> TMU_SCH_Q2_ALLOC_SHIFT;
	maps[3] = (val[0] & TMU_SCH_Q3_ALLOC) >> TMU_SCH_Q3_ALLOC_SHIFT;
	maps[4] = (val[1] & TMU_SCH_Q4_ALLOC) >> TMU_SCH_Q4_ALLOC_SHIFT;
	maps[5] = (val[1] & TMU_SCH_Q5_ALLOC) >> TMU_SCH_Q5_ALLOC_SHIFT;
	maps[6] = (val[1] & TMU_SCH_Q6_ALLOC) >> TMU_SCH_Q6_ALLOC_SHIFT;
	maps[7] = (val[1] & TMU_SCH_Q7_ALLOC) >> TMU_SCH_Q7_ALLOC_SHIFT;

	return 0;
}

int tmu_sched_queue_map_set(TMU_t *tmu, u32 port, u32 sched, u32 maps[8])
{
	u32 val[2] = { 0 };
	int err;

	val[0] |= (maps[0] << TMU_SCH_Q0_ALLOC_SHIFT) & TMU_SCH_Q0_ALLOC;
	val[0] |= (maps[1] << TMU_SCH_Q1_ALLOC_SHIFT) & TMU_SCH_Q1_ALLOC;
	val[0] |= (maps[2] << TMU_SCH_Q2_ALLOC_SHIFT) & TMU_SCH_Q2_ALLOC;
	val[0] |= (maps[3] << TMU_SCH_Q3_ALLOC_SHIFT) & TMU_SCH_Q3_ALLOC;
	val[1] |= (maps[4] << TMU_SCH_Q4_ALLOC_SHIFT) & TMU_SCH_Q4_ALLOC;
	val[1] |= (maps[5] << TMU_SCH_Q5_ALLOC_SHIFT) & TMU_SCH_Q5_ALLOC;
	val[1] |= (maps[6] << TMU_SCH_Q6_ALLOC_SHIFT) & TMU_SCH_Q6_ALLOC;
	val[1] |= (maps[7] << TMU_SCH_Q7_ALLOC_SHIFT) & TMU_SCH_Q7_ALLOC;

	if ((err = tmu_sched_writel(tmu, port, sched, TMU_SCH_QUEUE_ALLOC0, val[0])))
		return err;

	if ((err = tmu_sched_writel(tmu, port, sched, TMU_SCH_QUEUE_ALLOC1, val[1])))
		return err;

	return 0;
}

int tmu_sched_bitrate_mode_get(TMU_t *tmu, u32 port, u32 sched, u32 *mode)
{
	return tmu_sched_readl(tmu, port, sched, TMU_SCH_BIT_RATE, mode);
}

int tmu_sched_bitrate_mode_set(TMU_t *tmu, u32 port, u32 sched, u32 mode)
{
	return tmu_sched_writel(tmu, port, sched, TMU_SCH_BIT_RATE, mode);
}

int tmu_sched0_pos_get(TMU_t *tmu, u32 port, u32 *pos)
{
	return tmu_sched_rm32(tmu, port, 0,
	                      TMU_SCH0_POS,
	                      TMU_SCH0_POS_MASK,
	                      TMU_SCH0_POS_SHIFT,
	                      pos);
}

int tmu_sched0_pos_set(TMU_t *tmu, u32 port, u32 pos)
{
	return tmu_sched_rmw32(tmu, port, 0,
	                       TMU_SCH0_POS,
	                       TMU_SCH0_POS_MASK,
	                       TMU_SCH0_POS_SHIFT,
	                       pos);
}

int tmu_shaper_is_enabled(TMU_t *tmu, u32 port, u32 shaper)
{
	u32 val = 0;

	if (tmu_shaper_rm32(tmu, port, shaper,
	                    TMU_SHP_CTRL,
	                    TMU_SHP_EN,
	                    TMU_SHP_EN_SHIFT,
	                    &val)) {
		TMU_DBG(ERR_LV, "tmu_shaper_rm32(): failed\n");
		return 0;
	}

	return val;
}

/* disable shaper will clear credit */
int tmu_shaper_enable(TMU_t *tmu, u32 port, u32 shaper, u32 enable)
{
	enable = !!enable;

	return tmu_shaper_rmw32(tmu, port, shaper,
	                        TMU_SHP_CTRL,
	                        TMU_SHP_EN,
	                        TMU_SHP_EN_SHIFT,
	                        enable);
}

/* max thoughput = sys_clk / 2 ^ (credit_div + 1 ) * weight
 *
 * example:
 * 	sysclk = 600 MHz
 *	credit_rate_div = 6
 *	credit_weight = 200 Byte (TMU_SHP_WEIGHT_INT)
 *
 *	max thourghput = (600000000 / 2 ^ (6 + 1)) * 200 = 937.5MBps
 *
 * configurable throughput range:
 *	max: (600000000 * / 2 ^ (0 + 1)) * 255Byte ~= 75GBps
 *	min: (600000000 * / 2 ^ (15 + 1)) * 12Byte ~= 2.2Bps
 *
 */
int tmu_shaper_credit_rate_get(TMU_t *tmu, u32 port, u32 shaper, u32 *div)
{
	return tmu_shaper_rm32(tmu, port, shaper,
	                       TMU_SHP_CTRL,
	                       TMU_SHP_CLK_DIV,
	                       TMU_SHP_CLK_DIV_SHIFT,
	                       div);
}

int tmu_shaper_credit_rate_set(TMU_t *tmu, u32 port, u32 shaper, u32 div)
{
	return tmu_shaper_rmw32(tmu, port, shaper,
	                        TMU_SHP_CTRL,
	                        TMU_SHP_CLK_DIV,
	                        TMU_SHP_CLK_DIV_SHIFT,
	                        div);
}

int tmu_shaper_credit_int_weight_get(TMU_t *tmu, u32 port, u32 shaper, u32 *weight)
{
	return tmu_shaper_rm32(tmu, port, shaper,
	                       TMU_SHP_WEIGHT,
	                       TMU_SHP_WEIGHT_INT_MASK,
	                       TMU_SHP_WEIGHT_INT_SHIFT,
	                       weight);
}

int tmu_shaper_credit_int_weight_set(TMU_t *tmu, u32 port, u32 shaper, u32 weight)
{
	return tmu_shaper_rmw32(tmu, port, shaper,
	                        TMU_SHP_WEIGHT,
	                        TMU_SHP_WEIGHT_INT_MASK,
	                        TMU_SHP_WEIGHT_INT_SHIFT,
	                        weight);
}

int tmu_shaper_credit_frac_weight_get(TMU_t *tmu, u32 port, u32 shaper, u32 *weight)
{
        return tmu_shaper_rm32(tmu, port, shaper,
                               TMU_SHP_WEIGHT,
                               TMU_SHP_WEIGHT_FRAC_MASK,
                               TMU_SHP_WEIGHT_FRAC_SHIFT,
                               weight);
}

int tmu_shaper_credit_frac_weight_set(TMU_t *tmu, u32 port, u32 shaper, u32 weight)
{
        return tmu_shaper_rmw32(tmu, port, shaper,
                                TMU_SHP_WEIGHT,
                                TMU_SHP_WEIGHT_FRAC_MASK,
                                TMU_SHP_WEIGHT_FRAC_SHIFT,
                                weight);
}

int tmu_shaper_max_credit_get(TMU_t *tmu, u32 port, u32 shaper, u32 *val)
{
	return tmu_shaper_rm32(tmu, port, shaper,
	                       TMU_SHP_MAX_CREDIT,
	                       TMU_SHP_MAX_CREDIT_MASK,
	                       TMU_SHP_MAX_CREDIT_SHIFT,
	                       val);
}

int tmu_shaper_max_credit_set(TMU_t *tmu, u32 port, u32 shaper, u32 val)
{
	return tmu_shaper_rmw32(tmu, port, shaper,
	                        TMU_SHP_MAX_CREDIT,
	                        TMU_SHP_MAX_CREDIT_MASK,
	                        TMU_SHP_MAX_CREDIT_SHIFT,
	                        val);
}

int tmu_shaper_bitrate_mode_get(TMU_t *tmu, u32 port, u32 shaper, u32 *mode)
{
	return tmu_shaper_rm32(tmu, port, shaper,
	                       TMU_SHP_CTRL2,
	                       TMU_SHP_BIT_RATE,
	                       TMU_SHP_BIT_RATE_SHIFT,
	                       mode);
}

int tmu_shaper_bitrate_mode_set(TMU_t *tmu, u32 port, u32 shaper, u32 mode)
{
	return tmu_shaper_rmw32(tmu, port, shaper,
	                        TMU_SHP_CTRL2,
	                        TMU_SHP_BIT_RATE,
	                        TMU_SHP_BIT_RATE_SHIFT,
	                        mode);
}

int tmu_shaper_pos_get(TMU_t *tmu, u32 port, u32 shaper, u32 *pos)
{
	return tmu_shaper_rm32(tmu, port, shaper,
	                       TMU_SHP_CTRL2,
	                       TMU_SHP_POS,
	                       TMU_SHP_POS_SHIFT,
	                       pos);
}

int tmu_shaper_pos_set(TMU_t *tmu, u32 port, u32 shaper, u32 pos)
{
	return tmu_shaper_rmw32(tmu, port, shaper,
	                        TMU_SHP_CTRL2,
	                        TMU_SHP_POS,
	                        TMU_SHP_POS_SHIFT,
	                        pos);
}

/* if set, when shaper internal queue is empty, the credit will be cleared  */
int tmu_shaper_credit_clear_get(TMU_t *tmu, u32 port, u32 shaper, u32 *clear)
{
	return tmu_shaper_rm32(tmu, port, shaper,
	                       TMU_SHP_CTRL2,
	                       TMU_SHP_MODE,
	                       TMU_SHP_MODE_SHIFT,
	                       clear);
}

int tmu_shaper_credit_clear_set(TMU_t *tmu, u32 port, u32 shaper, u32 clear)
{
	clear = !!clear;

	return tmu_shaper_rmw32(tmu, port, shaper,
	                        TMU_SHP_CTRL2,
	                        TMU_SHP_MODE,
	                        TMU_SHP_MODE_SHIFT,
	                        clear);
}

int tmu_shaper_min_credit_get(TMU_t *tmu, u32 port, u32 shaper, u32 *val)
{
	return tmu_shaper_rm32(tmu, port, shaper,
	                       TMU_SHP_MIN_CREDIT,
	                       TMU_SHP_MIN_CREDIT_MASK,
	                       TMU_SHP_MIN_CREDIT_SHIFT,
	                       val);
}

int tmu_shaper_min_credit_set(TMU_t *tmu, u32 port, u32 shaper, u32 val)
{
	return tmu_shaper_rmw32(tmu, port, shaper,
	                        TMU_SHP_MIN_CREDIT,
	                        TMU_SHP_MIN_CREDIT_MASK,
	                        TMU_SHP_MIN_CREDIT_SHIFT,
	                        val);
}

int tmu_shaper_is_working(TMU_t *tmu, u32 port, u32 shaper)
{
	u32 val = 0;
	int err;

	if ((err = tmu_shaper_rm32(tmu, port, shaper,
	                           TMU_SHP_STATUS,
	                           TMU_SHP_CURR_STATUS,
	                           TMU_SHP_CURR_STATUS_SHIFT,
	                           &val))) {
		TMU_DBG(ERR_LV, "tmu_shaper_rm32(): failed\n");
		return 0;
	}

	return val;
}

int tmu_shaper_credit_get(TMU_t *tmu, u32 port, u32 shaper, u32 *credit)
{
	return tmu_shaper_rm32(tmu, port, shaper,
	                       TMU_SHP_STATUS,
	                       TMU_SHP_CREDIT_CNTR,
	                       TMU_SHP_CREDIT_CNTR_SHIFT,
	                       credit);
}

int tdq_ctrl_is_configurable(TMU_t *tmu, u32 port)
{
	u32 val = 0;
	int err;

	if ((err = tmu_port_rm32(tmu, port,
	                         TMU_TDQ_CTRL,
	                         TMU_TDQ_ALLOW_CFG,
	                         TMU_TDQ_ALLOW_CFG_SHIFT,
	                         &val))) {
		TMU_DBG(ERR_LV, "tmu_port_rm32() failed\n");
		return 0;
	}

	return val;
}

static int tmu_deinit(TMU_t *tmu)
{
	int err = 0;

#ifdef CONFIG_SIFLOWER_DPNS_TMU_GENL
        if ((err = tmu_genl_deinit()))
                TMU_DBG(ERR_LV, "tmu_genl_deinit() failed\n");
#endif

#ifdef CONFIG_SIFLOWER_DPNS_TMU_DEBUGFS
	if ((err = tmu_debugfs_deinit(tmu)))
		return err;
#endif

	return err;
}

void tmu_port_queue_cfg(TMU_t *tmu, u32 port)
{
	int comp;

	for (comp = 0; comp < QUE_MAX_NUM_PER_PORT; comp++) {
#ifdef CONFIG_DPNS_THROUGHPUT_WIFI_BEST
		if (port == DPNS_HOST_PORT)
			tmu_queue_writel(tmu, port, comp, TMU_PORT_QUEUE_CFG0, 0x0003ff01);
		else
			tmu_queue_writel(tmu, port, comp, TMU_PORT_QUEUE_CFG0, 0x00011f01);
#else
			tmu_queue_writel(tmu, port, comp, TMU_PORT_QUEUE_CFG0, 0x00011f00);
#endif

		tmu_queue_writel(tmu, port, comp, TMU_PORT_QUEUE_CFG1, 0x00000000);
		tmu_queue_writel(tmu, port, comp, TMU_PORT_QUEUE_CFG2, 0x00000000);
		tmu_queue_writel(tmu, port, comp, TMU_PORT_QUEUE_STS0, 0x00000000);
		tmu_queue_writel(tmu, port, comp, TMU_PORT_QUEUE_STS1, 0x00000000);
		tmu_queue_writel(tmu, port, comp, TMU_PORT_QUEUE_STS2, 0x00000000);
#ifdef CONFIG_DPNS_THROUGHPUT_BALANCE
		tmu_queue_writel(tmu, port, comp, TMU_PORT_QUEUE_CFG3, 0x000005ee);
#else
		tmu_queue_writel(tmu, port, comp, TMU_PORT_QUEUE_CFG3, 0x000005b0);
#endif
	}
}

void tmu_port_sched_cfg(TMU_t *tmu, u32 port)
{
	int comp;
	for (comp = 0; comp < QUE_SCH_NUM_PER_PORT; comp++) {
		tmu_sched_writel(tmu, port, comp, TMU_SCH_CTRL,      0x00000000);
		tmu_sched_writel(tmu, port, comp, TMU_SCH_Q0_WEIGHT, 0x00000000);
		tmu_sched_writel(tmu, port, comp, TMU_SCH_Q1_WEIGHT, 0x00000000);
		tmu_sched_writel(tmu, port, comp, TMU_SCH_Q2_WEIGHT, 0x00000000);
		tmu_sched_writel(tmu, port, comp, TMU_SCH_Q3_WEIGHT, 0x00000000);
		tmu_sched_writel(tmu, port, comp, TMU_SCH_Q4_WEIGHT, 0x00000000);
		tmu_sched_writel(tmu, port, comp, TMU_SCH_Q5_WEIGHT, 0x00000000);
		tmu_sched_writel(tmu, port, comp, TMU_SCH_Q6_WEIGHT, 0x00000000);
		tmu_sched_writel(tmu, port, comp, TMU_SCH_Q7_WEIGHT, 0x00000000);

		switch (comp) {
		case 0:
			tmu_sched_writel(tmu, port, comp, TMU_SCH_QUEUE_ALLOC0, 0x03020100);
			tmu_sched_writel(tmu, port, comp, TMU_SCH_QUEUE_ALLOC1, 0x08080808);
			break;

		case 1:
			tmu_sched_writel(tmu, port, comp, TMU_SCH_QUEUE_ALLOC0, 0x06050400);
			tmu_sched_writel(tmu, port, comp, TMU_SCH_QUEUE_ALLOC1, 0x08080807);
			break;

		default:
			break;
		}

		tmu_sched_writel(tmu, port, comp, TMU_SCH_BIT_RATE, 0x00000000);

		if (comp == 0)
			tmu_sched_writel(tmu, port, comp, TMU_SCH0_POS, 0x00000000);
	}
}

void tmu_port_shaper_cfg(TMU_t *tmu, u32 port)
{
	int comp;
	for (comp = 0; comp < QUE_SHAPER_NUM_PER_PORT; comp++) {
		tmu_shaper_writel(tmu, port, comp, TMU_SHP_CTRL,       0x00000000);
		tmu_shaper_writel(tmu, port, comp, TMU_SHP_WEIGHT,     0x00000000);
		tmu_shaper_writel(tmu, port, comp, TMU_SHP_CTRL2,      0x00000000);
		tmu_shaper_writel(tmu, port, comp, TMU_SHP_MIN_CREDIT, 0x0003ff00);
		tmu_shaper_writel(tmu, port, comp, TMU_SHP_MAX_CREDIT, 0x00000400);
		tmu_shaper_rmw32(tmu, port, comp, TMU_SHP_CTRL2, TMU_SHP_POS, TMU_SHP_POS_SHIFT, comp);
	}
}

void _tmu_reset(TMU_t *tmu, u32 port)
{
	tmu_port_queue_cfg(tmu, port);
	tmu_port_sched_cfg(tmu, port);
	tmu_port_shaper_cfg(tmu, port);

	// Cause tmu shaper rate limit not include pkt preamble(8byte)/IFG(12byte)/FCS(4Byte)
	// so config 24 byte here
	tmu_port_writel(tmu, port, TMU_TDQ_IFG, 0x00000018);

	if (tdq_ctrl_is_configurable(tmu, port))
		tmu_port_writel(tmu, port, TMU_TDQ_CTRL, 0x0000002f);
}

#ifdef CONFIG_DPNS_THROUGHPUT_BALANCE
extern int lif_speed_limit(u32 mbps);
#endif
int tmu_reset(TMU_t *tmu)
{
	int port;

	sf_writel(tmu, TMU_CTRL, 0x00000006);
	sf_writel(tmu, TMU_LLM_FIFO_CTRL0, 0x07fe07ff);
	sf_writel(tmu, TMU_LLM_FIFO_CTRL1, 0x00280024);
#ifdef CONFIG_DPNS_THROUGHPUT_WIFI_BEST
	sf_writel(tmu, TMU_BUF_THR0, 0x0);
	sf_writel(tmu, TMU_RD_CLR_EN, 0x00111111);
	sf_writel(tmu, PORT_CNT_NUM(DPNS_HOST_PORT), 0x320);

	for (port = 0; port < DPNS_HOST_PORT; port++) {
		sf_writel(tmu, PORT_CNT_NUM(port), 0);
	}
#endif

	for (port = 0; port < TMU_MAX_PORT_CNT; port++) {
		_tmu_reset(tmu, port);
	}

#ifdef CONFIG_DPNS_THROUGHPUT_BALANCE
	lif_speed_limit(2000);
#endif
	return 0;
}

int dpns_tmu_probe(struct platform_device *pdev)
{
	COMMON_t * common_priv = platform_get_drvdata(pdev);
	TMU_t* priv = NULL;
	int err;

	priv = devm_kzalloc(&pdev->dev, sizeof(TMU_t), GFP_KERNEL);
	if (priv == NULL)
		return -ENOMEM;

	g_tmu = priv;
	common_priv->tmu_priv = priv;
	priv->cpriv = common_priv;

	/* hw io resource */
	priv->iobase = common_priv->iobase;

	if ((err= tmu_reset(priv)))
		goto err_free;

#ifdef CONFIG_SIFLOWER_DPNS_TMU_GENL
        if ((err= tmu_genl_init()))
                TMU_DBG(DBG_LV, "tmu_genl_init() failed\n");
#endif

#ifdef CONFIG_SIFLOWER_DPNS_TMU_DEBUGFS
	if ((err= tmu_debugfs_init(priv)))
		goto err_free;
#endif

	printk("End %s\n", __func__);

	return err;

err_free:
	g_tmu = NULL;

	return err;
}
EXPORT_SYMBOL(dpns_tmu_probe);

void dpns_tmu_remove(struct platform_device *pdev)
{
	COMMON_t * common_priv = platform_get_drvdata(pdev);
	TMU_t* priv = common_priv->tmu_priv;

	if ((0 != tmu_deinit(priv)))
		TMU_DBG(ERR_LV, "tmu_deinit() failed\n");

	common_priv->tmu_priv = NULL;
	g_tmu = NULL;
	printk("End %s\n", __func__);
}
EXPORT_SYMBOL(dpns_tmu_remove);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("0xc0cafe");
MODULE_DESCRIPTION("DPNS TMU Control Interface");
