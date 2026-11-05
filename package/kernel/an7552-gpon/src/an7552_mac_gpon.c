// SPDX-License-Identifier: GPL-2.0
/*
 * Airoha AN7552/AN7563 GPON MAC: device control, PLOAM and activation.
 *
 * This is a port of the AN7552 SDK xpon_1g GPON layer (gpon_dev.c,
 * gpon_ploam.c, gpon_act.c, gpon.c) restricted to what the kernel has to do
 * on its own: bring the MAC up, follow the G.984.3 activation (O1..O7),
 * answer PLOAM, keep the T-CONT/GEM tables and report state.  Branches are
 * the ones compiled into the stock firmware for HIR 0xf.
 */
#include <linux/delay.h>
#include <linux/if_vlan.h>
#include <linux/interrupt.h>
#include <linux/iopoll.h>
#include <linux/math64.h>
#include <linux/pon_laser.h>
#include <linux/random.h>
#include <linux/regmap.h>
#include <linux/reset.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/soc/airoha/airoha_xpon.h>
#include <linux/unaligned.h>

#include "an7552_mac.h"

/* xpon_reset_qdma_tx_buf() for HIR 0xc/0xf; defined further down */
static void an7552_mac_reset_txbuf(unsigned int n);

static void gpon_silence_work(struct work_struct *w);

/* gpon_loss_of_light(); defined further down, next to the PHY event handler */
static void gpon_loss_of_light(struct an7552_mac *x);

static const char *const gpon_ploamd_name[PLOAMD_MAX] = {
	[PLOAMD_UPSTREAM_OVERHEAD] = "Upstream_Overhead",
	[PLOAMD_ASSIGN_ONU_ID] = "Assign_ONU-ID",
	[PLOAMD_RANGING_TIME] = "Ranging_Time",
	[PLOAMD_DEACTIVATE_ONU_ID] = "Deactivate_ONU-ID",
	[PLOAMD_DISABLE_SERIAL_NUM] = "Disable_Serial_Number",
	[PLOAMD_ENCRYPTED_PORT_ID] = "Encrypted_Port-ID",
	[PLOAMD_REQUEST_PASSWORD] = "Request_Password",
	[PLOAMD_ASSIGN_ALLOC_ID] = "Assign_Alloc-ID",
	[PLOAMD_POPUP] = "POPUP",
	[PLOAMD_REQUEST_KEY] = "Request_Key",
	[PLOAMD_CONFIG_PORT_ID] = "Configure_Port-ID",
	[PLOAMD_PEE] = "PEE",
	[PLOAMD_CPL] = "CPL",
	[PLOAMD_PST] = "PST",
	[PLOAMD_BER_INTERVAL] = "BER_Interval",
	[PLOAMD_KEY_SWITCHING_TIME] = "Key_Switching_Time",
	[PLOAMD_EXT_BURST_LENGTH] = "Extended_Burst_Length",
	[PLOAMD_PON_ID] = "PON-ID",
	[PLOAMD_SWIFT_POPUP] = "Swift_POPUP",
	[PLOAMD_RANGING_ADJUSTMENT] = "Ranging_Adjustment",
	[PLOAMD_SLEEP_ALLOW] = "Sleep_Allow",
};

static const char *const gpon_ploamu_name[] = {
	[PLOAMU_SERIAL_NUMBER] = "Serial_Number_ONU",
	[PLOAMU_PASSWORD] = "Password",
	[PLOAMU_DYING_GASP] = "Dying_Gasp",
	[PLOAMU_NO_MESSAGE] = "No_message",
	[PLOAMU_ENCRYPTION_KEY] = "Encryption_Key",
	[PLOAMU_PEE] = "PEE",
	[PLOAMU_PST] = "PST",
	[PLOAMU_REI] = "REI",
	[PLOAMU_ACKNOWLEDGE] = "Acknowledge",
	[PLOAMU_SLEEP_REQUEST] = "Sleep_Request",
};

static const char *gpon_ploam_name(const char *const *tab, size_t n, u8 id)
{
	return id < n && tab[id] ? tab[id] : "?";
}

/* SCU WAN_CONF, select_xpon_wan() */

/* ---------------------------------------------------------------------
 * Device layer (gpon_dev.c)
 * ------------------------------------------------------------------- */

static void gpon_select_wan(struct an7552_mac *x)
{
	u32 sel = x->mode == AN7552_XPON_GPON_SYM ? SCU_WAN_SEL_GPON_SYM :
						    SCU_WAN_SEL_GPON;

	regmap_update_bits(x->scu, SCU_WAN_CONF, SCU_WAN_SEL_MASK, sel);
}

/*
 * gponDevMbiStop(): also stops the GDM2/CDM2 path in airoha-eth.  The TX
 * stop is remembered for an7552_mac_burst_ctrl(), which must not lift it.
 */
static void gpon_mbi_stop(struct an7552_mac *x, bool stop)
{
	u32 bits = MBI_RX_STOP | MBI_TX_STOP;
	unsigned long flags;
	u32 v;

	spin_lock_irqsave(&x->reg_lock, flags);
	x->mbi_tx_stopped = stop;
	v = mac_rr(x, G_MBI_MPI_STOP);
	mac_wr(x, G_MBI_MPI_STOP, (v & ~bits) | (stop ? bits : 0));
	spin_unlock_irqrestore(&x->reg_lock, flags);
	udelay(1);
	airoha_xpon_gdm_stop(stop);
	udelay(50);
}

/* gponDevMpiStop() */
static int gpon_mpi_stop(struct an7552_mac *x, bool stop)
{
	u32 bits = MPI_RX_STOP | MPI_TX_STOP;
	u32 done = MPI_RX_STOP_DONE | MPI_TX_STOP_DONE;
	u32 val;

	mac_rmw(x, G_MBI_MPI_STOP, bits, stop ? bits : 0);
	if (!stop)
		return 0;

	return read_poll_timeout_atomic(mac_rr, val, (val & done) == done,
					1, 200, false, x, G_MBI_MPI_STOP);
}

/*
 * gponDevResetCtrl(): hold resets the MAC through the SCU XPON MAC reset
 * (RSTCTRL1 bit 31) and runs the PHY logic reset in between; release only
 * restarts the data interfaces and clears the debug error groups.
 */
static void gpon_reset_ctrl(struct an7552_mac *x, bool hold)
{
	if (hold) {
		gpon_mbi_stop(x, true);
		gpon_mpi_stop(x, true);
		reset_control_assert(x->mac_rst);
		udelay(1);
		an7552_phy_logic_reset(x->phy);
		reset_control_deassert(x->mac_rst);
		gpon_mbi_stop(x, true);
		gpon_mpi_stop(x, true);
		return;
	}

	gpon_mbi_stop(x, false);
	gpon_mpi_stop(x, false);
	mac_wr(x, DBG_GRP_0, 0xffffffff);
	mac_wr(x, DBG_GRP_1, 0xffffffff);
}

/*
 * gponDevSetDBABlockSize(): the MAC takes the rounded reciprocal 2048/size
 * with the 8 bits in reversed order (vendor loop over bit 0..7).
 */
static void gpon_set_dba_block_size(struct an7552_mac *x, u16 size)
{
	u8 q, val = 0;
	int i;

	if (size >= 0x800) {
		mac_wr(x, G_GBL_CFG, 0x80);
		return;
	}
	q = 0x800 / size + ((size >> 1) <= (0x800 % size));
	for (i = 0; i < 8; i++)
		if (q & BIT(i))
			val |= BIT(7 - i);
	mac_wr(x, G_GBL_CFG, val);
}

/* gponDevSetEncryptKey() */
static void gpon_set_key(struct an7552_mac *x, const u8 *key)
{
	int i;

	for (i = 0; i < 4; i++)
		mac_wr(x, G_AES_SHADOW_KEY(i),
			get_unaligned_be32(&key[(3 - i) * 4]));
}

/* gponDevGetEncryptKey(): only the active key words are restored later */
static void gpon_save_active_key(struct an7552_mac *x)
{
	int i;

	for (i = 0; i < 4; i++)
		x->typeb_key[i] = mac_rr(x, G_AES_ACTIVE_KEY(i));
}

/* gponDevSetAesKeySwitchBySw() */
static int gpon_key_switch_by_sw(struct an7552_mac *x)
{
	u32 val;

	mac_wr(x, G_AES_KEY_SWITCH_BY_SW, AES_KEY_SWITCH_SW_CMD);

	return read_poll_timeout_atomic(mac_rr, val,
					val & AES_KEY_SWITCH_SW_DONE, 1, 3000,
					false, x, G_AES_KEY_SWITCH_BY_SW);
}

/* gponDevSetKeySwithTime() */
static void gpon_set_key_switch_time(struct an7552_mac *x, u32 spf)
{
	if (!spf) {
		if (gpon_key_switch_by_sw(x))
			dev_warn(x->dev, "AES key switch by SW timed out\n");
		return;
	}
	mac_rmw(x, G_AES_CFG, AES_SPF_CNT_MASK, spf & AES_SPF_CNT_MASK);
}

/* gponDevSetGemInfo() */
static int gpon_hw_gem(struct an7552_mac *x, u16 gem, bool valid, bool enc)
{
	u32 val;

	mac_wr(x, G_GEM_PORT_CFG, GEM_PORT_CMD_WRITE |
		FIELD_PREP(GEM_PORT_ID_MASK, gem) |
		(valid ? GEM_PORT_VLD : 0) | (enc ? GEM_PORT_ENCRYPT : 0));

	return read_poll_timeout_atomic(mac_rr, val, val & GEM_PORT_CMD_DONE,
					1, 3000, false, x, G_GEM_PORT_STS);
}

/*
 * The hardware GEM entry of a port that is valid in the software table: a
 * multicast port is left out of the hardware table while gpond has the
 * multicast gated off (an7552_mac_mcast_rx()); the MAC drops the frames of
 * a GEM port that is not valid there, as for a removed port
 * (gwan_remove_gemport() -> gponDevSetGemInfo(id, DISABLE, DISABLE)).
 */
static int gpon_hw_gem_sync(struct an7552_mac *x, u16 gem)
{
	struct an7552_gpon_gem *g = &x->gem[gem];

	lockdep_assert_held(&x->lock);

	return gpon_hw_gem(x, gem,
			   !(g->channel == GPON_MULTICAST_CHANNEL &&
			     x->mcast_rx_off),
			   g->encrypt);
}

/* gponDevSetTCont0_15() / gponDevSetTCont16_31() */
static int gpon_hw_tcont(struct an7552_mac *x, int idx, bool valid, u16 id)
{
	u32 val;

	if (idx < 16) {
		if (idx & 1)
			mac_rmw(x, G_TCONT_ID(idx),
				 TCONT_ID_HI_MASK | TCONT_VLD_HI,
				 FIELD_PREP(TCONT_ID_HI_MASK, id) |
				 (valid ? TCONT_VLD_HI : 0));
		else
			mac_rmw(x, G_TCONT_ID(idx),
				 TCONT_ID_LO_MASK | TCONT_VLD_LO,
				 FIELD_PREP(TCONT_ID_LO_MASK, id) |
				 (valid ? TCONT_VLD_LO : 0));
		return 0;
	}

	mac_wr(x, G_TCONT_ID_16_31_CFG, TCONT_CMD_WRITE |
		FIELD_PREP(TCONT_ID_INDEX_MASK, idx - 16) |
		FIELD_PREP(TCONT_WR_ID_MASK, id) | (valid ? TCONT_WR_VLD : 0));

	return read_poll_timeout_atomic(mac_rr, val, val & TCONT_CMD_DONE,
					1, 3000, false, x, G_TCONT_ID_16_31_STS);
}

/* gponDevGemMibTablesInit() */
static int gpon_mib_tables_init(struct an7552_mac *x)
{
	u32 val;

	mac_wr(x, G_MIB_TBL_INIT, TBL_INIT_START);
	mac_wr(x, G_GPIDX_TBL_INIT, TBL_INIT_START);

	return read_poll_timeout_atomic(mac_rr, val,
		(val & TBL_INIT_DONE) &&
		(mac_rr(x, G_GPIDX_TBL_INIT) & TBL_INIT_DONE),
		1000, 3000, false, x, G_MIB_TBL_INIT);
}

/* gpon_INT_init() / gpon_dev_init() */
static void gpon_dev_init(struct an7552_mac *x)
{
	gpon_select_wan(x);

	mac_wr(x, G_INT_STATUS, 0xffffffff);
	mac_wr(x, G_INT_ENABLE, GPON_INT_ENABLE_DEFAULT);

	/* software dying gasp (gpon_init_dying_info(): isGponHwFlag = 0) */
	mac_rmw(x, DBG_US_DYING_GASP_CTRL, DG_HW_EN, 0);
	mac_rmw(x, DBG_IDLE_GEM_THLD, IDLE_GEM_THLD_MASK,
		 GPON_IDLE_GEM_THLD_DEFAULT);
	/* counter type GEM (0) */
	mac_rmw(x, DBG_CAP_SETTING, CAP_MIB_FRAME_TYPE, 0);
	gpon_set_dba_block_size(x, GPON_DBA_BLOCK_SIZE);
	/* modify_sleep_count() */
	mac_wr(x, SLEEP_CNT, GPON_SLEEP_CNT_DEFAULT);
	mac_rmw(x, SLEEP_GLB_CFG, 0, SLEEP_RELOAD_CNT);
	mac_rmw(x, G_TOD_CLK_PERIOD, TOD_PERIOD_MASK, GPON_TOD_CLK_PERIOD);
	mac_rmw(x, G_RSP_TIME, RSP_TIME_MASK, x->rsp_time);
	mac_rmw(x, DBG_DLY, DLY_FINE_INT_MASK,
		 FIELD_PREP(DLY_FINE_INT_MASK, GPON_FINE_INT_DLY_DEFAULT));
	mac_rmw(x, DBG_SW_RDI_CTRL, RDI_NUM_MASK,
		 FIELD_PREP(RDI_NUM_MASK, GPON_RDI_SEND_NUM_DEFAULT));
	/* gponDevSetRdiErrCtrlSwitch(0xf8): bits 7..3 of the byte -> [31:27] */
	mac_rmw(x, DBG_RDI_ERR_CTRL, RDI_ERR_EN_MASK,
		 FIELD_PREP(RDI_ERR_EN_MASK, GPON_RDI_ERR_EN_DEFAULT >> 3));
	mac_rmw(x, DBG_RDI_ERR_CTRL, RDI_ERR_THLD_MASK,
		 GPON_RDI_ERR_THLD_DEFAULT);

	if (x->mode == AN7552_XPON_GPON_SYM) {
		mac_rmw(x, DBG_CAP_SETTING, CAP_MAX_RDM_DLY_MASK,
			 GPON_SYM_MAX_RDM_DLY);
		mac_rmw(x, DBG_US_PREF_OFS, US_DT_PREF_OFS_MASK,
			 GPON_SYM_DT_PREF_OFS);
		mac_rmw(x, DBG_DLY, DLY_PHY_TX_MASK, GPON_SYM_PHY_TX_DLY);
		x->rsp_time = GPON_RSP_TIME_SYM;
		mac_rmw(x, G_RSP_TIME, RSP_TIME_MASK, x->rsp_time);
	}
}

