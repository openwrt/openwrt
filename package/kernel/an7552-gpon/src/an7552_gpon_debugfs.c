// SPDX-License-Identifier: GPL-2.0
/*
 * Airoha AN7552/AN7563 xPON - debugfs of the whole device
 *
 * One directory for both halves of the device, debugfs/an7552-gpon:
 * "state" (the full diagnostic dump: the MAC state machine, the PLOAM and
 * OMCI counters, the T-CONT and GEM tables with per-GEM packet and byte
 * counters), "regs" (the GPON MAC register dump) and "phy_status" (the
 * SerDes, see an7552_phy_debugfs.c), and the subdirectories "tune" and
 * "phy" with one file per knob.  Everything that is not a debug dump lives
 * in sysfs (an7552_mac_main.c: identity, activation, PPE downstream and
 * QoS), so it works without CONFIG_DEBUG_FS; the sysfs "state" is only the
 * one line summary, this file is where the counters are.
 */
#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/seq_file.h>

#include "an7552_mac.h"

static int an7552_mac_state_show(struct seq_file *m, void *v)
{
	struct an7552_mac *x = m->private;
	int i;

	guard(mutex)(&x->lock);
	seq_printf(m, "started:   %d\n", x->started);
	seq_printf(m, "state:     O%d\n", x->state);
	seq_printf(m, "phy_ready: %d\n", x->phy_ready);
	seq_printf(m, "emergency: %d\n", x->emergency);
	seq_printf(m, "onu_id:    0x%02x\n", x->onu_id);
	seq_printf(m, "omcc:      0x%04x\n", x->omcc);
	seq_printf(m, "us_gem:    0x%04x\n", x->us_gem);
	seq_printf(m, "eqd:       0x%08x (byte 0x%x bit %u)\n", x->eqd,
		   x->byte_delay, x->bit_delay);
	seq_printf(m, "alarms:    0x%08x\n", x->alarms);
	seq_printf(m, "ber_ms:    %u bip_total %u\n", x->ber_interval_ms,
		   x->bip_total);
	seq_printf(m, "spf_cnt:   0x%08x\n", an7552_mac_get_superframe(x));
	seq_printf(m, "ploam rx %llu tx %llu drop %llu\n", x->stats.ploam_rx,
		   x->stats.ploam_tx, x->stats.ploam_rx_drop);
	seq_printf(m, "omci  rx %llu tx %llu\n", x->stats.omci_rx,
		   x->stats.omci_tx);
	seq_printf(m, "rx_drop_gem %llu irq_err %llu last_err 0x%08x\n",
		   x->stats.rx_drop_gem, x->stats.irq_err,
		   x->stats.last_err_status);
	seq_printf(m, "rx_csum ok %llu bad %llu none %llu\n",
		   x->stats.rx_csum_ok, x->stats.rx_csum_bad,
		   x->stats.rx_csum_none);
	seq_printf(m, "mcast_rx:  %s\n",
		   x->mcast_rx_off ? "off (gated in the HW GEM table)" : "on");

	seq_puts(m, "t-cont:\n");
	for (i = 0; i < GPON_MAX_TCONT; i++)
		if (x->tcont_alloc[i] != GPON_UNASSIGN_ALLOC_ID)
			seq_printf(m, "  ch %2d alloc 0x%03x\n", i,
				   x->tcont_alloc[i]);

	seq_puts(m, "gem:\n");
	for (i = 0; i < GPON_MAX_GEM_ID; i++) {
		struct an7552_gpon_gem *g = &x->gem[i];

		if (!g->valid)
			continue;
		seq_printf(m, "  0x%03x alloc 0x%03x ch %2u enc %d rx %llu/%llu tx %llu/%llu\n",
			   i, g->alloc_id, g->channel, g->encrypt,
			   (u64)atomic64_read(&g->rx_packets),
			   (u64)atomic64_read(&g->rx_bytes),
			   (u64)atomic64_read(&g->tx_packets),
			   (u64)atomic64_read(&g->tx_bytes));
	}

	return 0;
}
DEFINE_SHOW_ATTRIBUTE(an7552_mac_state);

