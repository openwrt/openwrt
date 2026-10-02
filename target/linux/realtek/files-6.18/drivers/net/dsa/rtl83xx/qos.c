// SPDX-License-Identifier: GPL-2.0-only

#include <net/dcbnl.h>
#include <net/dsa.h>
#include <linux/delay.h>
#include <linux/regmap.h>
#include <linux/soc/realtek/otto_table.h>
#include <asm/mach-rtl-otto/mach-rtl-otto.h>

#include "qos.h"
#include "rtl-otto.h"

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

static int otto_qos_max_available_queue[] = {0, 1, 2, 3, 4, 5, 6, 7};
static int otto_qos_default_queue_weights[] = {1, 1, 1, 1, 1, 1, 1, 1};
static int dot1p_priority_remapping[] = {0, 1, 2, 3, 4, 5, 6, 7};

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
	u32 data[2];
	int err;

	if (port >= priv->r->cpu_port)
		return 0;

	mutex_lock(&priv->reg_mutex);
	err = otto_table_offset_read(RTL8390_TBL_SCHED, port, &data, 7);
	mutex_unlock(&priv->reg_mutex);
	if (err) {
		dev_err(priv->dev, "Failed to read egress rate on port %d: %d\n", port, err);
		return 0;
	}

	return ((data[0] & 0xff) << 12) | (data[1] >> 20);
}

/* Sets the rate limit, 10MBit/s is equal to a rate value of 625, returns previous rate */
static int otto_qos_839x_set_egress_rate_locked(struct otto_dsa_priv *priv, int port,
					     u32 rate)
{
	u32 data[9], old_rate;
	int tbl, err;

	lockdep_assert_held(&priv->reg_mutex);

	if (port >= priv->r->cpu_port)
		return -EINVAL;

	tbl = otto_table_acquire(RTL8390_TBL_SCHED);
	if (tbl < 0)
		return tbl;

	err = __otto_table_read(tbl, port, &data);
	if (err)
		goto out;

	old_rate = ((data[7] & 0xff) << 12) | (data[8] >> 20);
	data[7] = (data[7] & ~0xffU) | ((rate >> 12) & 0xff);
	data[8] = (data[8] & ~(0xfffU << 20)) | (rate << 20);
	err = __otto_table_write(tbl, port, &data);
	if (!err)
		err = old_rate;
out:
	otto_table_release(tbl);
	return err;
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
		v |= otto_qos_max_available_queue[i] << (i * 3);
	regmap_write(priv->map, reg, v);
}

static void otto_qos_838x_setup_default_prio2queue(struct otto_dsa_priv *priv)
{
	otto_qos_838x_setup_prio2queue_matrix(priv, otto_qos_max_available_queue);
	otto_qos_83xx_setup_prio2queue_cpu_matrix(priv, RTL838X_QM_PKT2CPU_INTPRI_MAP);
}