/* ---------------------------------------------------------------------
 * PLOAM transport
 * ------------------------------------------------------------------- */

/*
 * One message is three FIFO writes; the dying gasp interrupt writes the same
 * FIFO, so a message goes in whole under the register lock or not at all.
 */
static void gpon_ploam_write(struct an7552_mac *x, const u8 *msg)
{
	unsigned long flags;
	int i;

	spin_lock_irqsave(&x->reg_lock, flags);
	for (i = 0; i < 3; i++)
		mac_wr(x, G_PLOAMU_WDATA, get_unaligned_be32(&msg[i * 4]));
	spin_unlock_irqrestore(&x->reg_lock, flags);
}

/*
 * gponDevSendPloamMsg(); after a Dying_Gasp only Dying_Gasp goes out
 * (gponDevSendPloamMsg() with gp_dying_info[3] set).
 */
static void gpon_ploam_send(struct an7552_mac *x, u8 *msg, int times)
{
	int i;

	if (READ_ONCE(x->dying))
		return;

	msg[0] = x->onu_id;
	for (i = 0; i < times; i++)
		gpon_ploam_write(x, msg);
	x->stats.ploam_tx++;
	an7552_mac_dbg(x, AN7552_DBG_ACT, "ploam tx %s x%d: %*ph\n",
		 gpon_ploam_name(gpon_ploamu_name, ARRAY_SIZE(gpon_ploamu_name),
				 msg[1]), times, GPON_PLOAM_LEN, msg);
}

static void gpon_ploam_ack(struct an7552_mac *x, const u8 *dm)
{
	u8 msg[GPON_PLOAM_LEN] = { 0, PLOAMU_ACKNOWLEDGE, dm[1] };

	memcpy(&msg[3], dm, 9);
	gpon_ploam_send(x, msg, PLOAMU_ACK_TIMES);
}

/* gponDevGetPloamMsg(): returns the FIFO fill before the read */
static int gpon_ploam_read(struct an7552_mac *x, u8 *msg)
{
	u32 used = FIELD_GET(PLOAMD_FIFO_USED_MASK,
			     mac_rr(x, G_PLOAMD_FIFO_STS));
	int i;

	if (!used)
		return 0;
	if (used < 3)
		return -EFAULT;

	for (i = 0; i < 3; i++)
		put_unaligned_be32(mac_rr(x, G_PLOAMD_RDATA), &msg[i * 4]);

	return used;
}

/* ---------------------------------------------------------------------
 * Activation (gpon_act.c)
 * ------------------------------------------------------------------- */

static void gpon_set_alarm(struct an7552_mac *x, enum gpon_alarm a)
{
	if (x->alarms & BIT(a))
		return;
	x->alarms |= BIT(a);
	an7552_mac_genl_state(x);
}

static void gpon_clear_alarm(struct an7552_mac *x, enum gpon_alarm a)
{
	if (!(x->alarms & BIT(a)))
		return;
	x->alarms &= ~BIT(a);
	an7552_mac_genl_state(x);
}

/*
 * Faults the optical front end reports (xpon_alarm()).
 *
 * @alarm is a mask and any number of bits may come in one call, so each is
 * taken on its own rather than one of them winning: a front end that raises
 * LOS and TX_FAULT together has both, and both are recorded and acted on.
 *
 * LOS runs gpon_loss_of_light(), the same sequence the PHY's own LOS event
 * runs.  That is not an attempt at sharing code for its own sake: there are two
 * light detectors and either can see this first - TRANS_LOS in the SerDes, and
 * the comparator in the front end - and the ONU state cannot depend on which
 * one got there first.  Whichever arrives second finds the state already O1 or
 * O6 and has nothing left to do.  The transmitter therefore goes off the way
 * the PHY event switches it off, through gpon_disable() and the reset work, and
 * not from here: gpon_dev_reset() is the one that calls
 * an7552_trans_power_switch(), and a second regulator_disable() for a single
 * enable is a count the regulator core will complain about.
 *
 * TX_FAULT leaves the link up, because the light is still there; it records the
 * DACT and sends an RDI, and clearing the latch is
 * an7552_phy_tx_fault_reset()'s business, not this one's.
 *
 * [lock] takes x->lock itself, and every other caller of gpon_set_alarm()
 * does the same.  Nothing here runs in atomic context: the front end calls out
 * of its own work, and an7552_mac_genl_state() below already allocates.
 */
void an7552_mac_laser_fault(struct an7552_mac *x, u32 alarm)
{
	if (!alarm)
		return;

	mutex_lock(&x->lock);

	if (alarm & PON_LASER_ALARM_LOS) {
		gpon_set_alarm(x, GPON_ALM_LOS);
		gpon_set_alarm(x, GPON_ALM_RDI);
		gpon_loss_of_light(x);
	}

	if (alarm & PON_LASER_ALARM_TX_FAULT) {
		gpon_set_alarm(x, GPON_ALM_DACT);
		gpon_set_alarm(x, GPON_ALM_RDI);
	}

	mutex_unlock(&x->lock);

	/*
	 * LOS may have dropped the carrier, and the carrier is what tells the
	 * front end whether there is anything to send.  Handed to the workqueue
	 * for the reason gpon_set_state() gives: the update reaches the front
	 * end, and the front end calls back up into this MAC from its own timers
	 * with its own lock held, so running it inline would be here the call
	 * arrives from.  queue_work() collapses the repeat, and the O5 branch
	 * above has queued the same work already.
	 */
	queue_work(x->wq, &x->carrier_work);
}

/*
 * mbi_ctrl_set(): stop the transmit MBI around an optical front end DAC write.
 * See xpon_burst_ctrl() in <linux/pon_laser.h> for why the front end needs it.
 */
void an7552_mac_burst_ctrl(struct an7552_mac *x, bool enable)
{
	unsigned long flags;
	u32 v;

	spin_lock_irqsave(&x->reg_lock, flags);
	v = mac_rr(x, G_MBI_MPI_STOP);
	/* a stop of the MAC's own (O7, a reset in progress) stays */
	if (enable && !x->mbi_tx_stopped)
		v &= ~MBI_TX_STOP;
	else
		v |= MBI_TX_STOP;
	/* written twice as mbi_ctrl_set() of the stock PHY driver does (the second
	 * after the interrupt state is restored there); kept as it is */
	mac_wr(x, G_MBI_MPI_STOP, v);
	mac_wr(x, G_MBI_MPI_STOP, v);
	spin_unlock_irqrestore(&x->reg_lock, flags);
}

static void gpon_extend_preamble(struct an7552_mac *x, u32 mask, u8 t3_o4,
				 u8 t3_o5, u8 ext_bur, u8 oper_rang)
{
	struct an7552_phy_preamble p = {
		.mask = mask,
		.t3_o4_preamble = t3_o4,
		.t3_o5_preamble = t3_o5,
		.extend_burst_mode = ext_bur,
		.oper_ranged_st = oper_rang,
	};

	an7552_phy_gpon_extend_preamble(x->phy, &p);
}

/* the extended burst preamble, in the PHY and in the MAC (PLOU_EBL_EN) */
static void gpon_set_ext_burst(struct an7552_mac *x, u8 o3, u8 o5)
{
	gpon_extend_preamble(x, PHY_T3_O4_PRE_EN | PHY_T3_O5_PRE_EN |
			     PHY_EXT_BUR_MODE_EN | PHY_OPER_RANG_EN,
			     o3, o5, 1, 2);
	mac_rmw(x, G_PLOU_PRMBL_TYPE3,
		 PLOU_EBL_EN | PLOU_PRMB3_O5_MASK | PLOU_PRMB3_O3_O4_MASK,
		 PLOU_EBL_EN | FIELD_PREP(PLOU_PRMB3_O5_MASK, o5) |
		 FIELD_PREP(PLOU_PRMB3_O3_O4_MASK, o3));
}

/* gpon_act_change_gpon_state() */
static void gpon_set_state(struct an7552_mac *x, enum an7552_gpon_state st)
{
	enum an7552_gpon_state old = x->state;

	lockdep_assert_held(&x->lock);

	/* gpon_act_change_gpon_state(): silence keeps the ONU in O2 */
	if (x->silence && old == GPON_O2 && st == GPON_O3) {
		an7552_mac_dbg(x, AN7552_DBG_ACT, "O2 -> O3 refused: silence\n");
		return;
	}

	switch (st) {
	case GPON_O5:
		gpon_clear_alarm(x, GPON_ALM_DACT);
		gpon_extend_preamble(x, PHY_OPER_RANG_EN, 0, 0, 0, 3);
		/*
		 * gpon_act_change_gpon_state(), on every entry into O5, calls
		 * gponDevSetFilterExtBurstLengthPLOAM(1) and then
		 * gponDevSetFilterUpstreamOverheadPLOAM(1)
		 * (xpon.ann:0x13830-0x1383c).  Both land in the same register,
		 * DBG_PLOAMD_FILTER_IN_O5: the first sets bit 8, the second
		 * bit 0 (xpon.ann:0x20fec-0x20ff4 and 0x20f0c-0x20f14).  There
		 * is no default to fall back on - the register is only ever
		 * written from here - so both bits are set below.
		 *
		 * Both handlers reject anything outside O2/O3 regardless
		 * (ploam_upstream_overhead, ploam_ext_burst_length), which is
		 * why going without this has gone unnoticed: the filter is
		 * redundant while the state machine is where those messages are
		 * legal.  It is the hardware guard for the window in between.
		 */
		mac_rmw(x, DBG_PLOAMD_FILTER_IN_O5,
			FILTER_US_OVERHEAD_O5 | FILTER_EXT_BST_LEN_O5,
			FILTER_US_OVERHEAD_O5 | FILTER_EXT_BST_LEN_O5);
		break;
	case GPON_O3:
	case GPON_O4:
		gpon_extend_preamble(x, PHY_OPER_RANG_EN, 0, 0, 0, 2);
		break;
	case GPON_O2:
		gpon_extend_preamble(x, PHY_T3_O4_PRE_EN | PHY_T3_O5_PRE_EN |
				     PHY_EXT_BUR_MODE_EN | PHY_OPER_RANG_EN,
				     0, 0, 0, 0);
		an7552_phy_set_bit_delay(x->phy, 0);
		break;
	default:
		break;
	}

	mac_rmw(x, G_ACTIVATION_ST, ACT_ST_MASK, st);
	x->state = st;
	if (old != st) {
		an7552_mac_dbg(x, AN7552_DBG_ACT,
			 "state O%d -> O%d (onu_id %u eqd 0x%08x alarms 0x%08x)\n",
			 old, st, x->onu_id, x->eqd, x->alarms);
		an7552_led_gpon_state(x);
	}

	if (st == GPON_O5) {
		x->activations++;
		x->to1_left = GPON_TO1_MAX_CNT;
		x->bip_total = 0;
		x->bip_pending = 0;
		an7552_phy_counter_clear(x->phy, AN7552_PHY_BIP_CNT_CLR);
	}

	cancel_delayed_work(&x->to1_work);
	cancel_delayed_work(&x->to2_work);
	if (st == GPON_O3 || st == GPON_O4)
		queue_delayed_work(x->wq, &x->to1_work,
				   msecs_to_jiffies(x->to1_ms));
	else if (st == GPON_O6)
		queue_delayed_work(x->wq, &x->to2_work,
				   msecs_to_jiffies(x->to2_ms));

	if (old != st) {
		dev_dbg(x->dev, "O%d -> O%d\n", old, st);
		an7552_mac_genl_state(x);
		/*
		 * Not inline: an7552_mac_carrier() reaches the optical front end,
		 * and the front end calls back up into the MAC from its own timers
		 * with its own lock held.  Calling it with x->lock held is the
		 * order an7552_mac_set_mode() documents as the deadlock, so the
		 * update is handed to the workqueue and runs with the lock dropped.
		 * queue_work() collapses a repeat, and gpon_set_state() only asks
		 * when the state actually changed, so the last one wins.
		 */
		queue_work(x->wq, &x->carrier_work);
	}
}

/* ---------------------------------------------------------------------
 * T-CONT and GEM tables (gpon_wan.c)
 * ------------------------------------------------------------------- */

static int gpon_tcont_find(struct an7552_mac *x, u16 alloc_id)
{
	int i;

	for (i = 0; i < GPON_MAX_TCONT; i++)
		if ((x->tcont_valid & BIT(i)) && x->tcont_alloc[i] == alloc_id)
			return i;

	return -ENOENT;
}

static void gpon_txbuf_init(struct an7552_mac *x)
{
	airoha_xpon_set_txbuf_threshold(true, GPON_TXBUF_CHAN_THR,
					GPON_TXBUF_TOTAL_THR);
}

/*
 * gwan_update_gem_mib_table(): the HW GEM MIB is indexed through a table
 * of running counts of valid GEM ports, one entry per 16 port ids.  The SDK
 * rebuilds it 2 s after the GEM set changes.
 */
static int gpon_gpidx_write(struct an7552_mac *x, u8 addr, u16 val)
{
	u32 sts;

	mac_wr(x, G_GPIDX_TBL_CTRL, GPIDX_CMD_WRITE |
		FIELD_PREP(GPIDX_ADDR_MASK, addr) |
		FIELD_PREP(GPIDX_WDATA_MASK, val));

	return read_poll_timeout_atomic(mac_rr, sts, sts & GPIDX_CMD_DONE,
					1, 3, false, x, G_GPIDX_TBL_STS);
}

static void gpon_mib_table_update(struct an7552_mac *x)
{
	unsigned int i, addr = 1;
	u16 count = 0;

	gpon_gpidx_write(x, 0, 0);
	for (i = 0; i < GPON_MAX_GEM_ID; i++) {
		if (x->gem[i].valid)
			count++;
		if ((i & 0xf) == 0xf && addr != 0x100)
			gpon_gpidx_write(x, addr++, count);
	}
}

static void gpon_mib_work(struct work_struct *w)
{
	struct an7552_mac *x = container_of(to_delayed_work(w),
					     struct an7552_mac, mib_work);

	guard(mutex)(&x->lock);
	gpon_mib_table_update(x);
}

static void gpon_mib_schedule(struct an7552_mac *x)
{
	mod_delayed_work(x->wq, &x->mib_work, 2 * HZ);
}

/* gponDevGetGemPortCounter(): type 0..3, 64 bit, optional read-clear */
static int gpon_gem_counter(struct an7552_mac *x, u16 gem, u8 type,
			    bool clear, u64 *val)
{
	u32 sts;
	int ret;

	if (gem >= GPON_MAX_GEM_ID)
		return -EINVAL;

	mac_wr(x, G_MIB_CTRL_STS, FIELD_PREP(MIB_GPID_MASK, gem) |
		FIELD_PREP(MIB_TYPE_MASK, type) | (clear ? MIB_READ_CLR : 0));
	ret = read_poll_timeout_atomic(mac_rr, sts, sts & MIB_CMD_DONE,
				       1000, 3000, false, x, G_MIB_CTRL_STS);
	if (ret)
		return ret;

	*val = (u64)mac_rr(x, G_MIB_RDATA_H32) << 32 |
	       mac_rr(x, G_MIB_RDATA_L32);

	return 0;
}