static int an7552_mac_regs_show(struct seq_file *m, void *v)
{
	struct an7552_mac *x = m->private;

	an7552_mac_dump_regs(x, m);

	return 0;
}
DEFINE_SHOW_ATTRIBUTE(an7552_mac_regs);

/* ---- tune (GPON MAC debug/tuning knobs) ---- */

enum tune_kind {
	TUNE_BIT,
	TUNE_WR,
	TUNE_FLD,
	TUNE_EQD_O4,
	TUNE_EQD_O5,
	TUNE_EQD_SUB,
	TUNE_RSP_TIME,
	TUNE_PPS_WIDTH,
};

static const struct {
	const char	*name;
	u8		kind;
	u32		reg;
	u32		mask;
	u8		shift;
	u32		min;
	u32		max;
} an7552_dbg_tune[] = {
	{ "eqd_o4_offset", TUNE_EQD_O4, 0, 0, 0, 0, U8_MAX },
	{ "eqd_o5_offset", TUNE_EQD_O5, 0, 0, 0, 0, U8_MAX },
	{ "eqd_offset_sub", TUNE_EQD_SUB, 0, 0, 0, 0, 1 },
	{ "filter_us_overhead", TUNE_BIT, DBG_PLOAMD_FILTER_IN_O5, FILTER_US_OVERHEAD_O5, 0, 0, 1 },
	{ "filter_ext_burst", TUNE_BIT, DBG_PLOAMD_FILTER_IN_O5, FILTER_EXT_BST_LEN_O5, 0, 0, 1 },
	{ "ploamu_wait", TUNE_BIT, O3_O4_PLOAMU_CTRL, PLOAMU_IND_CTRL, 0, 0, 1 },
	{ "tx_align4", TUNE_BIT, DBG_GPON_MAC_SET, MAC_TX_ETH_4BYTE_ALIGN, 0, 0, 1 },
	{ "dba_backdoor", TUNE_BIT, DBG_DBA_BACK_DOOR, DBA_SW_RPT_EN, 0, 0, 1 },
	{ "dba_total", TUNE_WR, DBG_DBA_TOTAL_BUF_SIZE, 0, 0, 0, U32_MAX },
	{ "dba_green", TUNE_WR, DBG_DBA_GREEN_SIZE, 0, 0, 0, U32_MAX },
	{ "dba_yellow", TUNE_WR, DBG_DBA_YELLOW_SIZE, 0, 0, 0, U32_MAX },
	{ "dba_slight", TUNE_BIT, DBG_DBA_BACK_DOOR, DBA_SLIGHT_MODIFY_EN, 0, 0, 1 },
	{ "dba_shift", TUNE_BIT, DBG_DBA_BACK_DOOR, DBA_SHIFT_MODIFY_EN, 0, 0, 1 },
	{ "dba_mod_total", TUNE_FLD, DBG_SLIGHT_MODIFY_SIZE_2, DBA_MOD_TOTAL_MASK, 0, 0, U16_MAX },
	{ "dba_mod_green", TUNE_FLD, DBG_SLIGHT_MODIFY_SIZE_1, DBA_MOD_GREEN_MASK, 0, 0, U16_MAX },
	{ "dba_mod_yellow", TUNE_FLD, DBG_SLIGHT_MODIFY_SIZE_1, DBA_MOD_YELLOW_MASK, 0, 0, U16_MAX },
	{ "idle_gem_thld", TUNE_FLD, DBG_IDLE_GEM_THLD, IDLE_GEM_THLD_MASK, 0, 0, U16_MAX },
	{ "rsp_time", TUNE_RSP_TIME, G_RSP_TIME, RSP_TIME_MASK, 0, 0, U16_MAX },
	{ "fine_dly", TUNE_FLD, DBG_DLY, DLY_FINE_INT_MASK, 0, 0, U8_MAX },
	{ "rdi_num", TUNE_FLD, DBG_SW_RDI_CTRL, RDI_NUM_MASK, 0, 0, U16_MAX },
	{ "rdi_err_en", TUNE_FLD, DBG_RDI_ERR_CTRL, RDI_ERR_EN_MASK, 3, 0, U8_MAX },
	{ "rdi_err_thld", TUNE_FLD, DBG_RDI_ERR_CTRL, RDI_ERR_THLD_MASK, 0, 0, 0x7ffffff },
	{ "counter_type", TUNE_BIT, DBG_CAP_SETTING, CAP_MIB_FRAME_TYPE, 0, 0, 1 },
	{ "pps_width", TUNE_PPS_WIDTH, 0, 0, 0, 0, USEC_PER_SEC - 1 },
	{ "tod_period", TUNE_FLD, G_TOD_CLK_PERIOD, TOD_PERIOD_MASK, 0, 1, U8_MAX },
};

