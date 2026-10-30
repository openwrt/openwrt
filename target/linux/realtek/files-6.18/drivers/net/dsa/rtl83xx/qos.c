// SPDX-License-Identifier: GPL-2.0-only

#include <net/dsa.h>
#include <linux/delay.h>
#include <linux/regmap.h>
#include <asm/mach-rtl-otto/mach-rtl-otto.h>

#include "qos.h"
#include "rtl-otto.h"

#define RTL839X_TBL_ACCESS_DATA_2(i)		(0x6120 + (((i) << 2)))

#define RTL838X_SCHED_P_EGR_RATE_CTRL(p)	(0xC008 + (((p) << 7)))

/* QoS */
#define RTL838X_QM_INTPRI2QID_CTRL		(0x5F00)
#define RTL839X_QM_INTPRI2QID_CTRL(q)		(0x1110 + (q << 2))
#define RTL839X_QM_PORT_QNUM(p)			(0x1130 + (((p / 10) << 2)))
#define RTL839X_PRI_SEL_PORT_PRI(p)		(0x10A8 + (((p / 10) << 2)))
#define RTL838X_QM_PKT2CPU_INTPRI_MAP		(0x5F10)
#define RTL839X_QM_PKT2CPU_INTPRI_MAP		(0x1154)
#define RTL838X_PRI_SEL_TBL_CTRL(i)		(0x5FD8 + (((i) << 2)))
#define RTL839X_PRI_SEL_TBL_CTRL(i)		(0x10D0 + (((i) << 2)))
#define RTL838X_QM_PKT2CPU_INTPRI_1		(0x5F08)
#define RTL838X_QM_PKT2CPU_INTPRI_2		(0x5F0C)
#define RTL839X_OAM_CTRL			(0x2100)
#define RTL839X_OAM_PORT_ACT_CTRL(p)		(0x2104 + (((p) << 2)))
#define RTL839X_RMK_PORT_DEI_TAG_CTRL(p)	(0x6A9C + (((p >> 5) << 2)))
#define RTL839X_PRI_SEL_IPRI_REMAP		(0x1080)
#define RTL838X_PRI_SEL_IPRI_REMAP		(0x5F8C)
#define RTL839X_PRI_SEL_DEI2DP_REMAP		(0x10EC)
#define RTL839X_RMK_DEI_CTRL			(0x6AA4)
#define RTL839X_WRED_PORT_THR_CTRL(i)		(0x6084 + ((i) << 2))
#define RTL839X_WRED_QUEUE_THR_CTRL(q, i)	(0x6090 + ((q) * 12) + ((i) << 2))
#define RTL838X_PRI_DSCP_INVLD_CTRL0		(0x5FE8)
#define RTL838X_RMK_IPRI_CTRL			(0xA460)
#define RTL838X_RMK_OPRI_CTRL			(0xA464)
#define RTL838X_SCHED_P_TYPE_CTRL(p)		(0xC04C + (((p) << 7)))
#define RTL838X_SCHED_LB_CTRL(p)		(0xC004 + (((p) << 7)))
#define RTL838X_FC_P_EGR_DROP_CTRL(p)		(0x6B1C + (((p) << 2)))

#define RTL930X_REMAP_DSCP(p)			(0x9B04 + (((p) / 10) * 4))
#define RTL931X_REMAP_DSCP(p)			(0x9034 + (((p) / 10) * 4))
#define RTL93XX_REMAP_DSCP_INTPRI_DSCP_OFFSET(p) \
						(((p) % 10) * 3)
#define RTL93XX_REMAP_DSCP_INTPRI_DSCP_MASK(index) \
						(0x7 << RTL93XX_REMAP_DSCP_INTPRI_DSCP_OFFSET(index))

#define RTL930X_PORT_TBL_IDX_CTRL(port)		(0x9B20 + (((port) / 16) * 4))
#define RTL931X_PORT_TBL_IDX_CTRL(port)		(0x9064 + (((port) / 16) * 4))
#define RTL93XX_PORT_TBL_IDX_CTRL_IDX_OFFSET(port) \
						(((port) & 0xF) << 1)
#define RTL93XX_PORT_TBL_IDX_CTRL_IDX_MASK(port) \
						(0x3 << RTL93XX_PORT_TBL_IDX_CTRL_IDX_OFFSET(port))