int an7552_mac_gem_counters(struct an7552_mac *x, u16 gem, bool clear,
			     u64 cnt[GPON_GEM_CNT_MAX])
{
	int i, ret = 0;

	guard(mutex)(&x->lock);
	if (gem >= GPON_MAX_GEM_ID)
		ret = -EINVAL;
	else if (!x->gem[gem].valid)
		ret = -ENOENT;
	for (i = 0; !ret && i < GPON_GEM_CNT_MAX; i++)
		ret = gpon_gem_counter(x, gem, i, clear, &cnt[i]);

	return ret;
}

/*
 * gwan_create_new_tcont(): the ONU-ID alloc maps to channel 0 without a
 * table write (gponDevEnableTCont()); any other alloc takes the first free
 * index 1..31.
 */
static int gpon_tcont_create(struct an7552_mac *x, u16 alloc_id)
{
	int idx, i, ret;

	idx = gpon_tcont_find(x, alloc_id);
	if (idx >= 0)
		return idx;

	if (alloc_id == x->onu_id) {
		idx = 0;
	} else {
		for (idx = 1; idx < GPON_MAX_TCONT; idx++)
			if (!(x->tcont_valid & BIT(idx)))
				break;
		if (idx == GPON_MAX_TCONT)
			return -ENOSPC;
		ret = gpon_hw_tcont(x, idx, true, alloc_id);
		if (ret)
			return ret;
	}

	x->tcont_alloc[idx] = alloc_id;
	x->tcont_valid |= BIT(idx);
	for (i = 0; i < GPON_MAX_GEM_ID; i++)
		if (x->gem[i].valid && x->gem[i].alloc_id == alloc_id &&
		    x->gem[i].channel != GPON_MULTICAST_CHANNEL)
			WRITE_ONCE(x->gem[i].channel, idx);
	/* PPE upstream flows carry the channel of their GEM port */
	an7552_mac_ppe_flush(x);

	airoha_xpon_set_channel(idx, true);
	if (alloc_id == x->onu_id)
		airoha_xpon_set_channel_nonblocking(idx, false);

	/*
	 * The count belongs here, not at the call sites: a T-CONT can be
	 * created by PLOAM (Assign_Alloc-ID) as well as by OMCI, and both
	 * went through this function.  Counting only one of them left the
	 * QDMA TX buffer threshold sized for one channel while up to 32
	 * were open.  Channel 0 is the ONU-ID T-CONT and is always there, so
	 * the count starts at 1.
	 */
	if (idx)
		x->tcont_cnt = min_t(unsigned int, x->tcont_cnt + 1,
				     GPON_MAX_TCONT);

	return idx;
}

/* gwan_remove_tcont() */
static int gpon_tcont_remove(struct an7552_mac *x, u16 alloc_id)
{
	int idx = gpon_tcont_find(x, alloc_id);
	int i;

	if (idx < 0)
		return idx;

	x->tcont_valid &= ~BIT(idx);
	x->tcont_alloc[idx] = GPON_UNASSIGN_ALLOC_ID;
	/* no upstream for GEM ports of a removed T-CONT until it is back */
	for (i = 0; i < GPON_MAX_GEM_ID; i++)
		if (x->gem[i].valid && x->gem[i].channel == idx)
			WRITE_ONCE(x->gem[i].channel, GPON_UNKNOWN_CHANNEL);
	/* PPE upstream flows must not go on into the retired channel */
	an7552_mac_ppe_flush(x);
	airoha_xpon_set_channel(idx, false);
	if (idx) {
		gpon_hw_tcont(x, idx, false, GPON_UNASSIGN_ALLOC_ID);
		x->tcont_cnt = max(x->tcont_cnt - 1, 1U);
	}

	return 0;
}

static void gpon_tcont_remove_all(struct an7552_mac *x)
{
	int i;

	for (i = GPON_MAX_TCONT - 1; i >= 0; i--)
		if (x->tcont_valid & BIT(i))
			gpon_tcont_remove(x, x->tcont_alloc[i]);
	for (i = 1; i < GPON_MAX_TCONT; i++)
		gpon_hw_tcont(x, i, false, GPON_UNASSIGN_ALLOC_ID);
	/* stop_omci_oam_monitor(); gpWanPriv tcont count back to 1 */
	x->tcont_cnt = 1;
	x->cfg_done = false;
}

/* gwan_create_new_gemport() */
static int __gpon_gem_add(struct an7552_mac *x, u16 gem, u16 alloc_id,
			  bool encrypt, bool mcast)
{
	struct an7552_gpon_gem *g;
	int idx;

	if (gem >= GPON_MAX_GEM_ID)
		return -EINVAL;

	g = &x->gem[gem];

	if (mcast) {
		idx = GPON_MULTICAST_CHANNEL;
	} else {
		idx = gpon_tcont_find(x, alloc_id);
		if (idx < 0)
			idx = GPON_UNKNOWN_CHANNEL;
	}

	g->alloc_id = alloc_id;
	WRITE_ONCE(g->channel, idx);
	g->encrypt = encrypt || test_bit(gem, x->gem_encrypt_req);
	atomic64_set(&g->rx_packets, 0);
	atomic64_set(&g->rx_bytes, 0);
	atomic64_set(&g->tx_packets, 0);
	atomic64_set(&g->tx_bytes, 0);
	WRITE_ONCE(g->valid, true);
	gpon_mib_schedule(x);
	/* gwan_create_new_gemport(): change_pon_led_status(ALARM_LED_ON) */
	if (gem != x->omcc)
		an7552_led_set(&x->led_pon, XPON_LED_ON);

	return gpon_hw_gem_sync(x, gem);
}

static void gpon_us_rules_drop_gem(struct an7552_mac *x, u16 gem)
{
	int i;

	for (i = 0; i < GPON_MAX_US_RULES; i++)
		if (x->us_rule[i].valid && x->us_rule[i].gem == gem)
			WRITE_ONCE(x->us_rule[i].valid, false);
	if (x->us_gem == gem)
		WRITE_ONCE(x->us_gem, GPON_UNASSIGN_GEM_ID);
}

static void __gpon_gem_del(struct an7552_mac *x, u16 gem, bool hw)
{
	if (gem >= GPON_MAX_GEM_ID || !x->gem[gem].valid)
		return;
	WRITE_ONCE(x->gem[gem].valid, false);
	gpon_us_rules_drop_gem(x, gem);
	an7552_mac_ppe_flush(x);
	if (hw)
		gpon_hw_gem(x, gem, false, false);
	gpon_mib_schedule(x);
}

int an7552_mac_gem_add(struct an7552_mac *x, u16 gem, u16 alloc_id,
			bool mcast)
{
	int ret;

	mutex_lock(&x->lock);
	if (gem >= GPON_MAX_GEM_ID || gem == x->omcc) {
		ret = -EINVAL;
	} else if (x->gem[gem].valid) {
		/* xmcs_create_gem_port(): existing port, only rebind */
		ret = 0;
		x->gem[gem].alloc_id = alloc_id;
		if (mcast) {
			WRITE_ONCE(x->gem[gem].channel, GPON_MULTICAST_CHANNEL);
		} else {
			int idx = gpon_tcont_find(x, alloc_id);

			WRITE_ONCE(x->gem[gem].channel,
				   idx < 0 ? GPON_UNKNOWN_CHANNEL : idx);
		}
		/* a port that became, or stopped being, multicast */
		ret = gpon_hw_gem_sync(x, gem);
	} else {
		ret = __gpon_gem_add(x, gem, alloc_id, false, mcast);
	}
	mutex_unlock(&x->lock);
	an7552_mac_carrier(x);

	return ret;
}

/*
 * GPON_CMD_MCAST_RX: gpond takes the multicast GEM ports out of the
 * hardware GEM table while nothing on this side listens to a group, so the
 * multicast of the other ONUs on the tree (a multicast GEM port reaches
 * every ONU) is dropped in the MAC instead of going through the QDMA, NAPI
 * and the stack to be dropped there.  The ports stay valid in the software
 * table (OMCI, counters, debugfs); the setting holds for ports created
 * later as well.
 */
int an7552_mac_mcast_rx(struct an7552_mac *x, u16 on)
{
	int i, ret = 0;

	mutex_lock(&x->lock);
	if (x->mcast_rx_off != !on) {
		x->mcast_rx_off = !on;
		for (i = 0; i < GPON_MAX_GEM_ID; i++) {
			int err;

			if (!x->gem[i].valid ||
			    x->gem[i].channel != GPON_MULTICAST_CHANNEL)
				continue;
			err = gpon_hw_gem_sync(x, i);
			if (err && !ret)
				ret = err;
		}
		an7552_mac_dbg(x, AN7552_DBG_MAC, "multicast GEM rx %s\n",
			       on ? "on" : "off");
	}
	mutex_unlock(&x->lock);

	return ret;
}

int an7552_mac_gem_del(struct an7552_mac *x, u16 gem)
{
	int ret = 0;

	mutex_lock(&x->lock);
	if (gem >= GPON_MAX_GEM_ID || gem == x->omcc)
		ret = -EINVAL;
	else if (!x->gem[gem].valid)
		ret = -ENOENT;
	else
		__gpon_gem_del(x, gem, true);
	mutex_unlock(&x->lock);
	an7552_mac_carrier(x);

	return ret;
}

/* gwan_remove_all_gemport(): every GEM port but the OMCC one */
static void __gpon_gem_flush(struct an7552_mac *x)
{
	int i;

	for (i = 0; i < GPON_MAX_GEM_ID; i++)
		if (i != x->omcc)
			__gpon_gem_del(x, i, true);
	gpon_mib_table_update(x);
}

int an7552_mac_gem_flush(struct an7552_mac *x)
{
	mutex_lock(&x->lock);
	__gpon_gem_flush(x);
	mutex_unlock(&x->lock);
	an7552_mac_carrier(x);

	return 0;
}

/*
 * xmcs_create_tcont_info() / xmcs_remove_tcont_info(): T-CONT requested by
 * OMCI (ME 262 Alloc-ID); the ONU-ID alloc stays owned by PLOAM.
 */
int an7552_mac_tcont_add(struct an7552_mac *x, u16 alloc_id)
{
	int ret = 0;

	guard(mutex)(&x->lock);
	if (alloc_id != x->onu_id && gpon_tcont_find(x, alloc_id) < 0) {
		ret = gpon_tcont_create(x, alloc_id);
		if (ret >= 0) {
			/* resized only once the configuration settled */
			if (x->cfg_done)
				an7552_mac_reset_txbuf(x->tcont_cnt);
			ret = 0;
		}
	}

	return ret;
}

int an7552_mac_tcont_del(struct an7552_mac *x, u16 alloc_id)
{
	int ret;

	guard(mutex)(&x->lock);
	ret = alloc_id == x->onu_id ? -EPERM : gpon_tcont_remove(x, alloc_id);
	if (!ret)
		an7552_mac_reset_txbuf(x->tcont_cnt);

	return ret;
}

/*
 * gponDevSetNewTod() (non-EN7581 branch): the OLT time of @superframe
 * reaches the ONU (EqD / 1.24416 Gbit/s + 35 us response time) *
 * n1490 / (n1310 + n1490) later.  On a nanosecond borrow the SDK lets the
 * value wrap; the 1e9 carry of its EN7581 branch is used instead.
 */
int an7552_mac_set_tod(struct an7552_mac *x, u32 superframe, u32 sec,
			u32 nsec)
{
	const u64 n1310 = 14677, n1490 = 14682, rsp_ns = 35000, up = 124416;
	u64 d1, q, r;
	u32 delta;

	if (superframe > TOD_SPF_CNT_MASK || nsec >= NSEC_PER_SEC)
		return -EINVAL;

	guard(mutex)(&x->lock);
	d1 = (u64)x->eqd * 100000;
	q = div64_u64_rem(d1, up, &r);
	delta = div64_u64((q + rsp_ns) * n1490, n1310 + n1490) +
		div64_u64(r * n1490, up * (n1310 + n1490));
	if (delta > nsec) {
		sec--;
		nsec = nsec + NSEC_PER_SEC - delta;
	} else {
		nsec -= delta;
	}

	mac_wr(x, G_TOD_CFG, FIELD_PREP(TOD_SPF_CNT_MASK, superframe));
	mac_wr(x, G_NEW_TOD_SEC_L32, sec);
	mac_wr(x, G_NEW_TOD_NANO_SEC, nsec);
	/*
	 * Arm the one-shot that reports the new ToD taking effect.
	 *
	 * tod_update_done_int_en is deliberately not in GPON_INT_ENABLE_DEFAULT,
	 * unlike in gpon_INT_init(): the driver owns that bit rather than leaving
	 * it live, because the report is an event and not a level.  Writing
	 * G_NEW_TOD_* above only queues the value - it becomes the current time
	 * at the next 1PPS boundary - so nothing here can be read back to confirm
	 * it.  The hardware says so instead, and gpon_tod_irq() below turns that
	 * into the first 1PPS after the change, which is the first edge on which
	 * G_CUR_TOD already holds the new value.
	 *
	 * The status is cleared before the enable, so the one-shot cannot fire on
	 * the write itself.  mac_rmw() takes the mask to set first and the mask to
	 * clear second - the two are not interchangeable, and swapping them
	 * leaves this interrupt permanently disabled, which is what the line used
	 * to do.
	 */
	x->tod_update = false;
	mac_wr(x, G_INT_STATUS, INT_TOD_UPDATE_DONE);
	mac_rmw(x, G_INT_ENABLE, INT_TOD_UPDATE_DONE, 0);

	return 0;
}

/* gponDevGetCurrentTod() */
void an7552_mac_get_tod(struct an7552_mac *x, u32 *sec, u32 *nsec)
{
	*sec = mac_rr(x, G_CUR_TOD_SEC_L32);
	*nsec = mac_rr(x, G_CUR_TOD_NANO_SEC);
}

static void gpon_eqd_adjust(struct an7552_mac *x, u32 new_eqd);

/* gponDevSet1PPSInt(); the status bit is W1C */
static void gpon_set_1pps_int(struct an7552_mac *x, bool on)
{
	mac_wr(x, G_INT_STATUS, INT_TOD_1PPS);
	mac_rmw(x, G_INT_ENABLE, INT_TOD_1PPS, on ? INT_TOD_1PPS : 0);
}

/*
 * gpon_isr() GPON_INT_TOD part (gpon.c:956-975): the update is acknowledged,
 * and the next 1PPS edge is what reports the current ToD
 * (gpon_update_time_to_switch()).
 *
 * Both bits belong to the driver rather than to GPON_INT_ENABLE_DEFAULT, and
 * each is turned off again here, so neither is a level that can re-fire.  The
 * ISR clears G_INT_STATUS with whatever it took, and only what G_INT_ENABLE
 * allows can arrive at all, so a bit left enabled here would be one stray
 * report per hardware assertion rather than a storm - but the enable that
 * armed the one-shot is turned off because that is what the one is.
 */
static void gpon_tod_irq(struct an7552_mac *x, u32 status)
{
	u32 sec, nsec;

	if ((status & INT_TOD_1PPS) && x->tod_update) {
		x->tod_update = false;
		gpon_set_1pps_int(x, false);
		an7552_mac_get_tod(x, &sec, &nsec);
		an7552_mac_genl_tod(x, sec, nsec);
	}
	if (status & INT_TOD_UPDATE_DONE) {
		x->tod_update = true;
		mac_rmw(x, G_INT_ENABLE, 0, INT_TOD_UPDATE_DONE);
		gpon_set_1pps_int(x, true);
	}
}

