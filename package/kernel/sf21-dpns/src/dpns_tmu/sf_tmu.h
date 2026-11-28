#ifndef __SF_TMU_H__
#define __SF_TMU_H__

#include "dpns_common.h"
#include "sf_tmu_regs.h"

#define TMU_MAX_PORT_CNT 10
#define QUE_MAX_NUM_PER_PORT 8
#define QUE_SHAPER_NUM_PER_PORT 6
#define QUE_SCH_NUM_PER_PORT 2

enum TMU_QUEUE_TYPE {
	TMU_Q_MIX_TAIL_DROP = 0,
	TMU_Q_TAIL_DROP,
	TMU_Q_WRED,
	TMU_Q_BUF_CNT_TAIL_DROP,
	NUM_TMU_QUEUE_TYPES,
};

enum TMU_SCHED_ALG {
	TMU_SCHED_PQ = 0,
	TMU_SCHED_WFQ,
	TMU_SCHED_DWRR,
	TMU_SCHED_RR,
	TMU_SCHED_WRR,
	NUM_TMU_SCEHD_ALGS,
};

enum TMU_BITRATE_MODE {
	TMU_BITRATE_PKTLEN = 0,
	TMU_BITRATE_PKTCNT,
	NUM_TMU_BITRATE_MODES,
};

static const u8 sched_q_weight_regs[] = {
	TMU_SCH_Q0_WEIGHT,
	TMU_SCH_Q1_WEIGHT,
	TMU_SCH_Q2_WEIGHT,
	TMU_SCH_Q3_WEIGHT,
	TMU_SCH_Q4_WEIGHT,
	TMU_SCH_Q5_WEIGHT,
	TMU_SCH_Q6_WEIGHT,
	TMU_SCH_Q7_WEIGHT,
};

// private
TMU_t *tmu_get(void);

int tmu_reset(TMU_t *tmu);
void _tmu_reset(TMU_t *tmu, u32 port);

u32 tmu_rm32(TMU_t *tmu, u32 reg, u32 mask, u32 shift);
void tmu_rmw32(TMU_t *tmu, u32 reg, u32 mask, u32 shift, u32 val);

/* r/m/w functions
 * return 0 on success, otherwise error number is returned
 */
int tmu_port_readl(TMU_t *tmu, u32 port, u32 reg, u32 *val);
int tmu_port_writel(TMU_t *tmu, u32 port, u32 reg, u32 val);
int tmu_port_rm32(TMU_t *tmu, u32 port, u32 reg, u32 mask, u32 shift, u32 *val);
int tmu_port_rmw32(TMU_t *tmu, u32 port, u32 reg, u32 mask, u32 shift, u32 val);

int tmu_queue_readl(TMU_t *tmu, u32 port, u32 queue, u32 reg, u32 *val);
int tmu_queue_writel(TMU_t *tmu, u32 port, u32 queue, u32 reg, u32 val);
int tmu_queue_rm32(TMU_t *tmu, u32 port, u32 queue, u32 reg, u32 mask, u32 shift, u32 *val);
int tmu_queue_rmw32(TMU_t *tmu, u32 port, u32 queue, u32 reg, u32 mask, u32 shift, u32 val);

int tmu_sched_readl(TMU_t *tmu, u32 port, u32 sched, u32 reg, u32 *val);
int tmu_sched_writel(TMU_t *tmu, u32 port, u32 sched, u32 reg, u32 val);
int tmu_sched_rm32(TMU_t *tmu, u32 port, u32 sched, u32 reg, u32 mask, u32 shift, u32 *val);
int tmu_sched_rmw32(TMU_t *tmu, u32 port, u32 sched, u32 reg, u32 mask, u32 shift, u32 val);

int tmu_shaper_readl(TMU_t *tmu, u32 port, u32 shaper, u32 reg, u32 *val);
int tmu_shaper_writel(TMU_t *tmu, u32 port, u32 shaper, u32 reg, u32 val);
int tmu_shaper_rm32(TMU_t *tmu, u32 port, u32 shaper, u32 reg, u32 mask, u32 shift, u32 *val);
int tmu_shaper_rmw32(TMU_t *tmu, u32 port, u32 shaper, u32 reg, u32 mask, u32 shift, u32 val);

int tmu_qlen_max_get(TMU_t *tmu, u32 port, u32 queue, u32 *val);
int tmu_qlen_max_set(TMU_t *tmu, u32 port, u32 queue, u32 val);
int tmu_qlen_min_get(TMU_t *tmu, u32 port, u32 queue, u32 *val);
int tmu_qlen_min_set(TMU_t *tmu, u32 port, u32 queue, u32 val);

int tmu_queue_type_get(TMU_t *tmu, u32 port, u32 queue, u32 *type);
int tmu_queue_type_set(TMU_t *tmu, u32 port, u32 queue, u32 type);