#define RTL930X_PRI_SEL_TBL_CTRL(group)		(0x9B28 + ((group) * 4))
#define RTL931X_PRI_SEL_TBL_CTRL(group)		(0x9074 + ((group) * 8))

#define RTL93XX_PRI_SEL_TBL_CTRL_OTAG_MASK	GENMASK(19, 16)
#define RTL93XX_PRI_SEL_TBL_CTRL_ITAG_MASK	GENMASK(15, 12)
#define RTL93XX_PRI_SEL_TBL_CTRL_DSCP_MASK	GENMASK(11, 8)
#define RTL93XX_PRI_SEL_TBL_CTRL_PORT_MASK	GENMASK(3, 0)

/* port: 0-23, index: 0-7 */
#define RTL930X_SCHED_PORT_Q_CTRL_SET0(port, index) \
						(0x3D48 + ((port) * 384) + ((index) * 4))
/* port: 24-27, index: 0-11 */
#define RTL930X_SCHED_PORT_Q_CTRL_SET1(port, index) \
						((0xE860 + ((port) - 24) * 48) + ((index) * 4))
#define RTL930X_SCHED_Q_STRICT			BIT(7)
#define RTL930X_SCHED_Q_WEIGHT			GENMASK(6, 0)
/* port: 0-51, index: 0-7 */
#define RTL931X_SCHED_PORT_Q_CTRL_SET0(port, index) \
						(0x2888 + ((port) << 5) + ((index) * 4))
/* port: 52-55, index: 0-11 */
#define RTL931X_SCHED_PORT_Q_CTRL_SET1(port, index) \
						((0x2F08 + ((port) - 52) * 48) + ((index) * 4))

#define RTL930X_QM_INTPRI2QID_CTRL		(0xA320)
#define RTL931X_QM_INTPRI2QID_CTRL		(0xA9D0)

#define RTL930X_FC_GLB_HI_THR			(0xD880)
#define RTL930X_FC_GLB_LO_THR			(0xD884)
#define RTL930X_FC_GLB_FCOFF_HI_THR		(0xD888)
#define RTL930X_FC_GLB_FCOFF_LO_THR		(0xD88C)
#define RTL930X_FC_GLB_THR_ON_MASK		GENMASK(27, 16)
#define RTL930X_FC_GLB_THR_OFF_MASK		GENMASK(11, 0)

#define DSCP_MAP_MAX 64

enum scheduler_type {
	WEIGHTED_FAIR_QUEUE = 0,
	WEIGHTED_ROUND_ROBIN,
};

static int rtldsa_max_available_queue[] = {0, 1, 2, 3, 4, 5, 6, 7};
static int rtldsa_default_queue_weights[] = {1, 1, 1, 1, 1, 1, 1, 1};
static int dot1p_priority_remapping[] = {0, 1, 2, 3, 4, 5, 6, 7};

static void otto_qos_839x_read_scheduling_table(struct otto_dsa_priv *priv, int port)
{
	u32 cmd = 1 << 9 | /* Execute cmd */
		  0 << 8 | /* Read */
		  0 << 6 | /* Table type 0b00 */
		  (port & 0x3f);
	rtl839x_exec_tbl2_cmd(priv, cmd);
}

static void otto_qos_839x_write_scheduling_table(struct otto_dsa_priv *priv, int port)
{
	u32 cmd = 1 << 9 | /* Execute cmd */
		  1 << 8 | /* Write */
		  0 << 6 | /* Table type 0b00 */
		  (port & 0x3f);
	rtl839x_exec_tbl2_cmd(priv, cmd);
}

static void otto_qos_839x_read_out_q_table(struct otto_dsa_priv *priv, int port)
{
	u32 cmd = 1 << 9 | /* Execute cmd */
		  0 << 8 | /* Read */
		  2 << 6 | /* Table type 0b10 */
		  (port & 0x3f);
	rtl839x_exec_tbl2_cmd(priv, cmd);
}

u32 otto_qos_838x_get_egress_rate(struct otto_dsa_priv *priv, int port)
{
	u32 rate = 0;

	if (port > priv->r->cpu_port)
		return 0;

	regmap_read(priv->map, RTL838X_SCHED_P_EGR_RATE_CTRL(port), &rate);

	return rate & 0x3fff;
}