/* gponDevSwResync() */
void an7552_mac_sw_resync(struct an7552_mac *x)
{
	guard(mutex)(&x->lock);
	mac_wr(x, DBG_SW_RESYNC, SW_RESYNC_EN);
	udelay(1);
	mac_wr(x, DBG_SW_RESYNC, SW_RESYNC_EN | SW_RESYNC);
	/* GPON_MAC_SW_RESYNC_INTERVAL_US */
	usleep_range(1000, 1100);
	/* the SDK keeps dbg_sw_resync set in the last write */
	mac_wr(x, DBG_SW_RESYNC, SW_RESYNC);
}

/* gpon_dvt_eqd_adjustment(eqd): applied at once in O5 */
void an7552_mac_set_eqd_o5_offset(struct an7552_mac *x, u8 offset)
{
	guard(mutex)(&x->lock);
	x->eqd_o5_offset = offset;
	if (x->state == GPON_O5)
		gpon_eqd_adjust(x, x->eqd);
}

/* gponDevGetSuperframe(): read-only, no lock of its own */
u32 an7552_mac_get_superframe(struct an7552_mac *x)
{
	return mac_rr(x, DBG_DS_SPF_CNT) & DS_SPF_CNT_MASK;
}

/* xpon_reset_qdma_tx_buf() for HIR 0xc/0xf */
static void an7552_mac_reset_txbuf(unsigned int n)
{
	n = clamp(n, 1U, 32U);
	if (n < 9)
		airoha_xpon_set_txbuf_threshold(true, 0x20, n * 0x2f);
	else
		airoha_xpon_set_txbuf_threshold(true, 8, n * 0x17 - 0x16);
}

/*
 * monitor_done(): the SDK watches the OMCI rate and declares the
 * configuration done once it calms down; here the OMCI daemon says so.
 * With more than one T-CONT the QDMA TX buffers are resized.
 */
int an7552_mac_cfg_done(struct an7552_mac *x)
{
	guard(mutex)(&x->lock);
	if (x->state == GPON_O5 && !x->cfg_done) {
		x->cfg_done = true;
		if (x->tcont_cnt > 1)
			an7552_mac_reset_txbuf(x->tcont_cnt);
		an7552_mac_genl_state(x);
	}

	return 0;
}

/*
 * monitor_done() reports traffic up (xmcs_report_event(1, 0xc, 1)) when
 * is_really_up: any valid GEM port besides the OMCC.  Called with
 * x->lock held.
 */
bool an7552_mac_traffic_up(struct an7552_mac *x)
{
	int i;

	if (!x->cfg_done)
		return false;
	for (i = 0; i < GPON_MAX_GEM_ID; i++)
		if (x->gem[i].valid && i != x->omcc)
			return true;

	return false;
}

/*
 * xmcs_get_tcont_counter(): the sum of the counters of the GEM ports on
 * @alloc_id; @clear clears those GEM counters.
 */
int an7552_mac_tcont_counters(struct an7552_mac *x, u16 alloc_id,
			       bool clear, u64 cnt[GPON_GEM_CNT_MAX])
{
	int i, j, ret;
	u64 v;

	memset(cnt, 0, sizeof(u64) * GPON_GEM_CNT_MAX);
	guard(mutex)(&x->lock);
	ret = gpon_tcont_find(x, alloc_id) < 0 ? -ENOENT : 0;
	for (i = 0; !ret && i < GPON_MAX_GEM_ID; i++) {
		if (!x->gem[i].valid || x->gem[i].alloc_id != alloc_id ||
		    x->gem[i].channel == GPON_MULTICAST_CHANNEL)
			continue;
		for (j = 0; !ret && j < GPON_GEM_CNT_MAX; j++) {
			ret = gpon_gem_counter(x, i, j, clear, &v);
			cnt[j] += v;
		}
	}

	return ret;
}

static int an7552_mac_tcont_channel(struct an7552_mac *x, u16 alloc_id)
{
	int idx;

	guard(mutex)(&x->lock);
	idx = gpon_tcont_find(x, alloc_id);

	return idx;
}

static void gpon_gem_remove_all(struct an7552_mac *x)
{
	int i;

	for (i = 0; i < GPON_MAX_GEM_ID; i++)
		__gpon_gem_del(x, i, true);
	bitmap_zero(x->gem_encrypt_req, GPON_MAX_GEM_ID);
}

/* ---- upstream classification: VLAN/PCP -> GEM, PCP -> queue ---- */

int an7552_mac_us_rule_set(struct an7552_mac *x, unsigned int idx,
			    u16 vid, u8 pcp_mask, u16 gem)
{
	struct an7552_us_rule *r;
	int ret = 0;

	if (idx >= GPON_MAX_US_RULES || vid > GPON_US_VID_ANY)
		return -EINVAL;

	guard(mutex)(&x->lock);
	r = &x->us_rule[idx];
	if (gem >= GPON_MAX_GEM_ID || !x->gem[gem].valid || gem == x->omcc) {
		ret = -ENOENT;
	} else {
		/*
		 * an7552_mac_classify() reads the fields after an acquire of
		 * valid; the release makes them visible to it first
		 */
		WRITE_ONCE(r->valid, false);
		smp_wmb();
		r->vid = vid;
		r->pcp_mask = pcp_mask;
		r->gem = gem;
		smp_store_release(&r->valid, true);
		an7552_mac_ppe_flush(x);
	}

	return ret;
}

int an7552_mac_us_rule_del(struct an7552_mac *x, u16 idx)
{
	if (idx >= GPON_MAX_US_RULES)
		return -EINVAL;

	guard(mutex)(&x->lock);
	WRITE_ONCE(x->us_rule[idx].valid, false);
	an7552_mac_ppe_flush(x);

	return 0;
}

/*
 * The VID rewrite of the router side (struct an7552_vid_map), for the PPE
 * only: the CPU path rewrites with tc.  An UNI VID without an entry is
 * not offloaded while any entry exists, as the tc rules would drop it.
 */
int an7552_mac_vid_map_set(struct an7552_mac *x, unsigned int idx,
			    u16 uni_vid, u8 pcp_mask, u16 ani_vid, u8 pcp)
{
	struct an7552_vid_map *m;

	if (idx >= GPON_MAX_VID_MAP || !uni_vid || uni_vid >= VLAN_N_VID ||
	    !ani_vid || ani_vid >= VLAN_N_VID || pcp > GPON_PCP_KEEP)
		return -EINVAL;

	guard(mutex)(&x->lock);
	m = &x->vid_map[idx];
	/* the PPE callback reads the fields after an acquire of valid */
	WRITE_ONCE(m->valid, false);
	smp_wmb();
	m->uni_vid = uni_vid;
	m->pcp_mask = pcp_mask;
	m->ani_vid = ani_vid;
	m->pcp = pcp;
	smp_store_release(&m->valid, true);
	an7552_mac_ppe_flush(x);

	return 0;
}

int an7552_mac_vid_map_del(struct an7552_mac *x, u16 idx)
{
	if (idx >= GPON_MAX_VID_MAP)
		return -EINVAL;

	guard(mutex)(&x->lock);
	WRITE_ONCE(x->vid_map[idx].valid, false);
	an7552_mac_ppe_flush(x);

	return 0;
}

/*
 * Upstream flows in the PPE carry the GEM port, T-CONT, queue and VID of
 * the moment they were bound: every change of those has them unbound
 * (airoha_xpon_ppe_flush() sleeps, hence the work).
 */
static void an7552_mac_ppe_flush_work(struct work_struct *w)
{
	airoha_xpon_ppe_flush();
}

void an7552_mac_ppe_flush(struct an7552_mac *x)
{
	queue_work(x->wq, &x->ppe_flush_work);
}

int an7552_mac_us_default(struct an7552_mac *x, u16 gem)
{
	int ret = 0;

	mutex_lock(&x->lock);
	if (gem == GPON_UNASSIGN_GEM_ID)
		WRITE_ONCE(x->us_gem, gem);
	else if (gem < GPON_MAX_GEM_ID && x->gem[gem].valid && gem != x->omcc)
		WRITE_ONCE(x->us_gem, gem);
	else
		ret = -ENOENT;
	mutex_unlock(&x->lock);
	an7552_mac_ppe_flush(x);
	an7552_mac_carrier(x);

	return ret;
}

int an7552_mac_pcp_queue(struct an7552_mac *x, const u8 map[8])
{
	int i;

	for (i = 0; i < 8; i++)
		if (map[i] > AIROHA_XPON_MAX_QUEUE)
			return -EINVAL;
	for (i = 0; i < 8; i++)
		WRITE_ONCE(x->pcp_queue[i], map[i]);
	an7552_mac_ppe_flush(x);

	return 0;
}

/*
 * DSCP -> PCP for the upstream classification of frames without a VLAN
 * tag (OMCI 802.1p mapper, unmarked frame option 0): the PCP picks the
 * GEM port rule and the queue like the PCP of a tag.  @map is
 * GPON_DSCP_NUM PCPs (NULL: keep), @enable -1 keeps the flag.  The xmit path
 * reads both without a lock: the map is complete before the flag is set, and
 * the flag is cleared before a running map is replaced.
 */
int an7552_mac_dscp_map(struct an7552_mac *x, const u8 *map, int enable)
{
	int i;

	if (map)
		for (i = 0; i < GPON_DSCP_NUM; i++)
			if (map[i] > GPON_MAX_PCP)
				return -EINVAL;

	guard(mutex)(&x->lock);
	if (map) {
		bool was = x->dscp_en;

		if (was)
			smp_store_release(&x->dscp_en, false);
		for (i = 0; i < GPON_DSCP_NUM; i++)
			WRITE_ONCE(x->dscp_pcp[i], map[i]);
		if (enable < 0 && was)
			smp_store_release(&x->dscp_en, true);
	}
	if (enable >= 0)
		smp_store_release(&x->dscp_en, enable);
	an7552_mac_ppe_flush(x);

	return 0;
}

/* ---- QoS per T-CONT (OMCI ME 277/278/280) ---- */

int an7552_mac_tcont_sched(struct an7552_mac *x, u16 alloc_id, u8 mode,
			    const u16 weights[8])
{
	int idx = an7552_mac_tcont_channel(x, alloc_id);

	if (idx < 0)
		return idx;

	return airoha_xpon_set_chan_sched(idx, mode, weights);
}

int an7552_mac_tcont_rate(struct an7552_mac *x, u16 alloc_id, u32 kbps,
			   u32 bucket)
{
	int idx = an7552_mac_tcont_channel(x, alloc_id);

	if (idx < 0)
		return idx;

	return airoha_xpon_set_chan_rate(idx, kbps, bucket);
}

/* ---------------------------------------------------------------------
 * Enable / disable / reset (gpon.c, xpon_daemon.c)
 * ------------------------------------------------------------------- */

/* gpon_enable() */
static void gpon_enable(struct an7552_mac *x)
{
	if (x->state != GPON_O1 && x->state != GPON_O2 && x->state != GPON_O7)
		return;

	gpon_reset_ctrl(x, false);
	gpon_dev_init(x);
	/* a new activation: the PLOAM channel is open again */
	WRITE_ONCE(x->dying, false);
	/* gponDevSetSn(): the two halves of the serial number */
	mac_wr(x, G_VENDOR_ID, get_unaligned_be32(&x->sn[0]));
	mac_wr(x, G_VS_SN, get_unaligned_be32(&x->sn[4]));
	gpon_set_key(x, x->key);
}

static void gpon_reset_ids(struct an7552_mac *x)
{
	x->onu_id = GPON_UNASSIGN_ONU_ID;
	x->omcc = GPON_UNASSIGN_GEM_ID;
	x->omcc_encrypt = false;
	x->key_requested = false;
	x->password_warned = false;
}

/*
 * handle_xpon_daemon_job_gpon_dev_reset(): full MAC and PHY reset, then
 * O2 (and a new enable) when the PHY is still ready, else O1.
 */
/*
 * gpon_typeb_dev_reset_for_typeB(): stop the GEM delineation and the MPI/
 * MBI interfaces one by one (3000 polls each), pulse the MAC soft reset and
 * restart the interfaces; ONU-ID, Alloc-IDs, GEM ports and EqD survive.
 * The delineation stays stopped until Ranging_Time brings back O5.
 */
static void gpon_typeb_reset(struct an7552_mac *x)
{
	static const struct {
		u32 set;
		u32 done;
	} seq[] = {
		{ MBI_RX_STOP, MBI_RX_STOP_DONE },
		{ MBI_TX_STOP, MBI_TX_STOP_DONE },
		{ MPI_TX_STOP, MPI_TX_STOP_DONE },
	};
	u32 val;
	int i;

	lockdep_assert_held(&x->lock);

	if (x->emergency) {
		gpon_set_state(x, GPON_O7);
		return;
	}

	WRITE_ONCE(x->mbi_tx_stopped, true);
	mac_rmw(x, G_MBI_MPI_STOP, 0, DEL_RX_STOP);
	mac_rmw(x, G_MBI_MPI_STOP, 0, MPI_RX_STOP);
	read_poll_timeout_atomic(mac_rr, val, val & MPI_RX_STOP_DONE, 0,
				 3000, false, x, G_MBI_MPI_STOP);
	for (i = 0; i < ARRAY_SIZE(seq); i++) {
		mac_rmw(x, G_MBI_MPI_STOP, 0, seq[i].set);
		read_poll_timeout_atomic(mac_rr, val, val & seq[i].done, 0,
					 3000, false, x, G_MBI_MPI_STOP);
	}

	/* gponDevSwReset() */
	mac_rmw(x, DBG_GPON_MAC_SET, MAC_SW_RST_N, 0);
	udelay(1);
	mac_rmw(x, DBG_GPON_MAC_SET, 0, MAC_SW_RST_N);

	mac_rmw(x, G_MBI_MPI_STOP, MPI_TX_STOP, 0);
	WRITE_ONCE(x->mbi_tx_stopped, false);
	mac_rmw(x, G_MBI_MPI_STOP, MBI_TX_STOP, 0);
	mac_rmw(x, G_MBI_MPI_STOP, MBI_RX_STOP, 0);
	mac_rmw(x, G_MBI_MPI_STOP, MPI_RX_STOP, 0);
}

/*
 * Bring the MAC as far as it is allowed to go.
 *
 * Everything that announces - the state change to O2 and gpon_enable() with
 * it, which is what writes the serial number into G_VENDOR_ID / G_VS_SN -
 * sits behind x->armed.  With the gate closed the best this does is O1: the
 * receiver is up and the transmitter is not, so the OLT sees nothing from
 * this ONU and the splitter carries no light we did not mean to send.
 *
 * The O7 half is not behind the gate.  O7 is the shutdown Disable_Serial_
 * Number asks for, and it is silent already; treating a fail-safe as a thing
 * userspace can be asked to unlock would be the wrong way round.
 *
 * [lock] x->lock held
 */
static void gpon_activate(struct an7552_mac *x)
{
	if (x->emergency) {
		gpon_set_state(x, GPON_O7);
		if (x->phy_ready)
			gpon_enable(x);
		return;
	}

	if (!x->armed || !x->phy_ready) {
		gpon_set_state(x, GPON_O1);
		return;
	}

	gpon_set_state(x, GPON_O2);
	gpon_enable(x);
}