static int an7552_dbg_find_tune(const struct file *file)
{
	const char *name = file->f_path.dentry->d_name.name;
	int i;

	for (i = 0; i < ARRAY_SIZE(an7552_dbg_tune); i++)
		if (!strcmp(name, an7552_dbg_tune[i].name))
			return i;

	return -ENOENT;
}

static ssize_t an7552_dbg_tune_read(struct file *file, char __user *ubuf,
				    size_t count, loff_t *ppos)
{
	struct an7552_mac *x = file->private_data;
	int idx = an7552_dbg_find_tune(file);
	u32 val, r, mask, period;
	char buf[32];
	int len;

	if (idx < 0)
		return idx;

	mask = an7552_dbg_tune[idx].mask;
	mutex_lock(&x->lock);
	switch (an7552_dbg_tune[idx].kind) {
	case TUNE_EQD_O4:
		val = x->eqd_o4_offset;
		break;
	case TUNE_EQD_O5:
		val = x->eqd_o5_offset;
		break;
	case TUNE_EQD_SUB:
		val = x->eqd_offset_sub;
		break;
	case TUNE_PPS_WIDTH:
		period = FIELD_GET(TOD_PERIOD_MASK, mac_rr(x, G_TOD_CLK_PERIOD));
		val = mac_rr(x, TOD_1PPS_WD_CTRL) * period / 1000;
		break;
	case TUNE_BIT:
		val = !!(mac_rr(x, an7552_dbg_tune[idx].reg) & mask);
		break;
	case TUNE_WR:
		val = mac_rr(x, an7552_dbg_tune[idx].reg);
		break;
	default:
		r = mac_rr(x, an7552_dbg_tune[idx].reg);
		val = ((r & mask) >> __ffs(mask)) << an7552_dbg_tune[idx].shift;
		break;
	}
	mutex_unlock(&x->lock);

	len = scnprintf(buf, sizeof(buf), "%u\n", val);

	return simple_read_from_buffer(ubuf, count, ppos, buf, len);
}

static ssize_t an7552_dbg_tune_write(struct file *file,
				     const char __user *ubuf, size_t count,
				     loff_t *ppos)
{
	struct an7552_mac *x = file->private_data;
	int idx = an7552_dbg_find_tune(file);
	u32 v, mask, period;
	u64 val;
	int ret;

	if (idx < 0)
		return idx;

	ret = kstrtoull_from_user(ubuf, count, 0, &val);
	if (ret)
		return ret;
	if (val < an7552_dbg_tune[idx].min || val > an7552_dbg_tune[idx].max)
		return -ERANGE;

	if (an7552_dbg_tune[idx].kind == TUNE_EQD_O5) {
		an7552_mac_set_eqd_o5_offset(x, val);
		return count;
	}

	v = val;
	mask = an7552_dbg_tune[idx].mask;
	guard(mutex)(&x->lock);
	switch (an7552_dbg_tune[idx].kind) {
	case TUNE_EQD_O4:
		x->eqd_o4_offset = v;
		break;
	case TUNE_EQD_SUB:
		x->eqd_offset_sub = v;
		break;
	case TUNE_PPS_WIDTH:
		period = FIELD_GET(TOD_PERIOD_MASK, mac_rr(x, G_TOD_CLK_PERIOD));
		if (!period)
			ret = -EINVAL;
		else
			mac_wr(x, TOD_1PPS_WD_CTRL, v * 1000 / period);
		break;
	case TUNE_BIT:
		mac_rmw(x, an7552_dbg_tune[idx].reg, mask, v ? mask : 0);
		break;
	case TUNE_WR:
		mac_wr(x, an7552_dbg_tune[idx].reg, v);
		break;
	case TUNE_RSP_TIME:
		x->rsp_time = v;
		fallthrough;
	default:
		v = (v >> an7552_dbg_tune[idx].shift) << __ffs(mask);
		mac_rmw(x, an7552_dbg_tune[idx].reg, mask, v & mask);
		break;
	}

	return ret ? ret : count;
}