int tmu_wred_drop_probs_get(TMU_t *tmu, u32 port, u32 queue, u8 probs[8]);
int tmu_wred_drop_probs_set(TMU_t *tmu, u32 port, u32 queue, u8 probs[8]);

int tmu_queue_ptr_get(TMU_t *tmu, u32 port, u32 queue, u32 *head, u32 *tail);
int tmu_queue_pkt_cnt_get(TMU_t *tmu, u32 port, u32 queue, u32 *cnt);
int tmu_queue_buf_cnt_get(TMU_t *tmu, u32 port, u32 queue, u32 *cnt);

int tmu_queue_buf_max_get(TMU_t *tmu, u32 port, u32 queue, u32 *cnt);
int tmu_queue_buf_max_set(TMU_t *tmu, u32 port, u32 queue, u32 cnt);

int tmu_sched_type_get(TMU_t *tmu, u32 port, u32 sched, u32 *type);
int tmu_sched_type_set(TMU_t *tmu, u32 port, u32 sched, u32 type);

int tmu_sched_weight_get(TMU_t *tmu, u32 port, u32 sched, u32 queue, u32 *weight);
int tmu_sched_weight_set(TMU_t *tmu, u32 port, u32 sched, u32 queue, u32 weight);

int tmu_sched_queue_weight_get(TMU_t *tmu, u32 port, u32 sched, u32 weight[8]);
int tmu_sched_queue_weight_set(TMU_t *tmu, u32 port, u32 sched, u32 weight[8]);

int tmu_sched_queue_map_get(TMU_t *tmu, u32 port, u32 sched, u32 maps[8]);
int tmu_sched_queue_map_set(TMU_t *tmu, u32 port, u32 sched, u32 maps[8]);

int tmu_sched_bitrate_mode_get(TMU_t *tmu, u32 port, u32 sched, u32 *mode);
int tmu_sched_bitrate_mode_set(TMU_t *tmu, u32 port, u32 sched, u32 mode);

int tmu_sched0_pos_get(TMU_t *tmu, u32 port, u32 *pos);
int tmu_sched0_pos_set(TMU_t *tmu, u32 port, u32 pos);

int tmu_shaper_is_enabled(TMU_t *tmu, u32 port, u32 shaper);
int tmu_shaper_enable(TMU_t *tmu, u32 port, u32 shaper, u32 enable);

int tmu_shaper_credit_rate_get(TMU_t *tmu, u32 port, u32 shaper, u32 *div);
int tmu_shaper_credit_rate_set(TMU_t *tmu, u32 port, u32 shaper, u32 div); // value of @rate is not confirmed

int tmu_shaper_credit_int_weight_get(TMU_t *tmu, u32 port, u32 shaper, u32 *weight);
int tmu_shaper_credit_int_weight_set(TMU_t *tmu, u32 port, u32 shaper, u32 weight);

int tmu_shaper_credit_frac_weight_get(TMU_t *tmu, u32 port, u32 shaper, u32 *weight);
int tmu_shaper_credit_frac_weight_set(TMU_t *tmu, u32 port, u32 shaper, u32 weight);

int tmu_shaper_bitrate_mode_get(TMU_t *tmu, u32 port, u32 shaper, u32 *mode);
int tmu_shaper_bitrate_mode_set(TMU_t *tmu, u32 port, u32 shaper, u32 mode);

int tmu_shaper_pos_get(TMU_t *tmu, u32 port, u32 shaper, u32 *pos);
int tmu_shaper_pos_set(TMU_t *tmu, u32 port, u32 shaper, u32 pos);

int tmu_shaper_credit_clear_get(TMU_t *tmu, u32 port, u32 shaper, u32 *clear);
int tmu_shaper_credit_clear_set(TMU_t *tmu, u32 port, u32 shaper, u32 clear);

int tmu_shaper_max_credit_get(TMU_t *tmu, u32 port, u32 shaper, u32 *val);
int tmu_shaper_max_credit_set(TMU_t *tmu, u32 port, u32 shaper, u32 val);

int tmu_shaper_min_credit_get(TMU_t *tmu, u32 port, u32 shaper, u32 *val);
int tmu_shaper_min_credit_set(TMU_t *tmu, u32 port, u32 shaper, u32 val);

int tmu_shaper_is_working(TMU_t *tmu, u32 port, u32 shaper);

int tmu_shaper_credit_get(TMU_t *tmu, u32 port, u32 shaper, u32 *credit);

int tdq_ctrl_is_configurable(TMU_t *tmu, u32 port);

#ifdef CONFIG_SIFLOWER_DPNS_TMU_DEBUGFS
int tmu_debugfs_init(TMU_t *tmu);
int tmu_debugfs_deinit(TMU_t *tmu);
#endif

#ifdef CONFIG_SIFLOWER_DPNS_TMU_GENL
int tmu_genl_init(void);
int tmu_genl_deinit(void);
#endif

#endif /* __SF_TMU_H__ */