static void gpon_dev_reset(struct an7552_mac *x, bool from_o7)
{
	struct net_device *ndev = READ_ONCE(x->ndev);

	lockdep_assert_held(&x->lock);

	/*
	 * disable_cpu_us_traffic(): the vendor drops the
	 * CPU-upstream gate flags (gpPonSysData+0x94e0 bits 3|4) for the
	 * reset window, from handle_xpon_daemon_job_gpon_dev_reset() right
	 * after gponDevResetCtrl(1).  Ours is the pon0 queue:
	 * an7552_mac_xmit() gates on O5 itself, but until gpon_activate()
	 * takes the state down the queue would still feed the freshly reset
	 * tables under a carrier that has not gone down yet.  Stop the queue
	 * for the window, wake it at the end; the carrier work and the state
	 * gate keep the behavior outside it.
	 */
	if (ndev)
		netif_tx_disable(ndev);

	an7552_trans_power_switch(x->phy, false);
	an7552_phy_stop(x->phy);
	cancel_delayed_work(&x->ber_work);
	gpon_reset_ctrl(x, true);

	gpon_activate(x);

	an7552_phy_start(x->phy);
	an7552_phy_fw_ready(x->phy, true);
	/* the laser comes back with Upstream_Overhead, or here after O7 */
	if (from_o7 && !x->admin_down)
		an7552_trans_power_switch(x->phy, true);

	if (ndev)
		netif_tx_wake_all_queues(ndev);
}

/* gpon_disable_with_option() */
static void gpon_disable(struct an7552_mac *x, bool from_o7)
{
	lockdep_assert_held(&x->lock);

	x->reset_from_o7 = from_o7;
	x->typeb_popup = false;
	gpon_reset_ids(x);
	mac_rmw(x, G_GBL_CFG, GBL_US_FEC_EN, 0);
	gpon_tcont_remove_all(x);
	gpon_gem_remove_all(x);
	queue_work(x->wq, &x->reset_work);
}

static void gpon_reset_work(struct work_struct *w)
{
	struct an7552_mac *x = container_of(w, struct an7552_mac,
					     reset_work);

	guard(mutex)(&x->lock);
	if (x->started)
		gpon_dev_reset(x, x->reset_from_o7);
	x->reset_from_o7 = false;
}

/* ---------------------------------------------------------------------
 * Downstream PLOAM handlers (gpon_ploam.c)
 * ------------------------------------------------------------------- */

static bool gpon_dest_is(struct an7552_mac *x, const u8 *m, bool onu,
			 bool bcast)
{
	return (onu && m[0] == x->onu_id) ||
	       (bcast && m[0] == GPON_PLOAM_BROADCAST);
}

/*
 * ploam_eqd_adjustment(); with an O5 EqD offset (gpon_ploam.c:70-79) the
 * recorded EqD stays and the offset is added to / subtracted from the delta.
 */
static void gpon_eqd_adjust(struct an7552_mac *x, u32 new_eqd)
{
	int delta = new_eqd - x->eqd;
	int k, a, b, aa, int_byte;

	if (!x->eqd_o5_offset)
		x->eqd = new_eqd;
	else if (x->eqd_offset_sub)
		delta -= x->eqd_o5_offset;
	else
		delta += x->eqd_o5_offset;
	/* EN7521: MAC internal byte delay */
	int_byte = FIELD_GET(TX_SYNC_OFFSET_MASK, mac_rr(x, DBG_TX_SYNC_OFFSET));

	if (delta > 0) {
		k = delta + x->bit_delay;
		a = k >> 3;
		b = k & 7;
		aa = ((int_byte + (a & 3)) <= 3 ? 0 : 8) + (a - (a & 3)) - (a & 3);
		x->byte_delay += aa << 3;
		x->bit_delay = b;
	} else if (delta < 0) {
		k = -delta - x->bit_delay;
		a = k > 0 ? (k >> 3) + !!(k & 7) : 0;
		b = (a << 3) - k;
		aa = (int_byte >= (a & 3) ? 0 : 8) + (a - (a & 3)) - (a & 3);
		x->byte_delay -= aa << 3;
		x->bit_delay = b;
	} else {
		return;
	}

	mac_wr(x, G_EQD, x->byte_delay);
	an7552_phy_set_bit_delay(x->phy, x->bit_delay);
}

static void ploam_upstream_overhead(struct an7552_mac *x, const u8 *m)
{
	struct an7552_phy_preamble p = {
		.mask = PHY_GUARD_BIT_NUM_EN | PHY_PRE_T1_NUM_EN |
			PHY_PRE_T2_NUM_EN | PHY_PRE_T3_PAT_EN,
		.guard_bit_num = x->guard_bit_num,
		/* the SDK swaps t1/t2 for the PHY: gpon_ploam.c */
		.preamble_t1_num = m[4],
		.preamble_t2_num = m[3],
		.preamble_t3_pat = m[5],
	};
	u8 flags = m[9];

	if (m[0] != GPON_PLOAM_BROADCAST || x->state != GPON_O2)
		return;

	if (!x->emergency && !x->admin_down && !x->rogue_off)
		an7552_trans_power_switch(x->phy, true);

	an7552_phy_gpon_preamble(x->phy, &p);
	an7552_phy_gpon_delimiter_guard(x->phy,
					     get_unaligned_be24(&m[6]),
					     GPON_PHY_TX_EN_PATTERN);

	mac_rmw(x, G_PLOU_GUARD_BIT, PLOU_GUARD_BIT_MASK, x->guard_bit_num);
	mac_rmw(x, G_PLOU_PRMBL_TYPE1_2, PLOU_PRMB1_MASK | PLOU_PRMB2_MASK,
		 FIELD_PREP(PLOU_PRMB1_MASK, m[3]) |
		 FIELD_PREP(PLOU_PRMB2_MASK, m[4]));
	/* byte 9: tx_power[1:0] sn_tran_num[3:2] sn_mask[4] delay_mode[5] */
	mac_rmw(x, G_PRE_ASSIGNED_DLY, PRE_DLY_EN | PRE_DLY_MASK,
		 (flags & BIT(5) ? PRE_DLY_EN : 0) |
		 FIELD_PREP(PRE_DLY_MASK, get_unaligned_be16(&m[10])));

	gpon_set_state(x, GPON_O3);
}

static void ploam_assign_onu_id(struct an7552_mac *x, const u8 *m)
{
	bool sym = x->mode == AN7552_XPON_GPON_SYM;

	if (m[0] != GPON_PLOAM_BROADCAST &&
	    !(sym && m[0] == GPON_PLOAM_SYM_BROADCAST))
		return;
	if (memcmp(x->sn, &m[3], GPON_SN_LEN))
		return;
	if (x->state != GPON_O3)
		return;

	gpon_set_state(x, GPON_O4);
	mac_rmw(x, G_ONU_ID, ONU_ID_MASK | ONU_ID_VLD,
		 ONU_ID_VLD | FIELD_PREP(ONU_ID_MASK, m[2]));
	x->onu_id = m[2];
	gpon_txbuf_init(x);
}

/* gpon_check_gem_encrypt() */
static bool gpon_any_gem_encrypted(struct an7552_mac *x)
{
	int i;

	for (i = 0; i < GPON_MAX_GEM_ID; i++)
		if (x->gem[i].valid && x->gem[i].encrypt)
			return true;

	return false;
}

static void ploam_ranging_time(struct an7552_mac *x, const u8 *m)
{
	u32 eqd;

	if ((x->state == GPON_O4 || x->state == GPON_O5) && !(m[2] & 1)) {
		eqd = get_unaligned_be32(&m[3]);
		if (x->state == GPON_O4) {
			/* recorded before the O4 offset (gpon_ploam.c:509-519) */
			x->eqd = eqd;
			if (x->eqd_offset_sub)
				eqd -= x->eqd_o4_offset;
			else
				eqd += x->eqd_o4_offset;
			/* BYTE_DELAY_MASK / BIT_DELAY_MASK (gpon_ploam.h:8-9) */
			x->byte_delay = eqd & ~7;
			x->bit_delay = eqd & 7;
			mac_wr(x, G_EQD, x->byte_delay);
			an7552_phy_set_bit_delay(x->phy, x->bit_delay);
		} else {
			gpon_eqd_adjust(x, eqd);
		}
	}

	if (x->state != GPON_O4)
		return;

	mac_rmw(x, G_GBL_CFG, 0, GBL_US_FEC_EN);
	gpon_set_state(x, GPON_O5);
	/* start the GEM delineation RX interface */
	mac_rmw(x, G_MBI_MPI_STOP, DEL_RX_STOP, 0);
	/*
	 * Back from Type B protection (Popup in O6): the key in use before
	 * the LOS becomes active again, the ONU key goes back to shadow.
	 */
	if (x->typeb_popup && (gpon_any_gem_encrypted(x) || x->omcc_encrypt)) {
		int i;

		for (i = 0; i < 4; i++)
			mac_wr(x, G_AES_SHADOW_KEY(i), x->typeb_key[i]);
		gpon_set_key_switch_time(x, 0);
		gpon_set_key(x, x->key);
	}
	/* ploam_recv_ranging_time(): Type B, PON LED on */
	if (x->typeb_popup)
		an7552_led_set(&x->led_pon, XPON_LED_ON);
	x->typeb_popup = false;
	mac_wr(x, DBG_GRP_0, 0xffffffff);
	mac_wr(x, DBG_GRP_1, 0xffffffff);
}

static void ploam_deactivate(struct an7552_mac *x, const u8 *m)
{
	if (x->state == GPON_O4 || x->state == GPON_O5 ||
	    x->state == GPON_O6) {
		gpon_set_alarm(x, GPON_ALM_DACT);
		gpon_disable(x, false);
	}
}

/*
 * The emergency stop of Disable_Serial_Number: no bursts, no T-CONTs, the
 * transmitter off, and O7 until the OLT (or userspace) lets the ONU back.
 */
static void gpon_emergency_stop(struct an7552_mac *x)
{
	gpon_mbi_stop(x, true);
	gpon_tcont_remove_all(x);
	gpon_set_state(x, GPON_O7);
	an7552_trans_power_switch(x->phy, false);
	x->emergency = true;
}

/* PLOAM_DISABLE_* modes (gpon_ploam.h:11-16) */

static void ploam_disable_sn(struct an7552_mac *x, const u8 *m)
{
	u8 mode = m[2];
	bool mine = !memcmp(x->sn, &m[3], GPON_SN_LEN);

	if (x->state == GPON_O7) {
		if (mode == DISABLE_SN_PARTICIPATE_ALL ||
		    (mode == DISABLE_SN_PARTICIPATE && mine)) {
			x->emergency = false;
			gpon_disable(x, true);
		}
		gpon_clear_alarm(x, GPON_ALM_DIS);
	} else if (x->state != GPON_O1) {
		if (mode == DISABLE_SN_DENIED_ALL ||
		    (mode == DISABLE_SN_DENIED && mine)) {
			gpon_emergency_stop(x);
			gpon_set_alarm(x, GPON_ALM_DIS);
		}
	}
}

static void ploam_encrypted_port_id(struct an7552_mac *x, const u8 *m)
{
	u16 port = get_unaligned_be16(&m[3]) >> 4;
	bool enc = (m[2] & 3) == 3;

	/* the OLT may announce encryption before OMCI creates the port */
	if (enc)
		set_bit(port, x->gem_encrypt_req);
	else
		clear_bit(port, x->gem_encrypt_req);

	/*
	 * The OMCC as well (ploam_recv_encrypt_port_id() ->
	 * gwan_config_gemport(port, 3, enc), which writes the GEM entry of the
	 * OMCC and sets its encryption flag): an OLT with OMCI encryption on
	 * announces the OMCC here.
	 */
	if (x->gem[port].valid) {
		x->gem[port].encrypt = enc;
		if (port == x->omcc)
			x->omcc_encrypt = enc;
		gpon_hw_gem_sync(x, port);
	}

	gpon_ploam_ack(x, m);
}

/* Ten zeros and no password are one state, not two: nothing else marks it. */
static bool an7552_mac_has_password(const struct an7552_mac *x)
{
	static const u8 zero[GPON_PASSWD_LEN];

	return memcmp(x->passwd, zero, GPON_PASSWD_LEN) != 0;
}

/*
 * ploam_recv_request_passwd() answers in O5 only (state == 5 before
 * the send) and otherwise only forgets the previous message.
 *
 * Answering with no password would be a lie the OLT accepts and then fails
 * on further.  Saying so once and taking the ONU out of activation is the
 * honest end of it: the gate closes (as a new identity closes it) and the
 * reset brings the ONU down to O1 with the laser off and the ONU-ID,
 * T-CONTs and GEM ports gone, until userspace sets a password and activates
 * again.  x->password_warned keeps the log to one line per activation.
 */
static void ploam_request_password(struct an7552_mac *x, const u8 *m)
{
	u8 msg[GPON_PLOAM_LEN] = { 0, PLOAMU_PASSWORD };

	if (x->state != GPON_O5)
		goto out;

	if (an7552_mac_has_password(x)) {
		memcpy(&msg[2], x->passwd, GPON_PASSWD_LEN);
		gpon_ploam_send(x, msg, PLOAMU_PASSWD_TIMES);
	} else if (!x->password_warned) {
		x->password_warned = true;
		dev_err(x->dev, "OLT requests a password and none is set - giving up activation\n");
		gpon_set_alarm(x, GPON_ALM_NOPWD);
		x->armed = false;
		gpon_disable(x, false);
	}
out:
	memset(x->prev_ploam, 0, sizeof(x->prev_ploam));
}

static void ploam_assign_alloc_id(struct an7552_mac *x, const u8 *m)
{
	u16 alloc = get_unaligned_be16(&m[2]) >> 4;
	u8 type = m[4];

	if (x->state == GPON_O5 && alloc != x->onu_id) {
		if (type == 0x01) {		/* GEM-encapsulated */
			if (gpon_tcont_find(x, alloc) < 0 &&
			    gpon_tcont_create(x, alloc) >= 0) {
				gpon_txbuf_init(x);
				if (x->cfg_done)
					an7552_mac_reset_txbuf(x->tcont_cnt);
			}
		} else if (type == 0xff) {	/* de-allocate */
			gpon_tcont_remove(x, alloc);
			an7552_mac_reset_txbuf(x->tcont_cnt);
		}
	}

	gpon_ploam_ack(x, m);
}

static void ploam_popup(struct an7552_mac *x, const u8 *m)
{
	if (!x->phy_ready)
		return;
	/* typeB_delay_g defaults to 0 */
	if (x->state == GPON_O6) {
		x->typeb_popup = true;
		gpon_set_state(x, m[0] == GPON_PLOAM_BROADCAST ?
				  GPON_O4 : GPON_O5);
	}
}