static void otto_qos_839x_setup_default_prio2queue(struct otto_dsa_priv *priv)
{
	otto_qos_839x_setup_prio2queue_matrix(priv, otto_qos_max_available_queue);
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

static void otto_qos_839x_set_scheduling_algorithm(struct otto_dsa_priv *priv, int port,
						 enum scheduler_type sched)
{
	u32 data[9], oam_state = 0, oam_port_state = 0, egress_rate = 0;
	bool restore_oam = false, restore_rate = false;
	u32 count, v;
	int tbl, err, ret;

	mutex_lock(&priv->reg_mutex);
	err = otto_table_read(RTL8390_TBL_SCHED, port, &data);
	if (err)
		goto out;

	/* Erratum E0014503: drain the port when switching from WRR to WFQ. */
	if (sched == WEIGHTED_FAIR_QUEUE && (data[8] & BIT(19)) &&
	    port != priv->r->cpu_port) {
		egress_rate = ((data[7] & 0xff) << 12) | (data[8] >> 20);
		err = regmap_read(priv->map, RTL839X_OAM_CTRL, &oam_state);
		if (err)
			goto out;
		err = regmap_read(priv->map, RTL839X_OAM_PORT_ACT_CTRL(port), &oam_port_state);
		if (err)
			goto out;

		/* Restore saved state even if a write fails after reaching hardware. */
		restore_oam = true;
		err = regmap_write_bits(priv->map, RTL839X_OAM_CTRL, 1, 1);
		if (err)
			goto restore;
		err = regmap_read(priv->map, RTL839X_OAM_CTRL, &v);
		if (err)
			goto restore;
		err = regmap_write(priv->map, RTL839X_OAM_PORT_ACT_CTRL(port), 0x2);
		if (err)
			goto restore;

		restore_rate = true;
		ret = otto_qos_839x_set_egress_rate_locked(priv, port, 0xfffff);
		if (ret < 0) {
			err = ret;
			goto restore;
		}

		/* OUT_Q shares the SCHED access register: do not hold either table here. */
		for (int i = 0; i < 3500; i++) {
			usleep_range(100, 200);
			err = otto_table_offset_read(RTL8390_TBL_OUT_Q, port, &count, 6);
			if (err)
				goto restore;
			if (!(count >> 20))
				break;
		}
	}

	tbl = otto_table_acquire(RTL8390_TBL_SCHED);
	if (tbl < 0) {
		err = tbl;
		goto restore;
	}
	err = __otto_table_read(tbl, port, &data);
	if (!err) {
		data[8] &= ~BIT(19);
		if (sched == WEIGHTED_ROUND_ROBIN)
			data[8] |= BIT(19);
		err = __otto_table_write(tbl, port, &data);
	}
	otto_table_release(tbl);

restore:
	if (restore_oam) {
		ret = regmap_write(priv->map, RTL839X_OAM_CTRL, oam_state);
		if (ret && !err)
			err = ret;
		ret = regmap_write(priv->map, RTL839X_OAM_PORT_ACT_CTRL(port), oam_port_state);
		if (ret && !err)
			err = ret;
	}
	if (restore_rate) {
		ret = otto_qos_839x_set_egress_rate_locked(priv, port, egress_rate);
		if (ret < 0 && !err)
			err = ret;
	}
out:
	mutex_unlock(&priv->reg_mutex);
	if (err)
		dev_err(priv->dev, "Failed to set scheduling algorithm on port %d: %d\n", port, err);
}

static void otto_qos_839x_set_scheduling_queue_weights(struct otto_dsa_priv *priv, int port,
						    int *queue_weights)
{
	u32 data[9];
	int tbl, err;

	mutex_lock(&priv->reg_mutex);
	tbl = otto_table_acquire(RTL8390_TBL_SCHED);
	if (tbl < 0) {
		err = tbl;
		goto out;
	}
	err = __otto_table_read(tbl, port, &data);
	if (err)
		goto release;

	for (int i = 0; i < 8; i++) {
		int lsb = 48 + i * 10;
		int word = 8 - (lsb >> 5);
		int start_bit = lsb & 31;
		u32 weight = queue_weights[i] & 0x3ff;

		data[word] &= ~(0x3ffU << start_bit);
		data[word] |= weight << start_bit;
		if (start_bit > 22) {
			data[word - 1] &= ~(0x3ffU >> (32 - start_bit));
			data[word - 1] |= weight >> (32 - start_bit);
		}
	}

	err = __otto_table_write(tbl, port, &data);
release:
	otto_table_release(tbl);
out:
	mutex_unlock(&priv->reg_mutex);
	if (err)
		dev_err(priv->dev, "Failed to set queue weights on port %d: %d\n", port, err);
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
		otto_qos_839x_set_scheduling_queue_weights(priv, port, otto_qos_default_queue_weights);
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
		priv->r->queue_sched_set(priv, port, q, otto_qos_default_queue_weights[q], false);
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

	otto_qos_930x_prio2queue_matrix(priv, otto_qos_max_available_queue);

	/* configure priority weights */
	v = 0;
	v |= FIELD_PREP(RTL93XX_PRI_SEL_TBL_CTRL_PORT_MASK, 3);
	v |= FIELD_PREP(RTL93XX_PRI_SEL_TBL_CTRL_DSCP_MASK, 5);
	v |= FIELD_PREP(RTL93XX_PRI_SEL_TBL_CTRL_ITAG_MASK, 6);
	v |= FIELD_PREP(RTL93XX_PRI_SEL_TBL_CTRL_OTAG_MASK, 7);

	regmap_write(priv->map, RTL930X_PRI_SEL_TBL_CTRL(0), v);

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

			regmap_write(priv->map, addr, otto_qos_default_queue_weights[q]);
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

	otto_qos_931x_prio2queue_matrix(priv, otto_qos_max_available_queue);

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

static u32 otto_qos_prio_get(struct otto_dsa_priv *priv, int base, int index)
{
	u32 v = 0;

	regmap_read(priv->map, base + (index / 10) * 4, &v);

	return (v >> ((index % 10) * 3)) & 0x7;
}

static void otto_qos_prio_set(struct otto_dsa_priv *priv, int base, int index, u8 prio)
{
	regmap_update_bits(priv->map, base + (index / 10) * 4,
			   0x7 << ((index % 10) * 3), prio << ((index % 10) * 3));
}

/* DSA reads the DSCP map when it creates the user ports, which is before qos_init() */
void otto_qos_setup(struct dsa_switch *ds)
{
	struct otto_dsa_priv *priv = ds->priv;

	if (!priv->r->pri_sel_remap_dscp)
		return;

	for (int dscp = 0; dscp < DSCP_MAP_MAX; dscp++)
		otto_qos_prio_set(priv, priv->r->pri_sel_remap_dscp, dscp, dscp >> 3);

	ds->dscp_prio_mapping_is_global = true;
}

int otto_qos_port_get_default_prio(struct dsa_switch *ds, int port)
{
	struct otto_dsa_priv *priv = ds->priv;

	if (!priv->r->pri_sel_port_pri)
		return 0;

	return otto_qos_prio_get(priv, priv->r->pri_sel_port_pri, port);
}

int otto_qos_port_set_default_prio(struct dsa_switch *ds, int port, u8 prio)
{
	struct otto_dsa_priv *priv = ds->priv;

	if (!priv->r->pri_sel_port_pri)
		return -EOPNOTSUPP;

	if (prio >= MAX_PRIOS)
		return -EINVAL;

	scoped_guard(mutex, &priv->reg_mutex)
		otto_qos_prio_set(priv, priv->r->pri_sel_port_pri, port, prio);

	return 0;
}

int otto_qos_port_get_dscp_prio(struct dsa_switch *ds, int port, u8 dscp)
{
	struct otto_dsa_priv *priv = ds->priv;

	if (!priv->r->pri_sel_remap_dscp)
		return -EOPNOTSUPP;

	return otto_qos_prio_get(priv, priv->r->pri_sel_remap_dscp, dscp);
}

int otto_qos_port_add_dscp_prio(struct dsa_switch *ds, int port, u8 dscp, u8 prio)
{
	struct otto_dsa_priv *priv = ds->priv;

	if (!priv->r->pri_sel_remap_dscp)
		return -EOPNOTSUPP;

	if (prio >= MAX_PRIOS)
		return -EINVAL;

	scoped_guard(mutex, &priv->reg_mutex)
		otto_qos_prio_set(priv, priv->r->pri_sel_remap_dscp, dscp, prio);

	return 0;
}

/* A replace adds the new entry first, so only an entry that still holds @prio may change.
 * A failed add gets here with @prio still in the table.
 */
int otto_qos_port_del_dscp_prio(struct dsa_switch *ds, int port, u8 dscp, u8 prio)
{
	struct dcb_ieee_app_dscp_map map = {};
	struct otto_dsa_priv *priv = ds->priv;
	u8 mask;

	if (!priv->r->pri_sel_remap_dscp)
		return -EOPNOTSUPP;

	if (IS_ENABLED(CONFIG_DCB))
		dcb_ieee_getapp_dscp_prio_mask_map(dsa_to_port(ds, port)->user, &map);
	mask = map.map[dscp] & ~BIT(prio);

	scoped_guard(mutex, &priv->reg_mutex) {
		if (otto_qos_prio_get(priv, priv->r->pri_sel_remap_dscp, dscp) == prio)
			otto_qos_prio_set(priv, priv->r->pri_sel_remap_dscp, dscp,
					  mask ? __fls(mask) : dscp >> 3);
	}

	return 0;
}