static const struct file_operations an7552_dbg_tune_fops = {
	.owner	= THIS_MODULE,
	.open	= simple_open,
	.read	= an7552_dbg_tune_read,
	.write	= an7552_dbg_tune_write,
	.llseek	= noop_llseek,
};

static ssize_t an7552_dbg_resync_write(struct file *file,
				       const char __user *ubuf, size_t count,
				       loff_t *ppos)
{
	struct an7552_mac *x = file->private_data;
	u64 val;
	int ret;

	ret = kstrtoull_from_user(ubuf, count, 0, &val);
	if (ret)
		return ret;
	if (val)
		an7552_mac_sw_resync(x);

	return count;
}

static const struct file_operations an7552_dbg_resync_fops = {
	.owner	= THIS_MODULE,
	.open	= simple_open,
	.write	= an7552_dbg_resync_write,
	.llseek	= noop_llseek,
};

/* ---- phy (PON SerDes PHY controls, actions and measurements) ---- */

enum {
	DBG_PHY_TX_CONT,
	DBG_PHY_BEN_FORCE,
	DBG_PHY_TX_MODE,
	DBG_PHY_TX_CTL,
	DBG_PHY_TX_AMP,
	DBG_PHY_ROGUE,
	DBG_PHY_TRANS_INV,
	DBG_PHY_RO_FEC_RX,
	DBG_PHY_RO_FEC_TX,
	DBG_PHY_RO_FEC_DEC,
	DBG_PHY_RO_RTD,
};

static const struct {
	const char	*name;
	u8		kind;
	u8		inv;
	u8		min;
	u8		max;
	umode_t		mode;
} an7552_dbg_phy[] = {
	{ "tx_cont", DBG_PHY_TX_CONT, 0, 0, 1, 0600 },
	{ "ben_force", DBG_PHY_BEN_FORCE, 0, 0, 1, 0600 },
	{ "tx_mode", DBG_PHY_TX_MODE, 0, AN7552_TX_MODE_NORMAL, AN7552_TX_MODE_OFF, 0600 },
	{ "tx_ctl", DBG_PHY_TX_CTL, 0, 0, 1, 0600 },
	{ "tx_amp", DBG_PHY_TX_AMP, 0, 0, 15, 0600 },
	{ "rogue_en", DBG_PHY_ROGUE, 0, 0, 1, 0600 },
	{ "rx_sd_inv", DBG_PHY_TRANS_INV, AN7552_TRANS_RX_SD_INV, 0, 1, 0600 },
	{ "tx_sd_inv", DBG_PHY_TRANS_INV, AN7552_TRANS_TX_SD_INV, 0, 1, 0600 },
	{ "tx_fault_inv", DBG_PHY_TRANS_INV, AN7552_TRANS_TX_FAULT_INV, 0, 1, 0600 },
	{ "ben_inv", DBG_PHY_TRANS_INV, AN7552_TRANS_BURST_EN_INV, 0, 1, 0600 },
	{ "fec_rx", DBG_PHY_RO_FEC_RX, 0, 0, 0, 0400 },
	{ "fec_tx", DBG_PHY_RO_FEC_TX, 0, 0, 0, 0400 },
	{ "fec_dec", DBG_PHY_RO_FEC_DEC, 0, 0, 0, 0400 },
	{ "rtd", DBG_PHY_RO_RTD, 0, 0, 0, 0400 },
};