static void ploam_request_key(struct an7552_mac *x, const u8 *m)
{
	u8 msg[GPON_PLOAM_LEN] = { 0, PLOAMU_ENCRYPTION_KEY };
	int i;

	if (m[0] != x->onu_id || x->state != GPON_O5)
		return;

	get_random_bytes(x->key, sizeof(x->key));
	x->key_idx = !x->key_idx;
	gpon_set_key(x, x->key);

	for (i = 0; i < 2; i++) {
		msg[2] = x->key_idx;
		msg[3] = i;
		memcpy(&msg[4], &x->key[i * 8], 8);
		gpon_ploam_send(x, msg, PLOAMU_KEY_TIMES);
	}
	x->key_requested = true;
}

static void ploam_config_port_id(struct an7552_mac *x, const u8 *m)
{
	u16 port = get_unaligned_be16(&m[3]) >> 4;

	if (x->state == GPON_O5) {
		if (m[2] & 1) {
			x->omcc = port;
			mac_rmw(x, G_OMCI_ID, OMCI_GPID_MASK | OMCI_PORT_ID_VLD,
				 OMCI_PORT_ID_VLD |
				 FIELD_PREP(OMCI_GPID_MASK, port));
			gpon_tcont_create(x, x->onu_id);
			if (!x->gem[port].valid)
				__gpon_gem_add(x, port, x->onu_id, false, false);
			/* announced by Encrypted_Port-ID before or now */
			x->omcc_encrypt = x->gem[port].encrypt;
			if (!x->key_requested) {
				gpon_set_key_switch_time(x, 0);
				x->key_requested = true;
			}
		} else {
			x->omcc = GPON_UNASSIGN_GEM_ID;
			x->omcc_encrypt = false;
			mac_rmw(x, G_OMCI_ID, OMCI_PORT_ID_VLD, 0);
			__gpon_gem_del(x, port, true);
			gpon_tcont_remove(x, x->onu_id);
		}
		/* deferred: this reaches the front end, see gpon_set_state() */
		queue_work(x->wq, &x->carrier_work);
	}

	gpon_ploam_ack(x, m);
}

static void ploam_ber_interval(struct an7552_mac *x, const u8 *m)
{
	if (x->state == GPON_O5) {
		/* frames of 125 us to ms */
		x->ber_interval_ms = get_unaligned_be32(&m[2]) >> 3;
		cancel_delayed_work(&x->ber_work);
		if (x->ber_interval_ms) {
			an7552_phy_counter_clear(x->phy, AN7552_PHY_BIP_CNT_CLR);
			queue_delayed_work(x->wq, &x->ber_work,
					   msecs_to_jiffies(x->ber_interval_ms));
		}
	}

	gpon_ploam_ack(x, m);
}

static void ploam_key_switch_time(struct an7552_mac *x, const u8 *m)
{
	if (x->state == GPON_O5)
		gpon_set_key_switch_time(x, get_unaligned_be32(&m[2]));

	gpon_ploam_ack(x, m);
}

static void ploam_ext_burst_length(struct an7552_mac *x, const u8 *m)
{
	u32 overhead, o3, o5;

	if (m[0] != GPON_PLOAM_BROADCAST || x->state != GPON_O3)
		return;

	overhead = (FIELD_GET(PLOU_GUARD_BIT_MASK,
			      mac_rr(x, G_PLOU_GUARD_BIT)) +
		    FIELD_GET(PLOU_PRMB1_MASK,
			      mac_rr(x, G_PLOU_PRMBL_TYPE1_2)) +
		    FIELD_GET(PLOU_PRMB2_MASK,
			      mac_rr(x, G_PLOU_PRMBL_TYPE1_2)) +
		    FIELD_GET(PLOU_DELM_BIT_MASK,
			      mac_rr(x, G_PLOU_DELM_BIT))) >> 3;

	/* preambleFlag default: limit the overhead to 128 bytes */
	o3 = overhead + m[2] > 128 ? 128 - overhead : m[2];
	o5 = overhead + m[3] > 128 ? 128 - overhead : m[3];

	gpon_set_ext_burst(x, o3, o5);
}

static void ploam_swift_popup(struct an7552_mac *x, const u8 *m)
{
	if (m[0] == GPON_PLOAM_BROADCAST && x->state == GPON_O6)
		gpon_set_state(x, GPON_O5);
}

static void ploam_ranging_adjustment(struct an7552_mac *x, const u8 *m)
{
	u32 off;

	if (x->state == GPON_O5) {
		off = get_unaligned_be32(&m[3]);
		if (off)
			gpon_eqd_adjust(x, m[2] & BIT(1) ? x->eqd - off :
							   x->eqd + off);
	}

	gpon_ploam_ack(x, m);
}

/* PLOAMD_PEE and PLOAMD_PST carry no data, they only raise an alarm */
static void ploam_pee(struct an7552_mac *x, const u8 *m)
{
	if (m[0] == GPON_PLOAM_BROADCAST)
		gpon_set_alarm(x, GPON_ALM_PEE);
}

static void ploam_pst(struct an7552_mac *x, const u8 *m)
{
	if (gpon_dest_is(x, m, true, true))
		gpon_set_alarm(x, GPON_ALM_MIS);
}

/*
 * Downstream PLOAM: the handler of every id the kernel acts on, and who the
 * message has to be addressed to.  The destination test sits here so the
 * handlers do not each repeat it; PLOAM_ANY is for the two that check more
 * than the address (state, serial number), PLOAM_IGNORE for the ids the SDK
 * does not act on at all.
 */
enum ploam_dest {
	PLOAM_ANY,
	PLOAM_ONU,
	PLOAM_ONU_OR_BCAST,
	PLOAM_BCAST,
	PLOAM_IGNORE,
};

static const struct {
	void (*fn)(struct an7552_mac *, const u8 *);
	u8	dest;
} ploamd_ops[PLOAMD_MAX] = {
	[PLOAMD_UPSTREAM_OVERHEAD]	= { ploam_upstream_overhead, PLOAM_ANY },
	[PLOAMD_ASSIGN_ONU_ID]		= { ploam_assign_onu_id, PLOAM_ANY },
	[PLOAMD_RANGING_TIME]		= { ploam_ranging_time, PLOAM_ONU },
	[PLOAMD_DEACTIVATE_ONU_ID]	= { ploam_deactivate, PLOAM_ONU_OR_BCAST },
	[PLOAMD_DISABLE_SERIAL_NUM]	= { ploam_disable_sn, PLOAM_BCAST },
	[PLOAMD_ENCRYPTED_PORT_ID]	= { ploam_encrypted_port_id, PLOAM_ONU },
	[PLOAMD_REQUEST_PASSWORD]	= { ploam_request_password, PLOAM_ONU },
	[PLOAMD_ASSIGN_ALLOC_ID]	= { ploam_assign_alloc_id, PLOAM_ONU },
	[PLOAMD_POPUP]			= { ploam_popup, PLOAM_ONU_OR_BCAST },
	[PLOAMD_REQUEST_KEY]		= { ploam_request_key, PLOAM_ANY },
	[PLOAMD_CONFIG_PORT_ID]		= { ploam_config_port_id, PLOAM_ONU },
	[PLOAMD_PEE]			= { ploam_pee, PLOAM_ANY },
	[PLOAMD_PST]			= { ploam_pst, PLOAM_ANY },
	[PLOAMD_BER_INTERVAL]		= { ploam_ber_interval, PLOAM_ONU_OR_BCAST },
	[PLOAMD_KEY_SWITCHING_TIME]	= { ploam_key_switch_time, PLOAM_ONU },
	[PLOAMD_EXT_BURST_LENGTH]	= { ploam_ext_burst_length, PLOAM_ANY },
	[PLOAMD_SWIFT_POPUP]		= { ploam_swift_popup, PLOAM_ANY },
	[PLOAMD_RANGING_ADJUSTMENT]	= { ploam_ranging_adjustment, PLOAM_ONU_OR_BCAST },
	/* no action in the SDK, and no alarm either */
	[PLOAMD_CPL]			= { .dest = PLOAM_IGNORE },
	[PLOAMD_PON_ID]			= { .dest = PLOAM_IGNORE },
	[PLOAMD_SLEEP_ALLOW]		= { .dest = PLOAM_IGNORE },
};

static bool ploam_for_us(struct an7552_mac *x, const u8 *m, u8 dest)
{
	switch (dest) {
	case PLOAM_ONU:
		/* 0xff is both "no ONU-ID yet" and the broadcast address */
		return x->onu_id != GPON_UNASSIGN_ONU_ID && m[0] == x->onu_id;
	case PLOAM_ONU_OR_BCAST:
		return gpon_dest_is(x, m, true, true);
	case PLOAM_BCAST:
		return m[0] == GPON_PLOAM_BROADCAST;
	default:
		return true;
	}
}

static void gpon_ploam_dispatch(struct an7552_mac *x, const u8 *m)
{
	u8 id = m[1];

	/*
	 * PLOAM_IGNORE is the three ids the SDK does not act on and raises no
	 * alarm for (see ploamd_ops[]).  They have to return before the "did we
	 * handle it" test below, not fall through it: a destination of
	 * PLOAM_IGNORE is not a destination this ONU can match, so letting them
	 * reach the end would report an id the SDK deliberately ignores as an
	 * unknown one and raise a spurious GPON_ALM_MEM.
	 */
	if (id < PLOAMD_MAX && ploamd_ops[id].dest == PLOAM_IGNORE)
		return;

	if (id < PLOAMD_MAX && ploamd_ops[id].fn) {
		if (ploam_for_us(x, m, ploamd_ops[id].dest))
			ploamd_ops[id].fn(x, m);
		return;
	}

	gpon_set_alarm(x, GPON_ALM_MEM);
	/* hand the daemon what the kernel does not handle */
	an7552_mac_genl_ploam_unhandled(x, m);
}

/* should_ignore_ploam_msg(): three identical PLOAMs are processed once */
static bool gpon_ploam_ignore(struct an7552_mac *x, const u8 *m)
{
	size_t n = m[1] == PLOAMD_RANGING_TIME ? 7 : GPON_PLOAM_LEN;
	bool same = !memcmp(x->prev_ploam, m, n);

	if (!x->started)
		return true;

	if (same)
		x->same_ploam++;
	if (same && x->same_ploam % 3)
		return true;

	memcpy(x->prev_ploam, m, GPON_PLOAM_LEN);
	x->same_ploam = 0;

	return false;
}

static void gpon_ploam_rx(struct an7552_mac *x)
{
	u8 m[GPON_PLOAM_LEN];
	int depth;

	do {
		depth = gpon_ploam_read(x, m);
		if (depth <= 0) {
			if (depth < 0)
				x->stats.ploam_rx_drop++;
			return;
		}
		x->stats.ploam_rx++;
		if (gpon_ploam_ignore(x, m)) {
			an7552_mac_dbg(x, AN7552_DBG_ACT, "ploam rx %s ignored (O%d): %*ph\n",
				 gpon_ploam_name(gpon_ploamd_name, PLOAMD_MAX,
						 m[1]), x->state,
				 GPON_PLOAM_LEN, m);
			continue;
		}
		an7552_mac_dbg(x, AN7552_DBG_ACT, "ploam rx %s (O%d): %*ph\n",
			 gpon_ploam_name(gpon_ploamd_name, PLOAMD_MAX, m[1]),
			 x->state, GPON_PLOAM_LEN, m);
		gpon_ploam_dispatch(x, m);
	} while (depth > 3);
}

/* ---------------------------------------------------------------------
 * Interrupts, timers, PHY events
 * ------------------------------------------------------------------- */

irqreturn_t an7552_mac_irq_thread(int irq, void *data)
{
	struct an7552_mac *x = data;
	u32 status;

	status = mac_rr(x, G_INT_STATUS) & mac_rr(x, G_INT_ENABLE);
	mac_wr(x, G_INT_STATUS, status);
	if (!status)
		return IRQ_NONE;

	guard(mutex)(&x->lock);
	if (status & INT_PLOAMD_RECV)
		gpon_ploam_rx(x);
	if (status & INT_LOS_GEM_DEL)
		gpon_set_alarm(x, GPON_ALM_LCDG);
	if (status & GPON_INT_TOD)
		gpon_tod_irq(x, status);
	if (status & GPON_INT_ERROR) {
		x->stats.irq_err++;
		x->stats.last_err_status = status & GPON_INT_ERROR;
	}
	return IRQ_HANDLED;
}

static void gpon_to1_work(struct work_struct *w)
{
	struct an7552_mac *x = container_of(to_delayed_work(w),
					     struct an7552_mac, to1_work);

	guard(mutex)(&x->lock);
	if (x->state == GPON_O3 || x->state == GPON_O4) {
		/*
		 * gpon_act_to1_timer_expires(): every expiry raises SUF and
		 * drops back to O2.  When the count reaches zero (20 misses,
		 * "TO1_TIMEOUT_MAX_CNT ... trigger mac & phy reset") the vendor
		 * also restarts the MAC and the PHY through
		 * xmcs_set_connection_start(1) -> prepare_gpon(1) and reloads
		 * the count; an ONU stuck in ranging (EqD, preamble) only gets
		 * out of it that way.  gpon_disable() is that restart here, and
		 * the reset it queues brings the ONU back to O2 by itself.
		 */
		gpon_set_alarm(x, GPON_ALM_SUF);
		if (!--x->to1_left) {
			x->to1_left = GPON_TO1_MAX_CNT;
			dev_info(x->dev, "TO1 expired %d times, MAC and PHY reset\n",
				 GPON_TO1_MAX_CNT);
			gpon_disable(x, false);
			return;
		}
		gpon_set_state(x, GPON_O2);
	}
}

static void gpon_to2_work(struct work_struct *w)
{
	struct an7552_mac *x = container_of(to_delayed_work(w),
					     struct an7552_mac, to2_work);

	guard(mutex)(&x->lock);
	if (x->state == GPON_O6) {
		x->phy_ready = !an7552_phy_is_los(x->phy);
		if (x->phy_ready)
			gpon_set_state(x, GPON_O1);
		gpon_disable(x, false);
	}
}

/* xmcs_get_SD_SF_BIP_NUM(): BIP errors per second for a 10^-thld BER */
static u32 gpon_sd_sf_limit(u8 thld)
{
	u32 limit = GPON_SD_SF_ALARM_BASE;

	while (thld--)
		limit /= 10;

	return limit;
}

/* gpon_ber_interval_expires(): SD / SF alarms */
static void gpon_sd_sf_check(struct an7552_mac *x, u32 bip)
{
	u64 rate = div_u64((u64)bip * 1000, x->ber_interval_ms);

	if (rate >= gpon_sd_sf_limit(x->sd_thld))
		gpon_set_alarm(x, GPON_ALM_SD);
	else
		gpon_clear_alarm(x, GPON_ALM_SD);

	if (rate >= gpon_sd_sf_limit(x->sf_thld)) {
		gpon_set_alarm(x, GPON_ALM_SF);
		gpon_set_alarm(x, GPON_ALM_RDI);
	} else {
		gpon_clear_alarm(x, GPON_ALM_SF);
	}
}