/* Sets the rate limit, 10MBit/s is equal to a rate value of 625 */
int otto_qos_838x_set_egress_rate(struct otto_dsa_priv *priv, int port, u32 rate)
{
	u32 old_rate = 0;

	if (port > priv->r->cpu_port)
		return -1;

	regmap_read(priv->map, RTL838X_SCHED_P_EGR_RATE_CTRL(port), &old_rate);
	regmap_write(priv->map, RTL838X_SCHED_P_EGR_RATE_CTRL(port), rate);

	return old_rate;
}

/* Sets the rate limit, 10MBit/s is equal to a rate value of 625 */
u32 otto_qos_839x_get_egress_rate(struct otto_dsa_priv *priv, int port)
{
	u32 rate = 0, v = 0;

	if (port >= priv->r->cpu_port)
		return 0;

	mutex_lock(&priv->reg_mutex);

	otto_qos_839x_read_scheduling_table(priv, port);

	regmap_read(priv->map, RTL839X_TBL_ACCESS_DATA_2(7), &rate);
	rate <<= 12;
	regmap_read(priv->map, RTL839X_TBL_ACCESS_DATA_2(8), &v);
	rate |= v >> 20;

	mutex_unlock(&priv->reg_mutex);
	pr_debug("%s: Getting egress rate on port %d is %d\n", __func__, port, rate);

	return rate;
}

/* Sets the rate limit, 10MBit/s is equal to a rate value of 625, returns previous rate */
static int otto_qos_839x_set_egress_rate_locked(struct otto_dsa_priv *priv, int port,
					     u32 rate)
{
	u32 old_rate = 0, v = 0;

	lockdep_assert_held(&priv->reg_mutex);

	pr_debug("%s: Setting egress rate on port %d to %d\n", __func__, port, rate);
	if (port >= priv->r->cpu_port)
		return -1;

	otto_qos_839x_read_scheduling_table(priv, port);

	regmap_read(priv->map, RTL839X_TBL_ACCESS_DATA_2(7), &old_rate);
	old_rate &= 0xff;
	old_rate <<= 12;
	regmap_read(priv->map, RTL839X_TBL_ACCESS_DATA_2(8), &v);
	old_rate |= v >> 20;
	regmap_write_bits(priv->map, RTL839X_TBL_ACCESS_DATA_2(7), 0xff, (rate >> 12) & 0xff);
	regmap_write_bits(priv->map, RTL839X_TBL_ACCESS_DATA_2(8), 0xfffU << 20, rate << 20);

	otto_qos_839x_write_scheduling_table(priv, port);

	return old_rate;
}

int otto_qos_839x_set_egress_rate(struct otto_dsa_priv *priv, int port, u32 rate)
{
	int old_rate;

	mutex_lock(&priv->reg_mutex);
	old_rate = otto_qos_839x_set_egress_rate_locked(priv, port, rate);
	mutex_unlock(&priv->reg_mutex);

	return old_rate;
}

static void otto_qos_838x_setup_prio2queue_matrix(struct otto_dsa_priv *priv, int *min_queues)
{
	u32 v = 0, old = 0;

	regmap_read(priv->map, RTL838X_QM_INTPRI2QID_CTRL, &old);
	pr_info("Current Intprio2queue setting: %08x\n", old);
	for (int i = 0; i < MAX_PRIOS; i++)
		v |= i << (min_queues[i] * 3);
	regmap_write(priv->map, RTL838X_QM_INTPRI2QID_CTRL, v);
}

static void otto_qos_839x_setup_prio2queue_matrix(struct otto_dsa_priv *priv, int *min_queues)
{
	u32 old = 0;

	regmap_read(priv->map, RTL839X_QM_INTPRI2QID_CTRL(0), &old);
	pr_info("Current Intprio2queue setting: %08x\n", old);
	for (int i = 0; i < MAX_PRIOS; i++) {
		int q = min_queues[i];

		regmap_write(priv->map, RTL839X_QM_INTPRI2QID_CTRL(q), i << (q * 3));
	}
}