static const struct {
	const char	*name;
	int		(*fn)(struct an7552_phy *priv);
	/*
	 * True for an action that reaches the optical front end.  Those run
	 * under state_lock instead of x->lock: the front end takes its own
	 * lock and calls back up into this MAC from its timers, so holding
	 * x->lock across the call would close a cycle with that order.
	 * state_lock is what an7552_mac_down() takes before it can hand the
	 * front end back, so it keeps the lifetime guarantee x->lock was
	 * there for.
	 */
	bool		front_end;
} an7552_dbg_phy_act[] = {
	/*
	 * the whole pon_phy_scu_reset_init(): a bare an7552_phy_reset()
	 * leaves the csr (int enable, guard, counters) cleared by the reset
	 */
	{ "reset",		an7552_phy_scu_reset,		false },
	{ "pll_reset",		an7552_pll_reset,		false },
	{ "tx_fault_reset",	an7552_phy_tx_fault_reset,	true },
};

static int an7552_dbg_find_phy(const struct file *file)
{
	const char *name = file->f_path.dentry->d_name.name;
	int i;

	for (i = 0; i < ARRAY_SIZE(an7552_dbg_phy); i++)
		if (!strcmp(name, an7552_dbg_phy[i].name))
			return i;

	return -ENOENT;
}

static ssize_t an7552_dbg_phy_read(struct file *file, char __user *ubuf,
				   size_t count, loff_t *ppos)
{
	struct an7552_mac *x = file->private_data;
	struct an7552_phy *phy = x->phy;
	int idx = an7552_dbg_find_phy(file);
	char buf[32];
	u32 val = 0;
	int len, ret;

	if (idx < 0)
		return idx;

	if (an7552_dbg_phy[idx].kind == DBG_PHY_RO_RTD) {
		ret = an7552_phy_rtd(phy, &val);
		if (ret)
			return ret;
		len = scnprintf(buf, sizeof(buf), "%u\n", val);
		return simple_read_from_buffer(ubuf, count, ppos, buf, len);
	}

	mutex_lock(&x->lock);
	switch (an7552_dbg_phy[idx].kind) {
	case DBG_PHY_TX_CONT:
		val = an7552_phy_get_tx_cont(phy);
		break;
	case DBG_PHY_BEN_FORCE:
		val = an7552_phy_ben_forced(phy);
		break;
	case DBG_PHY_TX_MODE:
		val = an7552_phy_get_tx_mode(phy);
		break;
	case DBG_PHY_TX_CTL:
		val = an7552_phy_tx_ctl_get(phy);
		break;
	case DBG_PHY_TX_AMP:
		val = an7552_phy_get_tx_amp(phy);
		break;
	case DBG_PHY_ROGUE:
		val = an7552_phy_rogue_get(phy);
		break;
	case DBG_PHY_TRANS_INV:
		val = !!(an7552_phy_get_trans_inv(phy) &
			 an7552_dbg_phy[idx].inv);
		break;
	case DBG_PHY_RO_FEC_RX:
		val = !!(csr_read(phy, PHY_CSR_RX_STATUS) & PHY_RX_FEC);
		break;
	case DBG_PHY_RO_FEC_TX:
		val = !!(csr_read(phy, PHY_CSR_TX_STATUS) & PHY_TX_FEC);
		break;
	case DBG_PHY_RO_FEC_DEC:
		val = !!(csr_read(phy, PHY_CSR_FECDEC_CTL) & BIT(0));
		break;
	}
	mutex_unlock(&x->lock);

	len = scnprintf(buf, sizeof(buf), "%u\n", val);

	return simple_read_from_buffer(ubuf, count, ppos, buf, len);
}