/* gpon_ber_interval_expires() */
static void gpon_ber_work(struct work_struct *w)
{
	struct an7552_mac *x = container_of(to_delayed_work(w),
					     struct an7552_mac, ber_work);
	u8 msg[GPON_PLOAM_LEN] = { 0, PLOAMU_REI };
	u32 bip;

	guard(mutex)(&x->lock);
	if (x->state == GPON_O5 && x->ber_interval_ms) {
		bip = an7552_phy_bip_counter(x->phy) + x->bip_pending;
		x->bip_pending = 0;
		x->bip_total += bip;
		gpon_sd_sf_check(x, bip);
		put_unaligned_be32(bip, &msg[2]);
		msg[6] = x->rei_seq & 0xf;
		gpon_ploam_send(x, msg, 1);
		x->rei_seq = (x->rei_seq + 1) & 0xf;
		queue_delayed_work(x->wq, &x->ber_work,
				   msecs_to_jiffies(x->ber_interval_ms));
	}
}

/*
 * Loss of light, from whichever detector saw it.  [lock] x->lock held.
 *
 * Two of them, and either can fire first: TRANS_LOS in the SerDes, reported as
 * AN7552_EVENT_LOS, and the comparator in the optical front end, reported as
 * PON_LASER_ALARM_LOS.  The two are not merged earlier because they cannot be -
 * one is an interrupt in this driver and one is a value another driver reads -
 * so they are merged here instead, which is the only place the answer to "is
 * there light" can change the ONU.  Both end up in the same state transitions.
 *
 * A caller that arrives second is harmless.  O2..O4 goes to O1 through the
 * queued reset work, and queue_work() on a pending work item does not queue it
 * again; O5 goes to O6 immediately and is no longer a state this looks at.  O1,
 * O6 and O7 are left alone, which is what the PHY event has always done: the
 * transmitter is not on in O1, and O7 is the emergency shutdown the ONU is
 * already in.  The front end reaches this from xpon_alarm() and the PHY from
 * an7552_mac_handle_phy_event(), and both go through the same answer rather
 * than each having one.
 */
static void gpon_loss_of_light(struct an7552_mac *x)
{
	if (x->state >= GPON_O2 && x->state <= GPON_O4) {
		x->phy_ready = false;
		gpon_disable(x, false);
	} else if (x->state == GPON_O5) {
		/*
		 * O6: the SDK keeps T-CONT/GEM state for Type B protection,
		 * saves the AES key and resets the MAC datapath only.
		 */
		x->phy_ready = false;
		gpon_set_state(x, GPON_O6);
		gpon_save_active_key(x);
		gpon_typeb_reset(x);
	}
}

/*
 * Rogue ONU recovery.  The front end raises AN7552_EVENT_ROGUE and turns the
 * transmitter off; unlike the vendor front end it has no
 * PHY_EVENT_STOP_ROGUE_MODE to report that the condition went away.  A
 * restart is therefore scheduled, with a growing delay so a persistent fault
 * cannot keep flashing the splitter, and gpon_disable() takes the MAC back
 * through the normal reset to O2 so the OLT can range it again.  The laser
 * itself comes back with Upstream_Overhead, which also re-arms the detector.
 */
#define GPON_ROGUE_RETRIES	3
static const unsigned int gpon_rogue_delay_ms[GPON_ROGUE_RETRIES] = {
	5000, 30000, 300000,
};

static void gpon_rogue_work(struct work_struct *w)
{
	struct an7552_mac *x = container_of(to_delayed_work(w),
					     struct an7552_mac, rogue_work);

	guard(mutex)(&x->lock);
	if (!x->rogue || !x->started || x->rogue_off)
		return;

	x->rogue_retry++;
	dev_info(x->dev, "rogue ONU: restarting the PON datapath (attempt %u/%u)\n",
		 x->rogue_retry, GPON_ROGUE_RETRIES);
	gpon_disable(x, false);
}

/* gpon_detect_phy_ready() / gpon_detect_los_lof() */
void an7552_mac_handle_phy_event(struct an7552_mac *x, enum an7552_xpon_event ev)
{
	mutex_lock(&x->lock);
	switch (ev) {
	case AN7552_EVENT_PHY_RDY:
		x->phy_ready = true;
		gpon_clear_alarm(x, GPON_ALM_LOS);
		gpon_clear_alarm(x, GPON_ALM_LOF);
		if (x->state == GPON_O1) {
			if (x->emergency) {
				gpon_reset_ctrl(x, false);
				gpon_set_state(x, GPON_O7);
			} else if (x->armed) {
				gpon_set_state(x, GPON_O2);
			}
		}
		/*
		 * gpon_enable() accepts O1 as well as O2 and O7, so the gate
		 * has to cover this call and not only the state above: an
		 * unarmed PHY_RDY would otherwise announce straight from O1,
		 * which is the one thing the gate exists to stop.
		 */
		if (x->started && (x->armed || x->emergency))
			gpon_enable(x);
		break;
	case AN7552_EVENT_ROGUE:
		/*
		 * The front end switched the transmitter off: it saw it on
		 * outside a grant.  The vendor latches rogue_state and only
		 * clears it on PHY_EVENT_STOP_ROGUE_MODE, an event this front
		 * end never raises, so it is cleared here by a LOS or an
		 * explicit activation instead.
		 *
		 * Bring the ONU back, but not in a loop: an ONU that keeps
		 * tripping the detector would otherwise flash the splitter
		 * every few seconds.  Three restarts at 5 s, 30 s and 5 min;
		 * if the condition is still there after the last one the
		 * transmitter stays off until the next LOS or activation.
		 */
		x->stats.rogue++;
		if (!x->rogue) {
			x->rogue = true;
			x->rogue_retry = 0;
			dev_warn_ratelimited(x->dev,
					     "rogue ONU: transmitter off\n");
		}
		if (x->rogue_retry < GPON_ROGUE_RETRIES) {
			queue_delayed_work(x->wq, &x->rogue_work,
					   msecs_to_jiffies(gpon_rogue_delay_ms[
						   x->rogue_retry]));
		} else if (!x->rogue_off) {
			x->rogue_off = true;
			dev_warn(x->dev,
				 "rogue ONU: still present after %u restarts, transmitter stays off until LOS or activation\n",
				 x->rogue_retry);
		}
		break;
	case AN7552_EVENT_LOS:
	case AN7552_EVENT_LOF:
		/*
		 * The light is gone, so the rogue episode is over: the
		 * transmitter is off anyway, and the next PHY_RDY starts a
		 * fresh activation with the detector armed again.
		 */
		if (x->rogue) {
			x->rogue = false;
			x->rogue_off = false;
			x->rogue_retry = 0;
			cancel_delayed_work(&x->rogue_work);
		}
		gpon_set_alarm(x, ev == AN7552_EVENT_LOS ? GPON_ALM_LOS :
							   GPON_ALM_LOF);
		/*
		 * gpon_detect_los_lof() filters a LOF, and it filters it against
		 * the state rather than against the light:
		 *
		 *   O2..O4  a LOF is dropped unless the PON core is told there
		 *           is light, read through ECNT_HOOK 0x8001 - which is
		 *           the LOS the SerDes already reports as an event.
		 *   O5      a LOF is dropped only if gpGponPriv+0x29c bit 3
		 *           ("current lof control status in O5") is clear.
		 *
		 * That bit is not a constant, and the earlier comment here said
		 * it was never set, which was wrong: gpon_SD_SF_init() sets it
		 * at init for HIR 12/14/15/16 and only gpon_debug_write_proc()
		 * clears it again.  With it set - the normal case - a LOF in O5
		 * runs the whole Type B sequence, and O5 is where it matters
		 * most: this is the state G.984.3 Type B protection exists for,
		 * and dropping the event there leaves the ONU in O5 with a dead
		 * datapath until something else moves it.
		 *
		 * So the O5 half of the test is gone rather than reimplemented.
		 * Reproducing the vendor's debug bit would mean carrying a
		 * register it only ever writes from a procfs node; the two
		 * behaviours differ solely in whether a LOF with the light
		 * still present in O5 goes to O6, and going there is what the
		 * bit being set does.  A front end reports light and no more, so
		 * it has no LOF to qualify and never reaches this test.
		 */
		if (ev == AN7552_EVENT_LOF && !an7552_phy_is_los(x->phy) &&
		    x->state >= GPON_O2 && x->state <= GPON_O4)
			break;
		gpon_loss_of_light(x);
		break;
	default:
		break;
	}
	mutex_unlock(&x->lock);
	an7552_mac_carrier(x);
}

/*
 * The carrier/traffic update, deferred off x->lock.
 *
 * An7552_mac_carrier() hands the new mode to the optical front end, and the
 * front end calls back into the MAC from its own timers with its own lock held.
 * Whoever asks for it from a context that already holds x->lock therefore hands
 * it here instead of running it inline - see gpon_set_state() and
 * ploam_config_port_id().
 */
static void an7552_mac_carrier_work(struct work_struct *w)
{
	struct an7552_mac *x = container_of(w, struct an7552_mac, carrier_work);

	an7552_mac_carrier(x);
}

/* gpon_detect_dying_gasp(): one by one, see an7552_gpon_dying_gasp_next() */
void an7552_mac_dying_gasp(struct an7552_mac *x)
{
	u8 msg[GPON_PLOAM_LEN] = { READ_ONCE(x->onu_id), PLOAMU_DYING_GASP };
	int i;

	if (READ_ONCE(x->state) != GPON_O5)
		return;

	WRITE_ONCE(x->dying, true);
	/*
	 * Called from hard IRQ context while power is failing.  The SDK
	 * sends GPON_DG_ONEBYONE_TOTAL_CNT (3) messages GPON_DG_DEFAULT_DELAY
	 * (250 us) apart; the upstream FIFO holds them, so they are queued
	 * here directly instead of waiting for each PLOAMu-sent interrupt.
	 */
	for (i = 0; i < PLOAMU_DYING_GASP_TIMES; i++) {
		gpon_ploam_write(x, msg);
		if (i < PLOAMU_DYING_GASP_TIMES - 1)
			udelay(250);
	}
}

/*
 * Open the gate.  Deliberately a no-op beyond arming when there is nothing
 * to announce into: gpon_activate() stops at O1 without a front end, and
 * once the state machine is past O2 this leaves the working link alone.
 */
int an7552_mac_activate(struct an7552_mac *x)
{
	/*
	 * The same test an7552_mac_open() makes: without light there is
	 * nothing to activate, and arming anyway would leave a flag set that
	 * no front end would ever read.  Fail instead, so whoever asked can
	 * tell "not yet" from "done".
	 */
	if (an7552_mac_is_down(x) || !READ_ONCE(x->phy->laser_set_traffic))
		return -ENODEV;

	guard(mutex)(&x->lock);
	x->armed = true;
	/* an explicit activation is a fresh start for the rogue latch */
	x->rogue = false;
	x->rogue_off = false;
	x->rogue_retry = 0;
	cancel_delayed_work(&x->rogue_work);
	/*
	 * Only a link that has not been announced yet is brought forward.
	 * Past O2 the OLT already sees this ONU, and gpon_activate() would
	 * set the state back to O2 and reset the MAC under a working link.
	 */
	if (x->started &&
	    (x->state == GPON_O1 || x->state == GPON_O2))
		gpon_activate(x);

	return 0;
}

/*
 * Close the gate again.  The counterpart of an7552_mac_activate(), and what
 * a configuration that says "do not announce" needs: a running link goes back
 * to O1, where the transmitter is off, and a link that is not started only
 * has the flag cleared, so the next start stays silent too.
 *
 * Deliberately not folded into an7552_mac_stop(): a stop is also what a front
 * end detaching does, and that one is meant to re-announce when it comes back.
 */
int an7552_mac_deactivate(struct an7552_mac *x)
{
	guard(mutex)(&x->lock);
	x->armed = false;
	if (x->started)
		gpon_activate(x);

	return 0;
}

/*
 * One place that stores an identity, so the netlink attributes and the
 * debugfs node cannot disagree about what is legal.  The bytes arrive
 * already binary from netlink and parsed from text by the caller; what is
 * checked here is only that the caller said which of the two it is.
 *
 * Either write un-arms the gate: a serial number or password that was just
 * replaced has not been announced, and the next one will not be either
 * until whoever changed it says so again.
 */
int an7552_mac_set_identity(struct an7552_mac *x, const void *buf, bool is_pwd)
{
	guard(mutex)(&x->lock);
	if (is_pwd)
		memcpy(x->passwd, buf, GPON_PASSWD_LEN);
	else
		memcpy(x->sn, buf, GPON_SN_LEN);
	x->armed = false;

	return 0;
}

/* what is stored now, for the debugfs node to report.  A read is not a
 * change of identity and does not un-arm anything. */
void an7552_mac_get_identity(struct an7552_mac *x, u8 *buf, bool is_pwd)
{
	guard(mutex)(&x->lock);
	memcpy(buf, is_pwd ? x->passwd : x->sn,
	       is_pwd ? GPON_PASSWD_LEN : GPON_SN_LEN);
}

/*
 * The text forms of the two identities, for the debugfs node.
 *
 * The serial number is eight bytes in two 32-bit registers - four in
 * G_VENDOR_ID and four in G_VS_SN - so eight hex digits, not twelve, are
 * what the second half holds.  Both spellings are accepted: the 4+8 one,
 * which is what the vendor ID and the number read like, and the 16 hex
 * digits an7552_mac_read_sn() already takes out of the flash.  An upper
 * case letter is where the two differ in what they mean, so it is allowed
 * only in the first four.
 *
 * A password is ten bytes, plain or as twenty hex digits.  Ten zeros is
 * not a password: an7552_mac_has_password() reads it as none, which is
 * what "no password configured" looks like anyway.
 */
int an7552_mac_set_identity_text(struct an7552_mac *x, const char *str,
				 bool is_pwd)
{
	u8 buf[GPON_PASSWD_LEN];
	size_t len, i;

	while (*str == ' ' || *str == '\t')
		str++;
	len = strnlen(str, GPON_PASSWD_LEN * 2);
	while (len && (str[len - 1] == ' ' || str[len - 1] == '\t' ||
		       str[len - 1] == '\n' || str[len - 1] == '\r'))
		len--;

	if (is_pwd) {
		if (len == GPON_PASSWD_LEN) {
			memcpy(buf, str, len);
		} else if (len == GPON_PASSWD_LEN * 2) {
			if (hex2bin(buf, str, GPON_PASSWD_LEN))
				return -EINVAL;
		} else {
			return -EINVAL;
		}
	} else {
		if (len == GPON_SN_LEN * 2) {
			if (hex2bin(buf, str, GPON_SN_LEN))
				return -EINVAL;
		} else if (len == 12) {
			for (i = 0; i < 4; i++) {
				if (str[i] < 'A' || str[i] > 'Z')
					return -EINVAL;
				buf[i] = str[i];
			}
			if (hex2bin(buf + 4, str + 4, 4))
				return -EINVAL;
		} else {
			return -EINVAL;
		}
	}

	return an7552_mac_set_identity(x, buf, is_pwd);
}