/* Sets the CPU queue depending on the internal priority of a packet */
static void otto_qos_83xx_setup_prio2queue_cpu_matrix(struct otto_dsa_priv *priv, int reg)
{
	u32 v = 0, old = 0;

	regmap_read(priv->map, reg, &old);
	pr_info("QM_PKT2CPU_INTPRI_MAP: %08x\n", old);
	for (int i = 0; i < MAX_PRIOS; i++)
		v |= rtldsa_max_available_queue[i] << (i * 3);
	regmap_write(priv->map, reg, v);
}

static void otto_qos_838x_setup_default_prio2queue(struct otto_dsa_priv *priv)
{
	otto_qos_838x_setup_prio2queue_matrix(priv, rtldsa_max_available_queue);
	otto_qos_83xx_setup_prio2queue_cpu_matrix(priv, RTL838X_QM_PKT2CPU_INTPRI_MAP);
}

static void otto_qos_839x_setup_default_prio2queue(struct otto_dsa_priv *priv)
{
	otto_qos_839x_setup_prio2queue_matrix(priv, rtldsa_max_available_queue);
	otto_qos_83xx_setup_prio2queue_cpu_matrix(priv, RTL839X_QM_PKT2CPU_INTPRI_MAP);
}

/* Sets the output queue assigned to a port, the port can be the CPU-port */
static void otto_qos_839x_set_egress_queue(struct otto_dsa_priv *priv, int port, int queue)
{
	u32 shift = (port % 10) * 3;
	u32 mask = 0x7 << shift;

	regmap_write_bits(priv->map, RTL839X_QM_PORT_QNUM(port), mask, (queue & 0x7) << shift);
}

/* Sets the priority assigned of an ingress port, the port can be the CPU-port */
static void otto_qos_839x_set_ingress_priority(struct otto_dsa_priv *priv, int port, int priority)
{
	int shift = ((port % 10) * 3);
	u32 val = (u32)priority << shift;

	regmap_write_bits(priv->map, RTL839X_PRI_SEL_PORT_PRI(port), (7 << shift) | val, val);
}

static int otto_qos_839x_get_scheduling_algorithm_locked(struct otto_dsa_priv *priv, int port)
{
	u32 v = 0;

	lockdep_assert_held(&priv->reg_mutex);

	otto_qos_839x_read_scheduling_table(priv, port);
	regmap_read(priv->map, RTL839X_TBL_ACCESS_DATA_2(8), &v);

	if (v & BIT(19))
		return WEIGHTED_ROUND_ROBIN;

	return WEIGHTED_FAIR_QUEUE;
}

static void otto_qos_839x_set_scheduling_algorithm(struct otto_dsa_priv *priv, int port,
						   enum scheduler_type sched)
{
	enum scheduler_type t;
	bool drain;
	u32 v = 0, oam_state = 0, oam_port_state = 0;
	u32 count = 0;
	int i, egress_rate;

	mutex_lock(&priv->reg_mutex);
	t = otto_qos_839x_get_scheduling_algorithm_locked(priv, port);
	drain = sched == WEIGHTED_FAIR_QUEUE && t == WEIGHTED_ROUND_ROBIN &&
		port != priv->r->cpu_port;
	/* Check whether we need to empty the egress queue of that port due to Errata E0014503 */
	if (drain) {
		/* Read Operations, Adminstatrion and Management control register */
		regmap_read(priv->map, RTL839X_OAM_CTRL, &oam_state);

		/* Get current OAM state */
		regmap_read(priv->map, RTL839X_OAM_PORT_ACT_CTRL(port), &oam_port_state);

		/* Disable OAM to block traffice */
		regmap_read(priv->map, RTL839X_OAM_CTRL, &v);
		regmap_write_bits(priv->map, RTL839X_OAM_CTRL, 1, 1);
		regmap_read(priv->map, RTL839X_OAM_CTRL, &v);

		/* Set to trap action OAM forward (bits 1, 2) and OAM Mux Action Drop (bit 0) */
		regmap_write(priv->map, RTL839X_OAM_PORT_ACT_CTRL(port), 0x2);

		/* Set port egress rate to unlimited */
		egress_rate = otto_qos_839x_set_egress_rate_locked(priv, port, 0xFFFFF);

		/* Wait until the egress used page count of that port is 0 */
		i = 0;
		do {
			usleep_range(100, 200);
			otto_qos_839x_read_out_q_table(priv, port);
			regmap_read(priv->map, RTL839X_TBL_ACCESS_DATA_2(6), &count);
			count >>= 20;
			i++;
		} while (i < 3500 && count > 0);
	}