static ssize_t an7552_dbg_phy_write(struct file *file,
				    const char __user *ubuf, size_t count,
				    loff_t *ppos)
{
	struct an7552_mac *x = file->private_data;
	struct an7552_phy *phy = x->phy;
	int idx = an7552_dbg_find_phy(file);
	u32 inv;
	u64 val;
	int ret;

	if (idx < 0)
		return idx;
	if (!(an7552_dbg_phy[idx].mode & 0200))
		return -EACCES;

	ret = kstrtoull_from_user(ubuf, count, 0, &val);
	if (ret)
		return ret;
	if (val < an7552_dbg_phy[idx].min || val > an7552_dbg_phy[idx].max)
		return -ERANGE;

	inv = an7552_dbg_phy[idx].inv;
	guard(mutex)(&x->lock);
	switch (an7552_dbg_phy[idx].kind) {
	case DBG_PHY_TX_CONT:
		ret = an7552_phy_set_tx_cont(phy, val);
		if (!ret && !val && x->admin_down)
			an7552_trans_power_switch(phy, false);
		break;
	case DBG_PHY_BEN_FORCE:
		ret = an7552_phy_ben_force(phy, val);
		break;
	case DBG_PHY_TX_MODE:
		ret = an7552_phy_set_tx_mode(phy, val);
		break;
	case DBG_PHY_TX_CTL:
		ret = an7552_phy_tx_ctl(phy, val);
		break;
	case DBG_PHY_TX_AMP:
		ret = an7552_phy_set_tx_amp(phy, val);
		break;
	case DBG_PHY_ROGUE:
		ret = an7552_phy_rogue_en(phy, val);
		break;
	case DBG_PHY_TRANS_INV:
		ret = an7552_phy_set_trans_inv(phy, inv, val ? inv : 0);
		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret ? ret : count;
}

static const struct file_operations an7552_dbg_phy_fops = {
	.owner	= THIS_MODULE,
	.open	= simple_open,
	.read	= an7552_dbg_phy_read,
	.write	= an7552_dbg_phy_write,
	.llseek	= noop_llseek,
};

static ssize_t an7552_dbg_phy_act_write(struct file *file,
					const char __user *ubuf, size_t count,
					loff_t *ppos)
{
	struct an7552_mac *x = file->private_data;
	const char *name = file->f_path.dentry->d_name.name;
	u64 val;
	int i, ret;

	ret = kstrtoull_from_user(ubuf, count, 0, &val);
	if (ret)
		return ret;
	if (!val)
		return count;

	for (i = 0; i < ARRAY_SIZE(an7552_dbg_phy_act); i++) {
		if (!strcmp(name, an7552_dbg_phy_act[i].name)) {
			struct mutex *lock = an7552_dbg_phy_act[i].front_end ?
					     &x->state_lock : &x->lock;

			mutex_lock(lock);
			ret = an7552_dbg_phy_act[i].fn(x->phy);
			mutex_unlock(lock);
			return ret ? ret : count;
		}
	}

	return -ENOENT;
}

static const struct file_operations an7552_dbg_phy_act_fops = {
	.owner	= THIS_MODULE,
	.open	= simple_open,
	.write	= an7552_dbg_phy_act_write,
	.llseek	= noop_llseek,
};

void an7552_mac_debugfs_init(struct an7552_mac *x)
{
	struct dentry *dir;
	int i;

	x->debugfs = debugfs_create_dir(AN7552_DRV_NAME, NULL);
	debugfs_create_file("state", 0400, x->debugfs, x,
			    &an7552_mac_state_fops);
	debugfs_create_file("regs", 0400, x->debugfs, x,
			    &an7552_mac_regs_fops);

	dir = debugfs_create_dir("tune", x->debugfs);
	for (i = 0; i < ARRAY_SIZE(an7552_dbg_tune); i++)
		debugfs_create_file(an7552_dbg_tune[i].name, 0600, dir, x,
				    &an7552_dbg_tune_fops);
	debugfs_create_file("resync", 0200, dir, x, &an7552_dbg_resync_fops);

	dir = debugfs_create_dir("phy", x->debugfs);
	for (i = 0; i < ARRAY_SIZE(an7552_dbg_phy); i++)
		debugfs_create_file(an7552_dbg_phy[i].name,
				    an7552_dbg_phy[i].mode, dir, x,
				    &an7552_dbg_phy_fops);
	for (i = 0; i < ARRAY_SIZE(an7552_dbg_phy_act); i++)
		debugfs_create_file(an7552_dbg_phy_act[i].name, 0200, dir, x,
				    &an7552_dbg_phy_act_fops);

	/* the SerDes half files into the same directory */
	an7552_phy_debugfs_create(x->debugfs, x->phy);
}

void an7552_mac_debugfs_exit(struct an7552_mac *x)
{
	debugfs_remove_recursive(x->debugfs);
}