void an7552_mac_start(struct an7552_mac *x)
{
	/* not in the middle of a mode change that may still be taken back */
	guard(mutex)(&x->mode_lock);
	scoped_guard(mutex, &x->lock) {
		if (x->started)
			return;
		x->started = true;
		/*
		 * prepare_gpon() (gpon.c:100-104, GPON or GPON_SYM by working_mode):
		 * the WAN selection first, then the SCU reset of the PHY, then the GPON
		 * mode.  gpon_dev_init() selects it again, as the vendor does.
		 */
		gpon_select_wan(x);
		airoha_xpon_set_dei_drop(true);
		an7552_phy_scu_reset(x->phy);
		an7552_phy_set_mode(x->phy, x->mode);
		x->phy_ready = an7552_phy_is_ready(x->phy);
		gpon_reset_ids(x);
		gpon_dev_reset(x, false);
	}

	/*
	 * Re-say the mode to a front end that attached after it was set.  Out
	 * here, not under x->lock: the front end takes its own lock and calls
	 * back up into this MAC from its timers, and the two orders must not
	 * nest.  mode_lock still keeps a mode change from slipping in.
	 */
	if (READ_ONCE(x->phy->traffic_up))
		an7552_phy_set_traffic(x->phy, true);
}

void an7552_mac_stop(struct an7552_mac *x)
{
	mutex_lock(&x->lock);
	if (!x->started) {
		mutex_unlock(&x->lock);
		return;
	}
	x->started = false;
	cancel_delayed_work(&x->to1_work);
	cancel_delayed_work(&x->to2_work);
	cancel_delayed_work(&x->ber_work);
	cancel_delayed_work(&x->mib_work);
	cancel_delayed_work(&x->silence_work);
	cancel_delayed_work(&x->rogue_work);
	cancel_work(&x->reset_work);
	x->rogue = false;
	x->rogue_off = false;
	x->rogue_retry = 0;
	/*
	 * gpon_disable_with_option() clears gpGponPriv->typeBOnGoing
	 * (xpon.ann:0x18f8c-0x18f98, the AN7552_MAC_DOWN bit) on the
	 * way down, and so does gpon_disable() above, but stop takes its own
	 * path and did not.  Left set, the next ploam_recv_ranging_time() reads
	 * as "back from Type B protection" and loads the typeb_key[] of the
	 * previous session into the shadow registers before the OLT has sent a
	 * single key - and takes the PON LED with it.  The keys go with it;
	 * they are shadow material of a session that no longer exists.
	 *
	 * The vendor has no equivalent of silence_work, so its gpon_silence
	 * simply survives a disable; here the cancel above is what removed the
	 * only path that would have cleared the flag, and a stop is a full
	 * teardown, so the restarted ONU must not inherit an O2 -> O3 block
	 * from the one that was torn down.
	 */
	x->typeb_popup = false;
	memset(x->typeb_key, 0, sizeof(x->typeb_key));
	x->silence = false;
	an7552_trans_power_switch(x->phy, false);
	an7552_phy_stop(x->phy);
	gpon_reset_ids(x);
	gpon_tcont_remove_all(x);
	gpon_gem_remove_all(x);
	mac_wr(x, G_INT_ENABLE, 0);
	mac_wr(x, G_INT_STATUS, 0xffffffff);
	gpon_reset_ctrl(x, true);
	gpon_set_state(x, GPON_O1);
	mutex_unlock(&x->lock);

	/*
	 * Everything above is queued on x->wq and none of it may still be running
	 * when the caller goes on to drain or destroy the queue.  cancel_* above
	 * only takes the work off the queue; this is what waits for the handlers
	 * that had already started.  carrier_work is left to run - it only
	 * reports the state and reaches the front end, and gpon_set_state() has
	 * just queued it - but it too has to be finished before the queue goes.
	 */
	flush_workqueue(x->wq);
}

int an7552_mac_init(struct an7552_mac *x)
{
	int i;

	x->gem = devm_kcalloc(x->dev, GPON_MAX_GEM_ID, sizeof(*x->gem),
			      GFP_KERNEL);
	if (!x->gem)
		return -ENOMEM;

	for (i = 0; i < GPON_MAX_TCONT; i++)
		x->tcont_alloc[i] = GPON_UNASSIGN_ALLOC_ID;
	x->tcont_valid = 0;
	x->tcont_cnt = 1;

	x->state = GPON_O1;
	x->us_gem = GPON_UNASSIGN_GEM_ID;
	x->rsp_time = GPON_RSP_TIME_DEFAULT;
	x->guard_bit_num = GPON_GUARD_BIT_NUM_DEFAULT;
	x->to1_left = GPON_TO1_MAX_CNT;
	x->to1_ms = GPON_TO1_MS;
	x->to2_ms = GPON_TO2_MS;
	x->sd_thld = GPON_SD_DEFAULT;
	x->sf_thld = GPON_SF_DEFAULT;
	x->up_omci = true;
	x->up_data = true;
	x->dei_mark = true;
	gpon_reset_ids(x);

	INIT_DELAYED_WORK(&x->to1_work, gpon_to1_work);
	INIT_DELAYED_WORK(&x->to2_work, gpon_to2_work);
	INIT_DELAYED_WORK(&x->ber_work, gpon_ber_work);
	INIT_WORK(&x->reset_work, gpon_reset_work);
	INIT_WORK(&x->carrier_work, an7552_mac_carrier_work);
	INIT_WORK(&x->ppe_flush_work, an7552_mac_ppe_flush_work);
	INIT_DELAYED_WORK(&x->mib_work, gpon_mib_work);
	INIT_DELAYED_WORK(&x->silence_work, gpon_silence_work);
	INIT_DELAYED_WORK(&x->rogue_work, gpon_rogue_work);

	/* MAC idle until userspace starts the WAN */
	mac_wr(x, G_INT_ENABLE, 0);
	mac_wr(x, G_INT_STATUS, 0xffffffff);

	return gpon_mib_tables_init(x);
}

/* ---------------------------------------------------------------------
 * ONU control requested by the OMCI stack (xmcs_gpon.c / xmcs_if.c)
 * ------------------------------------------------------------------- */

/*
 * xmcs_set_emergency_state().  The stock only flips the state; here the
 * stop is the one Disable_Serial_Number makes (transmitter off, no bursts,
 * no T-CONTs), and the release leaves O7 the way the OLT's release does.
 */
int an7552_mac_set_emergency(struct an7552_mac *x, bool on)
{
	guard(mutex)(&x->lock);
	if (on) {
		gpon_emergency_stop(x);
	} else if (x->emergency) {
		x->emergency = false;
		gpon_disable(x, true);
	}

	return 0;
}

static void gpon_silence_work(struct work_struct *w)
{
	struct an7552_mac *x = container_of(to_delayed_work(w),
					     struct an7552_mac, silence_work);

	guard(mutex)(&x->lock);
	x->silence = false;
}

/* xmcs_set_gpon_silence(): gpon_start_silence() / gpon_stop_silence() */
int an7552_mac_set_silence(struct an7552_mac *x, bool on)
{
	guard(mutex)(&x->lock);
	x->silence = on;
	if (on)
		mod_delayed_work(x->wq, &x->silence_work,
				 msecs_to_jiffies(GPON_SILENCE_MS));
	else
		cancel_delayed_work(&x->silence_work);

	return 0;
}

/*
 * xmcs_gpon_reset_service() on MIB reset: drop every GEM port and T-CONT
 * the OMCI stack created; the OMCC and the ONU-ID T-CONT stay.
 */
int an7552_mac_mib_reset(struct an7552_mac *x)
{
	int i;

	mutex_lock(&x->lock);
	for (i = 0; i < GPON_MAX_GEM_ID; i++)
		if (i != x->omcc)
			__gpon_gem_del(x, i, true);
	for (i = 1; i < GPON_MAX_TCONT; i++)
		if (x->tcont_valid & BIT(i))
			gpon_tcont_remove(x, x->tcont_alloc[i]);
	/* every T-CONT but the ONU-ID one is gone */
	x->tcont_cnt = 1;
	for (i = 0; i < GPON_MAX_US_RULES; i++)
		WRITE_ONCE(x->us_rule[i].valid, false);
	for (i = 0; i < GPON_MAX_VID_MAP; i++)
		WRITE_ONCE(x->vid_map[i].valid, false);
	WRITE_ONCE(x->us_gem, GPON_UNASSIGN_GEM_ID);
	smp_store_release(&x->dscp_en, false);
	mutex_unlock(&x->lock);
	an7552_mac_carrier(x);

	return 0;
}

/* xmcs_set_SD_SF_threshold(): 0 keeps the current value */
int an7552_mac_set_sd_sf(struct an7552_mac *x, u8 sd, u8 sf)
{
	if (sd && (sd < GPON_SD_MIN || sd > GPON_SD_MAX))
		return -EINVAL;
	if (sf && (sf < GPON_SF_MIN || sf > GPON_SF_MAX))
		return -EINVAL;

	guard(mutex)(&x->lock);
	if ((sd ?: x->sd_thld) <= (sf ?: x->sf_thld))
		return -EINVAL;
	if (sd)
		x->sd_thld = sd;
	if (sf)
		x->sf_thld = sf;

	return 0;
}

/* xmcs_set_act_timer(): 0 keeps the current value */
int an7552_mac_set_timers(struct an7552_mac *x, u32 to1_ms, u32 to2_ms)
{
	guard(mutex)(&x->lock);
	if (to1_ms)
		x->to1_ms = to1_ms;
	if (to2_ms)
		x->to2_ms = to2_ms;

	return 0;
}

/* GPON / GPON-SYM; taken over at the next start (pon interface down) */
int an7552_mac_set_mode(struct an7552_mac *x, enum an7552_xpon_mode mode)
{
	enum an7552_xpon_mode old;
	int ret;

	/* xmcs_set_connection_start(): taken at the next start of pon%d */
	if (mode != AN7552_XPON_GPON && mode != AN7552_XPON_GPON_SYM)
		return -EOPNOTSUPP;

	/*
	 * mode_lock keeps a start away until the front end has answered: a start
	 * in that window would run on a mode that is then taken back.  It is
	 * taken before x->lock, as an7552_mac_start() does, and x->lock is not
	 * held across the call to the front end.
	 */
	guard(mutex)(&x->mode_lock);
	scoped_guard(mutex, &x->lock) {
		if (x->started)
			return -EBUSY;
		old = x->mode;
		x->mode = mode;
	}

	/*
	 * The front end runs in whatever mode its own calibration names until
	 * it is told otherwise, and the check above has just established that no
	 * link has been started - which is the only window in which the
	 * transmitter is still off, because nothing turns it on before a start.
	 * That makes this the moment to hand the mode over.
	 *
	 * Left to ride in with the traffic instead, the first and only telling
	 * would arrive at O5, with the transmitter already on since O2: every
	 * upstream burst from ranging to O5, and the T0/T1 delay byte that opens
	 * each one, would go out in the mode the flash named rather than the
	 * configured one - or not go out at all, since a front end that refuses
	 * a mode change while it transmits answers -EBUSY.
	 *
	 * Outside the MAC lock on purpose: the front end calls back up into the
	 * MAC from its own timers with its own lock held, and taking these two
	 * in the other order would be the deadlock.
	 *
	 * A refused mode is put back, so that the mode the MAC runs in and the
	 * mode the front end runs in stay the same one thing.
	 */
	ret = an7552_phy_laser_mode(x->phy, mode);
	if (ret) {
		guard(mutex)(&x->lock);
		x->mode = old;
	}

	return ret;
}

/* ANI-G administrative state: keep the transmitter off while locked */
int an7552_mac_set_admin(struct an7552_mac *x, bool down)
{
	guard(mutex)(&x->lock);
	x->admin_down = down;
	an7552_phy_tx_power(x->phy, !down);
	if (down)
		an7552_trans_power_switch(x->phy, false);
	else if (x->started && x->state >= GPON_O3 && x->state <= GPON_O6)
		an7552_trans_power_switch(x->phy, true);

	return 0;
}

/* gponDevClearHwCounter() plus the software counters */
void an7552_mac_clear_counters(struct an7552_mac *x)
{
	mutex_lock(&x->lock);
	gpon_mib_tables_init(x);
	gpon_mib_table_update(x);
	memset(&x->stats, 0, sizeof(x->stats));
	x->bip_total = 0;
	x->bip_pending = 0;
	mutex_unlock(&x->lock);
	an7552_phy_counter_clear(x->phy, AN7552_PHY_ERR_CNT_CLR |
				      AN7552_PHY_BIP_CNT_CLR | AN7552_PHY_RXFRAME_CNT_CLR |
				      AN7552_PHY_TXFRAME_CNT_CLR);
}

void an7552_mac_dump_regs(struct an7552_mac *x, struct seq_file *m)
{
	static const struct {
		const char *name;
		u32 off;
	} regs[] = {
		{ "G_ONU_ID", G_ONU_ID },
		{ "G_GBL_CFG", G_GBL_CFG },
		{ "G_INT_STATUS", G_INT_STATUS },
		{ "G_INT_ENABLE", G_INT_ENABLE },
		{ "G_OMCI_ID", G_OMCI_ID },
		{ "G_PLOAMd_FIFO_STS", G_PLOAMD_FIFO_STS },
		{ "G_PLOAMu_FIFO_STS", G_PLOAMU_FIFO_STS },
		{ "G_AES_CFG", G_AES_CFG },
		{ "G_PLOu_GUARD_BIT", G_PLOU_GUARD_BIT },
		{ "G_PLOu_PRMBL_TYPE1_2", G_PLOU_PRMBL_TYPE1_2 },
		{ "G_PLOu_PRMBL_TYPE3", G_PLOU_PRMBL_TYPE3 },
		{ "G_PLOu_DELM_BIT", G_PLOU_DELM_BIT },
		{ "G_PRE_ASSIGNED_DLY", G_PRE_ASSIGNED_DLY },
		{ "G_EQD", G_EQD },
		{ "G_RSP_TIME", G_RSP_TIME },
		{ "G_ACTIVATION_ST", G_ACTIVATION_ST },
		{ "G_MBI_MPI_STOP", G_MBI_MPI_STOP },
		{ "DBG_DLY", DBG_DLY },
		{ "DBG_GRP_0", DBG_GRP_0 },
		{ "DBG_GRP_1", DBG_GRP_1 },
		{ "DBG_RX_GEM_CNT", DBG_RX_GEM_CNT },
		{ "DBG_RX_CRC_ERR_CNT", DBG_RX_CRC_ERR_CNT },
		{ "DBG_RX_GTC_CNT", DBG_RX_GTC_CNT },
		{ "DBG_TX_GEM_CNT", DBG_TX_GEM_CNT },
		{ "DBG_TX_BST_CNT", DBG_TX_BST_CNT },
		{ "DBG_GEM_HEC_ONE_ERR_CNT", DBG_GEM_HEC_ONE_ERR_CNT },
		{ "DBG_GEM_HEC_TWO_ERR_CNT", DBG_GEM_HEC_TWO_ERR_CNT },
		{ "DBG_GEM_HEC_UC_ERR_CNT", DBG_GEM_HEC_UC_ERR_CNT },
		{ "CAL_GNT_SIZE_CTRL_STS", CAL_GNT_SIZE_CTRL_STS },
	};
	int i;

	for (i = 0; i < ARRAY_SIZE(regs); i++)
		seq_printf(m, "%-24s [0x%03x] 0x%08x\n", regs[i].name,
			   regs[i].off, mac_rr(x, regs[i].off));
	for (i = 0; i < 8; i++)
		seq_printf(m, "G_TCONT_ID_%d_%d%*s [0x%03x] 0x%08x\n",
			   i * 2, i * 2 + 1, i < 5 ? 10 : 9, "",
			   G_TCONT_ID(i * 2), mac_rr(x, G_TCONT_ID(i * 2)));
}