	/* Actually set the scheduling algorithm */
	otto_qos_839x_read_scheduling_table(priv, port);
	regmap_write_bits(priv->map, RTL839X_TBL_ACCESS_DATA_2(8), BIT(19), sched ? BIT(19) : 0);
	otto_qos_839x_write_scheduling_table(priv, port);

	if (drain) {
		/* Restore OAM state to control register */
		regmap_write(priv->map, RTL839X_OAM_CTRL, oam_state);

		/* Restore trap action state */
		regmap_write(priv->map, RTL839X_OAM_PORT_ACT_CTRL(port), oam_port_state);

		/* Restore port egress rate */
		otto_qos_839x_set_egress_rate_locked(priv, port, egress_rate);
	}

	mutex_unlock(&priv->reg_mutex);
}

static void otto_qos_839x_set_scheduling_queue_weights(struct otto_dsa_priv *priv, int port,
						       int *queue_weights)
{
	mutex_lock(&priv->reg_mutex);

	otto_qos_839x_read_scheduling_table(priv, port);

	for (int i = 0; i < 8; i++) {
		int lsb = 48 + i * 10;
		int word = 8 - (lsb >> 5);
		int start_bit = lsb & 31;
		u32 weight = queue_weights[i] & 0x3ff;

		regmap_write_bits(priv->map, RTL839X_TBL_ACCESS_DATA_2(word),
				  0x3ffU << start_bit, weight << start_bit);
		if (start_bit > 22)
			regmap_write_bits(priv->map, RTL839X_TBL_ACCESS_DATA_2(word - 1),
					  0x3ffU >> (32 - start_bit),
					  weight >> (32 - start_bit));
	}

	otto_qos_839x_write_scheduling_table(priv, port);
	mutex_unlock(&priv->reg_mutex);
}

void otto_qos_838x_init(struct otto_dsa_priv *priv)
{
	u32 v = 0;

	pr_info("Setting up RTL838X QoS\n");
	regmap_read(priv->map, RTL838X_PRI_SEL_TBL_CTRL(0), &v);
	pr_info("RTL838X_PRI_SEL_TBL_CTRL(i): %08x\n", v);
	otto_qos_838x_setup_default_prio2queue(priv);

	/* Enable inner (bit 12) and outer (bit 13) priority remapping from DSCP */
	regmap_write_bits(priv->map, RTL838X_PRI_DSCP_INVLD_CTRL0,
			  BIT(12) | BIT(13), BIT(12) | BIT(13));

	/* Set default weight for calculating internal priority, in prio selection group 0
	 * Port based (prio 3), Port outer-tag (4), DSCP (5), Inner Tag (6), Outer Tag (7)
	 */
	v = 3 | (4 << 3) | (5 << 6) | (6 << 9) | (7 << 12);
	regmap_write(priv->map, RTL838X_PRI_SEL_TBL_CTRL(0), v);

	/* Set the inner and outer priority one-to-one to re-marked outer dot1p priority */
	v = 0;
	for (int p = 0; p < 8; p++)
		v |= p << (3 * p);
	regmap_write(priv->map, RTL838X_RMK_OPRI_CTRL, v);
	regmap_write(priv->map, RTL838X_RMK_IPRI_CTRL, v);

	v = 0;
	for (int p = 0; p < 8; p++)
		v |= (dot1p_priority_remapping[p] & 0x7) << (p * 3);
	regmap_write(priv->map, RTL838X_PRI_SEL_IPRI_REMAP, v);

	/* On all ports set scheduler type to WFQ */
	for (int i = 0; i <= priv->r->cpu_port; i++)
		regmap_write(priv->map, RTL838X_SCHED_P_TYPE_CTRL(i), 0);

	/* Enable egress scheduler for CPU-Port */
	regmap_write_bits(priv->map, RTL838X_SCHED_LB_CTRL(priv->r->cpu_port), BIT(8), BIT(8));

	/* Enable egress drop allways on */
	regmap_write_bits(priv->map, RTL838X_FC_P_EGR_DROP_CTRL(priv->r->cpu_port),
			  BIT(11), BIT(11));

	/* Give special trap frames priority 7 (BPDUs) and routing exceptions: */
	regmap_write_bits(priv->map, RTL838X_QM_PKT2CPU_INTPRI_2, (7 << 3) | 7, (7 << 3) | 7);
	/* Give RMA frames priority 7: */
	regmap_write_bits(priv->map, RTL838X_QM_PKT2CPU_INTPRI_1, 7, 7);
}

void otto_qos_839x_init(struct otto_dsa_priv *priv)
{
	u32 v = 0;

	pr_info("Setting up RTL839X QoS\n");
	regmap_read(priv->map, RTL839X_PRI_SEL_TBL_CTRL(0), &v);
	pr_info("RTL839X_PRI_SEL_TBL_CTRL(i): %08x\n", v);
	otto_qos_839x_setup_default_prio2queue(priv);

	for (int port = 0; port <= priv->r->cpu_port; port++)
		otto_qos_839x_set_egress_queue(priv, port, 7);

	for (int port = 0; port <= priv->r->cpu_port; port++) {
		otto_qos_839x_set_ingress_priority(priv, port, 0);
		otto_qos_839x_set_scheduling_algorithm(priv, port, WEIGHTED_FAIR_QUEUE);
		otto_qos_839x_set_scheduling_queue_weights(priv, port, rtldsa_default_queue_weights);
		/* Do re-marking based on outer tag */
		regmap_write_bits(priv->map, RTL839X_RMK_PORT_DEI_TAG_CTRL(port),
				  BIT(port % 32), BIT(port % 32));
	}

	/* Remap dot1p priorities to internal priority, for this the outer tag needs be re-marked */
	v = 0;
	for (int p = 0; p < 8; p++)
		v |= (dot1p_priority_remapping[p] & 0x7) << (p * 3);
	regmap_write(priv->map, RTL839X_PRI_SEL_IPRI_REMAP, v);

	/* Configure Drop Precedence for Drop Eligible Indicator (DEI)
	 * Index 0: 0
	 * Index 1: 2
	 * Each indicator is 2 bits long
	 */
	regmap_write(priv->map, RTL839X_PRI_SEL_DEI2DP_REMAP, 2 << 2);

	/* Re-mark DEI: 4 bit-fields of 2 bits each, field 0 is bits 0-1, ... */
	regmap_write(priv->map, RTL839X_RMK_DEI_CTRL, (0x1 << 2) | (0x1 << 4));

	/* Set Congestion avoidance drop probability to 0 for drop precedences 0-2 (bits 24-31)
	 * low threshold (bits 0-11) to 4095 and high threshold (bits 12-23) to 4095
	 * Weighted Random Early Detection (WRED) is used
	 */
	regmap_write(priv->map, RTL839X_WRED_PORT_THR_CTRL(0), 4095 << 12 | 4095);
	regmap_write(priv->map, RTL839X_WRED_PORT_THR_CTRL(1), 4095 << 12 | 4095);
	regmap_write(priv->map, RTL839X_WRED_PORT_THR_CTRL(2), 4095 << 12 | 4095);

	/* Set queue-based congestion avoidance properties, register fields are as
	 * for forward RTL839X_WRED_PORT_THR_CTRL
	 */
	for (int q = 0; q < 8; q++) {
		regmap_write(priv->map, RTL839X_WRED_QUEUE_THR_CTRL(q, 0),
			     255 << 24 | 78 << 12 | 68);
		regmap_write(priv->map, RTL839X_WRED_QUEUE_THR_CTRL(q, 0),
			     255 << 24 | 74 << 12 | 64);
		regmap_write(priv->map, RTL839X_WRED_QUEUE_THR_CTRL(q, 0),
			     255 << 24 | 70 << 12 | 60);
	}
}

static void otto_qos_930x_set_group_selector(struct otto_dsa_priv *priv, int port, int group)
{
	u32 val = (u32)group << RTL93XX_PORT_TBL_IDX_CTRL_IDX_OFFSET(port);

	regmap_write_bits(priv->map, RTL930X_PORT_TBL_IDX_CTRL(port),
			  RTL93XX_PORT_TBL_IDX_CTRL_IDX_MASK(port) | val, val);
}

static void otto_qos_930x_setup_default_dscp2queue_map(struct otto_dsa_priv *priv)
{
	u32 queue;

	/* The default mapping between dscp and queue is based on
	 * the first 3 bits indicate the precedence (prio = dscp >> 3).
	 */
	for (int i = 0; i < DSCP_MAP_MAX; i++) {
		queue = (i >> 3) << RTL93XX_REMAP_DSCP_INTPRI_DSCP_OFFSET(i);
		regmap_write_bits(priv->map, RTL930X_REMAP_DSCP(i),
				  RTL93XX_REMAP_DSCP_INTPRI_DSCP_MASK(i), queue);
	}
}

static void otto_qos_930x_prio2queue_matrix(struct otto_dsa_priv *priv, int *min_queues)
{
	u32 v = 0;

	for (int i = 0; i < MAX_PRIOS; i++)
		v |= i << (min_queues[i] * 3);

	regmap_write(priv->map, RTL930X_QM_INTPRI2QID_CTRL, v);
}

void otto_qos_930x_queue_sched_set(struct otto_dsa_priv *priv, int port, int queue, u32 weight,
				   bool strict)
{
	u32 v = FIELD_PREP(RTL930X_SCHED_Q_WEIGHT, weight) | (strict ? RTL930X_SCHED_Q_STRICT : 0);

	if (port < 24)
		regmap_write(priv->map, RTL930X_SCHED_PORT_Q_CTRL_SET0(port, queue), v);
	else
		regmap_write(priv->map, RTL930X_SCHED_PORT_Q_CTRL_SET1(port, queue), v);
}

void otto_qos_queue_sched_reset(struct otto_dsa_priv *priv, int port)
{
	for (int q = 0; q < MAX_PRIOS; q++)
		priv->r->queue_sched_set(priv, port, q, rtldsa_default_queue_weights[q], false);
}

static void otto_qos_930x_set_scheduling_queue_weights(struct otto_dsa_priv *priv)
{
	struct dsa_port *dp;

	dsa_switch_for_each_user_port(dp, priv->ds)
		otto_qos_queue_sched_reset(priv, dp->index);
}

static void otto_qos_930x_set_fc_thresholds(struct otto_dsa_priv *priv)
{
	u32 mask = RTL930X_FC_GLB_THR_ON_MASK | RTL930X_FC_GLB_THR_OFF_MASK;
	int num_user_ports = 0;
	struct dsa_port *dp;
	bool few_ports;
	u32 hi, lo;

	dsa_switch_for_each_user_port(dp, priv->ds)
		num_user_ports++;

	/* The vendor SDK sets these per board profile. Its 8x10G profile gives the global pool
	 * more of the packet buffer than its 24G+4x10G and 24x2.5G+2x10G profiles, as fewer
	 * ports need less reserve. Take the 8x10G values for up to 8 user ports and the values
	 * the larger profiles share otherwise.
	 */
	few_ports = num_user_ports <= 8;

	hi = FIELD_PREP(RTL930X_FC_GLB_THR_ON_MASK, few_ports ? 0xa20 : 0x890) |
	     FIELD_PREP(RTL930X_FC_GLB_THR_OFF_MASK, few_ports ? 0x9d0 : 0x778);
	lo = FIELD_PREP(RTL930X_FC_GLB_THR_ON_MASK, few_ports ? 0x958 : 0x5d4) |
	     FIELD_PREP(RTL930X_FC_GLB_THR_OFF_MASK, few_ports ? 0x908 : 0x4bc);

	regmap_update_bits(priv->map, RTL930X_FC_GLB_HI_THR, mask, hi);
	regmap_update_bits(priv->map, RTL930X_FC_GLB_LO_THR, mask, lo);
	regmap_update_bits(priv->map, RTL930X_FC_GLB_FCOFF_HI_THR, mask, hi);
	regmap_update_bits(priv->map, RTL930X_FC_GLB_FCOFF_LO_THR, mask, lo);
}

void otto_qos_930x_init(struct otto_dsa_priv *priv)
{
	struct dsa_port *dp;
	u32 v;

	/* Assign all the ports to the Group-0 */
	dsa_switch_for_each_user_port(dp, priv->ds)
		otto_qos_930x_set_group_selector(priv, dp->index, 0);

	otto_qos_930x_prio2queue_matrix(priv, rtldsa_max_available_queue);

	/* configure priority weights */
	v = 0;
	v |= FIELD_PREP(RTL93XX_PRI_SEL_TBL_CTRL_PORT_MASK, 3);
	v |= FIELD_PREP(RTL93XX_PRI_SEL_TBL_CTRL_DSCP_MASK, 5);
	v |= FIELD_PREP(RTL93XX_PRI_SEL_TBL_CTRL_ITAG_MASK, 6);
	v |= FIELD_PREP(RTL93XX_PRI_SEL_TBL_CTRL_OTAG_MASK, 7);

	regmap_write(priv->map, RTL930X_PRI_SEL_TBL_CTRL(0), v);

	otto_qos_930x_setup_default_dscp2queue_map(priv);
	otto_qos_930x_set_scheduling_queue_weights(priv);
	otto_qos_930x_set_fc_thresholds(priv);
}

static void otto_qos_931x_set_group_selector(struct otto_dsa_priv *priv, int port, int group)
{
	u32 val = (u32)group << RTL93XX_PORT_TBL_IDX_CTRL_IDX_OFFSET(port);

	regmap_write_bits(priv->map, RTL931X_PORT_TBL_IDX_CTRL(port),
			  RTL93XX_PORT_TBL_IDX_CTRL_IDX_MASK(port) | val, val);
}

static void otto_qos_931x_setup_default_dscp2queue_map(struct otto_dsa_priv *priv)
{
	u32 queue;

	/* The default mapping between dscp and queue is based on
	 * the first 3 bits indicate the precedence (prio = dscp >> 3).
	 */
	for (int i = 0; i < DSCP_MAP_MAX; i++) {
		queue = (i >> 3) << RTL93XX_REMAP_DSCP_INTPRI_DSCP_OFFSET(i);
		regmap_write_bits(priv->map, RTL931X_REMAP_DSCP(i),
				  RTL93XX_REMAP_DSCP_INTPRI_DSCP_MASK(i), queue);
	}
}

static void otto_qos_931x_prio2queue_matrix(struct otto_dsa_priv *priv, int *min_queues)
{
	u32 v = 0;

	for (int i = 0; i < MAX_PRIOS; i++)
		v |= i << (min_queues[i] * 3);

	regmap_write(priv->map, RTL931X_QM_INTPRI2QID_CTRL, v);
}

static void otto_qos_931x_set_scheduling_queue_weights(struct otto_dsa_priv *priv)
{
	struct dsa_port *dp;
	u32 addr;

	dsa_switch_for_each_user_port(dp, priv->ds) {
		for (int q = 0; q < 8; q++) {
			if (dp->index < 51)
				addr = RTL931X_SCHED_PORT_Q_CTRL_SET0(dp->index, q);
			else
				addr = RTL931X_SCHED_PORT_Q_CTRL_SET1(dp->index, q);

			regmap_write(priv->map, addr, rtldsa_default_queue_weights[q]);
		}
	}
}

void otto_qos_931x_init(struct otto_dsa_priv *priv)
{
	struct dsa_port *dp;
	u32 v;

	/* Assign all the ports to the Group-0 */
	dsa_switch_for_each_user_port(dp, priv->ds)
		otto_qos_931x_set_group_selector(priv, dp->index, 0);

	otto_qos_931x_prio2queue_matrix(priv, rtldsa_max_available_queue);

	/* configure priority weights */
	v = 0;
	v |= FIELD_PREP(RTL93XX_PRI_SEL_TBL_CTRL_PORT_MASK, 3);
	v |= FIELD_PREP(RTL93XX_PRI_SEL_TBL_CTRL_DSCP_MASK, 5);
	v |= FIELD_PREP(RTL93XX_PRI_SEL_TBL_CTRL_ITAG_MASK, 6);
	v |= FIELD_PREP(RTL93XX_PRI_SEL_TBL_CTRL_OTAG_MASK, 7);

	regmap_write(priv->map, RTL931X_PRI_SEL_TBL_CTRL(0) + 4, v);
	regmap_write(priv->map, RTL931X_PRI_SEL_TBL_CTRL(0), 0);

	otto_qos_931x_setup_default_dscp2queue_map(priv);
	otto_qos_931x_set_scheduling_queue_weights(priv);
}
