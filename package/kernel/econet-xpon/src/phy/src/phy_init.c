/*
* File Name: phy_init.c
* Description: Initialization for xPON PHY
*
******************************************************************/
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/math64.h>	/* PORT 6.18: div_u64() for the fixed-point BER calc */
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <ecnt_hook/ecnt_hook_pon_mac.h>
#include <ecnt_hook/ecnt_hook_pon_phy.h>

#include <asm/tc3162/tc3162.h>
#include <asm/tc3162/ledcetrl.h>

#include "phy_global.h"
#include "i2c.h"
#include "phy.h"
#include "phy_def.h"
#include "phy_api.h"
#include "mt7570_reg.h"
#include "mt7570_def.h"
#include "phy_types.h"
#include "phy_debug.h"
#include "phy_init.h"
#include "phy_tx.h"


extern atomic_t eponMacRestart_flag;
extern int mt7570_select;						//by HC 20150302

extern int mt7570_version;						//by HC 20150320
extern int phy_version;							//The version of phy.c in main trunk 
int phy_date_version = 160329;						//The version of phy_init.c in main trunk

extern int internal_DDMI;                       //by HC 20150119
extern uint mt7570_alarm;						//by YMC 20150414
extern int TEC_switch;							// by HC 20150515
extern int BiasTracking_switch;				// by HC 20150921


#ifndef TCSUPPORT_CPU_EN7521
void pon_phy_reset_init(void);
extern void (*fe_reset_pon_phy_reset_init)(void);
#endif
extern void phy_tx_fault_reset(void);

/* TODO port 2.6.36->6.18: timers moved to timer_setup()/timer cbs take
 * (struct timer_list *).  phy_event_poll() keeps its old prototype (phy.h),
 * so phy.c provides a modern-signature wrapper used as the timer callback. */
extern void phy_event_poll_timer(struct timer_list *t);
#define PHY_EVENT_POLL_MS	1000
#define PHY_INT_RECOVER_MS	50000

/* TODO port: cross-file phy fns lacking a header prototype (defined in
 * phy.c / mt7570.c).  Forward-declare here for the api-dispatch table. */
extern void mt7570_param_status_real(PPHY_TransParam_T transceiver_param);
extern void phy_trans_param_status_real(PPHY_TransParam_T transceiver_param);
extern ushort phy_round_trip_delay_sof(void);

PHY_GlbPriv_T *gpPhyPriv = NULL;

/* PORT: RX-impedance manual code sweep (ANACAL1 auto-cal rails to 0 on this board).
 * -1 = leave whatever phy_rx_imp_check set; 0..31 = force PHYSET6[12:8] via 'J' cmd. */
int rx_imp_code = -1;
module_param(rx_imp_code, int, 0644);
/* RX signal-detect polarity (XPON_SETTING bit6); sweep 0/1 via 'J' cmd. */
int rx_sd_inv = 0;
module_param(rx_sd_inv, int, 0644);
/* APD bias DAC code: default 0x60 (~39V, tuned for LOS-detect); sweep higher for data-RX SNR. */
int apd_code = 0x60;
module_param(apd_code, int, 0644);
/* EXPERIMENT toggles (stock does NOT do these in phy_dev_init; decode Finding 5) — suspects for the
 * fixed PI=0x36 phase offset. Set via sysfs before running R. Default 0 = keep (current behaviour). */
int exp_skip_pll = 0;      module_param(exp_skip_pll, int, 0644);    /* skip A60928 PLL/SW reset (243-263) */
int exp_skip_sigdet = 0;   module_param(exp_skip_sigdet, int, 0644); /* skip sigdet/PBUS block (275-284) */
/* bisect mask for the 'w' config-diff cmd: b0=PHYSET5 b1=0x12c b2=ANAPWD b3=ANATXREG1 b4=ANADDS3
 * b5=TDCSET2 b6=0x228 b7=0x230. Default 0xff = all. */
int wmask = 0xff;          module_param(wmask, int, 0644);
int fin_b0 = 1;            module_param(fin_b0, int, 0644);  /* PHYSET2 bit0 in R finalize (suspected lock gate) */
/* SESSION 7: default 1 = HOLD THE LASER OFF. After each CDR lock, set XPON_SETTING(0x138)
 * bit7 (burst-invert = vendor's "switch off the laser"). The R bring-up writes 0x10F (bit7=0,
 * laser-capable) and the MAC still does sn_onu_send in O3, so without this the laser fires on
 * the live PON. This unit can't emit a registrable burst anyway (no laser cal). Set 0 (rebuild)
 * only to deliberately transmit. */
int tx_laser_off = 0;      module_param(tx_laser_off, int, 0644);  /* default 1 = laser held OFF (safe). Set 0 to transmit. */
int ploam_verbose = 0;     module_param(ploam_verbose, int, 0644);  /* 1 = print PLOAM-DS/SNDBG/GPON-INT flood (default 0 = quiet, so register dumps are readable) */
int tx_amp = 0;            module_param(tx_amp, int, 0644);  /* ANATXREG1[19:16] TX drive swing. LIVE diff: stock=0, port-default=9. Default 0=stock; sweep 0..0xf vs OLT. */
/* ANAPWD (0xbfaf0150) bits[8:7]. Port sets |=0x180 (both) mixing stock Ana_Power_Saving mode1; disasm says
 * mode0 (normal) CLEARS bit8. Default 0x180 = current behaviour (build unchanged); test 0 = clear both = stock mode0. */
int po_anapwd = 0x180;     module_param(po_anapwd, int, 0644);
/* dcl_mode: 0 = OPEN-LOOP (seed Ibias/Imod DACs, do NOT start the HW DCL/APC servo) — matches
 * STOCK on this unit (flash slot 0x90 empty => stock en7571_config runs NEITHER Single- nor
 * OpenLoopMode => no servo). The port unconditionally started the servo (en7571_reg_init_full +
 * single_closed_loop_mode), which regulated the laser bias/mod DACs (0x138/0x148) to 0 by O3 =>
 * under-biased laser => burst not OLT-decodable. 1 = old closed-loop-servo behavior. */
int dcl_mode = 0;          module_param(dcl_mode, int, 0644);
/* run_scl: gate en7571_single_closed_loop_mode() in tx_apc_bringup. STOCK gates SingleClosedLoopMode
 * on flash slot 0x90(144)==1 (en7571_config); on this unit flash[0x90]=0xffffffff => STOCK SKIPS IT.
 * The port wrongly gated on flash 0x94(148)==magic (always true here) => ran the mpdh-freeze+hw_reset
 * servo path stock does NOT run -> suspected cause of the Ibias/Imod DAC collapse to 0.
 * -1 = auto (match stock: run only if flash[0x90]==1), 0 = force-skip, 1 = force-run(old). */
int run_scl = -1;          module_param(run_scl, int, 0644);
/* dual_loop: ★ DEAD ON THIS HW (session-11 disasm, 2026-07-05). Sets 0x13c/0x14c bit0 = the mt7570
 * (external MT7570 transceiver) ERC register file. phy.ko dispatches en7571 vs mt7570 on select-globals
 * (en7571_phy_probe sets en7571_select); EN7528 = the en7571 path. 0x13c/0x14c/0x16c are touched ONLY by
 * mt7570_* fns => writing them on EN7528 is a NO-OP for the real loop. The real en7571 loop = 0x228 DCL
 * (see tx_closeloop). DEFAULT NOW 0 (was 1 = pointless writes). Kept for history; do NOT rely on it. */
int dual_loop = 0;         module_param(dual_loop, int, 0644);
/* tx_closeloop: ★ THE VERIFIED en7571 DCL close (session-11, disasm-faithful across H660+407 phy.ko).
 * Stock runs en7571_DCL_start (0x228 byte1 bit0) with the invariant DCL_stop -> mpdh_stepsize0 ->
 * hw_reset[clears the loop integrator, while stopped] -> DCL_start, over the 0x24c/0x25c setpoints
 * seeded by en7571_load_tx_cal_data. The port ran OPEN-LOOP (static DACs) on the WRONG premise that
 * "flash[0x90] empty => stock is open-loop" — but stock's DCL_start in en7571_init is UNCONDITIONAL
 * (flash[0x90] only gates the ADDITIONAL SingleClosedLoopMode re-close). The port's old dcl_mode=1 fired
 * a BARE DCL_start in reg_init_full BEFORE the setpoints were seeded (apc_bringup) => DCL regulated to
 * garbage => DAC collapse. THIS param runs the close in apc_bringup AFTER the seed, the stock way, so
 * the DCL holds the calibrated Ibias/Imod split (= correct extinction). 0 = current open-loop O3
 * baseline (default, NO regression); 1 = run the verified en7571 DCL close. */
int tx_closeloop = 0;      module_param(tx_closeloop, int, 0644);
/* erc_openloop: ★2026-07-08 RE (wf_86da7401, stock phy.ko disasm). Stock's en7571_OpenLoopMode(1) @0x41588,
 * called UNCONDITIONALLY at every O5 link-up edge (en7571_islinkstauschange @0x3f1f0), sets SIF-0x70
 * 0x13C/0x14C byte0 bits[1:0]=0b10 (ERC_open_loop_mode "DAC-follow") — this COUPLES the 0x138/0x148 DAC to the
 * real laser bias/mod current and exposes applied bias at 0x13C[2:3]. The port NEVER did this (left CS3 at
 * default 0b00), so 0x138 writes land in a decoupled shadow -> frozen 0x13C-read (0x13a) + frozen MPD (~0xe0)
 * = the low-launch-power service-admission wall. The OLD dual_loop block wrote |0x01 (ERC_start=closed-loop,
 * the WRONG mode) and was default-off. 1 = enable DAC-follow at apc_bringup (THE FIX). Pairs with apc_target
 * (the 1Hz MPD-hold loop that ramps 0x138 = stock en7571_Pavg_close_loop). */
int erc_openloop = 0;      module_param(erc_openloop, int, 0644);
/* NOTE (2026-07-05): tested mainline's G_PLOu_PRMBL_TYPE3 ebl_en@bit16 (vs our vendor headers' bit24)
 * via a t3_ebl16 param — HW REFUTED it: wrote=01010f77 read back 01000f77, i.e. EN7528 CLEARS bit16
 * (read-only-0); bit24 is the real ebl_en and is already set (ebl=1). mainline's bit16 is EN751221-only.
 * Burst overhead confirmed correct again; the O3->O4 wall stays optical. Test code reverted. */
/* tx_force / tx_kt (2026-07-05, merbanan mainline LDDLA diff): the mainline en7571 dual-closed-loop
 * runs the DCL with 0x258[1:0]=FORCE (en7571_force_mode, "auto-lock not used") + 0x230[0]=KT-on
 * (en7571_hwkt), forcing the flash-calibrated Pav/P1 extinction. Our port ran the DCL with 0x258=AUTO +
 * KT off = an INCOMPLETE dual-CL (free-runs to the open-loop point = session-11 "inert"). These apply
 * FORCE/KT in tx_apc_bringup (AFTER CDR lock, at TX-burst time — the prior "FORCE freezes CDR" was in
 * the pre-lock RX reg_init path, a different context). Test with tx_closeloop=2 tx_force=1 tx_kt=1. */
int tx_force = 0;          module_param(tx_force, int, 0644);
int tx_kt = 0;             module_param(tx_kt, int, 0644);
/* Laser DC bias code (12-bit DAC @ SIF 0x138). Default = LUT room-temp seed 0x1FB, but this
 * BOSA's threshold is higher; tune at runtime via /sys/module/econet_xpon/parameters/tx_ibias
 * then apply with `echo b > /proc/econet_xpon_los`. Vendor clamp 0xF7F; keep <= 0x666. */
int tx_ibias = 0x1FB;      module_param(tx_ibias, int, 0644);
/* Laser MODULATION swing code (12-bit DAC @ SIF 0x148). H660 flash-cal reads 0x157, but the
 * registered-working Nokia G-140W-F drove Mod=0xc02 (~9x higher) → 0x157 likely UNDER-modulates
 * (unslice-able eye at the OLT). Sweep live vs OLT-decodability: echo <v> > tx_imod; echo i > ... */
int tx_imod = 0x157;       module_param(tx_imod, int, 0644);
/* BIST pattern held by the 'c' continuous-CW command (bench power-meter measurement):
 * 0x09=ALL1 (solid high), 0x0a=ALL0 (solid low), 0x06=PRBS23. Sweep for the extinction test. */
int tx_pat = 0x09;         module_param(tx_pat, int, 0644);
/* tgen_settle: ms delay between the T0C/T1C timer write and the TGEN_reset latch in apc_bringup (H4 fix,
 * stock disasm SETTLE#2). Default 10 = stock-faithful; 0 = old (no settle) for A/B. */
int tgen_settle = 10;      module_param(tgen_settle, int, 0644);
/* tgen_dark: 1 = hold Ibias=0 (laser dark) across the TGEN latch in apc_bringup, restore the calibrated bias
 * after (H3 fix, stock latches TGEN dark-first). 0 = old (bias-then-latch). A/B lead for O4-ranging marginality. */
int tgen_dark = 0;         module_param(tgen_dark, int, 0644);
/* tdc_holdover: 1 = engage the TX TDC holdover (stock phy_tx_tdc_holdover, 0xbfaf01f0|=0x20 + 0xbfaf0104|=0x10)
 * at end of apc_bringup to hold the burst timing reference (kill burst-edge jitter). Port never had it. `echo t`
 * toggles at runtime. A/B lead for O4-ranging marginality. */
int tdc_holdover = 0;      module_param(tdc_holdover, int, 0644);
/* txsd_fail_cnt: ★2026-07-09 — counts TRANS_SD_FAIL_INT (transceiver TX signal-detect fail) IRQs. The 407 stock
 * calibrates the TxSD comparator (en7571_TxSD_level_set); the port never does. Read via /sys during O4 ranging:
 * if it climbs, an uncalibrated TxSD is mis-asserting (cutting the laser mid-burst) → run the `y` cal. If it
 * stays 0, TxSD is inert and the lead is dead. */
unsigned int txsd_fail_cnt = 0;  module_param(txsd_fail_cnt, uint, 0644);
/* ncpo_val: ★2026-07-09 — ANASET14(0xbfaf0194) NCPO seed. The port seeds 0x17E21965 but its loop never pulls it
 * to stock's LOCKED value 0x1E1A9FBE (CONFIRMED: port reads 0x17e21965 stuck-at-seed when rx_sync=0xa) → recovered/
 * loop-timed TX clock at the wrong point → ranging-burst edge un-measurable. Test stock's locked value directly. */
unsigned int ncpo_val = 0x17E21965u;  module_param(ncpo_val, uint, 0644);
int gpon_target = 0;       module_param(gpon_target, int, 0644);
/* Runtime GPON identity — no serial/password/MAC is hardcoded in the driver.
 * gpon_sn: ONU serial = 4 vendor ASCII chars + 8 hex digits (e.g. "MTKG00000001").
 * gpon_pw: PLOAM registration password (ASCII, up to GPON_PASSWD_LENS bytes).
 * wan_mac: WAN netdev MAC ("xx:xx:xx:xx:xx:xx"); empty => random locally-administered.
 * Provide these at insmod, sourced from the unit's own factory flash (e.g. an init
 * script that reads the device's romfile/nvmem), so the module ships no credentials. */
char *gpon_sn = "";        module_param(gpon_sn, charp, 0644);
char *gpon_pw = "";        module_param(gpon_pw, charp, 0644);
char *wan_mac = "";        module_param(wan_mac, charp, 0644);
/* XPON_SETTING(0xbfaf0138) value written at laser bring-up. bit7=PHY_BURST_EN_INV (burst-enable
 * POLARITY — BOSA-specific; the port never set it per-board), bit4=PHY_TX_SD_INV. If this BOSA's
 * burst-enable is active-low, bit7 must be 1 (0x18F) or the laser fires between bursts, not during
 * the SN burst. Sweep 0x10F/0x11F/0x18F/0x19F vs OLT response. bit6=0 keeps RX-SD correct. */
int tx_xpon = 0x10F;       module_param(tx_xpon, int, 0644);
/* auto-lock keeper (function bodies defined after en7571_reg_init_full) */
int gpon_do_lock(int quiet);
static int gpon_lock_keeper(void *arg);
struct task_struct *gpon_keeper_task = NULL;

struct workqueue_struct * i2c_access_queue = NULL;


#if defined(TCSUPPORT_CT_PON)
int los_status = 0;
#endif
void phy_tx_ctl(unchar val);
/*****************************************************************************
//function :
//		gpio_tx_dis_reset
//description : 
//		this function is used to pull down tx_dis of EN7570.
//		Since tx_dis is connected to a GPIO output pin in MT7520, the setting in this function need to 
//		fit customer's allocation of tx_dis.
//		ex: On reference BOB board, tx_dis matches GPIO25, so this function pulls GPIO25(tx_dis) low.
//		On ASB's BOB board, tx_dis matches GPIO40, so this function should pulls GPIO40(tx_dis) low.
//input :	
//		N/A
//output :
//		N/A
******************************************************************************/
void gpio_tx_dis_reset(void){
#ifdef CONFIG_USE_MT7520_ASIC
    phy_tx_ctl(PHY_DISABLE);
	phy_tx_fault_reset();
#endif
}


/*****************************************************************************
//function :
//		gpio_BOSA_Tx_power_on
//
//description : 
//		this function is used to turn on BOSA Tx 3V3.
//		BOSA_Tx power switch is connected to a GPIO output pin in MT7520, the setting in this function need to 
//		fit customer's allocation of 3V3_BOSA_Tx.
//		ex: On reference 7570 BOB board, 3V3_BOSA_Tx matches GPIO2, so this function pulls GPIO2 low.
//
//input :	
//		N/A
//
//output :
//		N/A
//
//date :
//		by YMC 20150731
//
******************************************************************************/
void gpio_BOSA_Tx_power_on(void)
{
#ifdef CONFIG_USE_MT7520_ASIC
	ledTurnOff(LED_PHY_VCC_DISABLE);
#endif
}


/*****************************************************************************
//function :
//		xPON_Phy_Reset
//description : 
//		this function is used to reset xPON PHY
//input :	
//		N/A
//output :
//		N/A
******************************************************************************/
//static spinlock_t  pon_phy_lock;
void xPON_Phy_Reset(void){
#ifdef CONFIG_USE_MT7520_ASIC
	uint val = 0;
#ifdef TCSUPPORT_CPU_EN7521
	//xpon phy top reset ,EN7521 high active
	val = IO_GPHYREG(TOP_RST_CTRL_SW2);
	val |=0x01;
	IO_SPHYREG(TOP_RST_CTRL_SW2, val);
	udelay(1);
	//release xpon phy top reset 
	val &=~(0x01);
	IO_SPHYREG(TOP_RST_CTRL_SW2, val);
#else
	//xpon phy top reset
	val = IO_GPHYREG(TOP_TEST_MISC0_CTRL);
	val &=~(1<<25);
	IO_SPHYREG(TOP_TEST_MISC0_CTRL, val);
	udelay(1);
	//release xpon phy top reset 
	val |=(1<<25);
	IO_SPHYREG(TOP_TEST_MISC0_CTRL, val);
#endif
#endif
}

	
/*****************************************************************************
//function :
//		phy_int_recover_expires
//description : 
//		this function is used to recover interrupt
//input :	
//		interrupt type          
//		TRANS_LOS_INT				0x01
//		PHY_LOF_INT					0x02
//		TRANS_TF_INT				0x04
//		TRANS_INT					0x08
//		TRANS_SD_FAIL_INT			0x10
//		PHYRDY_INT					0x20
//output :
//		N/A
******************************************************************************/
static void phy_int_recover_expires(struct timer_list *t)
{
#if defined(CONFIG_USE_MT7520_ASIC)
	phy_int_config((PHY_ILLG_INT|PHYRDY_INT|PHY_LOF_INT|TRANS_LOS_INT)) ;
#else
	phy_int_config((PHYRDY_INT|PHY_LOF_INT|TRANS_LOS_INT)) ;
#endif
}
	
int LDDLA_task_wait(void *arg)	/* TODO port: kthread fn sig int(void*) + return */
{
	PON_PHY_MSG(PHY_MSG_TRACE, "mt7570_task_wait\n");

	while(!kthread_should_stop())
	{
		/*wait_event_interruptible(pon_phy_wq_7570, (atomic_read(&pon_phy_7570_flag) != 0)); *//* replace !=0 with == 1 */

			mt7570_internal_clock();
			msleep(1000);
	}
	return 0;
}

/*****************************************************************************
//function :
//		phy_dev_init
//description : 
//		this function is used to set 7525 to XPON mode
//input :	
//		N/A
//output :
//		N/A
******************************************************************************/
//#define TCSUPPORT_PON_ROGUE_ONU		//YMC_20150206
int phy_dev_init(void)
{
    uint read_data = 0;
    uint write_data = 0;
	uint data = 0;

	/* INSTRUMENTED stock phy_dev_init prologue: flushed markers before each write so the
	 * last one printed before a hang pinpoints the crashing write. */
	printk(KERN_DEBUG "PDI-0 enter phy_dev_init\n"); mdelay(4);
	printk(KERN_DEBUG "PDI-1 pre 0xbfa2015c|=0x200001\n"); mdelay(4);
	regWrite32(0xbfa2015c, regRead32(0xbfa2015c) | 0x00200001);
	printk(KERN_DEBUG "PDI-2 pre 0xbfaf0108&=~4\n"); mdelay(4);
	regWrite32(0xbfaf0108, regRead32(0xbfaf0108) & ~0x4u);
	printk(KERN_DEBUG "PDI-3 pre 0xbfb0092c&=~4\n"); mdelay(4);
	regWrite32(0xbfb0092c, regRead32(0xbfb0092c) & ~0x4u);
	printk(KERN_DEBUG "PDI-4 pre 0xbfb00860&=~0x400\n"); mdelay(4);
	regWrite32(0xbfb00860, regRead32(0xbfb00860) & ~0x400u);   /* PORT FIX: TOP_LED1_MODE bit10 (was ~0x4) */
	printk(KERN_DEBUG "PDI-5 prologue done OK\n"); mdelay(4);

#if defined(CONFIG_USE_A60928)
    if (!exp_skip_pll) {   /* EXP: stock does NOT do this A60928 PLL/SW reset here (Finding 5) */
	//PLL & software reset
	read_data = IO_GPHYREG(PHY_CSR_PHYSET3);
	read_data = read_data |(PHY_PLL_RST);
	IO_SPHYREG(PHY_CSR_PHYSET3, read_data);
	mdelay(1);
	read_data = IO_GPHYREG(PHY_CSR_PHYSET3);
	read_data = read_data | (PHY_Software_Reset);
	IO_SPHYREG(PHY_CSR_PHYSET3, read_data);
	mdelay(1);

	// Release PLL & software reset
	read_data = IO_GPHYREG(PHY_CSR_PHYSET3);
	read_data = read_data & ~(PHY_PLL_RST);
	IO_SPHYREG(PHY_CSR_PHYSET3, read_data);
	mdelay(1);
	read_data = IO_GPHYREG(PHY_CSR_PHYSET3);
	read_data = read_data & ~(PHY_Software_Reset);
	IO_SPHYREG(PHY_CSR_PHYSET3, read_data);
	mdelay(1);
    }
	printk(KERN_DEBUG "PDI: A60928 PLL/SW reset %s\n", exp_skip_pll ? "SKIPPED" : "done");
#endif

	/* PORT FIX (stock/GPL diff): the GPIO->XPON datapath mux lived in the MT7520-only
	 * block below, which the A60928-only build compiles OUT -> the digital PHY is never
	 * muxed onto the PON RX datapath -> never frame-syncs. Run it here (after the A60928
	 * PLL/SW reset, per stock ordering). IOMUX_Control_1 (0xbfa20104):
	 * RG_GPIO_PON_MODE(1<<15) | RG_PON_I2C_MODE(1<<0). */
	read_data = IO_GREG(IOMUX_Control_1_register);
	read_data = read_data | (RG_GPIO_PON_MODE) | (RG_PON_I2C_MODE);
	IO_SREG(IOMUX_Control_1_register, read_data);
	mdelay(1);

	/* PORT FIX (Agent A finding D + MMIO/I2C): the rest of the MT7520 front-end block
	 * (sigdet/PBUS/power), done AFTER the A60928 PLL/SW reset (which else clobbers it), and
	 * the sigdet via the I2C PHY-CSR path (IO_SPHYREG), not raw MMIO. Enabling signal-detect
	 * (clearing squelch) is the likely fix for the intermittent (flickering) sync. */
    if (!exp_skip_sigdet) {   /* EXP: stock does NOT do this sigdet/PBUS block here (Finding 5) */
	read_data = IO_GPHYREG(PHY_CSR_PHYSET3);
	read_data = read_data & ~(PHY_SQUELCH_DEGLITCH);   /* enable sigdet (I2C PHY-CSR) */
	IO_SPHYREG(PHY_CSR_PHYSET3, read_data);
	regWrite32(0xbfb0092c, regRead32(0xbfb0092c) & ~0x4u);      /* PBUS_CTRL: allow reg writes */
	regWrite32(0xbfb00860, regRead32(0xbfb00860) & ~0x400u);    /* GPIO_SHARE: transceiver power */
	mdelay(1);
    }
	printk(KERN_DEBUG "PDI: sigdet/PBUS block %s\n", exp_skip_sigdet ? "SKIPPED" : "done");

#ifdef CONFIG_USE_MT7520_ASIC
#ifndef TCSUPPORT_CPU_EN7521
	/* switch GPIO to XPON mode*/
	read_data = IO_GREG(TOP_CSR_GPIO_SHARE);
	read_data = read_data | (GPIO_PHY_MODE);
	IO_SREG(TOP_CSR_GPIO_SHARE, read_data);
#endif /* CONFIG_USE_MT7520_ASIC */	
#ifdef TCSUPPORT_CPU_EN7521
		/* switch GPIO to XPON mode*/
		read_data = IO_GREG(IOMUX_Control_1_register);
		read_data = read_data | (RG_GPIO_PON_MODE) | (RG_PON_I2C_MODE) ;
		IO_SREG(IOMUX_Control_1_register, read_data);
#endif 		

	/*sigdet*/
	read_data = IO_GPHYREG(PHY_CSR_PHYSET3);
	read_data = read_data & ~(PHY_SQUELCH_DEGLITCH);
	IO_SPHYREG(PHY_CSR_PHYSET3, read_data);
	/*after sw reset, register can be modify by PBUS*/
	read_data = IO_GREG(TOP_CSR_PBUS_CTRL);
	read_data = read_data & ~(PHY_PBUS_CTRL_DISABLE);
	IO_SREG(TOP_CSR_PBUS_CTRL, read_data);
	/* transceiver power initial setting*/
	read_data = IO_GREG(TOP_CSR_GPIO_SHARE);
	read_data = read_data & ~(TOP_LED1_MODE);
	IO_SREG(TOP_CSR_GPIO_SHARE, read_data);
#endif /* CONFIG_USE_MT7520_ASIC */



#ifdef CONFIG_USE_A60901
	IO_SPHYREG(PHY_CSR_PHYSET3, 0xCD810110) ;
	mdelay(1);
	IO_SPHYREG(PHY_CSR_PHYSET3, 0x45810110) ;
	mdelay(1);
	IO_SPHYREG(PHY_CSR_ANAPLLREG3, 0x00086100) ;
	mdelay(1);
	IO_SPHYREG(PHY_CSR_PHYSET3, 0x4581E110) ;
	mdelay(1);
	IO_SPHYREG(PHY_CSR_TDCSET1, 0x400000C8) ;
	mdelay(1);
	IO_SPHYREG(PHY_CSR_ANAPLLREG4, 0x00000013) ;
	//mdelay(1);
	//IO_SPHYREG(PHY_CSR_ANASET10, 0x211A0202) ;
#endif /* CONFIG_USE_A60901 */

#if !defined CONFIG_USE_MT7520_ASIC && !defined CONFIG_USE_A60901 && !defined CONFIG_USE_A60928
			/* Reset the SerDes Tx FIFO */
			IO_SPHYREG(PHY_CSR_GIO2_SETTING, 0x1) ;
			IO_SPHYREG(PHY_CSR_GIO2_SETTING, 0x0) ;
#endif /* !defined CONFIG_USE_MT7520_ASIC && !defined CONFIG_USE_A60901 */


	/* PORT FIX: stock always includes PHY_ILLG_INT; the A60928 #else path dropped it. */
	phy_int_config((PHY_ILLG_INT|PHYRDY_INT|PHY_LOF_INT|TRANS_LOS_INT)) ;

	/* Enable BIP error counter */
	phy_cnt_enable(PHY_ENABLE, PHY_ENABLE, PHY_ENABLE) ;
	
	/* change the guard time pattern */
    gpPhyPriv->phyGuardBitDelm = 0xAAAB5983;
	IO_SPHYREG(PHY_CSR_GPON_DELIMITER_GUARD, 0xAAAB5983) ;

	/* clear phy register*/
	phy_reset_counter( );

	/* PORT FIX (stock disasm): TDC/CDR setting the stock phy_dev_init writes (=45) that
	 * the port omitted -- CDR-related, may be needed for downstream lock. */
	IO_SPHYREG(PHY_CSR_TDCSET2, 45) ;

	/* PORT FIX (stock disasm 0x21e64): Ana_Power_Saving(PHY_ENABLE) tail of phy_dev_init.
	 * RMW ANAPWD |= 0x180 (bits7,8) and ANATXREG1 [19:16] = TX_AMP_LEVEL.
	 * ★★★ ROOT-CAUSE FIX (2026-07-04): the original port transcribed only the CLEAR half of the DZS
	 * RMW (& 0xFFF0FFFF) and DROPPED the "| 0x00090000" -> TX drive amplitude [19:16] = 0 = ZERO SerDes
	 * swing -> the laser modulator sees a flat line -> it emits CW DC bias (TxMon ~14150) with NO data
	 * eye -> the OLT sees unmodulated light and decodes nothing (stuck O3, no Assign_ONU_ID; and it's
	 * why the Imod/eye calibration had no effect — Imod only SCALES a swing that was 0). DZS
	 * Ana_Power_Saving @0x24ff0 writes [19:16]=0x9 (lui v0,0x9 @0x25184). Restore it. */
	read_data = IO_GPHYREG(PHY_CSR_ANAPWD);
	read_data = (read_data & ~0x180u) | ((uint)po_anapwd & 0x180u);   /* po_anapwd bits[8:7]: 0x180=port, 0=stock mode0 */
	IO_SPHYREG(PHY_CSR_ANAPWD, read_data);
	read_data = IO_GPHYREG(PHY_CSR_ANATXREG1);
	read_data = (read_data & 0xFFF0FFFFu) | 0x00090000u;   /* [19:16] = TX_AMP level 9 (was cleared to 0 = no modulation) */
	IO_SPHYREG(PHY_CSR_ANATXREG1, read_data);

	/* PORT FIX (stock disasm 0x1f38): phy_rx_imp_check() — RX-impedance cal fallback.
	 * Stock gates on chip 0x0008; run unconditionally here (my chip=0x000b). Logs the
	 * cal code; only overrides to mid-scale (16) if auto-cal railed (code 0 or 31). */
	data = (IO_GPHYREG(PHY_CSR_ANACAL1) & 0x1F00u) >> 8;
	printk(KERN_DEBUG "phy_rx_imp_check: ANACAL1 rx-imp code=%u\n", data);
	if (data == 0 || data == 31) {
		read_data = IO_GPHYREG(PHY_CSR_PHYSET4);
		read_data = read_data & ~0x2u;                 /* manual override */
		IO_SPHYREG(PHY_CSR_PHYSET4, read_data);
		read_data = IO_GPHYREG(PHY_CSR_PHYSET6);
		read_data = (read_data & ~0x1F00u) | 0x1000u;  /* imp code [12:8]=16 */
		IO_SPHYREG(PHY_CSR_PHYSET6, read_data);
	}

	PON_PHY_MSG(PHY_MSG_TRACE, "xPON PHY initialization done\n") ;

	return 0 ;
}

/*****************************************************************************
//function :
//		phy_los_handler
//description : 
//		this function is used to process phy los interrupt
//input :	
//		N/A
//output :
//		N/A
******************************************************************************/
void phy_los_handler(void)
{
	uint phyIntStatus ;

	/* Get interrtup status and clear */
	phyIntStatus = IO_GPHYREG(PHY_CSR_XPON_INT_STA) ;
	//printk("enter pon phy isr, status=0x%x\n", phyIntStatus);		//YMC_20150206
	IO_SPHYREG(PHY_CSR_XPON_INT_STA_CLR, phyIntStatus) ;


		if( mt7570_select == 1 )							// by HC 20150512
		{			
			TEC_switch = 0;		
			BiasTracking_switch = 0;					// by HC 20150921
		}	
	
#if !defined CONFIG_USE_MT7520_ASIC && !defined CONFIG_USE_A60901
		/* Reset the SerDes Tx FIFO */
		IO_SPHYREG(PHY_CSR_GIO2_SETTING, 0x1) ;
		IO_SPHYREG(PHY_CSR_GIO2_SETTING, 0x0) ;
#endif /* !defined CONFIG_USE_MT7520_ASIC && !defined CONFIG_USE_A60901 */

		/* Clear PHY bit delay */
		phy_bit_delay(0) ;
		
		/* reset phy */
		phy_reset_counter() ;

	PON_PHY_MSG(PHY_MSG_INT, "phy_los_handler doing phy_trans_los \n");
}

void phy_ready_handler(void)
{
    if( mt7570_select == 1 )			// by HC 20150512					
    {						
    	TEC_switch = 1;
		BiasTracking_switch = 1;	// by HC 20150921
		} 
}

/* Sadlly, from what we've learned from our experience, this func cannot alaways 
   get the right result. We really should try to find out why.
*/
static inline int is_ready_int_authentic(uint int_status)
{
#ifdef TCSUPPORT_CT_PON
    return ((!(int_status & TRANS_LOS_INT)) || (!(int_status & PHY_ILLG_INT))) ;
#else
    return (!(int_status & (TRANS_LOS_INT | PHY_ILLG_INT))) ;
#endif
}

static inline void handle_hw_irq_event(PHY_Event_Type_t id)
{
    PON_PHY_Event_data_t phy_event = {.id = 0, .src = PON_PHY_EVENT_SOURCE_HW_IRQ };
    phy_event.id = id;
    phy_event_handler(&phy_event);

}
/*****************************************************************************
//function :
//		phy_isr
//description : 
//		this function is used to process phy interrupt
//input :	
//		N/A
//output :
//		N/A
******************************************************************************/
void phy_isr(void)
{
	uint phyIntStatus ;
	PHY_TransConfig_T trans_status;
	PPHY_TransConfig_T p_trans_status = & trans_status;
    PON_PHY_Event_data_t phy_event = {.id = 0, .src = PON_PHY_EVENT_SOURCE_HW_IRQ };
	
	/* Get interrtup status and clear */
	phyIntStatus = IO_GPHYREG(PHY_CSR_XPON_INT_STA) ;
	//printk("enter pon phy isr, status=0x%x\n", phyIntStatus);		//YMC_20150206
	IO_SPHYREG(PHY_CSR_XPON_INT_STA_CLR, phyIntStatus) ;

    spin_lock(&gpPhyPriv->event_handle_lock);
	
	if((phyIntStatus & TRANS_LOS_INT) || (phyIntStatus & PHY_ILLG_INT)) {
        PON_PHY_MSG(PHY_MSG_INT, "PHY IRQ: TRSNS_LOS(%d)|PHY_ILLG(%d) interrupt.\n", 
			((phyIntStatus & TRANS_LOS_INT)? 1 : 0), ((phyIntStatus & PHY_ILLG_INT)? 1 : 0 ));
        gpPhyPriv->phy_status = PHY_LINK_STATUS_LOS    ;
        
        if ((phyIntStatus & TRANS_LOS_INT ) && (phyIntStatus & PHY_ILLG_INT ) ) {
            handle_hw_irq_event(PHY_EVENT_TRANS_LOS_ILLG_INT) ;
        }else if (phyIntStatus & TRANS_LOS_INT) {
        handle_hw_irq_event(PHY_EVENT_TRANS_LOS_INT) ;
        }else if (phyIntStatus & PHY_ILLG_INT){
        	if(PHY_GPON_CONFIG != gpPhyPriv->phyCfg.flags.mode || !gpPhyPriv->phyCfg.flags.rogue)
                handle_hw_irq_event(PHY_EVENT_PHY_ILLG_INT) ;
	}
		} 
	
	if(phyIntStatus & PHY_LOF_INT) {
		PON_PHY_MSG(PHY_MSG_INT, "PHY IRQ: transceiver LOF interrupt.\n") ;
        handle_hw_irq_event(PHY_EVENT_PHY_LOF_INT);
	}
    
	if(phyIntStatus & TRANS_TF_INT) {
		phy_trans_tx_getting(p_trans_status);
		if((p_trans_status->trans_tx_fault_inv_status == PHY_DISABLE) ) {
			PON_PHY_MSG(PHY_MSG_INT, "turn off power.\n") ;
            handle_hw_irq_event(PHY_EVENT_TF_INT);
		}
		PON_PHY_MSG(PHY_MSG_INT, "PHY IRQ: transceiver TF interrupt.\n") ;
	}
    
	if(phyIntStatus & TRANS_INT) {
		PON_PHY_MSG(PHY_MSG_INT, "PHY IRQ: transceiver interrupt.\n") ;
        handle_hw_irq_event(PHY_EVENT_TRANS_INT);
	}
    
	if(phyIntStatus & TRANS_SD_FAIL_INT) {
		extern unsigned int txsd_fail_cnt; txsd_fail_cnt++;   /* ★ count TX_SD-fail IRQs (readable via /sys) */
		PON_PHY_MSG(PHY_MSG_INT, "PHY IRQ: transceiver TX_SD fail interrupt.\n") ;
        handle_hw_irq_event(PHY_EVENT_TRANS_SD_FAIL_INT);
	}
    
	if(phyIntStatus & PHYRDY_INT) {
		/* SW workaround for phy ready interrupt when LOS happened */
        if(is_ready_int_authentic(phyIntStatus))
                {
            gpPhyPriv->phy_status = PHY_LINK_STATUS_READY;
            handle_hw_irq_event(PHY_EVENT_PHYRDY_INT);
            
            PON_PHY_MSG(PHY_MSG_INT, "PHY IRQ: phy ready interrupt.\n") ;
        }else{
            PON_PHY_MSG(PHY_MSG_INT, "PHY IRQ: FALSE phy ready interrupt.\n");
		}
	}
	
	if(phyIntStatus & PHY_I2CM_INT) {
		PON_PHY_MSG(PHY_MSG_INT, "PHY IRQ: I2C master interrupt.\n") ;
        handle_hw_irq_event(PHY_EVENT_I2CM_INT);       
	}

	/* the following function is used to avoid frequently interrupt */
	if(gpPhyPriv->phyCfg.intCounter == 0) {
		gpPhyPriv->phyCfg.intTime = jiffies ;
	}
	gpPhyPriv->phyCfg.intCounter++ ;
	if(gpPhyPriv->phyCfg.intCounter == 10) {
		gpPhyPriv->phyCfg.intCounter = 0 ;

		if((jiffies - gpPhyPriv->phyCfg.intTime) < 50) { //phy interrupt occur every 5 system interrupt
			#if 0
			phy_int_config(0) ;
			phy_reset_counter() ;
			
			PHY_START_TIMER(gpPhyPriv->int_timer) ;
			#endif
			PON_PHY_MSG(PHY_MSG_ERR, "PHY detect interrupt error.\n") ;
		}
	}

    spin_unlock(&gpPhyPriv->event_handle_lock);

}

/******************************************************************************
//function :
//		phy_register_cb_handler
//description : 
//		this function is used to register MAC function
//input :	
//		type		: used to select function
//		*pCbFun 	: MAC function
//output :
//		0 	: success
//		-1	: no such type
******************************************************************************/
int phy_register_cb_handler(PHY_CbType_t type, void *pCbFun)
{
	if(!pCbFun) {
		return -1 ;	
	}
	
	switch(type) {
#ifdef TCSUPPORT_XPON_HAL_API_EXT		
		case PHY_CALLBACK_XPON_STATE_NOTIFY:
			gpPhyPriv->phyXponStateNotifyHandler = pCbFun ;
			break ;
#endif /* TCSUPPORT_XPON_HAL_API_EXT */	
		default:
			return -1 ;	
	}
	return 0 ;
}

/******************************************************************************
******************************************************************************/
int phy_unregister_cb_handler(PHY_CbType_t type)
{
	switch(type) {
#ifdef TCSUPPORT_XPON_HAL_API_EXT		
		case PHY_CALLBACK_XPON_STATE_NOTIFY:
			gpPhyPriv->phyXponStateNotifyHandler = NULL ;
			break ;
#endif /* TCSUPPORT_XPON_HAL_API_EXT */
		default:
			return -EFAULT ;	
	}
	return 0 ;
}

/*****************************************************************************
******************************************************************************/
int xpon_phy_start(void)
{
    int ret;
    gpPhyPriv->is_phy_start = TRUE ;

	IO_SPHYREG(PHY_CSR_XPON_INT_STA_CLR, 0xFF) ;
    
#ifdef TCSUPPORT_CPU_EN7521
	ret = QDMA_API_REGISTER_HOOKFUNC(ECNT_QDMA_WAN, QDMA_CALLBACK_XPON_PHY_HANDLER, phy_isr);
#else
	ret = qdma_register_callback_function(QDMA_CALLBACK_XPON_PHY_HANDLER, phy_isr) ; //register phy_isr from QDMA
#endif
	if(ret != 0) {
		PON_PHY_MSG(PHY_MSG_ERR, "Register GPON ISR handler function failed.\n") ;
		return ret ;
	}
	
    /* TODO port: PHY_START_TIMER macro used struct timer_list.data (removed) */
    mod_timer(&gpPhyPriv->event_poll_timer, jiffies + msecs_to_jiffies(PHY_EVENT_POLL_MS));
	return 0 ;
}

int xpon_phy_stop(void)
{
    gpPhyPriv->is_phy_start = FALSE ;
#ifdef TCSUPPORT_CPU_EN7521
	QDMA_API_UNREGISTER_HOOKFUNC(ECNT_QDMA_WAN, QDMA_CALLBACK_XPON_PHY_HANDLER);
#else
	qdma_unregister_callback_function(QDMA_CALLBACK_XPON_PHY_HANDLER) ;	
#endif

    phy_fw_ready(PHY_DISABLE);
    PHY_STOP_TIMER(gpPhyPriv->event_poll_timer);
    return 0;
}

/*****************************************************************************
******************************************************************************/

struct timer_list gphy_rx_test_timer;
extern unsigned int gphy_test_rx_ber;

void phy_rx_sens_timer(struct timer_list *t){
	uint data = 0;

	data = IO_GPHYREG(0xBFAF02C0);
	IO_SPHYREG(0xBFAF04A0, 0x0);
	IO_SPHYREG(0xBFAF04A4, 0x0);
	printk("Rx_Sensitivity_Test PRBS error counter: 0x%x\n", data);

	/* PORT 6.18: kernel has no FP.  Original:
	 *   ber  = data / 26718132554.957   (G248832)
	 *   out  = ber * 1e12               (G10_12)
	 * => out = data * 1e12 / 26718132554.957 = data * 37.42790...
	 * Integer fixed-point (multiplier x1e4) avoids u64 overflow: */
	data = (uint)div_u64((u64)data * 374279ULL, 10000ULL);
	gphy_test_rx_ber = data;

	printk("Rx_Sensitivity_Test BER: %u\n", data);

}

void phy_rx_sensitivity_test(void){
	uint data = 0;
	struct timer_list rx_timer;

	gphy_test_rx_ber = 0;

	gphy_rx_test_timer.expires = jiffies + 10*HZ;
	
	/* clear PRBS counter */
	IO_SPHYREG(0xBFAF04A0, 0x0);
	IO_SPHYREG(0xBFAF04A4, 0x0);

	/* Set to PRBS7 test */
	IO_SPHYREG(0xBFAF0124, 0xFF200000);
	IO_SPHYREG(0xBFAF02A0, 0x81FD53);
	IO_SPHYREG(0xBFAF02A4, 0xFFFFFF);
	IO_SPHYREG(0xBFAF04A0, 0x5);

	add_timer(&gphy_rx_test_timer);
	IO_SPHYREG(0xBFAF04A4, 0x1);

	data = IO_GPHYREG(0xBFAF02C4);
	printk("Rx_Sensitivity_Test start: 0x%x\n", data);
	
}

static inline int get_tx_log_flag(void)
{
    return gpPhyPriv->phyCfg.flags.txLongFlag;
}

static void pon_phy_get_api_dispatch(xpon_phy_api_data_t * api_data)
{
    api_data->ret = PHY_SUCCESS;
    
    switch(api_data->cmd_id)
    {
        case PON_GET_PHY_LOS_STATUS:
            api_data->ret = phy_los_status();
            break;
			
        case PON_GET_PHY_READY_STATUS:
            api_data->ret = phy_ready_status();
            break; 
			
        case PON_GET_PHY_IS_SYNC:
            api_data->ret = is_phy_sync(); // for this API, in_data is NULL
            break;

        case PON_GET_PHY_MODE:
            api_data->ret = get_phy_mode();
            break;
            
        case PON_GET_PHY_TX_LONG_FLAG:
            api_data->ret = get_tx_log_flag();
            break;

        case PON_GET_PHY_TRANS_STATUS:
    		if(1 == mt7570_select){
    			mt7570_param_status_real(&api_data->trasn_status->params);
                api_data->trasn_status->alarms = mt7570_alarm;
    		}else{
    			phy_trans_param_status_real(&api_data->trasn_status->params);
                api_data->trasn_status->alarms = phy_tx_alarm() | phy_rx_power_alarm(); // get transceiver alarm message, only for transceiver module.
    		}
            break;

        case PON_GET_PHY_GET_TX_POWER_EN_FLAG:
            api_data->ret = gpPhyPriv->phyCfg.flags.txPowerEnFlag;
            break;

        case PON_GET_PHY_BIP_COUNTER:
            api_data->ret = phy_bip_counter();
            break;

        case PON_GET_PHY_RX_FEC_COUNTER:
            phy_rx_fec_counter(api_data->rx_fec_cnt);
            break;

        case PON_GET_PHY_RX_FRAME_COUNTER:
            phy_rx_frame_counter(api_data->rx_frame_cnt);
            break;

        case PON_GET_PHY_RX_FEC_GETTING:
            api_data->ret = phy_rx_fec_getting();
            break;

        case PON_GET_PHY_RX_FEC_STATUS:
            api_data->ret = phy_rx_fec_status();
            break;

        case PON_GET_PHY_TX_FEC_STATUS:
            api_data->ret = phy_tx_fec_status();
            break;

        case PON_GET_PHY_TX_BURST_GETTING:
            api_data->ret = phy_tx_burst_getting();
            break;

        case PON_GET_PHY_TRANS_TX:
            phy_trans_tx_getting(api_data->tx_trans_cfg);
            break;

        case PON_GET_PHY_TRANS_RX_GETTING:
            api_data->ret = phy_trans_rx_getting();
            break;
#ifdef TCSUPPORT_CPU_EN7521
        case PON_GET_PHY_ROUND_TRIP_DELAY:
			api_data->ret=phy_round_trip_delay_sof();
			break;
#endif			
        default:
            dump_stack();
            printk("unknown command id!\n");
            api_data->ret = PHY_NO_API;
            break;
    }
}


static inline void resotre_preamble_and_guard_bit(void)
{   
     IO_SPHYREG(PHY_CSR_GPON_PREAMBLE, gpPhyPriv->phyPreamble);
     IO_SPHYREG(PHY_CSR_GPON_DELIMITER_GUARD, gpPhyPriv->phyGuardBitDelm);
     IO_SPHYREG(PHY_CSR_GPON_EXTEND_PREAMBLE, gpPhyPriv->phyExtPreamble);
}


static void pon_phy_set_api_dispatch(xpon_phy_api_data_t * api_data)
{
    api_data->ret = PHY_SUCCESS;
    
    switch(api_data->cmd_id)
    {
        case PON_SET_PHY_MODE_CONFIG:
            api_data->ret = phy_mode_config(api_data->phy_mode_cfg->mode, api_data->phy_mode_cfg->txEnable);
            break;

        case PON_SET_PHY_RESET_COUNTER:
            api_data->ret = phy_reset_counter();
            break;

        case PON_SET_PHY_START:
            xpon_phy_start();
            break;

        case PON_SET_PHY_STOP:
            xpon_phy_stop();
            break;

        case PON_SET_PHY_FW_READY:
            api_data->ret = phy_fw_ready(*api_data->data);
            break;

        case PON_SET_PHY_COUNTER_CLEAR:
            phy_counter_clear(*api_data->data);
            break;

        case PON_SET_EPONMODE_PHY_RESET:
            api_data->ret = eponMode_phy_reset();
            break;

        case PON_SET_PHY_BIT_DELAY:
            api_data->ret = phy_bit_delay(*api_data->data);
            break;

        case PON_SET_PHY_TX_POWER_CONFIG:
            phy_tx_power_config(*api_data->data);
            break;

        case PON_SET_PHY_ROGUE_PRBS_CONFIG:
            api_data->ret = phy_rogue_prbs_config(*api_data->data);
            break;

        case PON_SET_PHY_DEV_INIT:
            api_data->ret = phy_dev_init();
            break;

        case PON_SET_PHY_TRANS_POWER_SWITCH:
            api_data->ret = phy_trans_power_switch(*api_data->data);
            break;

        case PON_SET_PHY_RESTORE_PREAMBLE_AND_GUARD_BIT:
            resotre_preamble_and_guard_bit();
            break;

        case PON_SET_PHY_TRANS_MODEL_SETTING:
            phy_trans_model_setting();
            break;

        case PON_SET_PHY_RX_FEC_SETTING:
            api_data->ret = phy_rx_fec_setting(*api_data->data);
            break;

        case PON_SET_PHY_TX_BURST_CONFIG:
            api_data->ret = phy_tx_burst_config(*api_data->data);
            break;

        case PON_SET_PHY_GPON_DELIMITER_GUARD:
            api_data->ret = phy_gpon_delimiter_guard(api_data->delimiter_guard->delimiter, 
                                                     api_data->delimiter_guard->guard_time);
            break;

        case PON_SET_PHY_TRANS_TX_SETTINGS:
            api_data->ret = phy_trans_tx_setting(api_data->tx_trans_cfg);
            break;

        case PON_SET_PHY_TRANS_RX_SETTING:
            api_data->ret = phy_trans_rx_setting(*api_data->data);
            break;

        case PON_SET_PHY_GPON_PREAMBLE:
            api_data->ret = phy_gpon_preamble(api_data->gpon_preamble);
            break;

        case PON_SET_PHY_GPON_EXTEND_PREAMBLE:
            api_data->ret = phy_gpon_extend_preamble(api_data->gpon_ex_preamble);
            break;
#ifdef TCSUPPORT_CPU_EN7521
	case PON_SET_PHY_EPON_TS_CONTINUE_MODE:
            api_data->ret = phy_set_epon_ts_continue_mode(*api_data->data);
            break;
#endif       
#if (defined(TCSUPPORT_WAN_GPON) || defined(TCSUPPORT_WAN_EPON))&& defined(TCSUPPORT_XPON_HAL_API_EXT)
        case PON_SET_PHY_NOTIFY_EVENT:
            if(gpPhyPriv->phyXponStateNotifyHandler){
                gpPhyPriv->phyXponStateNotifyHandler(*api_data->data);
            }
            break;
#endif
        default:
            printk("unknown command id!\n");
            api_data->ret = PHY_NO_API;
            break;
    }
}


int pon_phy_api_dispatch(struct ecnt_data *in_data)
{
    xpon_phy_api_data_t * api_data = (xpon_phy_api_data_t *)in_data;
    switch(api_data->api_type) 
    {
        case XPON_PHY_API_TYPE_GET:
            pon_phy_get_api_dispatch(api_data);
            break;

        case XPON_PHY_API_TYPE_SET:
            pon_phy_set_api_dispatch(api_data);
            break;

        default:
            dump_stack();
            printk("unknown api_data->api_type: %d\n", api_data->api_type);
            api_data->ret = PHY_NO_API;
            break;
    }
    
    return ECNT_RETURN;
}


struct ecnt_hook_ops phy_api_dispatch_hook_ops = {
    .name = "pon_phy_api_dispatch",
    .hookfn = pon_phy_api_dispatch,
    .is_execute = 1,
    .maintype = ECNT_XPON_PHY,
    .subtype = ECNT_XPON_PHY_API,
    .priority   = 1,
};

/*****************************************************************************
//function :
//		pon_phy_init
//description : 
//		this function is the initial setting for XPON phy
//input :	
//		N/A
//output :
//		0 	: initial success
//		-1	: initial fail
******************************************************************************/
int pon_phy_init(void)
{
	int ret = 0 ;
	uint read_data = 0;
	uint write_data = 0;
	unchar ptr[4];
	//read_data = IO_GPHYREG(PHY_CSR_PHYSET4);
	//write_data = read_data & (0xfffffdfc) ;
	//IO_SPHYREG(PHY_CSR_PHYSET4,write_data);

	if (NULL == gpPhyPriv)
	{
		gpPhyPriv = (PHY_GlbPriv_T *)kmalloc(sizeof(PHY_GlbPriv_T), GFP_KERNEL) ;
		if( NULL == gpPhyPriv) {
			printk("Alloc data struct memory failed\n") ;
			return ret ;
		}
	}
	
	/*Reset the xpon_phy*/
	xPON_Phy_Reset();
	#ifndef TCSUPPORT_CPU_EN7521
	fe_reset_pon_phy_reset_init = pon_phy_reset_init;
	#endif

	gpPhyPriv->i2c_u2_clk_div = I2C_U2_CLK_DIV ;
	#if !defined(CONFIG_USE_MT7520_ASIC) && !defined(CONFIG_USE_A60928) && !defined(CONFIG_USE_A60901)
	if((ret = i2c_init()) != 0) {
		PON_PHY_MSG(PHY_MSG_ERR, "PHY I2C sccb controller initialization failed.\n") ;
		return ret ;
	}
	#endif /* !defined CONFIG_USE_MT7520_ASIC */
	
    /* these two line must be put before registering isr handler */
    gpPhyPriv->is_phy_start = FALSE   ;     
    gpPhyPriv->phyCfg.flags.rogue = 0 ;
    gpPhyPriv->phyCfg.flags.mode  = PHY_UNKNOWN_CONFIG ;

    spin_lock_init(&gpPhyPriv->event_handle_lock);
   
    gpPhyPriv->phy_status = PHY_LINK_STATUS_UNKNOWN;

	timer_setup(&gpPhyPriv->event_poll_timer, phy_event_poll_timer, 0);

	if((ret = phy_debug_init()) != 0) {
		PON_PHY_MSG(PHY_MSG_ERR, "phy debug initialization failed.\n") ;
		return ret ;
	}

#if defined(TCSUPPORT_CT_PON)
	if ((ret = los_status_init()) != 0) {
		PON_PHY_MSG(PHY_MSG_ERR, "phy los status initialization failed.\n") ;
		return ret;
	}
#endif

	if((ret = phy_dev_init()) != 0) {
            PON_PHY_MSG(PHY_MSG_ERR, "phy device initialization failed.\n") ;
            return ret ;
        }
												
	if( mt7570_EN7570_detection() == 1 )		//by HC 20150302
	{		
		printk("EN7570 found!\n");		
		if( (ret = mt7570_init()) != 0 )		//Initialization failed 
		{
			PON_PHY_MSG(PHY_MSG_ERR, "7570 initialization failed.\n") ;
			return ret ;
		}
		gpio_tx_dis_reset();
		gpio_BOSA_Tx_power_on();				// by YMC 20150731
					
		gpPhyPriv->LDDLA_task_wait = kthread_run(LDDLA_task_wait, NULL, "mt7570_task_wait");
		/* ★ auto-lock keeper: runs the proven gpon_do_lock() sequence whenever the CDR isn't locked,
		 * so the GPON RX locks automatically on real bring-up (no manual 'echo R' needed). */
		gpon_keeper_task = kthread_run(gpon_lock_keeper, NULL, "gpon_lock_keeper");
	}
	else
		printk("EN7570 not found!\n");


	phy_trans_model_setting(); 				// set transceiver config 

#ifdef CONFIG_USE_MT7520_ASIC
	/*enable FW ready*/
	//phy_fw_ready(PHY_ENABLE) ;
#endif /* CONFIG_USE_MT7520_ASIC */
	/*************************************************************************************/

	printk("PON PHY driver version is %d.%d.%d\n",phy_date_version,phy_version,mt7570_version);
	
	/*************************************************************************************/
	timer_setup(&gphy_rx_test_timer, phy_rx_sens_timer, 0);

	timer_setup(&gpPhyPriv->int_timer, phy_int_recover_expires, 0);

	/* callback function initial */
#ifdef TCSUPPORT_XPON_HAL_API_EXT
	gpPhyPriv->phyXponStateNotifyHandler = NULL ;
#endif /* TCSUPPORT_XPON_HAL_API_EXT */


	memset(&gpPhyPriv->phyCfg, 0, sizeof(PHY_Config_T)) ;
	gpPhyPriv->phyCfg.t1 = 200 ;
	gpPhyPriv->phyCfg.t2 = 30 ;
	gpPhyPriv->phyCfg.intCounter = 0 ;
	gpPhyPriv->phyCfg.intTime = 0 ;
	
	gpPhyPriv->phyCfg.flags.txPowerEnFlag = TRUE;
	gpPhyPriv->phyCfg.flags.txLongFlag = FALSE;
	gpPhyPriv->debugLevel = 0x0001;


    if (ECNT_REGISTER_SUCCESS != ecnt_register_hook(&phy_api_dispatch_hook_ops) ){
        panic("Register hook function failed! %s:%d", __FUNCTION__, __LINE__);
    }

	return ret ;
}

/*****************************************************************************
******************************************************************************/
void en7571_servo_stop(void);   /* fwd: adaptive bias/mod servo kthread (defined below) */
void pon_phy_deinit(void)
{
	en7571_servo_stop();   /* stop the ported adaptive bias/mod servo before teardown (no oops on rmmod) */
	/* Stop the phy and int timer */
	//PHY_STOP_TIMER(gpPhyPriv->phy_timer) ; /* mark it ,cause phy-timer didnt init */
	PHY_STOP_TIMER(gpPhyPriv->int_timer) ;
	#ifndef TCSUPPORT_CPU_EN7521
	fe_reset_pon_phy_reset_init = NULL;
	#endif
	
	if(!IS_ERR(gpPhyPriv->LDDLA_task_wait)){
		kthread_stop(gpPhyPriv->LDDLA_task_wait);
	}
	if (gpon_keeper_task && !IS_ERR(gpon_keeper_task)) {
		kthread_stop(gpon_keeper_task);
		gpon_keeper_task = NULL;
	}

#ifdef TCSUPPORT_CPU_EN7521
	QDMA_API_UNREGISTER_HOOKFUNC(ECNT_QDMA_WAN, QDMA_CALLBACK_XPON_PHY_HANDLER);
#else
	qdma_unregister_callback_function(QDMA_CALLBACK_XPON_PHY_HANDLER) ;	
#endif
	/* transceiver power off*/
	phy_trans_power_switch(PHY_DISABLE);

	#if !defined(CONFIG_USE_MT7520_ASIC) && !defined(CONFIG_USE_A60928) && !defined(CONFIG_USE_A60901)
	i2c_exit() ;
	#endif /* !defined CONFIG_USE_MT7520_ASIC */

	phy_debug_deinit();
	
	if(mt7570_select == 1)
	{
    flush_workqueue(i2c_access_queue);
    destroy_workqueue(i2c_access_queue);
	}
	
	if(gpPhyPriv != NULL) {
		kfree(gpPhyPriv) ;
		gpPhyPriv = NULL ;
	}

    ecnt_unregister_hook(&phy_api_dispatch_hook_ops);
}

#ifndef TCSUPPORT_CPU_EN7521
void pon_phy_reset_init(void)
{
#define TEST_MISC0  		(0xBFB00380)
#define XP_PHY_SW_RSTN 	(1<<25)

	uint csrXponSetting=0, rdata=0, wdata=0;

	//csrXponSetting = IO_GPHYREG(PHY_CSR_XPON_SETTING);

#ifdef CONFIG_USE_MT7520_ASIC
	phy_fw_ready(PHY_DISABLE); /*20150413*/
#endif /* CONFIG_USE_MT7520_ASIC */

	/* reset phy use SCU register */
	rdata = IO_GREG(TEST_MISC0);
	wdata = rdata & ~(XP_PHY_SW_RSTN);
	IO_SREG(TEST_MISC0, wdata);
	udelay(1);
	IO_SREG(TEST_MISC0, rdata);
	udelay(1);

	/* reinit phy settings after PHY RESET */
	phy_dev_init();

    phy_trans_model_setting();
	
	//IO_SPHYREG(PHY_CSR_XPON_SETTING, csrXponSetting);

#ifdef CONFIG_USE_MT7520_ASIC
	/*enable FW ready*/
	//phy_fw_ready(PHY_ENABLE) ;
#endif /* CONFIG_USE_MT7520_ASIC */

    if(	PHY_UNKNOWN_CONFIG != gpPhyPriv->phyCfg.flags.mode ) {
        phy_mode_config(gpPhyPriv->phyCfg.flags.mode, PHY_ENABLE);
    }else{
        printk("PON PHY mode unknown while reset! %s:%d\n", __FUNCTION__, __LINE__);
	}
}
#endif

/* PORT: the optical PHY is built INTO econet-xpon.ko (not a separate module),
 * so it must not provide its own init_module/cleanup_module/MODULE_LICENSE --
 * the single module entry point lives in xpondrv.c.  pon_phy_init() /
 * pon_phy_deinit() remain callable.
 * TODO (functional bring-up): call pon_phy_init()/pon_phy_deinit() from
 * xpondrv_init()/xpondrv_cleanup() once the ECNT hook dispatch (currently a
 * no-op stub) is replaced by direct PON_PHY calls. */
/* module_init(pon_phy_init) */
/* module_exit(pon_phy_deinit) */

EXPORT_SYMBOL(phy_register_cb_handler);
EXPORT_SYMBOL(phy_unregister_cb_handler);

/* ---- PHY-only optical recon (no fiber / no GPON-MAC / no laser) ------------
 * Resolve EN7570 (the in-source LUT_Ibias_Imod laser cal applies) vs EN7571 by
 * reading the BOB companion over the PHY-integrated I2C master. Only the xpon
 * PHY top reset is toggled and an I2C register is read; the laser is NEVER
 * enabled (gpio_BOSA_Tx_power_on is not called), so this is safe with or without
 * fiber and has no rogue-ONU exposure. Triggered on demand via
 *   echo 1 > /proc/econet_xpon_phy_recon ; cat /proc/econet_xpon_phy_recon
 * ("echo 2" also runs phy_dev_init). Kept OFF the module init path so loading
 * econet-xpon stays inert. */
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

extern int vEN7570;					/* set by mt7570_EN7570_detection() */
static int xpon_recon_result = -2;			/* -2=not run -1=allocfail 0/1=detect */
static int xpon_recon_devinit;				/* did this run also do phy_dev_init? */

int xpon_phy_recon(int do_devinit)
{
	if (!gpPhyPriv) {
		gpPhyPriv = (PHY_GlbPriv_T *)kzalloc(sizeof(PHY_GlbPriv_T), GFP_KERNEL);
		if (!gpPhyPriv)
			return (xpon_recon_result = -1);
	}
	gpPhyPriv->i2c_u2_clk_div = I2C_U2_CLK_DIV;
	xPON_Phy_Reset();				/* de-reset xpon phy top (CSR toggle) */
	udelay(100);
	if (do_devinit) {				/* optional digital PLL/reset bring-up */
		phy_dev_init();
		xpon_recon_devinit = 1;
		udelay(100);
	}
	xpon_recon_result = mt7570_EN7570_detection();	/* I2C rd 0x70[0x0170]==0x03 ? */
	printk("econet-xpon: PHY recon -> EN7570 detect=%d vEN7570=0x%02x clkdiv=0x%x devinit=%d (%s)\n",
	       xpon_recon_result, (unsigned)vEN7570, (unsigned)I2C_U2_CLK_DIV, xpon_recon_devinit,
	       xpon_recon_result == 1 ? "EN7570 present: LUT_Ibias_Imod cal applies"
				      : "no EN7570 response (try devinit / EN7571?)");
	return xpon_recon_result;
}

static ssize_t xpon_recon_write(struct file *f, const char __user *b, size_t l, loff_t *o)
{
	char k[8] = { 0 };
	(void)f; (void)o;
	if (l && copy_from_user(k, b, min(l, sizeof(k) - 1)))
		return -EFAULT;
	xpon_phy_recon(k[0] == '2');			/* "2" => also run phy_dev_init */
	return l;
}
static ssize_t xpon_recon_read(struct file *f, char __user *b, size_t l, loff_t *o)
{
	char k[80];
	int n = scnprintf(k, sizeof(k),
			  "en7570_detect %d vEN7570 0x%02x clkdiv 0x%x devinit %d\n",
			  xpon_recon_result, (unsigned)vEN7570, (unsigned)I2C_U2_CLK_DIV,
			  xpon_recon_devinit);
	(void)f;
	return simple_read_from_buffer(b, l, o, k, n);
}
static const struct proc_ops xpon_recon_ops = {
	.proc_write = xpon_recon_write,
	.proc_read  = xpon_recon_read,
};

/* ---- Laser-OFF optical RX / LOS detection (option-1 fiber bring-up) ---------
 * Reads the GPON PHY signal-detect + RX-sync status over the PHY I2C only. No
 * laser (gpio_BOSA_Tx_power_on is never called), no GPON MAC, no upstream TX ->
 * safe on a live/shared PON. Bring-up (xPON_Phy_Reset + phy_dev_init, both
 * laser-safe on EN7528: MT7520/A609xx transceiver-power blocks are #ifdef'd out)
 * runs once on first access; thereafter `cat /proc/econet_xpon_los` is a live
 * poll for watching fiber plug-in:
 *   los 1 -> NO SIGNAL  (fiber unplugged / no OLT downstream light)
 *   los 0 -> SIGNAL     (OLT downstream light detected); rx_sync 0xa => RX locked
 * `echo 1 > /proc/econet_xpon_los` forces a fresh PHY RX bring-up. */
static int xpon_phy_rx_up;
static int xpon_sif_inited;
static void xpon_phy_rx_bringup(void)
{
	if (!xpon_sif_inited) {
		/* SIF/optical clock+reset enable. The STOCK phy.ko::phy_dev_init does
		 * these four writes at its start (disasm 0x224cc..0x225fc); the GPL
		 * phy_dev_init we compiled is MISSING them, which is why the SIF engine
		 * clock is gated (STAT never ready). 0xbfa2015c bit21|bit0 = the SIF/
		 * optical functional clock enable; the others are PHY/reset config. */
		printk("sifclk: pre 0xbfa2015c=%08x 0xbfaf0108=%08x 0xbfb0092c=%08x 0xbfb00860=%08x\n",
		       regRead32(0xbfa2015c), regRead32(0xbfaf0108),
		       regRead32(0xbfb0092c), regRead32(0xbfb00860));
		regWrite32(0xbfa2015c, regRead32(0xbfa2015c) | 0x00200001); /* clock enable */
		regWrite32(0xbfaf0108, regRead32(0xbfaf0108) & ~0x00000004);
		regWrite32(0xbfb0092c, regRead32(0xbfb0092c) & ~0x00000004);
		regWrite32(0xbfb00860, regRead32(0xbfb00860) & ~0x00000400);
		udelay(100);
		printk("sifclk: post 0xbfa2015c=%08x\n", regRead32(0xbfa2015c));
		/* Bring up the EN7528 SIF I2C controller (0xbfbf8000) so the optical
		 * transceiver DDM can be read. Real driver in compat/sif_port.c. */
		extern void sif_port_init(void);
		sif_port_init();
		xpon_sif_inited = 1;
	}
	if (!gpPhyPriv) {
		gpPhyPriv = (PHY_GlbPriv_T *)kzalloc(sizeof(PHY_GlbPriv_T), GFP_KERNEL);
		if (!gpPhyPriv)
			return;
	}
	gpPhyPriv->i2c_u2_clk_div = I2C_U2_CLK_DIV;
	xPON_Phy_Reset();			/* de-reset xpon phy top (CSR toggle) */
	udelay(100);
	phy_dev_init();				/* MT7520 path: GPIO-XPON + sigdet + RX cfg; laser OFF */
	udelay(100);
	xpon_phy_rx_up = 1;
}

/* Read a 16-bit big-endian SFF-8472 DDM value from the optical transceiver
 * (device 0x51, 7-bit) over the now-real SIF I2C. -1 on SIF/transaction failure. */
static int xpon_ddm_rd16(unchar off)
{
	extern unsigned short SIF_X_Read(unsigned char, unsigned short, unsigned char,
					unsigned char, unsigned int, unsigned char *, unsigned short);
	unchar b[2] = { 0, 0 };
	if (SIF_X_Read(0, I2C_U2_CLK_DIV, 0x51, 1, off, b, 2) != 2)
		return -1;
	return ((int)b[0] << 8) | b[1];
}
/* ==========================================================================
 * EN7571 optical-transceiver RX bring-up (I2C dev 0x70, 16-bit reg addressing).
 * Decoded from phy.ko (en7571.c) -> pon_port_plan/scratchpad/en7571_rx_decode.md.
 * NOTE: SIF_X_Read/Write return the byte count on success (0 = fail); the e7_*
 * wrappers invert that to 0=OK / -1=err.
 * ========================================================================== */
#define EN7571_CLKDIV  I2C_U2_CLK_DIV   /* known-good SIF clkDiv (vendor: gpPhyPriv[0xAC]) */
extern unsigned short SIF_X_Read(unsigned char, unsigned short, unsigned char,
		unsigned char, unsigned int, unsigned char *, unsigned short);
extern unsigned short SIF_X_Write(unsigned char, unsigned short, unsigned char,
		unsigned char, unsigned int, unsigned char *, unsigned short);

static int e7_rd(u32 reg, u8 *buf, u16 len)
{ return (SIF_X_Read(0, EN7571_CLKDIV, 0x70, 2, reg, buf, len) == len) ? 0 : -1; }
static int e7_wr(u32 reg, u8 *buf, u16 len)
{ return (SIF_X_Write(0, EN7571_CLKDIV, 0x70, 2, reg, buf, len) == len) ? 0 : -1; }

/* ── en7571 (dev 0x70) analog-transceiver register dump ────────────────────────
 * The digital-PHY MMIO diff (dumptx) can NOT see this space; it is the optical
 * front-end's own config (laser bias/mod, burst envelope, DCL/loop, RX datapath).
 * This is the port-side capture to diff against stock's en7571_info @O5.
 * `dump70`         -> the key analog/TX/loop registers as 4-byte words
 * `dump70 <b> <n>` -> n words starting at reg b (4-byte stride)                */
void en7571_dump70(u32 base, u32 count)
{
	/* the register set stock's en7571_info touches + the TX/loop/burst crux */
	static const u16 keyregs[] = {
		0x00, 0x08, 0x0c, 0x14, 0x30, 0x38, 0x100, 0x11c, 0x120, 0x130,
		0x138, 0x148, 0x15c, 0x170, 0x208, 0x220, 0x224, 0x228, 0x22c,
		0x234, 0x236, 0x238, 0x23a, 0x23c, 0x240, 0x244, 0x248, 0x24c,
		0x250, 0x254, 0x258, 0x25c, 0x260, 0x280, 0x300,
	};
	u8 b[4]; u32 i;
	if (count) {                            /* explicit range */
		for (i = 0; i < count; i++) {
			u32 r = base + i * 4;
			if (e7_rd(r, b, 4))
				printk(KERN_DEBUG "e7[%03x] = <read-fail>\n", r);
			else
				printk(KERN_DEBUG "e7[%03x] = %02x %02x %02x %02x\n",
				       r, b[0], b[1], b[2], b[3]);
		}
		return;
	}
	for (i = 0; i < sizeof(keyregs) / sizeof(keyregs[0]); i++) {
		if (e7_rd(keyregs[i], b, 4))
			printk(KERN_DEBUG "e7[%03x] = <read-fail>\n", keyregs[i]);
		else
			printk(KERN_DEBUG "e7[%03x] = %02x %02x %02x %02x\n",
			       keyregs[i], b[0], b[1], b[2], b[3]);
	}
}

/* Read the optical module's SFF-8472 A0 identity page (SIF dev 0x50, 1-byte word addr):
 * vendor name (off 20), OUI (37), part# (40), rev (56), serial (68), date (84). ASCII fields. */
void en7571_read_sff(void)
{
	extern unsigned short SIF_X_Read(unsigned char, unsigned short, unsigned char,
			unsigned char, unsigned int, unsigned char *, unsigned short);
	u8 b[20]; int i;
	struct { const char *nm; u8 off, len; } f[] = {
		{"vendor", 20, 16}, {"oui", 37, 3}, {"partnum", 40, 16},
		{"rev", 56, 4}, {"serial", 68, 16}, {"date", 84, 8},
	};
	u8 id = 0;
	if (SIF_X_Read(0, EN7571_CLKDIV, 0x50, 1, 0, &id, 1) != 1) {
		printk(KERN_DEBUG "sff: dev 0x50 A0 EEPROM not responding (integrated BOSA / no id EEPROM?)\n");
		return;
	}
	printk(KERN_DEBUG "sff: A0[0]=0x%02x (SFF-8472 identifier)\n", id);
	for (i = 0; i < (int)(sizeof(f)/sizeof(f[0])); i++) {
		int n = SIF_X_Read(0, EN7571_CLKDIV, 0x50, 1, f[i].off, b, f[i].len);
		int k; char asc[20]; unsigned oui = 0;
		if (n != f[i].len) { printk(KERN_DEBUG "sff: %-8s <read-fail>\n", f[i].nm); continue; }
		for (k = 0; k < f[i].len && k < 19; k++) asc[k] = (b[k] >= 0x20 && b[k] < 0x7f) ? b[k] : '.';
		asc[k] = 0;
		if (!strcmp(f[i].nm, "oui")) { oui = (b[0]<<16)|(b[1]<<8)|b[2];
			printk(KERN_DEBUG "sff: %-8s = %06x\n", f[i].nm, oui); }
		else printk(KERN_DEBUG "sff: %-8s = '%s'\n", f[i].nm, asc);
	}
}

/* en7571 (dev 0x70) register WRITE for RE experiments.
 *   raw==0 : 12-bit DAC write — byte0=val&0xff, byte1[3:0]=val[11:8], preserve rest
 *            (matches en7571_set_ibias/set_imod; use for 0x138 Ibias / 0x148 Imod)
 *   raw==1 : raw 4-byte little-endian write of val (use for control regs e.g. 0x228) */
void en7571_poke70(u32 reg, u32 val, int raw)
{
	u8 b[4];
	if (e7_rd(reg, b, 4)) { printk(KERN_DEBUG "poke70[%03x] read-fail\n", reg); return; }
	if (raw) {
		b[0] = val & 0xff; b[1] = (val >> 8) & 0xff;
		b[2] = (val >> 16) & 0xff; b[3] = (val >> 24) & 0xff;
	} else {
		b[0] = (u8)(val & 0xff);
		b[1] = (u8)((b[1] & 0xF0) | ((val >> 8) & 0x0F));
	}
	e7_wr(reg, b, 4);
	e7_rd(reg, b, 4);
	printk(KERN_DEBUG "poke70[%03x] <= %x raw=%d rb=%02x %02x %02x %02x\n",
	       reg, val, raw, b[0], b[1], b[2], b[3]);
}

/* en7571_openloop_mode: stock en7571_OpenLoopMode(@0x41588). RMW SIF-0x70 0x13C/0x14C byte0 bits[1:0] to
 * 0b10 (on, ERC "DAC-follow": couples 0x138/0x148 DAC -> laser; 0x13C[2:3] then tracks applied bias) or 0b00
 * (off). 4-byte read-modify-write touching ONLY byte0 -> the 0x13C[2:3] ADC bytes are preserved from the read
 * (unlike poke70r which overwrites them). Byte0 default here reads 0x30 -> becomes 0x32 for on. */
void en7571_openloop_mode(int on)
{
	u8 c[4]; u8 v = on ? 0x02 : 0x00;
	if (!e7_rd(0x13C, c, 4)) { c[0] = (u8)((c[0] & 0xFC) | v); e7_wr(0x13C, c, 4); }
	if (!e7_rd(0x14C, c, 4)) { c[0] = (u8)((c[0] & 0xFC) | v); e7_wr(0x14C, c, 4); }
	e7_rd(0x13C, c, 4);
	printk(KERN_DEBUG "openloop_mode(%d): 0x13C[1:0]<-%s rb=%02x %02x %02x %02x\n",
	       on, on ? "0b10" : "0b00", c[0], c[1], c[2], c[3]);
}

/* phy_tx_tdc_holdover: ★2026-07-08 disasm port of stock phy_tx_tdc_holdover @0xedcc (called from
 * phy_event_poll on an a_tdc_holdover_time window). HOLDS the TX timing-reference (TDC output) steady so the
 * upstream burst edges don't drift/jitter vs the OLT ranging window. on=1: 0xbfaf01f0|=0x20, 0xbfaf0104|=0x10;
 * on=0: clear both. The PORT never implemented this at all -> suspected cause of the register-perfect-but-
 * un-rangeable burst (burst-edge jitter). Test live via `echo t` (on) / `echo t0` (off) during O4 ranging. */
void phy_tx_tdc_holdover(int on)
{
	u32 v;
	v = IO_GREG(0xbfaf01f0u); IO_SREG(0xbfaf01f0u, on ? (v | 0x20u) : (v & ~0x20u));
	v = IO_GREG(0xbfaf0104u); IO_SREG(0xbfaf0104u, on ? (v | 0x10u) : (v & ~0x10u));
	printk(KERN_DEBUG "tdc_holdover(%d): 01f0=%08x 0104=%08x\n", on,
	       IO_GREG(0xbfaf01f0u), IO_GREG(0xbfaf0104u));
}

/* PWRADC dark-offset, measured by en7571_PWRADC_calibration, subtracted by PWRADC_get. */
static u32 en7571_PWRADC_offset;

/* set once en7571_optical_bringup() has powered the front-end + biased the APD;
 * cleared by en7571_apd_off() so the next optical read re-brings-up. */
static int en7571_optical_up;

/* en7571_PWRADC_calibration (phy.ko @0x316b0): sample the PWRADC dark offset with the
 * mux to internal ref (0x3B bit4) + channel enabled, 20ms settle, store 16-bit offset. */
static void en7571_PWRADC_calibration(void)
{
	u8 b[4]; int i; u32 raw;
	if (!e7_rd(0x38,  b, 4)) { b[3] |= 0x10; e7_wr(0x38,  b, 4); } /* 0x3B.4 mux->intref */
	if (!e7_rd(0x208, b, 4)) { b[3] |= 0x07; e7_wr(0x208, b, 4); } /* 0x20B[2:0] ch en    */
	if (!e7_rd(0x208, b, 4)) { b[3] |= 0x80; e7_wr(0x208, b, 4); } /* 0x20B.7 START       */
	for (i = 0; i < 20; i++) udelay(1000);                         /* 20ms settle         */
	if (!e7_rd(0x208, b, 4) && ((b[2] & 0x10) >> 4) == 1) {        /* 0x20A.4 done        */
		e7_rd(0x208, b, 4);
		raw = (((u32)b[2] << 16) | ((u32)b[1] << 8) | b[0]) & 0xFFFFF;
		en7571_PWRADC_offset = raw >> 4;
		printk(KERN_INFO "en7571: PWRADC offset = 0x%x\n", en7571_PWRADC_offset);
	}
	if (!e7_rd(0x208, b, 4)) { b[3] &= ~0x07; e7_wr(0x208, b, 4); } /* ch disable  */
	if (!e7_rd(0x38,  b, 4)) { b[3] &= ~0x10; e7_wr(0x38,  b, 4); } /* mux->PD     */
}

static int en7571_detect(void)
{
	u8 id = 0, rev = 0;
	if (e7_rd(0x170, &id, 1) || e7_rd(0x15C, &rev, 1))
		return 0;
	return (id == 3) && (rev >= 3);
}

/* One-time LOS comparator enable + default threshold DAC (LOS_init/_level_set). */
static void en7571_los_init(void)
{
	u8 b[4];
	if (!e7_rd(0x11C, b, 4)) { b[0] |= 0x01; b[1] |= 0x1F; e7_wr(0x11C, b, 4); }
	if (!e7_rd(0x024, b, 4)) { b[3] |= 0x04;               e7_wr(0x024, b, 4); }
	if (!e7_rd(0x024, b, 4)) { b[2] |= 0x40;               e7_wr(0x024, b, 4); }
	if (!e7_rd(0x120, b, 4)) { b[1] |= 0x1F;
				   b[0] = (b[0] & 0x80) | 0x05; e7_wr(0x120, b, 4); }
	if (!e7_rd(0x11C, b, 4)) {           /* thresholds 0x11E=0x30(LOS), 0x11F=0x20(SD) */
		b[2] = (b[2] & 0x80) | 0x30;
		b[3] = (b[3] & 0x80) | 0x20;
		e7_wr(0x11C, b, 4);
	}
}

/* reg 0x133 bit0: 1 = signal above threshold (light), 0 = LOS. -1 on I2C err. */
int en7571_los_present(void)
{
	u8 s[4];
	if (e7_rd(0x130, s, 4))
		return -1;
	return s[3] & 0x01;
}

/* en7571_sw_reset (phy.ko @0x2c31c): reg 0x300 byte0 low3 -> 001 (sw reset). */
static void en7571_sw_reset(void)
{
	u8 b[4];
	if (!e7_rd(0x300, b, 4)) { b[0] = (u8)((b[0] & 0xF8) | 0x01); e7_wr(0x300, b, 4); }
}

/* en7571_hw_reset (phy.ko @0x2ae08): reg 0x15D bit0 pulse -- clear (assert) then set
 * (deassert), back-to-back (vendor relies on I2C latency, no explicit delay). */
static void en7571_hw_reset(void)
{
	u8 b[4];
	if (!e7_rd(0x15C, b, 4)) { b[1] = (u8)(b[1] & 0xFE); e7_wr(0x15C, b, 4); }
	if (!e7_rd(0x15C, b, 4)) { b[1] = (u8)(b[1] | 0x01); e7_wr(0x15C, b, 4); }
}

/* en7571_7571_enable (phy.ko @0x2bb5c): MASTER analog enable -- reg 0x228 bit0 = 1.
 * Powers up the EN7571 analog front-end (incl. the APD charge pump). Without this the
 * bias DAC (reg 0x30) has no running pump to drive -> a single write does nothing. */
static void en7571_master_enable(void)
{
	u8 b[4];
	if (!e7_rd(0x228, b, 4)) { b[0] |= 0x01; e7_wr(0x228, b, 4); }
}

static void en7571_pwradc_enable(void)
{
	u8 b[4];
	if (!e7_rd(0x000, b, 1)) { b[0] &= ~0x0E;               e7_wr(0x000, b, 1); }
	if (!e7_rd(0x038, b, 4)) { b[3] &= ~0x10;               e7_wr(0x038, b, 4); } /* 0x03B mux->PD */
	if (!e7_rd(0x2B4, b, 1)) { b[0] = (b[0] & ~0x03) | 0x01; e7_wr(0x2B4, b, 1); }
}

/* raw16 power-ADC code; rises with received optical power. dark!=0 => mux to internal ref. */
static int en7571_rx_raw_x(int dark)
{
	u8 b[4]; int i; u32 v;
	if (dark) { if (!e7_rd(0x038, b, 4)) { b[3] |= 0x10; e7_wr(0x038, b, 4); } }
	if (e7_rd(0x208, b, 4)) return -1; b[3] |= 0x07; e7_wr(0x208, b, 4); /* ch enable 0x20B[2:0] */
	e7_rd(0x208, b, 4);                b[3] |= 0x80; e7_wr(0x208, b, 4); /* START   0x20B[7]      */
	udelay(20 * 1000);
	for (i = 0; i < 100; i++) { if (e7_rd(0x208, b, 4)) break; if ((b[2] >> 4) & 1) break; udelay(1000); }
	e7_rd(0x208, b, 4);
	v = ((((u32)b[2] << 16) | ((u32)b[1] << 8) | b[0]) & 0xFFFFF) >> 4;
	if (!e7_rd(0x208, b, 4)) { b[3] &= ~0x07; e7_wr(0x208, b, 4); }       /* cleanup ch disable */
	if (dark) { if (!e7_rd(0x038, b, 4)) { b[3] &= ~0x10; e7_wr(0x038, b, 4); } }
	return (int)v;
}

/* en7571_APD_initialization (phy.ko @0x2de64): power on the APD RX front-end.
 * RMW the 4-byte word based at reg 0x30: set reg 0x32 bit5 (b[2]|=0x20) then, on a
 * fresh re-read, reg 0x31 bit0 (b[1]|=0x01). These are the RX front-end ENABLE bits
 * ONLY -- NO high-voltage bias DAC is written here (that is en7571_APD_DAC /
 * en7571_APD_control's 30-45V VBR loop), so this carries zero HV/APD-damage risk. */
static void en7571_apd_init(void)
{
	u8 b[4];
	if (!e7_rd(0x30, b, 4)) { b[2] |= 0x20; e7_wr(0x30, b, 4); }
	if (!e7_rd(0x30, b, 4)) { b[1] |= 0x01; e7_wr(0x30, b, 4); }
}

/* en7571_APD_DAC (phy.ko @0x2eba0): enable the RX front-end, then write ONE bias
 * DAC code byte to reg 0x30. Higher code => higher APD reverse-bias voltage.
 * CAUTION: this programs a real ~30-45V APD bias. The caller MUST ramp up from a
 * low code and stop as soon as light is detected (stays well below breakdown). */
static void en7571_apd_dac(u8 code)
{
	u8 b[1];
	en7571_apd_init();          /* 0x31.0/0x32.5 front-end enables */
	b[0] = code;
	e7_wr(0x30, b, 1);          /* reg 0x30 = bias DAC code */
}

/* Shut the APD front-end fully OFF: clear the enable bits (0x31.0/0x32.5) and zero
 * the bias DAC (reg 0x30). Disconnects the HV charge pump -> stops dark-current/
 * thermal drift. Invoked via `echo x > /proc/econet_xpon_los`. */
static void en7571_apd_off(void)
{
	u8 b[4];
	if (!e7_rd(0x30, b, 4)) {
		b[0] = 0x00;        /* DAC code = 0        */
		b[1] &= ~0x01;      /* clear 0x31.0 enable */
		b[2] &= ~0x20;      /* clear 0x32.5 enable */
		e7_wr(0x30, b, 4);
	}
	/* also drop the master analog enable so the HV charge pump powers down and
	 * the APD bias cap bleeds off -> repeatable measurements between runs. */
	if (!e7_rd(0x228, b, 4)) { b[0] &= ~0x01; e7_wr(0x228, b, 4); }
	en7571_optical_up = 0;
	e7_rd(0x30, b, 4);
	printk(KERN_INFO "en7571_apd: front-end DISABLED (+master off), reg0x30=%02x%02x%02x%02x\n",
	       b[0], b[1], b[2], b[3]);
}

/* Pure RX read: sample PWRADC + LOS WITHOUT touching the APD enables/DAC (reg 0x30).
 * Lets us watch a held bias for stability (settling vs runaway) and do a light on/off
 * test without the per-call re-enable that ratchets the charge pump. `echo r`. */
static void en7571_pure_read(void)
{
	u8 b[4]; int raw, los, i;
	/* ensure PWRADC mux points at the photodiode (0x3B bit4 = 0), do not re-enable APD */
	if (!e7_rd(0x038, b, 4)) { b[3] &= ~0x10; e7_wr(0x038, b, 4); }
	e7_rd(0x30, b, 4);
	for (i = 0; i < 3; i++) {
		raw = en7571_rx_raw_x(0);
		los = en7571_los_present();
		printk(KERN_INFO "en7571_rd: reg0x30=%02x%02x%02x%02x LOS=%d rx_raw=%d\n",
		       b[0], b[1], b[2], b[3], los, raw);
		udelay(20 * 1000);
	}
}

/* Host-controlled ramp step: set bias=code, then measure rx_raw/LOS and report
 * net vs the dark floor. Invoked via `echo dXX > /proc/econet_xpon_los`. */
/* One-time EN7571 optical front-end bring-up (subset of en7571_init, in vendor order):
 * resets -> master analog enable -> PWRADC offset cal -> PWRADC enable -> LOS init.
 * Run ONCE via `echo I`; then dXX sets bias + reads without re-initing. */
static void en7571_front_end_init(void)
{
	if (!en7571_detect()) { printk(KERN_INFO "en7571_fe: not present\n"); return; }
	en7571_sw_reset();
	en7571_hw_reset();
	en7571_master_enable();             /* reg 0x228 bit0 -> power the analog front-end */
	en7571_PWRADC_calibration();        /* measure PWRADC dark offset */
	en7571_pwradc_enable();
	en7571_los_init();
	printk(KERN_INFO "en7571_fe: front-end init done (resets+enable+pwradc-cal+los)\n");
}

/* One-shot optical RX bring-up: power the EN7571 front-end + bias the APD into the
 * detection point. Idempotent. Afterwards reg 0x133 bit0 (LOS) tracks the GPON
 * downstream optical carrier (0 = signal present, 1 = loss of signal). PROVEN:
 * fiber-in -> LOS 0, fiber-out -> LOS 1 (30/30 each). No per-unit cal needed. */
void en7571_optical_bringup(void)
{
	if (en7571_optical_up)
		return;
	/* Ensure the SIF/optical-RX functional clock (0xbfa2015c bit21|bit0) + SIF I2C
	 * controller are up BEFORE probing the BOSA. Idempotent (gated by xpon_sif_inited).
	 * Without this, detect reads 0xff after a cold boot / GPON-PHY reset cleared the clock. */
	xpon_phy_rx_bringup();
	if (!en7571_detect()) { printk(KERN_INFO "en7571: optical BOSA not present\n"); return; }
	en7571_front_end_init();
	en7571_apd_dac(0x60);              /* ~39V: APD gain sufficient for LOS signal-detect */
	en7571_optical_up = 1;
	printk(KERN_INFO "en7571: optical RX up (APD bias 0x60); LOS = reg0x133.0\n");
}

/* en7571 HIGH-SPEED RX datapath enable — the "tail" of stock en7571_init that our
 * port omitted (we only did master_enable 0x228.0 + los_init). This brings the
 * BOSA's LA / recovered-data output driver + digital clock-recovery loop out of
 * their squelched power-on defaults so the EN7528 SerDes CDR gets a data eye.
 * Faithful transcription of stock en7571_reg_init(@0x2af54) + force_mode(@0x38868)
 * + burst_ctrl(@0x39090) + DCL_start(@0x2bc0c). All SIF dev 0x70, 4-byte RMW words
 * (buf[N] = reg base+N). Decoded 2026-07-02. */
static void en7571_reg_init_full(void)
{
	u8 b[4];
	/* --- en7571_reg_init RX-datapath config (0x220..0x263) --- */
	if (!e7_rd(0x220,b,4)) { b[2]=0x80; b[3]&=~0x0f;                 e7_wr(0x220,b,4); }
	if (!e7_rd(0x224,b,4)) { b[2]=0x32; b[3]&=~0x0f;                 e7_wr(0x224,b,4); }
	b[0]=0x00; b[1]=0x00; b[2]=0xff; b[3]=0x0f;                      e7_wr(0x240,b,4);
	b[0]=0x00; b[1]=0x00; b[2]=0xff; b[3]=0x0f;                      e7_wr(0x244,b,4);
	if (!e7_rd(0x24c,b,4)) { b[0]=0; b[1]=(b[1]&~0x0f)|3; b[2]=0; b[3]=(b[3]&~0x0f)|2; e7_wr(0x24c,b,4); }
	if (!e7_rd(0x234,b,4)) { b[0]=(b[0]&~0x03)|2; b[1]|=0x3f; b[2]=(b[2]&~0x3f)|0x25; b[3]|=0x3f; e7_wr(0x234,b,4); }
	if (!e7_rd(0x238,b,4)) { b[0]=(b[0]&~0x3f)|0x25; b[1]|=0x3f; b[2]=(b[2]&~0x3f)|0x25; e7_wr(0x238,b,4); }
	if (!e7_rd(0x248,b,4)) { b[0]=0x04; b[1]=0x02; b[3]=(b[3]&~0x07)|1; e7_wr(0x248,b,4); }
	/* en7571 AUTO-lock mode: reg 0x25B[1:0] = 0. WAS force_mode (=11) — WRONG for the RX
	 * boot path: stock uses en7571_force_mode ONLY transiently during TX calibration and
	 * always releases to auto (en7571_auto_lock_mode @0x38918 clears [1:0]). Forced mode
	 * freezes the CDR -> PHYSTA1[20:18] can never advance to ready=6 no matter the eye.
	 * (Root-caused 2026-07-02: this is why every APD/gain sweep failed to lock.) */
	if (!e7_rd(0x258,b,4)) { b[3]&=~0x03;                           e7_wr(0x258,b,4); }
	if (!e7_rd(0x23c,b,4)) { b[0]=0x07; b[1]&=~0x03;                e7_wr(0x23c,b,4); }
	if (!e7_rd(0x22c,b,4)) { b[0]|=0x07; b[3]=(b[3]&~0x3f)|0x02;    e7_wr(0x22c,b,4); }
	if (!e7_rd(0x260,b,4)) { b[3]=(b[3]&~0x07)|3;                   e7_wr(0x260,b,4); }
	if (!e7_rd(0x254,b,4)) { b[2]=0; b[3]=0;                        e7_wr(0x254,b,4); }
	if (!e7_rd(0x250,b,4)) { b[2]&=0x80;                            e7_wr(0x250,b,4); }
	/* en7571_RSSI_gain_init (@0x2c3e0, self-contained): reg 0x16 bits[2:0]=5 = LA/RSSI gain level.
	 * Sets the limiting-amp gain for the recovered-data eye (stock en7571_init step 20). */
	if (!e7_rd(0x14,b,4)) { b[2]=(b[2]&0xf8)|0x05;                 e7_wr(0x14,b,4); }
	/* en7571_safe_circuit_reset (@0x33288): reg 0x101 bit6 = 1 (self-clearing analog datapath
	 * reset). STOCK ORDER: safe_circuit_reset BEFORE link_reg/burst_ctrl (was reversed here). */
	if (!e7_rd(0x100,b,4)) { b[1]|=0x40;                           e7_wr(0x100,b,4); }
	/* en7571_link_reg(1)/burst_ctrl: reg 0x15D bit2 = 1 (stock runs this AFTER safe_circuit_reset) */
	if (!e7_rd(0x15c,b,4)) { b[1]|=0x04;                           e7_wr(0x15c,b,4); }
	/* en7571_DCL_start: reg 0x229 bit0 = 1 -> START the recovered-clock/data loop.
	 * dcl_mode==0 (open-loop, stock-on-this-unit): DO NOT start the servo (it zeroes the DACs). */
	{ extern int dcl_mode;
	  if (dcl_mode && !e7_rd(0x228,b,4)) { b[1]|=0x01;             e7_wr(0x228,b,4); } }
	/* en7571_CDR(ENABLE) (@0x2a518): PHYSET1(0xbfaf0100) bit24=0 = lock the SerDes to the
	 * recovered DATA clock (not the REF clock). (Added 2026-07-02.) */
	{ uint p1 = IO_GPHYREG(PHY_CSR_PHYSET1); IO_SPHYREG(PHY_CSR_PHYSET1, p1 & ~0x01000000u); }
	printk(KERN_INFO "en7571: HS-RX tail applied (reg_init+AUTO_lock+burst_ctrl+safe_reset+DCL_start+CDR_data)\n");
}

/* ★★★ The proven GPON RX-CDR lock sequence (extracted from the 'R' diag cmd, SOLVED 2026-07-03).
 * dev_init -> mode_config(GPON) -> XPON_SETTING -> fw_ready -> start -> optical FE -> HS-RX tail ->
 * NCPO/DDS seed(0x17E21965) + arm bits -> stock config diffs -> IMMEDIATE finalize (the lock gate:
 * the arm->finalize MUST be one tight sequence). quiet=1 suppresses trace (for the keeper).
 * Returns PHYSTA1 rdy[20:18] (6 = LOCKED). */
/* ★★ OPEN-LOOP GPON LASER BRING-UP (en7571/A60928) — the missing TX drive.
 * The port never biased the laser: Ibias(SIF 0x138)/Imod(SIF 0x148) sat at power-on reset
 * (~0) -> laser below threshold -> NO light -> the OLT never heard our O3 SN burst. The vendor
 * seeds these DACs in en7571_config->LUT_recover, but that's gated behind flash slot 144, absent
 * on this ex-mesh 407 -> never ran. Load the generic portable en7571_LUT_Ibias_Imod room-temp
 * seed (25 C, index 26) = the vendor's OWN uncalibrated fallback (mt7570_load_init_current), then
 * clear XPON_SETTING bit7 so the MAC's grant-timed burst-enable keys the laser. Open-loop
 * typical-BOSA power (no per-unit cal, ±few dB); GRANT-TIMED, not continuous. Decode:
 * ~/407/pon_port_plan/scratchpad/en7571_TX_laser_decode.md */
#define EN7571_TX_IBIAS_DEFAULT  0x1FB      /* P0 DC bias current DAC [11:0] (25 C LUT seed) */
#define EN7571_TX_IMOD_DEFAULT   0x44D      /* modulation swing current DAC [11:0] */

/* en7571_change_Ibias @0x36e5c: reg 0x138=Ibias[7:0], 0x139[3:0]=Ibias[11:8]. */
static void en7571_set_ibias(u16 ibias)
{
	u8 b[4];
	if (e7_rd(0x138, b, 4)) return;
	b[1] = (u8)((b[1] & 0xF0) | ((ibias >> 8) & 0x0F));
	b[0] = (u8)(ibias & 0xFF);
	e7_wr(0x138, b, 4);
}
/* en7571_change_Imod @0x36d84: reg 0x148=Imod[7:0], 0x149[3:0]=Imod[11:8]. */
static void en7571_set_imod(u16 imod)
{
	u8 b[4];
	if (e7_rd(0x148, b, 4)) return;
	b[1] = (u8)((b[1] & 0xF0) | ((imod >> 8) & 0x0F));
	b[0] = (u8)(imod & 0xFF);
	e7_wr(0x148, b, 4);
}
/* Read the on-chip Tx-power / laser-MPD monitor: same shared 20-bit PWRADC as the RX read
 * (reg 0x208/0x20B) but with the TIA input mux (SIF reg 0x00 bits[3:1]) pointed at the Tx
 * monitor (=0x100 -> b|0x08) instead of the RX photodiode. Restored after. >dark => lasing. */
int en7571_tx_power_read(void)
{
	u8 b[4], save0, save38 = 0; int i; u32 v;
	/* ★ FIX (2026-07-05, stock en7571_PWRADC_enable/_get RE): stock reads the PWRADC with
	 * reg 0x00 mux bits[3:1]=0 (NOT |0x08) AND reg 0x38 bit4=0 (external photodiode, not the
	 * internal reference). The port set reg 0x00|0x08 and never forced reg 0x38.4=0 -> if a
	 * prior en7571_PWRADC_calibration left 0x3B.4=1 (mux->intref), this read returns the FIXED
	 * internal reference (~14191, flat, laser-independent) instead of the MPD. Match stock. */
	if (e7_rd(0x00, &save0, 1)) return -1;
	{ u8 m = (u8)(save0 & 0xF1); e7_wr(0x00, &m, 1); }               /* mux[3:1]=0 (stock PWRADC) */
	if (!e7_rd(0x38, b, 4)) { save38 = b[3]; b[3] &= ~0x10; e7_wr(0x38, b, 4); } /* 0x3B.4=0 external PD */
	if (e7_rd(0x208, b, 4)) { e7_wr(0x00, &save0, 1); return -1; }
	b[3] |= 0x07; e7_wr(0x208, b, 4);                                /* PWRADC channel enable */
	e7_rd(0x208, b, 4); b[3] |= 0x80; e7_wr(0x208, b, 4);            /* START */
	udelay(1000);
	for (i = 0; i < 100; i++) { if (e7_rd(0x208, b, 4)) break; if ((b[2] >> 4) & 1) break; udelay(1000); }
	e7_rd(0x208, b, 4);
	v = ((((u32)b[2] << 16) | ((u32)b[1] << 8) | b[0]) & 0xFFFFF) >> 4;
	if (!e7_rd(0x208, b, 4)) { b[3] &= ~0x07; e7_wr(0x208, b, 4); }  /* channel disable */
	if (!e7_rd(0x38, b, 4)) { b[3] = (u8)((b[3] & ~0x10) | (save38 & 0x10)); e7_wr(0x38, b, 4); } /* restore 0x3B.4 */
	e7_wr(0x00, &save0, 1);                                          /* restore RX mux */
	if (en7571_PWRADC_offset && v > en7571_PWRADC_offset) v -= en7571_PWRADC_offset; /* stock subtracts dark offset */
	return (int)v;
}

/* Split (a) emitting-but-weak vs (b) still-dark: compare Tx monitor with laser held OFF
 * (XPON_SETTING bit7=1) vs burst-capable (bit7=0). net>8 => the laser is lasing. Leaves bit7=0. */
int en7571_tx_emitting(void)
{
	int dark, lit; uint x;
	x = IO_GREG(0xbfaf0138u); IO_SREG(0xbfaf0138u, x | 0x80u);  mdelay(5); dark = en7571_tx_power_read();
	x = IO_GREG(0xbfaf0138u); IO_SREG(0xbfaf0138u, x & ~0x80u); mdelay(5); lit  = en7571_tx_power_read();
	printk(KERN_DEBUG "en7571_tx: dark=%d lit=%d net=%d => %s\n", dark, lit, lit - dark, (lit - dark > 8) ? "EMITTING" : "DARK");
	return (lit - dark > 8);
}

/* Ramp Ibias up from the room-temp seed until the Tx monitor shows lasing (net>8). Vendor
 * clamp = 0xF7F; practical ceiling = LUT-hot 0x666, step 0x20. Leaves the lasing code set
 * (or the seed if none lases). Returns the lasing Ibias code, or 0 if nothing lased. */
int en7571_tx_ramp_bias(void)
{
	int code, lit, base; uint x;
	x = IO_GREG(0xbfaf0138u); IO_SREG(0xbfaf0138u, x | 0x80u); mdelay(5);
	base = en7571_tx_power_read();                                   /* dark baseline */
	x = IO_GREG(0xbfaf0138u); IO_SREG(0xbfaf0138u, x & ~0x80u);      /* burst-capable */
	for (code = EN7571_TX_IBIAS_DEFAULT; code <= 0x666; code += 0x20) {
		en7571_set_ibias((u16)code); mdelay(5);
		lit = en7571_tx_power_read();
		printk(KERN_DEBUG "en7571_tx: ramp Ibias=0x%03x txpwr=%d dark=%d net=%d%s\n",
			code, lit, base, lit - base, (lit - base > 8) ? " *** LASING ***" : "");
		if (lit - base > 8) return code;
	}
	en7571_set_ibias(EN7571_TX_IBIAS_DEFAULT);                       /* nothing lased: restore seed */
	return 0;
}

/* GPON burst-envelope engine (reg 0x08 T1DELAY block), fixed defaults, NO laser-on PRBS.
 * Mirrors en7571_TGEN's RESULT: T0/T1 delay=0xAA, T0C/T1C turn-on/off timers=0x7F (generous
 * basis; precise per-BOSA values only matter for tight O5 bursts), then ENABLE the engine
 * (reg 0x0B bit3 ERC_enable) with a TGEN_reset pulse. Without the engine on + T0C/T1C, the
 * laser envelope isn't gated to the burst -> reg 0x08=0xAA alone is insufficient. */
static void en7571_gpon_burst_timing(void)
{
	u8 b[4];
	/* FIX 2026-07-04 (stock RE): stock en7571_TGEN derives the burst-envelope timers
	 * from flash slot 0x0c ([7:0]=delay [23:16]=T0C-on [31:24]=T1C-off), NOT fixed 0x7F.
	 * H660 slot 0x0c = 0x444800AA -> delay 0xAA, T0C 0x48, T1C 0x44. The 0x7F/0x7F guesses
	 * mistimed the laser on/off envelope vs the grant window -> OLT burst-CDR couldn't lock
	 * our SN burst -> no Assign_ONU_ID -> O3 stall. Use the cal value to match stock. */
	u32 tg = 0x444800AAu;                   /* h660_flash_cal slot 0x0c (defined below) */
	if (e7_rd(0x08, b, 4)) return;
	b[0] = (u8)(tg & 0xff);                 /* 0xAA  T0/T1 delay   */
	b[1] = (u8)((tg >> 24) & 0xff);         /* 0x44  T1C turn-off  */
	b[2] = (u8)((tg >> 16) & 0xff);         /* 0x48  T0C turn-on   */
	b[3] = (u8)(((tg >> 8) & 0xff) & 0xF7 & 0xDF & 0xFE); /* slot byte1(0x00) + clear ERC/reset/method2 */
	e7_wr(0x08, b, 4);
	if (!e7_rd(0x08, b, 4)) { b[3] |= 0x20;  e7_wr(0x08, b, 4); }               /* TGEN_reset hi */
	if (!e7_rd(0x08, b, 4)) { b[3] &= ~0x20; e7_wr(0x08, b, 4); }               /* pulse lo (latch) */
	if (!e7_rd(0x08, b, 4)) { b[3] = (u8)((b[3] & 0xF7) | 0x08); e7_wr(0x08, b, 4); } /* ERC_enable */
}

/* ★★ FACTORY OPTICAL CAL for the DASAN H660GM-A (device 24), extracted from its NAND
 * (the 400B /tmp/7570_bob.conf blob @0xde10600, GPON magic 0x07050701 @slot 0x94). flash_matrix
 * = 100 LE u32; h660_get_flash_reg(byte_off) = matrix[off>>2]. Per-unit crux: slot 0x00/0x04 =
 * the DCL MPD power targets the HW servo regulates the laser to; slot 0x0c = burst timing. This
 * is PER-UNIT (this specific H660GM-A) — a real port would read /tmp/7570_bob.conf at runtime. */
static const u32 h660_flash_cal[100] = {
	0x054407ce,0x05230157,0xffffffff,0x444800aa,0xffffffff,0xffffffff,0xffffffff,0xffffffff,
	0x00260019,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,
	0x00ae5226,0xffffffff,0xffffffff,0xffffffff,0x27100d00,0x03e80148,0x00640020,0xffffffff,
	0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,
	0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0x07050701,0xffffffff,0xffffffff,
	0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,
	0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,
	0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,
	0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,
	0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,
	0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,
	0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,
	0xffffffff,0xffffffff,0xffffffff,0xffffffff };
static u32 h660_get_flash_reg(u32 off) { return (off < 400) ? h660_flash_cal[off >> 2] : 0xffffffffu; }
static void pack12(u8 *b, u32 hi, u32 lo){ b[0]=hi&0xff; b[1]=(hi>>8)&0xff; b[2]=lo&0xff; b[3]=(lo>>8)&0xff; }

/* en7571_load_Tx_cal_data @0x2f8ac: load the per-unit DCL MPD power targets from flash 0/4 into
 * the servo target regs 0x24C/0x27C (P0) + 0x25C/0x280 (P1). Without these the DCL servo has no
 * reference — THIS is what sets calibrated launch power (vs open-loop's typical-BOSA guess). */
static void en7571_load_tx_cal_data(void)
{
	u32 v; u8 b[4], t0, t1;
	if (h660_get_flash_reg(0) != 0xffffffffu) {
		v = h660_get_flash_reg(0);
		pack12(b, (v >> 16) & 0xfff, v & 0xfff); e7_wr(0x24C, b, 4); e7_wr(0x27C, b, 4);
	}
	if (h660_get_flash_reg(4) == 0xffffffffu) return;
	v = h660_get_flash_reg(4);
	pack12(b, (v >> 16) & 0xfff, v & 0x3ff); e7_wr(0x25C, b, 4);
	t0 = b[0]; t1 = b[1]; b[0] = b[2]; b[1] = b[3]; b[2] = t0; b[3] = t1; e7_wr(0x280, b, 4);
}

/* en7571_SingleClosedLoopMode @0x3c4e4: (re)start the HW DCL servo that holds the laser at the
 * flash MPD target. DCL_stop -> mpdh_stepsize0 -> hw_reset -> DCL_start (reg 0x229 bit0). */
static void en7571_single_closed_loop_mode(void)
{
	u8 b[4];
	if (!e7_rd(0x228, b, 4)) { b[1] &= ~0x01; e7_wr(0x228, b, 4); }   /* 0x229 bit0=0 DCL stop */
	if (!e7_rd(0x236, b, 1)) { b[0] &= 0xC0;  e7_wr(0x236, b, 1); }   /* mpdh_stepsize=0 */
	if (!e7_rd(0x238, b, 1)) { b[0] &= 0xC0;  e7_wr(0x238, b, 1); }
	if (!e7_rd(0x23A, b, 1)) { b[0] &= 0xC0;  e7_wr(0x23A, b, 1); }
	en7571_hw_reset();
	/* Start the DCL when EITHER the old dcl_mode OR the verified tx_closeloop is set. hw_reset above
	 * just cleared the integrator (while DCL stopped); starting now = the stock invariant. With
	 * tx_closeloop this is invoked from apc_bringup AFTER 0x24c/0x25c are seeded (not the old early
	 * bare start), so the loop regulates to the calibrated setpoint instead of collapsing. */
	{ extern int dcl_mode, tx_closeloop;
	  if ((dcl_mode || tx_closeloop) && !e7_rd(0x228, b, 4)) { b[1] |= 0x01;  e7_wr(0x228, b, 4); } } /* 0x229 bit0=1 DCL start */
}

/* ★ REGULATING DCL close (session-11 HW refinement): tx_closeloop=1 used single_closed_loop_mode which
 * ZEROES mpdh_stepsize (0x236/238/23A) → DCL enabled but INERT (DACs frozen at seed, HW-confirmed).
 * On this flash[0x90]-empty unit STOCK skips SingleClosedLoopMode and does a PLAIN DCL_start over the
 * seed, leaving reg_init_full's non-zero stepsize (0x25) so the loop HAS GAIN and actually servos the
 * DACs to the MPD equilibrium. Match that: DCL_stop → hw_reset (integrator clear) → DCL_start, WITHOUT
 * touching mpdh_stepsize. Setpoints 0x24c/0x25c must already be seeded (they are, in apc_bringup). */
static void en7571_dcl_close_regulating(void)
{
	u8 b[4];
	if (!e7_rd(0x228, b, 4)) { b[1] &= ~0x01; e7_wr(0x228, b, 4); }   /* DCL stop */
	en7571_hw_reset();                                               /* clear loop integrator */
	if (!e7_rd(0x228, b, 4)) { b[1] |= 0x01;  e7_wr(0x228, b, 4); }   /* DCL start (stepsize preserved) */
}

/* en7571_T1delay_setting @0x41858 (stock): laser T1 turn-on/off delay adjust. Stock's link_reg(1)
 * runs this with mode=0 (arg inverted vs link) AFTER the burst timers are latched. mode==0 clears
 * the low nibble of the SIF-0x08 delay byte (0xAA -> 0xA0). The port never did this -> its delay
 * stayed 0xAA, shifting the whole burst envelope vs the OLT-assigned start-of-burst. RMW touches
 * only byte0 so T1C/T0C/ctrl are preserved. (mode==1 = link-down, restore from flash slot 0x94.) */
static void en7571_T1delay_setting(int mode)
{
	u8 b[4];
	if (e7_rd(0x08, b, 4)) return;
	if (mode == 1) {
		u32 t = h660_get_flash_reg(0x94);
		if      (t == 0x07050701u) b[0] = 0xAA;
		else if (t == 0xE7050701u || t == 0xA7050701u) b[0] = 0x77;
	} else {
		b[0] &= 0xF0;                        /* 0xAA -> 0xA0 */
	}
	e7_wr(0x08, b, 4);
}

/* ★ Closed-loop factory-cal APC bring-up: load per-unit MPD targets, start the DCL servo (which
 * regulates the laser to the CALIBRATED power), and set the cal's burst timing. The fix open-loop
 * can't do. Call after the DACs are seeded (bring-up) + CDR locked. */
/* en7571_txsd_level_set: ★2026-07-09 port of the 407-stock TX signal-detect comparator cal (disasm @0x2d1e8)
 * that the econet-xpon port never ran. Sequence: mux 0x00->TxSD node (nibble 0x8), start the TxSD ADC
 * (0x208 byte3 |= 0x80), read the TxSD level (0x204 & 0x3ff minus PWRADC dark offset), program the threshold
 * DAC (0x0c, with 0x0d bit0 cleared), restore mux (0x00 nibble 0x2). Threshold here = ~half the measured TxSD
 * level as a first probe (stock uses a flash-slot-4 softfloat coeff). Run via `echo y` after `l` (laser up). */
static void en7571_txsd_level_set(void)
{
	u8 b[4]; u32 adc = 0, thr;
	if (e7_rd(0x00, b, 4)) { printk(KERN_DEBUG "txsd: 0x00 rd-fail\n"); return; }
	b[0] = (u8)((b[0] & ~0x0f) | 0x08); e7_wr(0x00, b, 4); mdelay(2);           /* mux -> TxSD node */
	if (!e7_rd(0x208, b, 4)) { b[3] |= 0x80; e7_wr(0x208, b, 4); } mdelay(5);   /* TxSD ADC start */
	if (!e7_rd(0x204, b, 4)) {
		adc = (u32)(b[0] | (b[1] << 8)) & 0x3ff;
		if (en7571_PWRADC_offset && adc > en7571_PWRADC_offset) adc -= en7571_PWRADC_offset;
	}
	thr = adc >> 1;                                                            /* probe threshold */
	if (!e7_rd(0x0d, b, 4)) { b[0] &= (u8)~0x01; e7_wr(0x0d, b, 4); }           /* clear 0x0d bit0 */
	if (!e7_rd(0x0c, b, 4)) { b[0] = thr & 0xff; b[1] = (u8)((b[1] & 0xf0) | ((thr >> 8) & 0x0f)); e7_wr(0x0c, b, 4); }
	if (!e7_rd(0x00, b, 4)) { b[0] = (u8)((b[0] & ~0x0f) | 0x02); e7_wr(0x00, b, 4); } /* restore mux */
	printk(KERN_DEBUG "txsd_level_set: TxSD_adc=0x%x -> threshold 0x0c=0x%x\n", adc, thr);
}

static void en7571_tx_apc_bringup(void)
{
	u8 b[4]; u32 tv;
	if (h660_get_flash_reg(148) != 0x07050701u) {
		printk(KERN_DEBUG "en7571_apc: no GPON cal (slot148=0x%x) -> open-loop fixed timing\n",
			h660_get_flash_reg(148));
		en7571_gpon_burst_timing();
		return;
	}
	en7571_load_tx_cal_data();               /* per-unit MPD P0/P1 targets (0x24C/0x25C) */
	/* ★ FIX (2026-07-05, servo audit): STOCK runs SingleClosedLoopMode only when flash[0x90]==1
	 * (en7571_config); on this flash-0x90-empty unit stock SKIPS it, leaving the mpdh servo NOT
	 * frozen. The port ran it unconditionally (gated on the wrong slot 0x94) -> froze the servo +
	 * hw_reset -> DAC collapse. Match stock: run only if flash[0x90]==1. DCL_start for RX still
	 * comes from en7571_reg_init_full (dcl_mode), so skipping this does NOT break O2->O3. */
	{ extern int run_scl;
	  int do_scl = (run_scl < 0) ? (h660_get_flash_reg(0x90) == 1u) : (run_scl != 0);
	  if (do_scl) en7571_single_closed_loop_mode(); }
	/* ★★ FIX A/γ (2026-07-04): write the CALIBRATED Ibias/Imod DACs from the flash cal. The port was
	 * seeding the RAW generic LUT (Ibias 0x1FB / Imod 0x44D) — Imod 0x44D(1101) is ~3.2x this unit's
	 * calibrated 0x157(343) -> gross OVER-MODULATION -> asymmetric/rail-clipped eye -> the OLT's burst
	 * receiver can't slice 0/1 -> the burst emits but is UNDECODABLE = the O3 stall. cal[0]&0xfff=Ibias
	 * (0x7CE), cal[1]&0xfff=Imod (0x157). single_closed_loop_mode froze the mod servo so this write sticks. */
	/* ★ H3 FIX (2026-07-08, wf_71169978 vs stock disasm: en7571_TGEN @0x2e97c latches TGEN FIRST with the
	 * laser DARK, then DCL_start @0x2ec8c last). The port DC-biases the laser here (before the latch) so the
	 * ERC counter phase reference seats against a lit laser -> a few-ns edge offset -> marginal ranging.
	 * tgen_dark=1 -> hold Ibias=0 (dark) across the TGEN latch (PRBS23 still drives the serializer per FIX 3),
	 * restore the calibrated bias AFTER the latch = stock order. tgen_dark=0 = old behavior. */
	{ extern int tgen_dark; u16 _im = (u16)(h660_get_flash_reg(4) & 0xfff);   /* Imod 0x157 */
	  en7571_set_imod(_im);
	  en7571_set_ibias(tgen_dark ? 0 : (u16)(h660_get_flash_reg(0) & 0xfff)); }   /* Ibias 0x7CE, or 0=dark for H3 */
	tv = h660_get_flash_reg(12);             /* slot 0x0c burst timing: [15:0]delay [23:16]T0C [31:24]T1C = 0x444800AA */
	/* ★ FIX 3 (2026-07-05, stock en7571_TGEN RE): stock LATCHES the burst envelope (TGEN_reset pulse)
	 * WHILE PRBS23 drives the TX serializer and the CDR is forced to REF clock (PHYSET1 bit24=1). The
	 * port latched it with the serializer IDLE -> TGEN_reset counters init against no signal -> laser
	 * on/off edges not aligned to the grant burst -> OLT burst-CDR can't frame the SN -> O3 stall.  */
	{ extern void phy_tx_test_pattern(uint);
	  uint _p = IO_GREG(0xbfaf0100u); IO_SREG(0xbfaf0100u, _p | (1u << 24));   /* en7571_CDR(0) */
	  phy_tx_test_pattern(PHY_BIST_PRBS23); mdelay(10); }
	if (!e7_rd(0x08, b, 4)) {
		b[0] = tv & 0xff; b[1] = (tv >> 24) & 0xff; b[2] = (tv >> 16) & 0xff;  /* delay=0xAA, T1C=0x44, T0C=0x48 */
		b[3] &= 0xF7;                        e7_wr(0x08, b, 4);               /* clear ERC_enable */
		/* ★ H4 FIX (2026-07-08, wf_71169978 vs stock disasm SETTLE#2 @0x31b34): stock waits ~10ms between
		 * writing T0C/T1C and pulsing TGEN_reset hi, letting the timers propagate through the transceiver
		 * reg->analog delay-line before the latch. The port latched immediately -> timers may not have
		 * seated -> burst-envelope edges a few ns off (marginal ranging). tgen_settle ms (0=old behavior). */
		{ extern int tgen_settle; if (tgen_settle > 0) mdelay(tgen_settle); }
		/* ★ FIX 1 (2026-07-04): TGEN_reset pulse LATCHES the calibrated T0C/T1C into the burst
		 * engine. Without it the envelope stayed at the reset default (~0x7F, too long) which smears
		 * the preamble/delimiter edges -> emits but the OLT can't frame it. */
		e7_rd(0x08, b, 4); b[3] |= 0x20;    e7_wr(0x08, b, 4); mdelay(10);   /* TGEN_reset hi */
		e7_rd(0x08, b, 4); b[3] &= 0xDF;    e7_wr(0x08, b, 4); mdelay(10);   /* TGEN_reset lo */
		e7_rd(0x08, b, 4); b[3] |= 0x08;    e7_wr(0x08, b, 4);               /* ERC_enable */
	}
	{ extern int tgen_dark; if (tgen_dark) en7571_set_ibias((u16)(h660_get_flash_reg(0) & 0xfff)); }  /* ★ H3: apply laser DC bias 0x7CE AFTER the dark TGEN latch (stock order) */
	en7571_T1delay_setting(0);                  /* ★ FIX 4: laser T1 delay byte0 0xAA->0xA0 (stock link_reg(1)->T1delay_setting(0), AFTER timers latch) */
	{ extern void phy_tx_test_pattern(uint);
	  uint _p = IO_GREG(0xbfaf0100u); IO_SREG(0xbfaf0100u, _p & ~(1u << 24));  /* en7571_CDR(1): back to recovered-data clk */
	  phy_tx_test_pattern(PHY_BIST_IDLE); }
	/* ★ FIX (2026-07-05, SFF-8472/ERC): CLOSE the P0/P1 Extinction-Ratio loop. mt7570_reg.h:
	 * 0x138=P0_CS2 ("Ibias"=0-level), 0x148=P1_CS2 ("Imod"=1-level), 0x13c/0x14c=P0/P1_CS3=ERC mode.
	 * Port left CS3 at open-loop default -> P0/P1 levels not regulated / not data-switched (SWING≈0).
	 * mt7570_DualClosedLoopMode sets 0x13c/0x14c[1:0]=ERC_start after the ERC engine (0x08.3) is on. */
	/* ★★★ THE FIX (2026-07-08, wf_86da7401 RE): stock's en7571_OpenLoopMode(1) — set 0x13C/0x14C[1:0]=0b10
	 * (ERC_open_loop_mode "DAC-follow") to COUPLE the calibrated 0x138/0x148 DAC (seeded just above) to the
	 * real laser. Replaces the old DEAD dual_loop block which wrote the WRONG constant |0x01 (ERC_start=
	 * closed-loop) and was default-off. NOT the 0x228 DCL (that path is falsified on this flash-empty unit). */
	{ extern int erc_openloop; extern void en7571_openloop_mode(int on);
	  if (erc_openloop) en7571_openloop_mode(1); }
	/* ★ TDC HOLDOVER (2026-07-08, disasm port of stock phy_tx_tdc_holdover): hold the TX timing reference so
	 * the ranging-burst edges don't jitter vs the OLT window — the one burst-timing mechanism the port lacked. */
	{ extern int tdc_holdover; extern void phy_tx_tdc_holdover(int on);
	  if (tdc_holdover) phy_tx_tdc_holdover(1); }
	/* ★★ VERIFIED en7571 DCL CLOSE (session-11, disasm-faithful). Setpoints 0x24c/0x25c (load_tx_cal_data
	 * above) + calibrated 0x138/0x148 DAC seed + burst timing are all in place now -> close the REAL loop
	 * the stock way (0x228 DCL, via single_closed_loop_mode: stop -> mpdh_stepsize0 -> hw_reset[integrator
	 * clear] -> DCL_start). Holds the calibrated Ibias/Imod split = correct extinction, vs open-loop static
	 * DACs. This is the en7571 path, NOT the dead mt7570 0x13c/0x14c above. Gated by tx_closeloop (default
	 * 0 = current open-loop O3 baseline). NEXT INCREMENT (documented, not yet wired): the en7571 ERC
	 * fine-tune layer = en7571_fine_tune_PWR_ER (407 phy.ko @0x3bbdc) periodically nudges 0x25c MPD-High
	 * (1-level target), gated by en7571_ERC (flash[0x60]!=0xFF); add to the servo kthread once the DCL
	 * close is proven on the rig. */
	/* ★ mainline dual-CL config (merbanan LDDLA diff): FORCE the Pav/P1 loop to the calibrated
	 * setpoints (0x258[1:0]=3) + enable KT Imod-adjust (0x230[0]=1) BEFORE closing the DCL, so the
	 * loop holds the flash extinction instead of free-running. Applied here (post-CDR-lock) not in the
	 * RX reg_init. */
	{ extern int tx_force, tx_kt; u8 f[4];
	  if (tx_force && !e7_rd(0x258, f, 4)) { f[3] = (u8)((f[3] & 0xfc) | 0x03); e7_wr(0x258, f, 4);
		printk(KERN_DEBUG "en7571_apc: FORCE mode 0x258[1:0]=3 (mainline en7571_force_mode)\n"); }
	  if (tx_kt && !e7_rd(0x230, f, 4)) { f[0] = (u8)((f[0] & 0xfe) | 0x01); e7_wr(0x230, f, 4);
		printk(KERN_DEBUG "en7571_apc: KT Imod-adjust 0x230[0]=1 (mainline en7571_hwkt)\n"); } }
	{ extern int tx_closeloop;
	  if (tx_closeloop >= 2) {
		en7571_dcl_close_regulating();      /* stepsize PRESERVED -> loop has gain, servos DACs (stock-faithful on flash[0x90]-empty) */
		printk(KERN_DEBUG "en7571_apc: en7571 DCL REGULATING close (stepsize preserved, 0x228 stop->hw_reset->start)\n");
	  } else if (tx_closeloop == 1) {
		en7571_single_closed_loop_mode();   /* OLD: zeroes mpdh_stepsize -> inert (HW-confirmed session-11). kept for A/B */
		printk(KERN_DEBUG "en7571_apc: en7571 DCL closed-loop ON [INERT: stepsize=0] (0x228 seed->hw_reset->start)\n");
	  }
	}
	{
		u8 ib[4] = {0}, im[4] = {0};
		e7_rd(0x138, ib, 4); e7_rd(0x148, im, 4);
		printk(KERN_DEBUG "en7571_apc: CALIBRATED drive Ibias=0x%03x Imod=0x%03x (want 0x7CE/0x157, NOT raw 0x1FB/0x44D) | delay=%02x T0C=%02x T1C=%02x\n",
			(uint)(((ib[1] & 0xf) << 8) | ib[0]), (uint)(((im[1] & 0xf) << 8) | im[0]),
			tv & 0xff, (tv >> 16) & 0xff, (tv >> 24) & 0xff);
	}
}

/* Faithful en7571_TGEN(1) @0x2c4a4: auto-measure the TRUE per-BOSA burst T0C/T1C timers.
 * 32x PRBS-driven measure loop (clear ERC -> meas-delay 0x66 -> T1C/T0C reset 0x7F -> TGEN_reset
 * pulse -> method2 pulse -> read reg 0x10=T0C / 0x11=T1C, keep MAX), then latch delay 0xAA +
 * measured T0C/T1C + ERC_enable. Replaces the guessed fixed 0x7F. A functional-TX unit that emits
 * but stalls at O3 = wrong burst envelope -> this is the likely fix. Brief continuous PRBS
 * (user-authorized). Needs the laser BIASED + KEYED first (measures the real turn-on/off). */
static void en7571_tgen_measure(int gpon)
{
	extern void phy_tx_test_pattern(uint);
	u8 b[4]; int i, rgs_t0c = 0, rgs_t1c = 0; uint p;
	p = IO_GREG(0xbfaf0100u); IO_SREG(0xbfaf0100u, p | (1u << 24));      /* en7571_CDR(DISABLE): PHYSET1.24=1 */
	phy_tx_test_pattern(PHY_BIST_PRBS23);
	for (i = 0; i < 32; i++) {
		if (e7_rd(0x08, b, 4)) goto done;
		b[3] &= 0xF7;                                e7_wr(0x08, b, 4); udelay(2);  /* clear ERC_enable */
		e7_rd(0x08, b, 4); b[0] = 0x66;              e7_wr(0x08, b, 4); udelay(2);  /* meas delay 0x66  */
		e7_rd(0x08, b, 4); b[1] = 0x7F; b[2] = 0x7F; e7_wr(0x08, b, 4); udelay(2);  /* T1C/T0C reset max */
		e7_rd(0x08, b, 4); b[3] |= 0x20;             e7_wr(0x08, b, 4); udelay(2);  /* TGEN_reset hi */
		e7_rd(0x08, b, 4); b[3] &= 0xDF;             e7_wr(0x08, b, 4); udelay(2);  /* TGEN_reset lo */
		e7_rd(0x08, b, 4); b[3] |= 0x01;             e7_wr(0x08, b, 4); udelay(2);  /* method2 hi */
		e7_rd(0x08, b, 4); b[3] &= 0xFE;             e7_wr(0x08, b, 4);             /* method2 lo */
		if (e7_rd(0x10, b, 4)) goto done;                                          /* capture */
		if (rgs_t0c < b[0]) rgs_t0c = b[0];   /* reg 0x10 = measured T0C (turn-ON)  */
		if (rgs_t1c < b[1]) rgs_t1c = b[1];   /* reg 0x11 = measured T1C (turn-OFF) */
	}
	e7_rd(0x08, b, 4); b[0] = gpon ? 0xAA : 0x77; e7_wr(0x08, b, 4);              /* T0/T1 delay */
	printk(KERN_DEBUG "en7571_tgen: measured T0C=0x%02x T1C=0x%02x\n", rgs_t0c, rgs_t1c);
	e7_rd(0x08, b, 4); b[1] = (u8)rgs_t1c; b[2] = (u8)rgs_t0c; e7_wr(0x08, b, 4); /* latch T1C/T0C */
	e7_rd(0x08, b, 4); b[3] |= 0x20; e7_wr(0x08, b, 4); mdelay(10);              /* TGEN_reset hi */
	e7_rd(0x08, b, 4); b[3] &= 0xDF; e7_wr(0x08, b, 4); mdelay(10);              /* TGEN_reset lo */
	e7_rd(0x08, b, 4); b[3] |= 0x08; e7_wr(0x08, b, 4);                          /* ERC_enable ON */
done:
	p = IO_GREG(0xbfaf0100u); IO_SREG(0xbfaf0100u, p & ~(1u << 24));   /* en7571_CDR(ENABLE): PHYSET1.24=0 */
	phy_tx_test_pattern(PHY_BIST_IDLE);
}

void en7571_tx_laser_bringup(void)
{
	u8 ib[4] = {0}, im[4] = {0};
	en7571_set_ibias((u16)(tx_ibias & 0xFFF));     /* seed the DACs (servo start point) */
	en7571_set_imod(EN7571_TX_IMOD_DEFAULT);
	en7571_tx_apc_bringup();                        /* ★ closed-loop factory-cal power + cal burst timing */
	/* XPON_SETTING(0xbfaf0138) = tx_xpon: bit7 = burst-enable POLARITY (BOSA-specific), bit4 = tx_sd
	 * invert. This keys the laser to the burst; wrong polarity = laser fires between bursts, not
	 * during the SN. Default 0x10F; sweep 0x10F/0x11F/0x18F/0x19F vs OLT. */
	IO_SREG(0xbfaf0138u, (uint)tx_xpon);
	/* ★ FIX 5 (2026-07-05, LIVE stock-vs-port register diff): stock ANATXREG1(0xbfaf01a0)[19:16]
	 * TX-drive-swing = 0x0; port left it at the HW default 0x9 (port never calls phy_tx_amp_setting
	 * on the activation path — only via a debug cmd). Non-zero digital swing on top of the analog
	 * en7571 Imod distorts the eye/extinction -> emitted burst not OLT-decodable -> O3 stall. Set 0
	 * to match stock (clears port's 0x00990006 -> 0x00900006 = stock's exact value). */
	{ uint v = IO_GREG(0xbfaf01a0u); IO_SREG(0xbfaf01a0u, (v & ~0x000F0000u) | (((uint)tx_amp & 0xF) << 16)); }  /* raw MMIO (phy_tx_amp_setting's IO_SPHYREG doesn't land here) */
	e7_rd(0x138, ib, 4); e7_rd(0x148, im, 4);
	printk(KERN_DEBUG "en7571_tx: LASER bringup: Ibias=0x%03x Imod=0x%03x burst-timing(en) XPON_SETTING=%08x ANATXREG1=%08x\n",
		((ib[1] & 0x0F) << 8) | ib[0], ((im[1] & 0x0F) << 8) | im[0], IO_GREG(0xbfaf0138u), IO_GREG(0xbfaf01a0u));
}

int gpon_do_lock(int quiet)
{
	extern int phy_mode_config(Xpon_Phy_Mode_t, int);
	extern int phy_fw_ready(unchar);
	extern int xpon_phy_start(void);
	uint p, ps;
	phy_dev_init();
	phy_mode_config(PHY_GPON_CONFIG, PHY_ENABLE);
	IO_SREG(0xbfaf0138u, 0x0000010Fu);                              /* XPON_SETTING (clear RX_SD_INV) */
	phy_fw_ready(PHY_ENABLE);
	xpon_phy_start();
	en7571_optical_bringup();
	en7571_reg_init_full();
	/* NCPO seed + arm (stock phy_los_handler preamble + GPON arm) */
	IO_SREG(0xbfaf05f4u, IO_GREG(0xbfaf05f8u));                     /* clear XPON int */
	phy_bit_delay(0);
	phy_reset_counter();
	p = IO_GREG(0xbfaf010cu); IO_SREG(0xbfaf010cu, p & ~0x100u);    /* PHYSET4 bit8=0 */
	{ extern unsigned int ncpo_val; IO_SREG(0xbfaf0194u, ncpo_val); } /* ANASET14 seed (ncpo_val: test stock-locked 0x1E1A9FBE) */
	IO_SREG(0xbfaf01d8u, 0x17E21965u);                             /* ANADDS1 seed (stock stays at seed) */
	p = IO_GREG(0xbfaf01f0u); IO_SREG(0xbfaf01f0u, p | 0x20u);      /* TDCSET1.5=1 */
	p = IO_GREG(0xbfaf0104u); IO_SREG(0xbfaf0104u, p | 0x10u);      /* PHYSET2.4=1 */
	/* stock config diffs (match locked stock; not strictly required for lock but stock-faithful) */
	if (wmask & 0x01) IO_SREG(0xbfaf0110u, 0x00a0e004u);
	if (wmask & 0x02) IO_SREG(0xbfaf012cu, 0x00407f7fu);
	if (wmask & 0x04) IO_SREG(0xbfaf0150u, 0x0000e000u | ((uint)po_anapwd & 0x180u));
	if (wmask & 0x08) { p = IO_GREG(0xbfaf01a0u); IO_SREG(0xbfaf01a0u, p & ~0x90000u); }
	if (wmask & 0x10) IO_SREG(0xbfaf01e0u, 0x00003c05u);
	if (wmask & 0x20) IO_SREG(0xbfaf01f8u, 0x0000002du);
	if (wmask & 0x40) IO_SREG(0xbfaf0228u, 0x00070000u);
	if (wmask & 0x80) IO_SREG(0xbfaf0230u, 0x00000007u);
	mdelay(5);
	/* ★ IMMEDIATE finalize -> tracking (PHYSET4.8=1, TDCSET1.5=0, PHYSET2->0x..01) */
	p = IO_GREG(0xbfaf010cu); IO_SREG(0xbfaf010cu, p | 0x100u);
	p = IO_GREG(0xbfaf01f0u); IO_SREG(0xbfaf01f0u, p & ~0x20u);
	p = (IO_GREG(0xbfaf0104u) & ~0x10u); if (fin_b0) p |= 0x1u; else p &= ~0x1u;
	IO_SREG(0xbfaf0104u, p);
	mdelay(5);
	/* ★ HOLD THE LASER OFF: set XPON_SETTING(0x138) bit7 (burst-invert = TX-DISABLE). Done
	 * AFTER the CDR finalize so it can't perturb the lock. Re-applied on every lock (the
	 * bring-up above cleared it via 0x10F), so it sticks across the poll's re-lock attempts. */
	if (tx_laser_off) {
		p = IO_GREG(0xbfaf0138u); IO_SREG(0xbfaf0138u, p | 0x80u);   /* laser held OFF (bit7=1) */
	} else {
		uint atx;
		en7571_tx_laser_bringup();   /* ★ bias the laser open-loop + un-hold (bit7=0) so O3 SN TX emits */
		/* ★★★ ROOT-CAUSE FIX (2026-07-04, v2): set SerDes TX drive swing ANATXREG1[19:16]=0x9 via MMIO.
		 * v1 used phy_tx_amp_setting()/IO_SPHYREG which routes the WRITE through phy_I2C_write_translet
		 * -> SIF I2C dev 0x70 (optical transceiver), NOT MMIO 0xbfaf01a0 where the read lives -> the
		 * amp never actually changed (proven: readback stayed 0x00900006 after every write). Write MMIO
		 * directly here, on the O1->O5 path, AFTER the CDR relock's phy reset. Without swing the laser
		 * emits CW DC (no data eye) so the OLT decodes nothing. DZS drives level 9. */
		atx = IO_GREG(0xbfaf01a0u);
		IO_SREG(0xbfaf01a0u, (atx & 0xfff0ffffu) | 0x00090000u);
	}
	ps = IO_GREG(0xbfaf0130u);
	if (!quiet)
		printk(KERN_DEBUG "gpon_do_lock: PHYSTA1=%08x rdy=%u RXSTAT=%08x\n",
			ps, (ps >> 18) & 0x7, IO_GREG(0xbfaf021cu));
	return (ps >> 18) & 0x7;
}

/* 1 if the GPON CDR is currently locked (PHYSTA1 rdy[20:18]==6). */
int gpon_is_locked(void)
{
	return (((IO_GREG(0xbfaf0130u) >> 18) & 0x7) == 6);
}

/* ★ O1->O2 datapath probe: START the MAC<->PHY MBI interface, then watch DS_SPF.
 * gpon_init() leaves MBI STOPPED (gponDevMbiStop(ENABLE)); the auto-lock poll never runs
 * gpon_enable()/gpon_dev_init_reset() which would START it -> the GTC RX gets no downstream
 * -> DBG_DS_SPF_CNT stays 0 -> stuck at O1. Here we clear mbi_rx_stop(bit0)+mbi_tx_stop(bit8)
 * of G_MBI_STOP(0xbfb64160) WITHOUT resetting the MAC/PHY (keeps the CDR lock) and log DS_SPF.
 * If DS_SPF starts counting -> the O1->O2 gap is the un-started MBI datapath (confirm the fix).
 * Shared by the 'm' proc cmd and the one-shot auto-probe after the CDR locks. */
void gpon_mbi_start_probe(void)
{
	uint mbi, act, spf1, spf2; int i;
	mbi = IO_GREG(0xbfb64160u);
	printk(KERN_DEBUG "MBI-probe: pre  G_MBI_STOP=%08x (rx_stop=%u tx_stop=%u) G_GBL_CFG=%08x G_ACT=%08x rdy=%u DS_SPF=%08x\n",
		mbi, mbi & 1u, (mbi >> 8) & 1u, IO_GREG(0xbfb64004u), IO_GREG(0xbfb640bcu),
		(IO_GREG(0xbfaf0130u) >> 18) & 0x7u, IO_GREG(0xbfb64358u));
	mbi &= ~((1u << 0) | (1u << 8));		/* START MBI RX + TX */
	IO_SREG(0xbfb64160u, mbi);
	mdelay(2);
	printk(KERN_DEBUG "MBI-probe: post G_MBI_STOP=%08x (started)\n", IO_GREG(0xbfb64160u));
	for (i = 0; i < 6; i++) {
		act  = IO_GREG(0xbfb640bcu);
		spf1 = IO_GREG(0xbfb64358u);
		mdelay(300);
		spf2 = IO_GREG(0xbfb64358u);
		printk(KERN_DEBUG "MBI-probe %d: G_ACT=%08x act_st=O%u | DS_SPF %08x->%08x d=%d | rdy=%u RXSTAT=%08x\n",
			i, act, act & 0xfu, spf1, spf2,
			(int)((spf2 & 0x3fffffffu) - (spf1 & 0x3fffffffu)),
			(IO_GREG(0xbfaf0130u) >> 18) & 0x7u, IO_GREG(0xbfaf021cu));
	}
}

/* DEFAULT 0 (2026-07-03): the MBI-start auto-probe PROVED DS_SPF counts at 8kHz (datapath
 * healthy) but starting the MBI (feDevGdm2Cdm2Stop) disables the shared FE -> KILLS the wired
 * LAN eth MAC (no carrier) -> can't reflash over the wire. Now off by default; the SM-drive
 * (mac_activate) advances O1->O2 without touching the MBI, so the LAN stays up. Set 1 (param
 * or 'm' proc cmd) only when you deliberately want the DS_SPF/MBI probe (accepting LAN loss). */
int mbi_autoprobe = 0; module_param(mbi_autoprobe, int, 0644);

/* Auto-lock keeper kthread: whenever the CDR is not locked (PHYSTA1 rdy!=6), run gpon_do_lock().
 * This makes the port lock automatically on real bring-up + re-acquire if the fiber drops. */
int auto_lock = 1; module_param(auto_lock, int, 0644);
static int gpon_lock_keeper(void *arg)
{
	int rdy, locked = 0;
	msleep(4000);   /* let the standard init settle */
	printk(KERN_INFO "econet_xpon: gpon_lock_keeper started (auto_lock=%d)\n", auto_lock);
	while (!kthread_should_stop()) {
		rdy = (IO_GREG(0xbfaf0130u) >> 18) & 0x7;
		if (rdy == 6) {
			if (!locked) { printk(KERN_INFO "econet_xpon: CDR LOCKED (rdy=6)\n"); locked = 1; }
		} else {
			if (locked) printk(KERN_INFO "econet_xpon: CDR lost lock (rdy=%d), re-acquiring\n", rdy);
			locked = 0;
			if (auto_lock)
				rdy = gpon_do_lock(1);
			if (rdy == 6) { printk(KERN_INFO "econet_xpon: CDR LOCKED (rdy=6, auto-acquire)\n"); locked = 1; }
		}
		msleep(locked ? 3000 : 2000);
	}
	return 0;
}

/* Set the APD bias DAC = code and read rx_raw/LOS. Assumes front-end already inited
 * (echo I). Does NOT re-run resets/init -> avoids the per-call accumulation. */
static void en7571_apd_measure(u8 code)
{
	int raw, los, i;
	u8 r[4] = {0,0,0,0};

	en7571_apd_dac(code);               /* program APD bias (enable + DAC write) */
	e7_rd(0x30, r, 4);
	for (i = 0; i < 3; i++) {
		raw = en7571_rx_raw_x(0);
		los = en7571_los_present();
		printk(KERN_INFO "en7571_apd: code=0x%02x reg0x30=%02x%02x%02x%02x LOS=%d rx_raw=%d\n",
		       code, r[0], r[1], r[2], r[3], los, raw);
		udelay(30 * 1000);
	}
}

/* echo 4: full EN7571 RX bring-up + read (LOS bit + raw PWRADC w/ dark offset). */
static void en7571_rx_test(void)
{
	int present = en7571_detect();
	int dark, raw, los, i;
	u8 apd[4] = {0,0,0,0};

	printk(KERN_INFO "en7571_rx: detect=%d (id/rev gate)\n", present);
	if (!present) {
		printk(KERN_INFO "en7571_rx: not present -> abort\n");
		return;
	}
	/* Power on the APD RX front-end (enable bits only, no bias voltage). */
	e7_rd(0x30, apd, 4);
	printk(KERN_INFO "en7571_rx: reg0x30 pre-APD  = %02x %02x %02x %02x\n",
	       apd[0], apd[1], apd[2], apd[3]);
	en7571_apd_init();
	e7_rd(0x30, apd, 4);
	printk(KERN_INFO "en7571_rx: reg0x30 post-APD = %02x %02x %02x %02x (0x31.0/0x32.5 enable)\n",
	       apd[0], apd[1], apd[2], apd[3]);
	en7571_pwradc_enable();
	dark = en7571_rx_raw_x(1);            /* dark/offset baseline (mux->internal ref) */
	en7571_los_init();

	for (i = 0; i < 3; i++) {
		raw = en7571_rx_raw_x(0);
		los = en7571_los_present();
		printk(KERN_INFO "en7571_rx[%d]: LOS_present=%d  rx_raw=%d (0x%x)  dark=%d  net=%d\n",
		       i, los, raw, raw, dark, (raw >= 0 && dark >= 0) ? raw - dark : -1);
		udelay(50 * 1000);
	}
}

/* ★ regdump buffer: the 'o' diag fills this (NO printk — bulk console output HANGS the 115200 board);
 * read it over LAN via /proc/econet_xpon_regdump. */
static char g_regdump[16384];
static int  g_regdump_len;
static ssize_t xpon_los_read(struct file *f, char __user *b, size_t l, loff_t *o)
{
	char k[620];
	uint xpon_sta, rx_status, physta1, setting;
	int los, los_rxlos, los_rxsd, n;
	int temp, volt, txpwr, rxpwr, sif_ok, temp_c;

	(void)f;
	if (!xpon_phy_rx_up)
		xpon_phy_rx_bringup();
	xpon_sta  = IO_GPHYREG(PHY_CSR_XPON_STA);	/* direct KSEG1 MMIO (MT7520) */
	rx_status = IO_GPHYREG(PHY_CSR_RX_STATUS);
	physta1   = IO_GPHYREG(PHY_CSR_PHYSTA1);
	los = (xpon_sta & PHYLOS_STATUS) ? 1 : 0;

	/* RX signal-detect pin polarity probe (laser-OFF: only bit6 of the RX cfg
	 * register, restored after). The BOSA indicates via either an RX_SD pin
	 * (high=signal) or an RX_LOS pin (high=loss); PHY_RX_SD_INV (bit6) selects
	 * how the PHY latches it. The vendor sets this per-board (phy_trans_rx_setting),
	 * which our path skips. Read LOS under both so a fiber test shows which
	 * polarity locks (correct one => los 0 when light present). If both stay 1
	 * regardless, the PHY isn't reading the sigdet pin (deeper RX-bringup gap). */
	setting = IO_GPHYREG(PHY_CSR_XPON_SETTING);
	IO_SPHYREG(PHY_CSR_XPON_SETTING, setting | PHY_RX_SD_INV);	/* RX_LOS pin def */
	udelay(200);
	los_rxlos = (IO_GPHYREG(PHY_CSR_XPON_STA) & PHYLOS_STATUS) ? 1 : 0;
	IO_SPHYREG(PHY_CSR_XPON_SETTING, setting & ~PHY_RX_SD_INV);	/* RX_SD pin def */
	udelay(200);
	los_rxsd = (IO_GPHYREG(PHY_CSR_XPON_STA) & PHYLOS_STATUS) ? 1 : 0;
	IO_SPHYREG(PHY_CSR_XPON_SETTING, setting);			/* restore */

	/* Stage 1: read the transceiver DDM (SFF-8472) over the real SIF I2C.
	 * rx_power is the DEFINITIVE "is light hitting the BOSA" measurement that the
	 * stuck sigdet bit can't give us. temp/vcc serve as a SIF-alive sanity check. */
	temp  = xpon_ddm_rd16(0x60);	/* 1/256 C, signed */
	volt  = xpon_ddm_rd16(0x62);	/* 100uV units */
	txpwr = xpon_ddm_rd16(0x66);	/* 0.1uW units */
	rxpwr = xpon_ddm_rd16(0x68);	/* 0.1uW units */
	sif_ok = (temp >= 0 && volt >= 0 && rxpwr >= 0);
	temp_c = sif_ok ? (int)(short)temp / 256 : 0;

	n = scnprintf(k, sizeof(k),
		"los %d (%s)\n"
		"xpon_sta 0x%08x  rx_sync 0x%x (%s)  phy_rdy 0x%x  phy_rx_up %d\n"
		"xpon_setting 0x%08x (rx_sd_inv bit6=%d)\n"
		"sigdet probe: RX_LOS-pol los=%d  RX_SD-pol los=%d  (%s)\n"
		"ddm[SIF]: %s rx_power=0x%04x (%d.%01d uW)  tx_power=0x%04x  temp=%dC  vcc=%d.%03dV\n"
		"verdict: %s\n",
		los, los ? "NO SIGNAL (LOS / no OLT light)"
			 : "SIGNAL PRESENT (OLT downstream detected)",
		xpon_sta, rx_status & 0xf,
		(rx_status & 0xf) == 0xa ? "RX in sync" : "not synced",
		(physta1 >> 18) & 0x7, xpon_phy_rx_up,
		setting, !!(setting & PHY_RX_SD_INV),
		los_rxlos, los_rxsd,
		(los_rxlos != los_rxsd) ? "polarity changes reading => sigdet pin IS read"
					: "both equal => sigdet pin not toggling (no light, or not read)",
		sif_ok ? "OK" : "READ-FAIL",
		rxpwr & 0xffff, sif_ok ? rxpwr / 10 : 0, sif_ok ? rxpwr % 10 : 0,
		txpwr & 0xffff, temp_c, sif_ok ? volt / 10000 : 0, sif_ok ? (volt % 10000) / 10 : 0,
		!sif_ok ? "SIF DDM read FAILED (controller/transaction issue - see sif_port.c)" :
		(rxpwr > 0 && rxpwr != 0xffff) ? "OPTICAL LIGHT PRESENT at BOSA (rx_power > 0)" :
		"DDM alive (temp/vcc sane) but rx_power ~0 => NO light reaching BOSA");
	return simple_read_from_buffer(b, l, o, k, n);
}

/* ── Adaptive laser bias/mod SERVO (ported from H660 stock en7571_internal_clock, 2026-07-05) ──
 * The H660(DZS) stock — which reaches O5 — runs a 1Hz kthread that continuously re-tracks the laser
 * Ibias/Imod against temperature via the per-temperature LUT. Our port applied them ONCE at bring-up
 * and never re-tracked (session-9 even DISABLED the port's own broken servo). This replicates the
 * stock loop: every servo_ms, refresh temperature + mt7570_LUT_tracking_step(). OFF by default;
 * start/stop with `echo s > /proc/econet_xpon_los`. */
int servo_ms = 0;          module_param(servo_ms, int, 0644);
/* servo_force: 1 = FORCE-HOLD mode (pin Ibias=tx_ibias, Imod=tx_imod every period so NOTHING can
 * revert/collapse them — used to isolate the under-modulation test: hold Ibias=0x7ce known-good +
 * Imod=0x44d LUT-calibrated). 0 = adaptive LUT tracking (temp -> LUT). Default force. */
int servo_force = 1;       module_param(servo_force, int, 0644);
/* apc_target: SOFTWARE APC (2026-07-08). This session's forced-re-range capture proved STOCK's DCL servo
 * RAMPS the laser bias (0x13a->0x194) to HOLD MPD at ~0x106 vs thermal droop; the PORT runs open-loop so its
 * power droops. When apc_target>0, the servo kthread reads MPD (mt7570_MPD_current) every period and nudges
 * Ibias (SIF 0x138, via en7571_set_ibias) toward apc_target (bounded apc_biasmin..apc_biasmax; the MPD-vs-Ibias
 * curve peaks ~0x900 so cap below rolloff). Overrides servo_force/LUT. Start/stop with `echo s`. */
int apc_target  = 0;       module_param(apc_target, int, 0644);
int apc_step    = 4;       module_param(apc_step, int, 0644);   /* stock en7571_Pavg_close_loop integrates Iav +/-4/tick */
int apc_biasmax = 0x900;   module_param(apc_biasmax, int, 0644);
int apc_biasmin = 0x500;   module_param(apc_biasmin, int, 0644);
static int apc_bias = 0, apc_cnt = 0;
extern int mt7570_MPD_current(void);
static struct task_struct *en7571_servo_task;
extern void mt7570_temperature_get(void);
extern void mt7570_LUT_recover(void);
extern uint LUT_Ibias_Imod[64][2];
static int en7571_servo_fn(void *arg)
{
	int p = (servo_ms > 0) ? servo_ms : (servo_force ? 200 : 1000);
	(void)arg;
	printk(KERN_DEBUG "servo: START mode=%s period=%dms (Ibias=0x%x Imod=0x%x)\n",
		servo_force ? "FORCE-HOLD" : "LUT-track", p, tx_ibias & 0xfff, tx_imod & 0xfff);
	while (!kthread_should_stop()) {
		if (apc_target > 0) {                            /* ★ SOFTWARE APC: hold MPD at apc_target */
			int mpd = mt7570_MPD_current();
			if (apc_bias == 0) apc_bias = tx_ibias & 0xfff;   /* init from seed */
			if (mpd < apc_target) { apc_bias += apc_step; if (apc_bias > apc_biasmax) apc_bias = apc_biasmax; }
			else if (mpd > apc_target) { apc_bias -= apc_step; if (apc_bias < apc_biasmin) apc_bias = apc_biasmin; }
			en7571_set_ibias((u16)(apc_bias & 0xfff));
			en7571_set_imod((u16)(tx_imod & 0xfff));      /* keep Imod pinned to cal */
			if ((apc_cnt++ % 8) == 0)
				printk(KERN_DEBUG "APC: mpd=0x%x target=0x%x -> Ibias=0x%x\n", mpd, apc_target, apc_bias);
		} else if (servo_force) {
			en7571_set_ibias((u16)(tx_ibias & 0xfff));   /* pin Ibias (nothing overrides) */
			en7571_set_imod((u16)(tx_imod & 0xfff));      /* pin Imod   (nothing overrides) */
		} else {
			mt7570_temperature_get();      /* refresh BOSA_temperature (stock: internal_DDMI) */
			mt7570_LUT_tracking_step();    /* temp -> LUT -> Ibias(0x138)/Imod(0x148) */
		}
		msleep(p);
	}
	printk(KERN_DEBUG "servo: STOP\n");
	return 0;
}
void en7571_servo_stop(void)   /* called from pon_phy_deinit so rmmod can't oops on a live kthread */
{
	if (en7571_servo_task) { kthread_stop(en7571_servo_task); en7571_servo_task = NULL; }
}

static ssize_t xpon_los_write(struct file *f, const char __user *b, size_t l, loff_t *o)
{
	extern void sif_i2c_scan(unsigned char ch);
	char kb[16]; char c = 0; int kl;
	(void)f; (void)o;
	kl = (l < sizeof(kb) - 1) ? (int)l : (int)sizeof(kb) - 1;
	if (kl > 0 && copy_from_user(kb, b, kl) == 0) { kb[kl] = 0; c = kb[0]; }
	/* Ensure the PHY global priv exists before any bring-up/diag command derefs it.
	 * A fresh xpon_hw=1 insmod can run this proc before the daemon's pon_phy_init
	 * allocates gpPhyPriv -> NULL deref in phy_dev_init / phy_I2C_*_translet (crash at
	 * offset 0x54/0x88). Allocate + set the SIF clk-div here (mirrors xpon_phy_rx_bringup). */
	if (!gpPhyPriv) {
		gpPhyPriv = (PHY_GlbPriv_T *)kzalloc(sizeof(PHY_GlbPriv_T), GFP_KERNEL);
		if (gpPhyPriv)
			gpPhyPriv->i2c_u2_clk_div = I2C_U2_CLK_DIV;
	}
	if (c == 'y') {		/* ★ run the 407-stock TxSD comparator calibration (en7571_txsd_level_set) that the
				 * port skips. Run AFTER l (laser up). Watch /sys txsd_fail_cnt before/after. */
		en7571_txsd_level_set();
		return l;
	}
	if (c == 't') {		/* ★ toggle TX TDC holdover (stock phy_tx_tdc_holdover) — hold the burst timing
				 * reference to kill burst-edge jitter. `echo t` = on, `echo t0` = off. */
		extern void phy_tx_tdc_holdover(int on);
		phy_tx_tdc_holdover(kb[1] == '0' ? 0 : 1);
		return l;
	}
	if (c == 'o') {		/* ★ toggle ERC open-loop DAC-follow (0x13C/0x14C[1:0]) at runtime — the RE'd fix.
				 * `echo o` = on (0b10, couple DAC->laser); `echo o0` = off (0b00). Lets us confirm
				 * the coupling unfreezes 0x13C-read/MPD without a re-range. */
		extern void en7571_openloop_mode(int on);
		en7571_openloop_mode(kb[1] == '0' ? 0 : 1);
		return l;
	}
	if (c == 's') {		/* ★ start/stop the adaptive LUT bias/mod SERVO (ported H660 stock
				 * en7571_internal_clock + tuneBiasModCurrent). First LUT_recover + report LUT
				 * validity; start the 1Hz kthread only if the LUT is populated. echo s again to
				 * stop. Auto-stopped on rmmod via en7571_servo_stop(). */
		int i, valid = 0;
		if (en7571_servo_task) {
			kthread_stop(en7571_servo_task); en7571_servo_task = NULL;
			printk(KERN_DEBUG "s: servo STOPPED (LDDLA reverter stays stopped until reload)\n");
			return l;
		}
		/* Stop the port's own LDDLA kthread (mt7570_internal_clock @1Hz) — it re-seeds/collapses
		 * the DACs (OpenLoopControl/BiasTracking) and is what reverted Imod 0x44d->0x157. */
		if (gpPhyPriv && !IS_ERR_OR_NULL(gpPhyPriv->LDDLA_task_wait)) {
			kthread_stop(gpPhyPriv->LDDLA_task_wait);
			gpPhyPriv->LDDLA_task_wait = ERR_PTR(-ESHUTDOWN);   /* mark stopped so deinit skips it */
			printk(KERN_DEBUG "s: stopped LDDLA (mt7570_internal_clock) DAC reverter\n");
		}
		if (!servo_force) {
			mt7570_LUT_recover();          /* populate LUT_Ibias_Imod[] from flash (0xa0+) */
			for (i = 0; i < 64; i++)
				if (LUT_Ibias_Imod[i][0] != 0xfff && LUT_Ibias_Imod[i][1] != 0xfff) valid++;
			printk(KERN_DEBUG "s: LUT_recover -> %d/64 valid; idx26(~25C) Ibias=0x%x Imod=0x%x\n",
				valid, LUT_Ibias_Imod[26][0], LUT_Ibias_Imod[26][1]);
		}
		en7571_servo_task = kthread_run(en7571_servo_fn, NULL, "en7571_servo");
		printk(KERN_DEBUG "s: servo STARTED (mode=%s Ibias=0x%x Imod=0x%x)\n",
			servo_force ? "FORCE-HOLD" : "LUT-track", tx_ibias & 0xfff, tx_imod & 0xfff);
		return l;
	}
	/* "echo dXX > ..." => program APD bias DAC 0xXX then measure rx_raw/LOS.
	 * Ramp control for finding the minimum working bias (see en7571_apd_measure). */
	if (c == 'd') {
		unsigned long code = simple_strtoul(kb + 1, NULL, 16) & 0xff;
		en7571_apd_measure((u8)code);
		return l;
	}
	if (c == 'x') {			/* disable APD front-end (safety off) */
		en7571_apd_off();
		return l;
	}
	if (c == 'r') {			/* pure RX read (no APD re-enable) */
		en7571_pure_read();
		return l;
	}
	if (c == 'I') {			/* one-time EN7571 front-end init */
		en7571_front_end_init();
		return l;
	}
	if (c == 'R') {			/* REAL PHY GPON bring-up (Agent B): dev_init -> mode_config(GPON) -> fw_ready -> start.
				 * gpon_dev_reset SKIPS phy_mode_config on EN7521 (only_reset_mac=1), so the
				 * GPON-mode bit (PHYSET10|=PHY_GPON_MODE) was never actually set. Do it directly. */
		extern int phy_mode_config(Xpon_Phy_Mode_t, int);
		extern int phy_fw_ready(unchar);
		extern int xpon_phy_start(void);
		printk(KERN_DEBUG "R: phy_dev_init...\n"); mdelay(4);
		phy_dev_init();
		printk(KERN_DEBUG "R: phy_mode_config(GPON) -> PHYSET10|=GPON_MODE...\n"); mdelay(4);
		phy_mode_config(PHY_GPON_CONFIG, PHY_ENABLE);
		/* XPON_SETTING(BASE_1+0x38) GPON datapath cfg. Value = 0x10F for the en7571 INTEGRATED
		 * front-end (stock en7571_init@0x2a78c writes 0x10f). The generic phy_mode_config 0x14F is
		 * board-strapped (skipped when *0xbfb00284 & 0x200 == 0, which is the case here) -> the live
		 * value is en7571's 0x10f. bit6 = PHY_RX_SD_INV: 0x14F left it SET (INVERTED RX signal-detect)
		 * so the PHY read "light present" as "loss of signal" -> XPON_STA PHYLOS stuck 1, no sync.
		 * Write via raw MMIO (single write, safe) so the bit6-clear actually lands. */
		IO_SREG(0xbfaf0138u, 0x0000010Fu);
		printk(KERN_DEBUG "R: XPON_SETTING=0x10F (bit6 RX_SD_INV clear) + phy_fw_ready(EN) + xpon_phy_start()...\n"); mdelay(4);
		phy_fw_ready(PHY_ENABLE);
		xpon_phy_start();
		/* Optical RX datapath: power the en7571 front-end (FE + APD 0x60) then apply the
		 * HS-RX tail (reg_init + force_mode + burst_ctrl + DCL_start @en7571_reg_init_full)
		 * so the recovered clock/data reaches the digital SerDes. Order matches the tested
		 * R->I->d60->X. On a fiber-calibrated unit this completes RX bring-up to frame-lock;
		 * on THIS uncalibrated ex-mesh 407 the datapath engages (PHYSTA1 0xf1919->0x71919)
		 * but the CDR can't lock the eye (no per-unit optical cal). Laser stays OFF. */
		printk(KERN_DEBUG "R: en7571 optical FE + HS-RX datapath enable...\n"); mdelay(4);
		en7571_optical_bringup();
		en7571_reg_init_full();
		/* ★ CDR ARM + NCPO/DDS FREQUENCY SEED — the missed step (stock phy_los_handler@0x22798 GPON
		 * arm; the whole GPON arm/finalize block is ABSENT from the GPL src the port was built on).
		 * Seed the DDS freq word so the recovered clock acquires at ~GPON 2.488G; the loop then
		 * PULLS ANASET14(0x194) to the locked value (CONFIRMED on locked stock: ANADDS1(0x1d8)=
		 * 0x17E21965, ANASET14(0x194)=0x1E1A9FBE). Done here AFTER the HS-RX datapath + light present
		 * (XPON_STA=0), matching stock's LOS-deassert arm timing. */
		{
			uint p;
			/* stock phy_los_handler@0x22798 preamble (steps 1-4), then the GPON arm (steps 6-10) */
			IO_SREG(0xbfaf05f4u, IO_GREG(0xbfaf05f8u));                    /* clear XPON int (05f8->05f4) */
			phy_bit_delay(0);
			phy_reset_counter();
			p = IO_GREG(0xbfaf010cu); IO_SREG(0xbfaf010cu, p & ~0x100u);   /* PHYSET4 &= ~0x100 (bit8=0) */
			{ extern unsigned int ncpo_val; IO_SREG(0xbfaf0194u, ncpo_val); } /* ANASET14 = NCPO seed (ncpo_val) */
			IO_SREG(0xbfaf01d8u, 0x17E21965u);                            /* ANADDS1  = NCPO/DDS seed */
			p = IO_GREG(0xbfaf01f0u); IO_SREG(0xbfaf01f0u, p | 0x20u);     /* TDCSET1 |= 0x20 (bit5=1) */
			p = IO_GREG(0xbfaf0104u); IO_SREG(0xbfaf0104u, p | 0x10u);     /* PHYSET2 |= 0x10 (bit4=1) */
		}
		printk(KERN_DEBUG "R: phy_los_handler ARM (preamble + NCPO seed 0x17E21965 + acq bits)\n"); mdelay(4);
		/* ★★★ STOCK CONFIG DIFFS + FINALIZE — the final lock gate (found 2026-07-03 via register-diff
		 * vs locked stock on the calibrated main). These digital-PHY regs stock sets that the port left
		 * default; with them + the tracking-mode finalize, PHYSTA1 rdy->6 and RXSTAT RX_SYNC->0xa = LOCK. */
		if (wmask & 0x01) IO_SREG(0xbfaf0110u, 0x00a0e004u);   /* PHYSET5 (bits23:16=0xa0) */
		if (wmask & 0x02) IO_SREG(0xbfaf012cu, 0x00407f7fu);   /* 0x12c */
		if (wmask & 0x04) IO_SREG(0xbfaf0150u, 0x0000e000u | ((uint)po_anapwd & 0x180u));   /* ANAPWD (|0x180) */
		if (wmask & 0x08) { uint p2 = IO_GREG(0xbfaf01a0u); IO_SREG(0xbfaf01a0u, p2 & ~0x90000u); }  /* ANATXREG1 */
		if (wmask & 0x10) IO_SREG(0xbfaf01e0u, 0x00003c05u);   /* ANADDS3 (NCPO probe-select) */
		if (wmask & 0x20) IO_SREG(0xbfaf01f8u, 0x0000002du);   /* TDCSET2 = 45 */
		if (wmask & 0x40) IO_SREG(0xbfaf0228u, 0x00070000u);   /* 0x228 RX-path */
		if (wmask & 0x80) IO_SREG(0xbfaf0230u, 0x00000007u);   /* 0x230 RX-path */
		mdelay(5);
		/* FINALIZE -> tracking: PHYSET4.8=1, TDCSET1.5=0, PHYSET2 -> 0x..01 (b4=0,b0=1) */
		{ uint p2, ph; p2=IO_GREG(0xbfaf010cu); IO_SREG(0xbfaf010cu, p2|0x100u);
		  p2=IO_GREG(0xbfaf01f0u); IO_SREG(0xbfaf01f0u, p2&~0x20u);
		  ph=IO_GREG(0xbfaf0104u) & ~0x10u; if (fin_b0) ph|=0x1u; else ph&=~0x1u;
		  IO_SREG(0xbfaf0104u, ph); }
		printk(KERN_DEBUG "R: stock config diffs + FINALIZE applied\n"); mdelay(4);
		printk(KERN_DEBUG "R: GPON PHY bring-up done (digital + optical RX datapath + CDR LOCK)\n"); mdelay(4);
		return l;
	}
	if (c == 'F') {			/* NEVER-RUN vendor RX-only front-end conditioning. This is an
				 * uncalibrated ex-mesh 407: no /tmp/7570_bob.conf, flash_matrix=0 -> every
				 * APD/cal gate (get_flash_register()>0xffff) silently FAILS, so the custom
				 * bring-up never ran TIA-gain / ADC+RSSI self-cal / APD-bias. Seed flash_matrix
				 * to the vendor defaults (0xffffffff) + force the GPON magic so the gates pass,
				 * then run the RX-side conditioning from mt7570_init's GPON branch. DELIBERATELY
				 * SKIPS every TX step (TGEN / laser I_bias load_init_current / MPDL-MPDH / TxSD)
				 * -> laser stays OFF, no rogue-ONU risk on the shared PON. Run AFTER R (digital
				 * GPON mode already set). Goal: PHYSTA1[20:18]=6 (RX CDR lock). */
		int i;
		printk(KERN_DEBUG "F: flash_matrix defaults + force GPON magic[0x94]=0x07050700...\n"); mdelay(4);
		set_flash_register_default();
		set_flash_register(0x07050700, flash_magic_number);
		printk(KERN_DEBUG "F: TIAGAIN_set + ERC_filter (RX front-end gain)...\n"); mdelay(4);
		mt7570_TIAGAIN_set();   mdelay(2);
		mt7570_ERC_filter();    mdelay(2);
		printk(KERN_DEBUG "F: ADC_calibration + RSSI_calibration (live self-cal baselines)...\n"); mdelay(4);
		mt7570_ADC_calibration();   mdelay(2);
		mt7570_RSSI_calibration();  mdelay(2);
		printk(KERN_DEBUG "F: RSSI_gain_init + LOS_level_set + XPON_SETTING=0x10F...\n"); mdelay(4);
		mt7570_RSSI_gain_init();
		mt7570_LOS_level_set();
		IO_SREG(0xbfaf0138u, 0x0000010Fu);
		printk(KERN_DEBUG "F: APD_initialization + APD_control (RX photodiode bias, computed)...\n"); mdelay(4);
		mt7570_APD_initialization();
		mt7570_APD_control();
		printk(KERN_DEBUG "F: safe_circuit_reset (digital CDR reset) + re-engage HS-RX datapath...\n"); mdelay(4);
		mt7570_safe_circuit_reset();
		en7571_reg_init_full();  mdelay(5);
		for (i = 0; i < 6; i++) {
			printk(KERN_DEBUG "F%d: PHYSTA1=%08x RXSTAT=%08x XPON_STA=%08x ANACAL1=%08x TDCSTA1=%08x\n",
				i, IO_GPHYREG(PHY_CSR_PHYSTA1), IO_GPHYREG(PHY_CSR_RX_STATUS),
				IO_GPHYREG(PHY_CSR_XPON_STA), regRead32(0xbfaf0140u), regRead32(0xbfaf01f4u));
			mdelay(250);
		}
		printk(KERN_DEBUG "F: done (want PHYSTA1[20:18]=6 / RXSTAT low-nibble=0xa for CDR lock)\n");
		return l;
	}
	if (c == 'L') {			/* AUTO-LOCK long-settle. Run AFTER R (R now applies the FIXED
				 * en7571_reg_init_full: auto-lock 0x25B[1:0]=00 + safe_circuit_reset +
				 * CDR-lock-to-data PHYSET1 bit24=0). Give the CDR a long uninterrupted
				 * window, re-strobing en7571 safe_circuit_reset (reg 0x101 bit6) each round
				 * to (re)acquire — mirrors the stock re-acquire workqueue. Watch [20:18]->6. */
		u8 b[4]; int i; uint ps; int locked = 0;
		printk(KERN_DEBUG "L: auto-lock long-settle (30 rounds x 300ms, re-strobe safe_circuit_reset)\n"); mdelay(4);
		for (i = 0; i < 30; i++) {
			if (!e7_rd(0x100,b,4)) { b[1]|=0x40; e7_wr(0x100,b,4); }  /* re-strobe safe_circuit_reset */
			mdelay(300);
			ps = IO_GPHYREG(PHY_CSR_PHYSTA1);
			printk(KERN_DEBUG "L%d: PHYSTA1=%08x rdy[20:18]=%u RXSTAT=%08x XPON_STA=%08x\n",
				i, ps, (ps >> 18) & 0x7, IO_GPHYREG(PHY_CSR_RX_STATUS), IO_GPHYREG(PHY_CSR_XPON_STA));
			if (((ps >> 18) & 0x7) == 6) { locked = 1; printk(KERN_DEBUG "L: *** CDR LOCKED (rdy=6) at round %d ***\n", i); break; }
		}
		if (!locked) printk(KERN_DEBUG "L: no lock after 30 rounds (rdy never reached 6)\n");
		return l;
	}
	if (c == 'o') {			/* ★ COMPLETE register dump for stock-vs-port diff: en7571 I2C-0x70 0x00-0x300
					 * + MMIO PHY 0xbfaf 0x100-0x600, into g_regdump (NO printk flood!).
					 * Retrieve over LAN: cat /proc/econet_xpon_regdump. */
		u32 r; u8 b[4]; int p = 0;
		p += scnprintf(g_regdump+p, sizeof(g_regdump)-p, "===EN7571-070===\n");
		for (r = 0; r <= 0x300; r += 4) {
			if (e7_rd(r, b, 4)) p += scnprintf(g_regdump+p, sizeof(g_regdump)-p, "e7[%03x]=xxxxxxxx\n", r);
			else p += scnprintf(g_regdump+p, sizeof(g_regdump)-p, "e7[%03x]=%02x%02x%02x%02x\n", r, b[0],b[1],b[2],b[3]);
		}
		p += scnprintf(g_regdump+p, sizeof(g_regdump)-p, "===MMIO-BFAF===\n");
		for (r = 0x100; r <= 0x600; r += 4)
			p += scnprintf(g_regdump+p, sizeof(g_regdump)-p, "mm[%03x]=%08x\n", r, (uint)IO_GREG(0xbfaf0000u + r));
		p += scnprintf(g_regdump+p, sizeof(g_regdump)-p, "===END===\n");
		g_regdump_len = p;
		printk(KERN_DEBUG "o: regdump ready %d bytes -> cat /proc/econet_xpon_regdump\n", p);  /* ONE line only */
		return l;
	}
	if (c == 'l') {			/* ★ manual TX laser bring-up: seed DACs + apc_bringup (calibrated 0x7ce/0x157
					 * + burst timing) + FORCE/KT (if tx_force/tx_kt) + DCL close (if tx_closeloop).
					 * The automatic path (gpon_do_lock finalize) doesn't run it once already CDR-locked. */
		extern void en7571_tx_laser_bringup(void);
		printk(KERN_DEBUG "l: manual en7571_tx_laser_bringup()\n");
		en7571_tx_laser_bringup();
		return l;
	}
	if (c == 'u') {			/* READ-ONLY en7571 TX-loop state dump (force/KT experiment). Modifies nothing. */
		u8 b[4];
		u16 ibias=0, imod=0, frc=0, kt=0, dcl=0, pav=0, p1=0, iav=0;
		if (!e7_rd(0x138,b,4)) ibias = ((b[1]&0xf)<<8)|b[0];
		if (!e7_rd(0x148,b,4)) imod  = ((b[1]&0xf)<<8)|b[0];
		if (!e7_rd(0x258,b,4)) frc   = b[3]&0x3;
		if (!e7_rd(0x230,b,4)) kt    = b[0]&0x1;
		if (!e7_rd(0x228,b,4)) dcl   = b[1]&0x1;
		if (!e7_rd(0x2a4,b,4)) { pav=((b[1]&0xf)<<8)|b[0]; p1=((b[3]&0xf)<<8)|b[2]; }
		if (!e7_rd(0x2b0,b,4)) iav   = ((b[1]&0xf)<<8)|b[0];
		printk(KERN_DEBUG "u: Ibias=0x%03x Imod=0x%03x | force=%u KT=%u DCL=%u | Pav_now=0x%03x P1_now=0x%03x Iav_now=0x%03x | rdy=%u act_st=O%u\n",
			ibias, imod, frc, kt, dcl, pav, p1, iav,
			(IO_GPHYREG(PHY_CSR_PHYSTA1)>>18)&7, (uint)(IO_GREG(0xbfb640bcu)&0xf));
		/* ★ 2026-07-08 unit-matched vs stock /proc/pon_phy: read the MEASURED currents via the SAME fns
		 * stock's phy_read_proc uses (mt7570_information_output / MPD / RSSI). Stock@O5 was Bias~0x1a9
		 * Mod~0x813 MPD~0x109 (actively servoed). These are the current-sense (CS3) ADC readbacks, NOT the
		 * 0x138/0x148 DACs above. */
		{ int bc=mt7570_information_output(Ibias), mc=mt7570_information_output(Imod);
		  int mpd=mt7570_MPD_current(), rssi=mt7570_RSSI_current();
		  int tc=mt7570_ADC_temperature_get(), vc=mt7570_ADC_voltage_get();
		  printk(KERN_DEBUG "STOCKCMP: BiasCurrent=0x%x ModulationCurrent=0x%x MPDCurrent=0x%x RSSI=0x%x Temp=0x%x VCC=0x%x | stock@O5: Bias~0x1a9 Mod~0x813 MPD~0x109\n",
		    bc, mc, mpd, rssi, tc, vc); }
		return l;
	}
	if (c == 'V') {			/* VERIFY the auto-lock/CDR writes actually landed on the BOSA (dev 0x70)
				 * and the digital PHY, and read the SIF-read status. Distinguishes "write
				 * didn't land" from "landed but no lock". Then re-assert auto-lock and read back. */
		u8 b[4]; int rc;
		rc = e7_rd(0x258,b,4);
		printk(KERN_DEBUG "V: BOSA[0x258..0x25B] rc=%d -> %02x %02x %02x %02x  (0x25B[1:0]=%u: 0=auto 3=force)\n",
			rc, b[0], b[1], b[2], b[3], b[3] & 0x3);
		rc = e7_rd(0x228,b,4);
		printk(KERN_DEBUG "V: BOSA[0x228..0x22B] rc=%d -> %02x %02x %02x %02x  (0x229[0]=DCL_start=%u)\n",
			rc, b[0], b[1], b[2], b[3], b[1] & 0x1);
		rc = e7_rd(0x14,b,4);
		printk(KERN_DEBUG "V: BOSA[0x14..0x17]   rc=%d -> %02x %02x %02x %02x  (0x16[2:0]=gain=%u)\n",
			rc, b[0], b[1], b[2], b[3], b[2] & 0x7);
		rc = e7_rd(0x100,b,4);
		printk(KERN_DEBUG "V: BOSA[0x100..0x103] rc=%d -> %02x %02x %02x %02x  (0x101[6]=safe_rst=%u)\n",
			rc, b[0], b[1], b[2], b[3], (b[1] >> 6) & 0x1);
		printk(KERN_DEBUG "V: PHYSET1(0xbfaf0100)=%08x bit24(lock2ref)=%u  PHYSTA1=%08x rdy=%u\n",
			IO_GPHYREG(PHY_CSR_PHYSET1), (IO_GPHYREG(PHY_CSR_PHYSET1) >> 24) & 1,
			IO_GPHYREG(PHY_CSR_PHYSTA1), (IO_GPHYREG(PHY_CSR_PHYSTA1) >> 18) & 7);
		/* re-assert auto-lock explicitly and read back to confirm it sticks */
		if (!e7_rd(0x258,b,4)) { b[3] &= ~0x03; e7_wr(0x258,b,4); }
		if (!e7_rd(0x258,b,4))
			printk(KERN_DEBUG "V: after re-clear, 0x25B=%02x [1:0]=%u\n", b[3], b[3] & 0x3);
		return l;
	}
	if (c == 'G') {			/* MAC-side GPON activation (Agent B): xmcs_set_connection_start(ENABLE)
				 * -> working_mode UNKNOWN triggers auto-detect (check_sync loop) -> on is_sync
				 * -> working_mode=GPON -> prepare_gpon -> act SM -> gpon_enable -> PLOAM O1..O5. */
		extern int xmcs_set_connection_start(int);
		printk(KERN_DEBUG "G: xmcs_set_connection_start(XPON_ENABLE=1)...\n"); mdelay(4);
		xmcs_set_connection_start(1);   /* XPON_ENABLE */
		printk(KERN_DEBUG "G: connection_start returned, NO CRASH\n"); mdelay(4);
		return l;
	}
	if (c == 'S') {			/* PHY sync/ready diagnostic + kick the event poll */
		extern int is_phy_sync(void);
		extern int phy_ready_status(void);
		extern int get_phy_mode(void);
		printk(KERN_DEBUG "S: is_sync=%d ready_status=%d mode=%d phy_status=%d\n",
			is_phy_sync(), phy_ready_status(), get_phy_mode(),
			gpPhyPriv ? gpPhyPriv->phy_status : -99);
		if (gpPhyPriv)
			mod_timer(&gpPhyPriv->event_poll_timer, jiffies + msecs_to_jiffies(100));
		return l;
	}
	if (c == 'T') {			/* SIF controller diagnostic: raw regs + bus scan */
		extern void sif_port_init(void);
		extern void sif_i2c_scan(unsigned char ch);
		printk(KERN_DEBUG "T: SIF ch0 base 0xbfbf8000: CFG=%08x CMD=%08x CFG2=%08x STAT=%08x GO=%08x\n",
			regRead32(0xbfbf8040), regRead32(0xbfbf8044), regRead32(0xbfbf805c),
			regRead32(0xbfbf8060), regRead32(0xbfbf8064));
		sif_port_init();
		sif_i2c_scan(0);
		return l;
	}
	if (c == 'U') {			/* dump PHY RX/sync status regs (3x for read-stability) */
		int i;
		for (i = 0; i < 3; i++)
			printk(KERN_DEBUG "U%d: PHYSTA1=%08x RXSTAT=%08x PSYNC_CTL=%08x LOF_CNT=%08x XPON_STA=%08x\n",
				i, IO_GPHYREG(PHY_CSR_PHYSTA1), IO_GPHYREG(PHY_CSR_RX_STATUS),
				IO_GPHYREG(PHY_CSR_GPON_PSYNC_CTL), IO_GPHYREG(PHY_CSR_LOF_CNT),
				IO_GPHYREG(PHY_CSR_XPON_STA));
		return l;
	}
	if (c == 'W') {			/* characterize write path: writable-bit mask + consistency */
		uint b = IO_GPHYREG(PHY_CSR_GPON_PSYNC_CTL);
		uint rf, r0, rf2;
		IO_SPHYREG(PHY_CSR_GPON_PSYNC_CTL, 0xffffffffu); rf  = IO_GPHYREG(PHY_CSR_GPON_PSYNC_CTL);
		IO_SPHYREG(PHY_CSR_GPON_PSYNC_CTL, 0x00000000u); r0  = IO_GPHYREG(PHY_CSR_GPON_PSYNC_CTL);
		IO_SPHYREG(PHY_CSR_GPON_PSYNC_CTL, 0xffffffffu); rf2 = IO_GPHYREG(PHY_CSR_GPON_PSYNC_CTL);
		IO_SPHYREG(PHY_CSR_GPON_PSYNC_CTL, b);   /* restore */
		printk(KERN_DEBUG "W: PSYNC_CTL orig=%08x  wrFF->%08x  wr00->%08x  wrFF->%08x  (rf==rf2? %s)\n",
			b, rf, r0, rf2, (rf == rf2) ? "consistent" : "RANDOM");
		return l;
	}
	if (c == 'J') {			/* RX tuning sweep: XPON_SETTING(BASE_1+0x38) SD-polarity bit6 = rx_sd_inv;
				 * RX-imp manual code = rx_imp_code (if >=0). Then re-check sync via S. */
		uint rd = IO_GPHYREG(REG_BASE_1 + 0x0038);
		if (rx_sd_inv) rd |= 0x40u; else rd &= ~0x40u;
		IO_SPHYREG(REG_BASE_1 + 0x0038, rd);
		if (rx_imp_code >= 0) {
			rd = IO_GPHYREG(PHY_CSR_PHYSET4);
			IO_SPHYREG(PHY_CSR_PHYSET4, rd & ~0x2u);
			rd = IO_GPHYREG(PHY_CSR_PHYSET6);
			IO_SPHYREG(PHY_CSR_PHYSET6, (rd & ~0x1F00u) | (((uint)(rx_imp_code) & 0x1f) << 8));
		}
		printk(KERN_DEBUG "J: rx_sd_inv=%d rx_imp_code=%d applied\n", rx_sd_inv, rx_imp_code);
		return l;
	}
	if (c == 'K') {			/* MMIO-vs-I2C PHY-CSR read compare: does MMIO 0xbfaf01xx mirror the I2C@0x70 PHY? */
		printk(KERN_DEBUG "K: PHYSTA1 mmio(0xbfaf0130)=%08x i2c[0x130]=%08x | RXSTAT mmio(0xbfaf021c)=%08x i2c[0x21c]=%08x | PHYSET10 mmio(0xbfaf0124)=%08x i2c[0x124]=%08x\n",
			regRead32(0xbfaf0130), IO_GPHYREG(REG_BASE_1 + 0x30),
			regRead32(0xbfaf021c), IO_GPHYREG(REG_BASE_2 + 0x1c),
			regRead32(0xbfaf0124), IO_GPHYREG(REG_BASE_1 + 0x24));
		return l;
	}
	if (c == 'C') {			/* trigger GPON analog cal (PLL-reset pulse) + read cal outputs with settle delay.
				 * phy_calibration_test shows PLL_RST|COUNT_RST pulse triggers VCO/PI/imp/SQTH cal. */
		uint rd, a1, a2, td;
		rd = IO_GPHYREG(PHY_CSR_PHYSET10); IO_SPHYREG(PHY_CSR_PHYSET10, rd | PHY_GPON_MODE); /* GPON mode */
		rd = IO_GPHYREG(PHY_CSR_PHYSET3);
		IO_SPHYREG(PHY_CSR_PHYSET3, rd | PHY_PLL_RST | PHY_COUNT_RST);   /* assert cal reset */
		mdelay(2);
		IO_SPHYREG(PHY_CSR_PHYSET3, rd);                                /* release -> cal runs */
		mdelay(rx_imp_code >= 0 ? rx_imp_code : 5);                     /* settle (rx_imp_code repurposed as ms) */
		a1 = IO_GPHYREG(PHY_CSR_ANACAL1);
		a2 = IO_GPHYREG(PHY_CSR_ANACAL2);
		td = IO_GPHYREG(PHY_CSR_TDCSTA1);
		printk(KERN_DEBUG "C: settle=%dms ANACAL1=%08x[imp_tx=%u imp_rx=%u sqth=%u] ANACAL2=%08x[vco_tx=%u vco_rx=%u pi=%u] TDCSTA1=%08x\n",
			rx_imp_code >= 0 ? rx_imp_code : 5, a1, a1 & 0x1f, (a1 >> 8) & 0x1f, (a1 >> 16) & 0xf,
			a2, (a2 >> 8) & 0xff, a2 & 0xff, (a2 >> 16) & 0xff, td);
		return l;
	}
	if (c == 'D') {			/* force the A60901 analog/PLL/TDC init (skipped for A60928) then re-trigger cal */
		uint rd;
		IO_SPHYREG(PHY_CSR_PHYSET3, 0xCD810110); mdelay(1);
		IO_SPHYREG(PHY_CSR_PHYSET3, 0x45810110); mdelay(1);
		IO_SPHYREG(PHY_CSR_ANAPLLREG3, 0x00086100); mdelay(1);
		IO_SPHYREG(PHY_CSR_PHYSET3, 0x4581E110); mdelay(1);
		IO_SPHYREG(PHY_CSR_TDCSET1, 0x400000C8);
		IO_SPHYREG(PHY_CSR_ANAPLLREG4, 0x00000013); mdelay(1);
		/* re-set GPON mode + PLL-reset pulse to run the cal with the new analog config */
		rd = IO_GPHYREG(PHY_CSR_PHYSET10); IO_SPHYREG(PHY_CSR_PHYSET10, rd | PHY_GPON_MODE);
		rd = IO_GPHYREG(PHY_CSR_PHYSET3);
		IO_SPHYREG(PHY_CSR_PHYSET3, rd | PHY_PLL_RST | PHY_COUNT_RST); mdelay(2);
		IO_SPHYREG(PHY_CSR_PHYSET3, rd); mdelay(10);
		{
			uint a1 = IO_GPHYREG(PHY_CSR_ANACAL1), a2 = IO_GPHYREG(PHY_CSR_ANACAL2), tdd = IO_GPHYREG(PHY_CSR_TDCSTA1);
			printk(KERN_DEBUG "D: A60901 init+cal -> ANACAL1=%08x[imp_rx=%u] ANACAL2=%08x TDCSTA1=%08x\n",
				a1, (a1 >> 8) & 0x1f, a2, tdd);
		}
		return l;
	}
	if (c == 'A') {			/* set APD bias = apd_code (raise RX gain for data-RX SNR) + re-trigger cal */
		uint rd, a1, td;
		en7571_apd_dac((u8)(apd_code & 0xff));
		mdelay(30);                                                    /* APD settle */
		rd = IO_GPHYREG(PHY_CSR_PHYSET10); IO_SPHYREG(PHY_CSR_PHYSET10, rd | PHY_GPON_MODE);
		rd = IO_GPHYREG(PHY_CSR_PHYSET3);
		IO_SPHYREG(PHY_CSR_PHYSET3, rd | PHY_PLL_RST | PHY_COUNT_RST); mdelay(2);
		IO_SPHYREG(PHY_CSR_PHYSET3, rd); mdelay(10);
		a1 = IO_GPHYREG(PHY_CSR_ANACAL1); td = IO_GPHYREG(PHY_CSR_TDCSTA1);
		printk(KERN_DEBUG "A: apd=0x%02x ANACAL1=%08x[imp_rx=%u sqth=%u] TDCSTA1=%08x\n",
			apd_code & 0xff, a1, (a1 >> 8) & 0x1f, (a1 >> 16) & 0xf, td);
		return l;
	}
	if (c == 'e') {		/* ★ session-11: MANUAL PLOAM-FIFO poll. On k6.18 the GPON PLOAM IRQ doesn't drive
				 * gpon_recv_ploam_message, so DS PLOAM (Upstream_Overhead/Extended_Burst_Length/
				 * Assign_ONU_ID) accumulates unread. Poll it manually — the FIFO reader
				 * (gponDevGetPloamMsg) logs each msg "PLOAM-DS: raw.. onu_id.. msg_id.." under
				 * ploam_verbose. Reads+dispatches like the real RX path (advances state on real msgs). */
		extern void gpon_recv_ploam_message(void);
		int k;
		printk(KERN_DEBUG "e: manual PLOAM poll x80 (ploam_verbose gates per-msg log)\n");
		for (k = 0; k < 80; k++) { gpon_recv_ploam_message(); mdelay(25); }
		printk(KERN_DEBUG "e: PLOAM poll done, act_st=O%u\n", IO_GREG(0xbfb640bcu) & 0xfu);
		return l;
	}
	if (c == 'v') {		/* ★ session-11: FORCE O3->O4 (synthetic Assign_ONU_ID). Validates the O4 code path
				 * via the SM's own transition (not a bare poke) — should HOLD (SM internally O4).
				 * RUN LASER-OFF (tx_laser_off=1). Diagnostic fiction; no real OLT session. */
		extern void gpon_force_state_test(int, int);
		printk(KERN_DEBUG "v: forcing O3->O4 (synthetic assign onu_id=0x21)\n");
		gpon_force_state_test(4, 0x21);
		return l;
	}
	if (c == 'z') {		/* ★ session-11: FORCE O4->O5 (synthetic Ranging_Time: EqD=0 + US-FEC + MBI start).
				 * Run AFTER 'v'. Validates the O5 code path. RUN LASER-OFF. Fiction; no grants. */
		extern void gpon_force_state_test(int, int);
		printk(KERN_DEBUG "z: forcing O4->O5 (synthetic ranging)\n");
		gpon_force_state_test(5, 0x21);
		return l;
	}
	if (c == 'G') {		/* ★ M1 Step-4 diag: dump GDM2 FE datapath regs + RX counters + OMCC latch (did WAN RX reach GDM2/CPU?) */
		printk(KERN_DEBUG "G: GDM2 fwd=%08x rxch=%08x txch=%08x chen=%08x rxlen=%08x omci_id=%08x\n",
			regRead32(0xBFB51500u), regRead32(0xBFB51528u), regRead32(0xBFB51524u), regRead32(0xBFB5140Cu), regRead32(0xBFB51514u), regRead32(0xBFB64048u));
		printk(KERN_DEBUG "G: RXcnt 48=%08x 54=%08x 58=%08x 5c=%08x 60=%08x 74=%08x 80=%08x 84=%08x\n",
			regRead32(0xBFB51648u), regRead32(0xBFB51654u), regRead32(0xBFB51658u), regRead32(0xBFB5165Cu),
			regRead32(0xBFB51660u), regRead32(0xBFB51674u), regRead32(0xBFB51680u), regRead32(0xBFB51684u));
		return l;
	}
	if (c == 'a') {		/* GPON MAC activation + downstream-sync diag: G_ACTIVATION(0xbfb640bc) act_st = O-state;
				 * DBG_DS_SPF_CNT(0xbfb64358) = downstream super-frame count (increments if the MAC
				 * is receiving the OLT downstream). Read 2x to see if it's advancing. */
		uint act, spf1, spf2, i;
		printk(KERN_DEBUG "a: --- GPON MAC activation/downstream diag ---\n"); mdelay(2);
		for (i = 0; i < 4; i++) {
			act  = IO_GREG(0xbfb640bcu);
			spf1 = IO_GREG(0xbfb64358u);
			mdelay(400);
			spf2 = IO_GREG(0xbfb64358u);
			printk(KERN_DEBUG "a%u: G_ACT=%08x act_st=O%u | DS_SPF %08x->%08x d=%d | PHYSTA1=%08x rdy=%u RXSTAT=%08x\n",
				i, act, act & 0xfu, spf1, spf2, (int)((spf2 & 0x3fffffffu) - (spf1 & 0x3fffffffu)),
				IO_GREG(0xbfaf0130u), (IO_GREG(0xbfaf0130u) >> 18) & 0x7u, IO_GREG(0xbfaf021cu));
		}
		return l;
	}
	if (c == 'm') {		/* ★ O1->O2 datapath probe: START the MAC<->PHY MBI interface + watch DS_SPF.
				 * See gpon_mbi_start_probe(). Run AFTER the CDR is locked (rdy=6). */
		gpon_mbi_start_probe();
		return l;
	}
	if (c == 'Y') {		/* ★ dump burst/laser config (READ-ONLY, safe) — verify FIX 1 (timing latch) took effect */
		u8 b[4] = {0}, ib[4] = {0}, im[4] = {0};
		e7_rd(0x08, b, 4); e7_rd(0x138, ib, 4); e7_rd(0x148, im, 4);
		printk(KERN_DEBUG "D: burst reg0x08 = delay:%02x T1C:%02x T0C:%02x ctrl:%02x  (FIX1 wants T0C=0x48 T1C=0x44, ctrl bit3=ERC bit5=TGENrst)\n",
			b[0], b[1], b[2], b[3]);
		printk(KERN_DEBUG "D: Ibias DAC=0x%03x  Imod DAC=0x%03x  TxMon=%d  (Nokia registered @ Bias~0xc02)\n",
			(uint)(((ib[1] & 0xf) << 8) | ib[0]), (uint)(((im[1] & 0xf) << 8) | im[0]), en7571_tx_power_read());
		return l;
	}
	if (c == 'k') {		/* ★ measure TRUE per-BOSA burst T0C/T1C (en7571_tgen_measure) — replaces the
				 * guessed fixed 0x7F envelope. Run at O3 (laser biased+keyed). Likely fix for a
				 * functional-TX unit stuck at O3. Brief PRBS (user-authorized, quiet PON). */
		printk(KERN_DEBUG "k: TGEN-measure true burst T0C/T1C...\n");
		en7571_tgen_measure(1);
		return l;
	}
	if (c == 'Z') {		/* ★ DECISIVE amplitude sweep via MMIO. ROOT CAUSE of the "amp won't change":
				 * phy_tx_amp_setting()/IO_SPHYREG route the WRITE through phy_I2C_write_translet
				 * -> SIF I2C dev 0x70 (the optical transceiver), NOT MMIO 0xbfaf01a0 where the
				 * READ comes from. So every prior amp write hit the wrong device. Here we write
				 * ANATXREG1 [19:16] via MMIO (IO_SREG 0xbfaf01a0) -- coherent with the read --
				 * and sweep 0,3,6,9,12,15. If the readback tracks + SWING grows -> amplitude IS
				 * the modulation lever and the real fix is an MMIO write on the O1->O5 path. */
		extern void phy_tx_test_pattern(uint);
		uint xset = IO_GREG(0xbfaf0138u), p, a, base; int dark, t0, t1, lvl;
		en7571_set_ibias(0x7ce);
		IO_SREG(0xbfaf0138u, xset & ~0x80u); mdelay(5);                       /* burst-capable / laser on */
		p = IO_GREG(0xbfaf0108u); IO_SREG(0xbfaf0108u, (p & 0xffffff5fu) | 0xa0u); /* continuous mode */
		base = IO_GREG(0xbfaf01a0u) & 0xfff0ffffu;                            /* keep all but [19:16] */
		for (lvl = 0; lvl <= 15; lvl += 3) {
			IO_SREG(0xbfaf01a0u, base | ((uint)lvl << 16));                  /* MMIO write [19:16]=lvl */
			a = IO_GREG(0xbfaf01a0u);                                        /* MMIO read back */
			phy_tx_test_pattern(0x00); mdelay(20); dark = en7571_tx_power_read();
			phy_tx_test_pattern(0x0a); mdelay(40); t0 = en7571_tx_power_read();   /* ALL0 */
			phy_tx_test_pattern(0x09); mdelay(40); t1 = en7571_tx_power_read();   /* ALL1 */
			printk(KERN_DEBUG "Z amp=%2d ANATXREG1=%08x [19:16]=%x: dark=%d ALL0=%d ALL1=%d SWING=%d\n",
				lvl, a, (a >> 16) & 0xf, dark, t0, t1, t1 - t0);
		}
		phy_tx_test_pattern(0x00);
		p = IO_GREG(0xbfaf0108u); IO_SREG(0xbfaf0108u, p & 0xffffff5fu);      /* burst mode */
		IO_SREG(0xbfaf0138u, xset | 0x80u);                                  /* laser OFF */
		IO_SREG(0xbfaf01a0u, base | 0x00090000u);                            /* leave [19:16]=9 (MMIO) */
		return l;
	}
	if (c == 'q') {		/* ★★★ MMIO-BIST modulation test. The 'Z'/'t' BIST used phy_tx_test_pattern()
				 * which writes BISTCTL_LOOPBACK_SEL/PRBS_TX_EN via IO_SPHYREG -> I2C 0x70
				 * (MISROUTED) so ALL0/ALL1 never actually differ -> 'Z' SWING was a null test.
				 * Here: (1) PROVE the misroute — an IO_SPHYREG ALL1 write must leave MMIO
				 * LOOPBACK_SEL (0xbfaf04a0) UNCHANGED; (2) inject ALL0(0x0a)/ALL1(0x09) via MMIO
				 * IO_SREG and measure REAL optical swing. PRBS_TX_EN=0xbfaf04a4. */
		extern void phy_tx_test_pattern(uint);
		uint xset = IO_GREG(0xbfaf0138u), p, base;
		uint sel0 = IO_GREG(0xbfaf04a0u), sel_sphy;
		int t0, t1, idle, bi;
		static const uint biases[] = {0x120u, 0x2ffu, 0x4ffu, 0x7ceu};
		IO_SREG(0xbfaf0138u, xset & ~0x80u); mdelay(5);                       /* laser on (bit7=0) */
		p = IO_GREG(0xbfaf0108u); IO_SREG(0xbfaf0108u, (p & 0xffffff5fu) | 0xa0u); /* continuous mode */
		base = IO_GREG(0xbfaf01a0u) & 0xfff0ffffu; IO_SREG(0xbfaf01a0u, base | 0x00090000u); /* amp=9 */
		en7571_set_imod(EN7571_TX_IMOD_DEFAULT);
		phy_tx_test_pattern(0x09); mdelay(5); sel_sphy = IO_GREG(0xbfaf04a0u); /* misroute check (now via MMIO) */
		printk(KERN_DEBUG "q: SEL orig=%08x sphyALL1=%08x misrouted=%d -- MMIO-BIST bias sweep (ALL0=0a ALL1=09):\n",
			sel0, sel_sphy, (sel0 == sel_sphy));
		for (bi = 0; bi < 4; bi++) {
			en7571_set_ibias((u16)biases[bi]); mdelay(8);
			IO_SREG(0xbfaf04a4u, 0);
			IO_SREG(0xbfaf04a0u, 0x00u); mdelay(20); idle = en7571_tx_power_read(); /* idle pattern */
			IO_SREG(0xbfaf04a0u, 0x0au); mdelay(30); t0 = en7571_tx_power_read();   /* ALL0 */
			IO_SREG(0xbfaf04a0u, 0x09u); mdelay(30); t1 = en7571_tx_power_read();   /* ALL1 */
			printk(KERN_DEBUG "q ib=%03x: idle=%d ALL0=%d ALL1=%d SWING(1-0)=%d emit(idle-ALL1)=%d\n",
				biases[bi], idle, t0, t1, t1 - t0, idle - t1);
		}
		IO_SREG(0xbfaf04a0u, 0x00u);                                         /* back to idle */
		p = IO_GREG(0xbfaf0108u); IO_SREG(0xbfaf0108u, p & 0xffffff5fu);      /* burst mode */
		IO_SREG(0xbfaf0138u, xset | 0x80u);                                  /* laser OFF */
		en7571_set_ibias(0x80);                                              /* bias to min */
		return l;
	}
	if (c == 't') {		/* ★ TGEN emission-confirm (BRIEF rogue continuous TX — USER-AUTHORIZED, quiet PON).
				 * Bias the laser clearly above threshold, force CONTINUOUS mode (PHYSET3|=0xa0) +
				 * BIST ALL1 (solid CW) for ~0.5s, read the Tx monitor. Real laser => large net;
				 * ~0 => TX optics dead / monitor unwired. Then STOP + laser held OFF + bias min. */
		extern void phy_tx_test_pattern(uint);
		uint p, xset = IO_GREG(0xbfaf0138u); int dark, on, i, mx = 0;
		en7571_set_ibias(0x33b);                                      /* clear above-threshold */
		en7571_set_imod(EN7571_TX_IMOD_DEFAULT);
		IO_SREG(0xbfaf0138u, xset & ~0x80u); mdelay(5);               /* burst-capable */
		dark = en7571_tx_power_read();
		printk(KERN_DEBUG "t: TGEN emission test (Ibias=0x33b) dark=%d -- continuous CW ~0.5s\n", dark);
		p = IO_GREG(0xbfaf0108u); IO_SREG(0xbfaf0108u, (p & 0xffffff5fu) | 0xa0u); /* PHYSET3 cont mode */
		phy_tx_test_pattern(0x09);                                    /* PHY_BIST_ALL1 (solid CW) */
		for (i = 0; i < 8; i++) { mdelay(60); on = en7571_tx_power_read(); if (on > mx) mx = on;
			printk(KERN_DEBUG "t%d: driven txpwr=%d net=%d\n", i, on, on - dark); }
		phy_tx_test_pattern(0x00);                                    /* PHY_BIST_IDLE (stop) */
		p = IO_GREG(0xbfaf0108u); IO_SREG(0xbfaf0108u, p & 0xffffff5fu); /* PHYSET3 burst mode */
		IO_SREG(0xbfaf0138u, xset | 0x80u);                          /* laser OFF (bit7=1) */
		en7571_set_ibias(0x80);                                      /* bias to min */
		printk(KERN_DEBUG "t: TGEN END: dark=%d max=%d net=%d => %s\n", dark, mx, mx - dark,
			(mx - dark > 40) ? "*** LASER EMITS (alive) ***" : "no emission (TX optics dead / monitor unwired)");
		return l;
	}
	if (c == 'c') {		/* ★ HOLD continuous CW at tx_ibias/tx_pat for a bench power-meter read
				 * (USER-AUTHORIZED). Like 't' but does NOT auto-stop and uses the tx_ibias
				 * param so bias is SWEEPABLE: 't' hardcoded 0x33b which under-lases; this unit's
				 * calibrated operating bias is 0x7CE. tx_pat: 0x09=ALL1 0x0a=ALL0 0x06=PRBS23.
				 * Run after 'R' (front-end powered). echo h to stop. */
		extern void phy_tx_test_pattern(uint);
		extern int tx_pat;
		uint p, xset = IO_GREG(0xbfaf0138u); int on;
		en7571_set_ibias((u16)(tx_ibias & 0xfff));
		en7571_set_imod((u16)(tx_imod & 0xfff));
		IO_SREG(0xbfaf0138u, xset & ~0x80u); mdelay(5);              /* un-hold laser (bit7=0) */
		p = IO_GREG(0xbfaf0108u); IO_SREG(0xbfaf0108u, (p & 0xffffff5fu) | 0xa0u); /* PHYSET3 continuous */
		phy_tx_test_pattern((uint)(tx_pat & 0xff));                  /* held pattern (ALL1/ALL0/PRBS) */
		mdelay(20); on = en7571_tx_power_read();
		printk(KERN_DEBUG "c: HOLD CW pat=0x%02x Ibias=0x%03x Imod=0x%03x monitor=%d (laser held ON, echo h to stop)\n",
			(uint)(tx_pat & 0xff), (uint)(tx_ibias & 0xfff), (uint)(tx_imod & 0xfff), on);
		return l;
	}
	if (c == 'h') {		/* halt the 'c' CW hold: pattern idle + burst mode + laser OFF + bias min */
		extern void phy_tx_test_pattern(uint);
		uint p, xset = IO_GREG(0xbfaf0138u);
		phy_tx_test_pattern(0x00);
		p = IO_GREG(0xbfaf0108u); IO_SREG(0xbfaf0108u, p & 0xffffff5fu);
		IO_SREG(0xbfaf0138u, xset | 0x80u);
		en7571_set_ibias(0x80);
		printk(KERN_DEBUG "h: CW halted, laser OFF\n");
		return l;
	}
	if (c == 'n') {		/* ★ LIVE PRBS23 APC optical-extinction cal — the ONE GPON_Tx_calibration
				 * step the 'R' path skips (mt7570_calibration_pattern, mt7570.c:981).
				 * USER-AUTHORIZED brief rogue PRBS on the shared PON. ATOMIC in-kernel
				 * enable->settle->disable so PRBS can NEVER persist if a round-trip fails.
				 * calibration_pattern: cal-start + CDR(DIS) + PRBS23 on; let the running
				 * DCL/APC loop converge on real modulation; disable_pattern: BIST_IDLE +
				 * CDR(EN) -> back to normal burst. Watch peek bfb640bc for O3->O4. */
		uint a0, a1;
		a0 = IO_GREG(0xbfb640bcu) & 0xfu;
		printk(KERN_DEBUG "n: PRBS23 APC cal START state=O%u (brief rogue PRBS ~1200ms)\n", a0);
		mt7570_calibration_pattern(PHY_GPON_CONFIG);   /* cal-start + CDR(DIS) + PRBS23 on */
		mdelay(1200);                                  /* let running DCL/APC converge on modulation */
		mt7570_disable_pattern();                      /* BIST_IDLE + CDR(EN) -> normal burst */
		mdelay(80);
		a1 = IO_GREG(0xbfb640bcu) & 0xfu;
		printk(KERN_DEBUG "n: PRBS23 DISABLED state O%u->O%u -- poll peek bfb640bc for O3->O4\n", a0, a1);
		return l;
	}
	if (c == 'y') {		/* ★ EN7571 factory calibration_pattern — BRIEF, PON-SAFE (live fiber).
				 * Faithful port of stock en7571_calibration_pattern (@0x2e2e4): the factory
				 * TX cal the port SKIPS (it only had the dead mt7570 path). Applies the cal
				 * (CDR->REF + DCL cycle + PRBS23 + GPON mode + safe_reset + RogueONU_clear +
				 * KT-on), lets it settle ~1.2s, then REVERTS to normal burst mode (like 'n').
				 * NO continuous CW -> not a rogue transmitter. Watch peek bfb640bc for O3->O4. */
		u8 b[4]; uint a0m, a1m, p;
		a0m = IO_GREG(0xbfb640bcu) & 0xfu;
		printk(KERN_DEBUG "y: factory cal-pattern START state=O%u (brief PRBS23 ~1.2s)\n", a0m);
		/* --- apply cal-pattern (stock order) --- */
		p = IO_GREG(0xbfaf0100u); IO_SREG(0xbfaf0100u, p | 0x01000000u);       /* en7571_CDR(0): TX clk -> REF */
		if (!e7_rd(0x228, b, 4)) { b[1] &= (u8)~0x01u; e7_wr(0x228, b, 4); }   /* en7571_DCL_stop  (0x229.0=0) */
		if (!e7_rd(0x228, b, 4)) { b[1] |=       0x01u; e7_wr(0x228, b, 4); }   /* en7571_DCL_start (0x229.0=1) */
		phy_tx_test_pattern(0x06);                                            /* PHY_BIST_PRBS23 (factory pattern) */
		phy_mode_config(PHY_GPON_CONFIG, PHY_ENABLE);                         /* GPON datapath + tx_enable */
		if (!e7_rd(0x101, b, 4)) { b[0] |= 0x40u; e7_wr(0x101, b, 4); }        /* en7571_safe_circuit_reset (0x101.6) */
		if (!e7_rd(0x169, b, 1)) { b[0] |= 0x01u; e7_wr(0x169, b, 1); }        /* en7571_RogueONU_clear (0x169.0) */
		if (!e7_rd(0x230, b, 4)) { b[0] |= 0x01u; e7_wr(0x230, b, 4); }        /* en7571_tune_KT: KT-on (0x230.0) */
		mdelay(1200);                                                         /* let the cal loop converge */
		/* --- revert to normal burst mode (PON-safe) --- */
		phy_tx_test_pattern(0x00);                                            /* PHY_BIST_IDLE (stop pattern) */
		if (!e7_rd(0x228, b, 4)) { b[1] &= (u8)~0x01u; e7_wr(0x228, b, 4); }   /* DCL_stop (restore no-servo default) */
		p = IO_GREG(0xbfaf0100u); IO_SREG(0xbfaf0100u, p & ~0x01000000u);      /* en7571_CDR(1): TX clk -> recovered */
		mdelay(80);
		a1m = IO_GREG(0xbfb640bcu) & 0xfu;
		printk(KERN_DEBUG "y: factory cal-pattern DONE+reverted O%u->O%u -- poll peek bfb640bc for O3->O4\n", a0m, a1m);
		return l;
	}
	if (c == 'p') {		/* apply tx_xpon (XPON_SETTING = burst-enable polarity/tx_sd) live. Sweep
				 * 0x10F/0x11F/0x18F/0x19F vs OLT: echo <v> > /sys/.../tx_xpon; echo p > ... */
		IO_SREG(0xbfaf0138u, (uint)tx_xpon);
		printk(KERN_DEBUG "p: XPON_SETTING set to %08x (param tx_xpon=0x%x)\n", IO_GREG(0xbfaf0138u), tx_xpon);
		return l;
	}
	if (c == 'f') {		/* ★ TX frame/burst COUNTER delta — is the PHY TX engine actually EMITTING upstream
				 * bursts at O3? Pure digital observability (no optics/loopback; GPON RX can't
				 * frame our own upstream). TX_FRAME_COUNTER=0xbfaf0434 TX_BURST_COUNTER=0xbfaf0438,
				 * latched via TX_TEST_TRIG|PHY_TX_CNT_LCH. Now that IO_SPHYREG is MMIO the latch lands. */
		extern int phy_tx_frame_counter(void);
		extern int phy_tx_burst_counter(void);
		int f0, b0, f1, b1;
		f0 = phy_tx_frame_counter(); b0 = phy_tx_burst_counter();
		mdelay(3000);
		f1 = phy_tx_frame_counter(); b1 = phy_tx_burst_counter();
		printk(KERN_DEBUG "f: TX counters/3s: frames %d->%d (d=%d) bursts %d->%d (d=%d) => %s\n",
			f0, f1, f1 - f0, b0, b1, b1 - b0,
			((f1 - f0) || (b1 - b0)) ? "TX ENGINE FIRING BURSTS" : "TX ENGINE NOT FIRING (MAC not transmitting)");
		return l;
	}
	if (c == 'i') {		/* apply tx_imod param to the laser MODULATION-current DAC (SIF 0x148) LIVE.
				 * The APC froze the mod servo so this sticks. Sweep vs OLT-decodability:
				 * echo <v> > /sys/module/econet_xpon/parameters/tx_imod; echo i > /proc/econet_xpon_los
				 * then watch dmesg for Assign_ONU_ID / O3->O4. Nokia registered Mod=0xc02; flash-cal=0x157. */
		u8 im[4] = {0};
		en7571_set_imod((u16)(tx_imod & 0xFFF));
		e7_rd(0x148, im, 4);
		printk(KERN_DEBUG "i: Imod set to 0x%03x (param tx_imod=0x%x)\n",
			(uint)(((im[1] & 0x0F) << 8) | im[0]), tx_imod);
		return l;
	}
	if (c == 'b') {		/* apply the current tx_ibias param to the laser DC bias DAC (live tuning:
				 * echo <val> > /sys/module/econet_xpon/parameters/tx_ibias; echo b > ...). */
		u8 ib[4] = {0};
		en7571_set_ibias((u16)(tx_ibias & 0xFFF));
		e7_rd(0x138, ib, 4);
		printk(KERN_DEBUG "b: Ibias set to 0x%03x (param tx_ibias=0x%x)\n",
			((ib[1] & 0x0F) << 8) | ib[0], tx_ibias);
		return l;
	}
	if (c == 'B') {		/* ★ TX laser emission check + bias ramp. en7571_tx_emitting() = dark vs lit
				 * (net>8 => lasing). If dark, ramp Ibias 0x1FB->0x666 watching the Tx monitor.
				 * Run after the CDR is locked (front-end powered). Leaves the lasing bias set. */
		int emit = en7571_tx_emitting();
		if (!emit) {
			int code = en7571_tx_ramp_bias();
			printk(KERN_DEBUG "B: ramp result: %s (Ibias=0x%03x)\n", code ? "LASING" : "no lase up to 0x666", code);
		} else {
			printk(KERN_DEBUG "B: laser already EMITTING at seed bias\n");
		}
		return l;
	}
	if (c == 'w') {		/* apply STOCK-LOCKED config diffs (from the register diff) + finalize + poll lock.
				 * These are the digital-PHY regs stock sets that the port leaves default. Run AFTER R. */
		int i; uint ps, p;
		printk(KERN_DEBUG "w: apply stock config diffs wmask=0x%02x + finalize\n", wmask); mdelay(2);
		if (wmask & 0x01) IO_SREG(0xbfaf0110u, 0x00a0e004u);            /* PHYSET5 (bits23:16=0xa0) */
		if (wmask & 0x02) IO_SREG(0xbfaf012cu, 0x00407f7fu);           /* 0x12c */
		if (wmask & 0x04) IO_SREG(0xbfaf0150u, 0x0000e000u | ((uint)po_anapwd & 0x180u));           /* ANAPWD (|0x180) */
		if (wmask & 0x08) { p=IO_GREG(0xbfaf01a0u); IO_SREG(0xbfaf01a0u, p & ~0x90000u); } /* ANATXREG1 */
		if (wmask & 0x10) IO_SREG(0xbfaf01e0u, 0x00003c05u);           /* ANADDS3 (NCPO probe-select) */
		if (wmask & 0x20) IO_SREG(0xbfaf01f8u, 0x0000002du);           /* TDCSET2 = 45 */
		if (wmask & 0x40) IO_SREG(0xbfaf0228u, 0x00070000u);           /* 0x228 */
		if (wmask & 0x80) IO_SREG(0xbfaf0230u, 0x00000007u);           /* 0x230 */
		mdelay(5);
		/* finalize -> tracking (PHYSET4.8=1, TDCSET1.5=0, PHYSET2=0x3c01 stock tracking) */
		p=IO_GREG(0xbfaf010cu); IO_SREG(0xbfaf010cu, p | 0x100u);
		p=IO_GREG(0xbfaf01f0u); IO_SREG(0xbfaf01f0u, p & ~0x20u);
		p=IO_GREG(0xbfaf0104u); IO_SREG(0xbfaf0104u, (p & ~0x10u) | 0x1u);
		mdelay(5);
		for (i=0;i<12;i++){
			ps=IO_GREG(0xbfaf0130u);
			printk(KERN_DEBUG "w%d: PHYSTA1=%08x rdy=%u ANACAL2=%08x RXSTAT=%08x TDCSTA1=%08x\n",
				i, ps, (ps>>18)&0x7, IO_GREG(0xbfaf0144u), IO_GREG(0xbfaf021cu), IO_GREG(0xbfaf01f4u));
			if (((ps>>18)&0x7)==6){ printk(KERN_DEBUG "w: *** LOCKED rdy=6 ***\n"); break; }
			mdelay(300);
		}
		return l;
	}
	if (c == 'g') {		/* full-range digital-PHY MMIO dump (0x100-0x260 + extras) for stock-diff.
				 * Raw MMIO (IO_GREG), no SIF -> no sifdbg spam. Run AFTER R (clear dmesg first). */
		uint sub;
		for (sub = 0x100u; sub <= 0x260u; sub += 4u)
			printk(KERN_DEBUG "g 0x1faf%04x=0x%08x\n", sub, IO_GREG(0xbfaf0000u + sub));
		printk(KERN_DEBUG "g 0x1faf021c=0x%08x\n", IO_GREG(0xbfaf021cu));
		printk(KERN_DEBUG "g 0x1faf05e0=0x%08x\n", IO_GREG(0xbfaf05e0u));
		return l;
	}
	if (c == 'E') {		/* dump key PHY CSRs via BOTH MMIO(0xbfaf0000+sub) and I2C@0x70[sub],
				 * side by side, to settle which register file is the live digital PHY.
				 * The stock en7571_CDR/analog uses raw MMIO; our A60928 build routes
				 * everything through I2C. If they differ, we've been writing to the wrong space. */
		#define MM(sub) (0xbfaf0000u + (uint)(sub))
		struct { const char *n; uint sub; } regs[] = {
			{"PHYSET1 ", PHY_CSR_PHYSET1}, {"PHYSET2 ", 0x104u},
			{"PHYSET3 ", PHY_CSR_PHYSET3},
			{"PHYSET4 ", PHY_CSR_PHYSET4}, {"PHYSET5 ", PHY_CSR_PHYSET5},
			{"PHYSET6 ", PHY_CSR_PHYSET6},
			{"PHYSET10", PHY_CSR_PHYSET10}, {"XPONSET ", PHY_CSR_XPON_SETTING},
			{"ANACAL1 ", PHY_CSR_ANACAL1}, {"ANACAL2 ", PHY_CSR_ANACAL2},
			{"ANASET14", 0x194u}, {"ANADDS1 ", 0x1d8u},
			{"TDCSET1 ", PHY_CSR_TDCSET1}, {"TDCSET2 ", PHY_CSR_TDCSET2},
			{"TDCSTA1 ", PHY_CSR_TDCSTA1},
			{"PHYSTA1 ", PHY_CSR_PHYSTA1}, {"RXSTAT  ", PHY_CSR_RX_STATUS},
			{"XPON_STA", PHY_CSR_XPON_STA}, {"LOF_CNT ", PHY_CSR_LOF_CNT},
		};
		unsigned i;
		for (i = 0; i < ARRAY_SIZE(regs); i++)
			printk(KERN_DEBUG "E: %s sub=0x%03x  MMIO(0x%08x)=%08x  I2C=%08x\n",
				regs[i].n, regs[i].sub, MM(regs[i].sub),
				IO_GREG(MM(regs[i].sub)), IO_GPHYREG(regs[i].sub));
		return l;
	}
	if (c == 'H') {		/* Force the captured LOCKED-STOCK digital PHY config (session-6 reference) via
				 * RAW MMIO (stock's real access path): PHYSET4(0x10c)=0x000023FF,
				 * PHYSET6(0x114)[12:8]=0x10 (RX-imp=16; STOCK forces it, our port leaves the
				 * auto-cal ~19), TDCSET1(0x1f0)=0x400000C0, TDCSET2(0x1f8)=45. Do NOT clear
				 * PHYSET4 bit1 (stock leaves it set, unlike J). Re-poll PHYSTA1 for lock. Run
				 * AFTER R. rx_imp_code (if >=0) overrides the RX-imp code. */
		uint rd; int imp = (rx_imp_code >= 0) ? (rx_imp_code & 0x1f) : 0x10;
		int i; uint ps;
		printk(KERN_DEBUG "H: stock-digital-match PHYSET4=23FF PHYSET6[12:8]=%#x TDCSET1=400000C0 TDCSET2=45\n", imp); mdelay(2);
		IO_SREG(0xbfaf010cu, 0x000023FFu);
		rd = IO_GREG(0xbfaf0114u);
		IO_SREG(0xbfaf0114u, (rd & ~0x1F00u) | ((uint)imp << 8));
		IO_SREG(0xbfaf01f0u, 0x400000C0u);
		IO_SREG(0xbfaf01f8u, 45u);
		mdelay(5);
		for (i = 0; i < 8; i++) {
			ps = IO_GREG(0xbfaf0130u);
			printk(KERN_DEBUG "H%d: PHYSTA1=%08x rdy=%u RXSTAT=%08x XPON_STA=%08x PHYSET6=%08x\n",
				i, ps, (ps >> 18) & 0x7, IO_GREG(0xbfaf021cu), IO_GREG(0xbfaf05e0u), IO_GREG(0xbfaf0114u));
			if (((ps >> 18) & 0x7) == 6) { printk(KERN_DEBUG "H: *** LOCKED rdy=6 ***\n"); break; }
			mdelay(250);
		}
		return l;
	}
	if (c == 'O') {		/* ★ CDR ARM + NCPO/DDS FREQUENCY SEED — the MISSED step (stock phy_los_handler
				 * @0x22798 GPON arm; absent from the GPL src the port was built on). My port NEVER
				 * seeds ANASET14(0xbfaf0194)/ANADDS1(0xbfaf01d8) with ncpo_online=0x17E21965 (nominal
				 * GPON 2.488G DDS word) -> recovered-clock DDS free-runs wrong freq -> PI wraps
				 * (ANACAL2 PI=0x39) -> PHYSTA1[20:18] stuck 3. Seed + arm bits (PHYSET4&=~0x100,
				 * TDCSET1|=0x20, PHYSET2|=0x10) -> PI should null->0, PHYSTA1[20]->1. Run AFTER R w/light. */
		int i; uint ps, rd;
		printk(KERN_DEBUG "O: seed NCPO 0x17E21965 -> ANASET14(0x194)+ANADDS1(0x1d8) + arm CDR\n"); mdelay(2);
		rd = IO_GREG(0xbfaf010cu); IO_SREG(0xbfaf010cu, rd & ~0x100u);   /* PHYSET4 &= ~0x100 */
		IO_SREG(0xbfaf0194u, 0x17E21965u);                              /* ANASET14 = NCPO seed */
		IO_SREG(0xbfaf01d8u, 0x17E21965u);                              /* ANADDS1 = NCPO/DDS seed */
		rd = IO_GREG(0xbfaf01f0u); IO_SREG(0xbfaf01f0u, rd | 0x20u);     /* TDCSET1 |= 0x20 */
		rd = IO_GREG(0xbfaf0104u); IO_SREG(0xbfaf0104u, rd | 0x10u);     /* PHYSET2 |= 0x10 */
		/* verify the seed writes landed via MMIO; also try SIF path if MMIO didn't stick */
		printk(KERN_DEBUG "O: seed-rd MMIO ANASET14(0x194)=%08x ANADDS1(0x1d8)=%08x TDCSET1=%08x PHYSET2=%08x PHYSET4=%08x\n",
			IO_GREG(0xbfaf0194u), IO_GREG(0xbfaf01d8u), IO_GREG(0xbfaf01f0u), IO_GREG(0xbfaf0104u), IO_GREG(0xbfaf010cu));
		if (IO_GREG(0xbfaf0194u) != 0x17E21965u) {
			printk(KERN_DEBUG "O: MMIO seed did NOT stick -> retry via SIF (IO_SPHYREG)\n");
			IO_SPHYREG(0x194u, 0x17E21965u); IO_SPHYREG(0x1d8u, 0x17E21965u);
			printk(KERN_DEBUG "O: seed-rd after SIF ANASET14=%08x ANADDS1=%08x\n", IO_GREG(0xbfaf0194u), IO_GREG(0xbfaf01d8u));
		}
		mdelay(5);
		for (i = 0; i < 6; i++) {
			ps = IO_GREG(0xbfaf0130u);
			printk(KERN_DEBUG "Oa%d: PHYSTA1=%08x rdy=%u ANACAL2=%08x(PI=%u) RXSTAT=%08x XPON_STA=%08x\n",
				i, ps, (ps>>18)&0x7, IO_GREG(0xbfaf0144u), (IO_GREG(0xbfaf0144u)>>16)&0xff,
				IO_GREG(0xbfaf021cu), IO_GREG(0xbfaf05e0u));
			if (((ps>>18)&0x7)==6) { printk(KERN_DEBUG "O: *** LOCKED rdy=6 (seed only) ***\n"); return l; }
			mdelay(250);
		}
		printk(KERN_DEBUG "O: phase2 re-pulse PHYSET3|=0x88000000 after seed\n");
		rd = IO_GREG(0xbfaf0108u); IO_SREG(0xbfaf0108u, rd | 0x88000000u); mdelay(1); IO_SREG(0xbfaf0108u, rd);
		mdelay(5);
		for (i = 0; i < 8; i++) {
			ps = IO_GREG(0xbfaf0130u);
			printk(KERN_DEBUG "Ob%d: PHYSTA1=%08x rdy=%u ANACAL2=%08x(PI=%u) RXSTAT=%08x\n",
				i, ps, (ps>>18)&0x7, IO_GREG(0xbfaf0144u), (IO_GREG(0xbfaf0144u)>>16)&0xff, IO_GREG(0xbfaf021cu));
			if (((ps>>18)&0x7)==6) { printk(KERN_DEBUG "O: *** LOCKED rdy=6 (after re-pulse) ***\n"); break; }
			mdelay(250);
		}
		return l;
	}
	if (c == 'j') {		/* SEED SWEEP: for DDS seed values around 0x17E21965, do arm(preamble+seed+bits)
				 * then finalize(tracking), watch PI/rdy. A static PI offset can be a freq trim. Run AFTER R. */
		int k; uint p, ps;
		static const uint seeds[] = {0x17222165u,0x17622165u,0x17A21965u,0x17C21965u,0x17E21965u,
					     0x18021965u,0x18221965u,0x18621965u,0x18A21965u};
		for (k = 0; k < (int)(sizeof(seeds)/sizeof(seeds[0])); k++) {
			phy_reset_counter();
			p=IO_GREG(0xbfaf010cu); IO_SREG(0xbfaf010cu, p&~0x100u);      /* arm: PHYSET4.8=0 */
			IO_SREG(0xbfaf0194u, seeds[k]); IO_SREG(0xbfaf01d8u, seeds[k]);
			p=IO_GREG(0xbfaf01f0u); IO_SREG(0xbfaf01f0u, p|0x20u);        /* TDCSET1.5=1 */
			p=IO_GREG(0xbfaf0104u); IO_SREG(0xbfaf0104u, p|0x10u);        /* PHYSET2.4=1 */
			mdelay(60);
			p=IO_GREG(0xbfaf010cu); IO_SREG(0xbfaf010cu, p|0x100u);       /* finalize: PHYSET4.8=1 */
			p=IO_GREG(0xbfaf01f0u); IO_SREG(0xbfaf01f0u, p&~0x20u);       /* TDCSET1.5=0 */
			p=IO_GREG(0xbfaf0104u); IO_SREG(0xbfaf0104u, p&~0x10u);       /* PHYSET2.4=0 */
			mdelay(250);
			ps=IO_GREG(0xbfaf0130u);
			printk(KERN_DEBUG "j seed=%08x: PHYSTA1=%08x rdy=%u ANACAL2=%08x(PI=%u) TDCSTA1=%08x\n",
				seeds[k], ps, (ps>>18)&0x7, IO_GREG(0xbfaf0144u),
				(IO_GREG(0xbfaf0144u)>>16)&0xff, IO_GREG(0xbfaf01f4u));
			if (((ps>>18)&0x7)==6) { printk(KERN_DEBUG "j: *** LOCKED at seed=%08x ***\n", seeds[k]); break; }
		}
		return l;
	}
	if (c == 'B') {		/* FINALIZE (stock phy_ready_handler@0x22c5c GPON tracking mode): after ARM gets
				 * the CDR to the right freq (VCO matches stock) with PI~0x36, transition to
				 * TRACKING to pull the phase. PHYSET4.8=1, TDCSET1.5=0, PHYSET2.4=0. Run AFTER R. */
		int i; uint ps, p;
		printk(KERN_DEBUG "K: FINALIZE -> tracking (PHYSET4.8=1 TDCSET1.5=0 PHYSET2.4=0)\n"); mdelay(2);
		p = IO_GREG(0xbfaf010cu); IO_SREG(0xbfaf010cu, p | 0x100u);     /* PHYSET4 |= 0x100 */
		p = IO_GREG(0xbfaf01f0u); IO_SREG(0xbfaf01f0u, p & ~0x20u);     /* TDCSET1 &= ~0x20 */
		p = IO_GREG(0xbfaf0104u); IO_SREG(0xbfaf0104u, p & ~0x10u);     /* PHYSET2 &= ~0x10 */
		mdelay(5);
		for (i = 0; i < 10; i++) {
			ps = IO_GREG(0xbfaf0130u);
			printk(KERN_DEBUG "K%d: PHYSTA1=%08x rdy=%u ANACAL2=%08x(PI=%u) TDCSTA1=%08x RXSTAT=%08x\n",
				i, ps, (ps>>18)&0x7, IO_GREG(0xbfaf0144u), (IO_GREG(0xbfaf0144u)>>16)&0xff,
				IO_GREG(0xbfaf01f4u), IO_GREG(0xbfaf021cu));
			if (((ps>>18)&0x7)==6) { printk(KERN_DEBUG "K: *** LOCKED rdy=6 ***\n"); break; }
			mdelay(300);
		}
		return l;
	}
	if (c == 'Q') {		/* Cal-TRANSPLANT experiment: write the LOCKED-STOCK analog-cal OUTPUTS
				 * (ANACAL1/ANACAL2/TDCSET2) via raw MMIO, to test if the CDR uses the register
				 * values directly and locks — a shortcut that would bypass porting the en7571
				 * conditioning that makes the SoC cal converge. Read-back shows if the write
				 * even sticks (ANACAL may be a HW cal output). Run AFTER R. */
		int i; uint ps;
		printk(KERN_DEBUG "Q: transplant stock cal ANACAL1=f00b120b ANACAL2=01005b60 TDCSET2=45\n"); mdelay(2);
		IO_SREG(0xbfaf0140u, 0xf00b120bu);   /* ANACAL1: imp_rx=18 imp_tx=11 */
		IO_SREG(0xbfaf0144u, 0x01005b60u);   /* ANACAL2: PI=0x00 vco_rx=0x60 bit24=1 */
		IO_SREG(0xbfaf01f8u, 45u);           /* TDCSET2=0x2d */
		mdelay(5);
		for (i = 0; i < 8; i++) {
			ps = IO_GREG(0xbfaf0130u);
			printk(KERN_DEBUG "Q%d: PHYSTA1=%08x rdy=%u RXSTAT=%08x ANACAL1=%08x ANACAL2=%08x TDCSTA1=%08x\n",
				i, ps, (ps >> 18) & 0x7, IO_GREG(0xbfaf021cu), IO_GREG(0xbfaf0140u),
				IO_GREG(0xbfaf0144u), IO_GREG(0xbfaf01f4u));
			if (((ps >> 18) & 0x7) == 6) { printk(KERN_DEBUG "Q: *** LOCKED rdy=6 ***\n"); break; }
			mdelay(250);
		}
		return l;
	}
	if (c == 'Z') {		/* RX-CDR-enable experiment via the stock MMIO path.
				 * Stock en7571_CDR(ENABLE) clears PHYSET1(0xbfaf0100) bit24 (PHY_TX_LOCK_2_REF)
				 * so the CDR locks to DATA (not the free-running REF clk). Our port NEVER did
				 * this. Do it via MMIO (stock's actual path), set GPON mode + run the cal via
				 * MMIO too, read cal outputs both ways, then poll is_phy_sync for HOLD. */
		extern int is_phy_sync(void);
		uint p1a, p1b, p10, ps3, a1m, a2m, tdm, a1i, tdi;
		int i;
		p1a = IO_GREG(0xbfaf0100u);
		IO_SREG(0xbfaf0100u, p1a & 0xFEFFFFFFu);           /* clear bit24 = CDR ON (lock to DATA) */
		mdelay(2);
		p1b = IO_GREG(0xbfaf0100u);
		p10 = IO_GREG(0xbfaf0124u);
		IO_SREG(0xbfaf0124u, p10 | 0x80000000u);           /* PHYSET10 bit31 = GPON mode (MMIO) */
		mdelay(2);
		ps3 = IO_GREG(0xbfaf0108u);                        /* PHYSET3 */
		IO_SREG(0xbfaf0108u, ps3 | PHY_PLL_RST | PHY_COUNT_RST);  /* cal reset pulse */
		mdelay(2);
		IO_SREG(0xbfaf0108u, ps3);                         /* release -> cal runs */
		mdelay(8);
		a1m = IO_GREG(0xbfaf0140u); a2m = IO_GREG(0xbfaf0144u); tdm = IO_GREG(0xbfaf01f4u);
		a1i = IO_GPHYREG(PHY_CSR_ANACAL1); tdi = IO_GPHYREG(PHY_CSR_TDCSTA1);
		printk(KERN_DEBUG "Z: PHYSET1 MMIO %08x->%08x (bit24 %s)  PHYSET10 MMIO now %08x\n",
			p1a, p1b, (p1b & 0x01000000u) ? "STILL SET" : "CLEARED(CDR-ON)", IO_GREG(0xbfaf0124u));
		printk(KERN_DEBUG "Z: cal-out MMIO ANACAL1=%08x[imp_rx=%u sqth=%u] ANACAL2=%08x TDCSTA1=%08x | I2C ANACAL1=%08x TDCSTA1=%08x\n",
			a1m, (a1m >> 8) & 0x1f, (a1m >> 16) & 0xf, a2m, tdm, a1i, tdi);
		for (i = 0; i < 12; i++) {
			printk(KERN_DEBUG "Z-sync%d: is_sync=%d  PHYSTA1(mmio)=%08x RXSTAT(mmio)=%08x\n",
				i, is_phy_sync(), IO_GREG(0xbfaf0130u), IO_GREG(0xbfaf021cu));
			mdelay(250);
		}
		#undef MM
		return l;
	}
	if (c == 'Y') {		/* CDR-acquisition arm (stock phy_los_handler @0x22798): after PHYLOS de-asserts
				 * (polarity fix) the CDR still needs arming to search+lock. Arm: PHYSET4 &= ~0x100,
				 * TDCSET1 |= 0x20, PHYSET2 |= 0x10 (+ ANASET14/ANADDS1 = ncpo, dynamic/0 on first lock).
				 * Poll PHYSTA1[20:18] for ready(6); on ready finalize (phy_ready_handler @0x22c5c):
				 * PHYSET4 |= 0x100, TDCSET1 &= ~0x20, PHYSET2 &= ~0x10. All raw MMIO (single writes). */
		uint p4, td, p2, i, sta, rdy = 0;
		p4 = IO_GREG(0xbfaf010cu); td = IO_GREG(0xbfaf01f0u); p2 = IO_GREG(0xbfaf0104u);
		printk(KERN_DEBUG "Y: pre  PHYSET4=%08x TDCSET1=%08x PHYSET2=%08x ANASET14=%08x ANADDS1=%08x\n",
			p4, td, p2, IO_GREG(0xbfaf0194u), IO_GREG(0xbfaf01d8u));
		IO_SREG(0xbfaf010cu, p4 & ~0x100u);   mdelay(1);   /* PHYSET4 clr 0x100 = arm CDR search */
		IO_SREG(0xbfaf01f0u, IO_GREG(0xbfaf01f0u) | 0x20u); mdelay(1);  /* TDCSET1 |= 0x20 */
		IO_SREG(0xbfaf0104u, IO_GREG(0xbfaf0104u) | 0x10u); mdelay(1);  /* PHYSET2 |= 0x10 */
		for (i = 0; i < 20; i++) {          /* poll up to ~4s for CDR lock */
			uint ps = IO_GREG(0xbfaf0130u);
			sta = IO_GREG(0xbfaf05e0u);
			if (((ps >> 18) & 0x7) == 0x6) { rdy = 1; }
			printk(KERN_DEBUG "Y%d: PHYSTA1=%08x rdy=%u RXSTAT=%08x XPON_STA=%08x LOF=%08x\n",
				i, ps, (ps >> 18) & 0x7, IO_GREG(0xbfaf021cu), sta, IO_GREG(0xbfaf0258u));
			if (rdy) break;
			mdelay(200);
		}
		if (rdy) {                          /* phy_ready_handler finalize */
			IO_SREG(0xbfaf010cu, IO_GREG(0xbfaf010cu) | 0x100u);
			IO_SREG(0xbfaf01f0u, IO_GREG(0xbfaf01f0u) & ~0x20u);
			IO_SREG(0xbfaf0104u, IO_GREG(0xbfaf0104u) & ~0x10u);
			printk(KERN_DEBUG "Y: READY reached -> finalized CDR\n");
		} else
			printk(KERN_DEBUG "Y: not ready after poll\n");
		return l;
	}
	if (c == 'X') {		/* enable the en7571 HIGH-SPEED RX datapath (reg_init tail + DCL_start),
				 * re-pulse the EN7528 RX PLL so it re-locks to the now-present data, then poll
				 * PHYSTA1[20:18] for ready(6) / RXSTAT[3:0] for lock(0xa). Run after I + d60. */
		uint ps3, i, rdy = 0;
		en7571_reg_init_full();
		mdelay(5);
		/* Phase 1: poll WITHOUT the MMIO PLL re-pulse — does the CDR acquire on its own
		 * now the recovered-data loop (DCL) is running? */
		for (i = 0; i < 12; i++) {
			uint pst = IO_GREG(0xbfaf0130u), rxs = IO_GREG(0xbfaf021cu);
			if (((pst >> 18) & 0x7) == 0x6 || (rxs & 0xf) == 0xa) rdy = 1;
			printk(KERN_DEBUG "Xa%d: PHYSTA1=%08x rdy=%u RXSTAT=%08x XPON_STA=%08x LOF=%08x\n",
				i, pst, (pst >> 18) & 0x7, rxs, IO_GREG(0xbfaf05e0u), IO_GREG(0xbfaf0258u));
			if (rdy) break;
			mdelay(200);
		}
		if (!rdy) {   /* Phase 2: NOW re-pulse the MMIO RX PLL and poll again */
			printk(KERN_DEBUG "X: phase1 no-lock -> re-pulse RX PLL\n");
			ps3 = IO_GREG(0xbfaf0108u);
			IO_SREG(0xbfaf0108u, ps3 | PHY_PLL_RST | PHY_COUNT_RST); mdelay(2);
			IO_SREG(0xbfaf0108u, ps3); mdelay(10);
			for (i = 0; i < 12; i++) {
				uint pst = IO_GREG(0xbfaf0130u), rxs = IO_GREG(0xbfaf021cu);
				if (((pst >> 18) & 0x7) == 0x6 || (rxs & 0xf) == 0xa) rdy = 1;
				printk(KERN_DEBUG "Xb%d: PHYSTA1=%08x rdy=%u RXSTAT=%08x XPON_STA=%08x LOF=%08x\n",
					i, pst, (pst >> 18) & 0x7, rxs, IO_GREG(0xbfaf05e0u), IO_GREG(0xbfaf0258u));
				if (rdy) break;
				mdelay(200);
			}
		}
		printk(KERN_DEBUG "X: %s\n", rdy ? "*** RX LOCK / READY ***" : "no lock after HS-RX enable");
		return l;
	}
	if (c == 'M') {		/* APD-bias sweep vs DIGITAL CDR lock. This unit has NO factory optical cal
				 * (was a mesh node), so the calibrated APD operating point is unknown. With the
				 * HS-RX datapath engaged + correct digital reads, sweep the APD bias and watch
				 * PHYSTA1 bit20 (RX-lock) / RXSTAT[3:0]=0xa to EMPIRICALLY find a locking bias. */
		uint code, ps3, best_code = 0, best_ps = 0;
		en7571_reg_init_full();     /* engage datapath once */
		mdelay(5);
		for (code = 0x30; code <= 0xd8; code += 0x06) {
			uint pst, rxs; int k, locked = 0;
			en7571_apd_dac((u8)code);
			mdelay(40);
			/* re-pulse the RX PLL so the CDR re-acquires at this bias */
			ps3 = IO_GREG(0xbfaf0108u);
			IO_SREG(0xbfaf0108u, ps3 | PHY_PLL_RST | PHY_COUNT_RST); mdelay(2);
			IO_SREG(0xbfaf0108u, ps3); mdelay(8);
			for (k = 0; k < 4; k++) {   /* settle+watch ~0.6s */
				pst = IO_GREG(0xbfaf0130u); rxs = IO_GREG(0xbfaf021cu);
				if ((pst & 0x00100000u) || (rxs & 0xf) == 0xa) locked = 1;
				if (locked) break;
				mdelay(150);
			}
			if (pst > best_ps) { best_ps = pst; best_code = code; }
			printk(KERN_DEBUG "M apd=0x%02x: PHYSTA1=%08x bit20=%u rdyfld=%u RXSTAT=%08x[%x] XPON_STA=%08x %s\n",
				code, pst, (pst >> 20) & 1, (pst >> 18) & 7, rxs, rxs & 0xf,
				IO_GREG(0xbfaf05e0u), locked ? "*** LOCK ***" : "");
			if (locked) { printk(KERN_DEBUG "M: LOCKED at apd=0x%02x\n", code); break; }
		}
		printk(KERN_DEBUG "M: sweep done (best PHYSTA1=%08x at apd=0x%02x)\n", best_ps, best_code);
		return l;
	}
	if (c == 'N') {		/* freq-meter: measure each of the 16 selectable clocks (PHYSET11 freq_meter).
				 * "detects signal but won't lock" => is the recovered RX/CDR clock near the target
				 * (phase issue) or way off (frequency/NCPO issue)? PHYSET11 result[14:0] = count vs ref
				 * over the gate. vendor phy_freq_meter's done-loop is backwards; use a fixed gate wait. */
		uint fs, ps3, ps11, res;
		ps3 = IO_GREG(0xbfaf0108u);
		IO_SREG(0xbfaf0108u, ps3 & ~(1u << 24));      /* PHYSET3 clr PHY_MON_PROB_CLK = enable prob clk */
		mdelay(1);
		printk(KERN_DEBUG "N: ANASET14=%08x ANADDS1=%08x (loaded NCPO); PHYSTA1=%08x RXSTAT=%08x\n",
			IO_GREG(0xbfaf0194u), IO_GREG(0xbfaf01d8u), IO_GREG(0xbfaf0130u), IO_GREG(0xbfaf021cu));
		for (fs = 0; fs < 16; fs++) {
			ps11 = IO_GREG(0xbfaf0128u);
			ps11 = (ps11 & 0xf0ffffffu) | (fs << 24);  /* select clock fs into bits[27:24] */
			IO_SREG(0xbfaf0128u, ps11);
			IO_SREG(0xbfaf0128u, ps11 | (1u << 16));   /* FREQ_METER_EN */
			mdelay(10);                                 /* gate time */
			res = IO_GREG(0xbfaf0128u) & 0x7fffu;
			printk(KERN_DEBUG "N sel=%2u: freq_count=%5u (0x%04x)\n", fs, res, res);
			IO_SREG(0xbfaf0128u, IO_GREG(0xbfaf0128u) & ~(1u << 16));  /* disable */
			mdelay(1);
		}
		IO_SREG(0xbfaf0108u, ps3);                    /* restore PHYSET3 */
		return l;
	}
	if (c == 'P') {		/* 2D sweep: LA/RSSI gain (en7571 reg 0x16[2:0]=0..7) x APD {0x50,0x70,0x90},
				 * re-pulse the RX PLL each combo, watch for CDR lock (PHYSTA1 bit20 / RXSTAT 0xa).
				 * The eye level (LA gain) + APD together are the last analog knobs for lock. */
		u8 apds[3] = {0x50, 0x70, 0x90}; u8 b[4];
		uint g, ai, ps3; int k, locked = 0;
		en7571_reg_init_full();  mdelay(3);
		for (ai = 0; ai < 3 && !locked; ai++) {
			en7571_apd_dac(apds[ai]); mdelay(20);
			for (g = 0; g < 8 && !locked; g++) {
				uint pst = 0, rxs = 0;
				if (!e7_rd(0x14, b, 4)) { b[2] = (b[2] & 0xf8) | (g & 0x7); e7_wr(0x14, b, 4); }
				mdelay(5);
				ps3 = IO_GREG(0xbfaf0108u);
				IO_SREG(0xbfaf0108u, ps3 | PHY_PLL_RST | PHY_COUNT_RST); mdelay(2);
				IO_SREG(0xbfaf0108u, ps3); mdelay(8);
				for (k = 0; k < 4; k++) {
					pst = IO_GREG(0xbfaf0130u); rxs = IO_GREG(0xbfaf021cu);
					if ((pst & 0x00100000u) || (rxs & 0xf) == 0xa) { locked = 1; break; }
					mdelay(120);
				}
				printk(KERN_DEBUG "P apd=0x%02x gain=%u: PHYSTA1=%08x[rdy=%u] RXSTAT=%08x[%x] %s\n",
					apds[ai], g, pst, (pst >> 18) & 7, rxs, rxs & 0xf, locked ? "*** LOCK ***" : "");
			}
		}
		printk(KERN_DEBUG "P: %s\n", locked ? "*** CDR LOCKED ***" : "no lock across gain x APD");
		return l;
	}
	/* NOTE: forcing GPON PHY mode via phy_mode_config(PHY_GPON_CONFIG,1) standalone
	 * (PHYSET10 GPON bit + PLL reset) DESTABILIZES the SoC and triggers a watchdog
	 * reboot on this EN7528 -- the digital GPON PHY needs the full clock/init
	 * sequencing first. Left unwired pending that. */
	/* "echo 2 > ..." => I2C bus scan on ch0 (ACK map dumped to dmesg);
	 * anything else => force a fresh PHY RX bring-up. */
	if (c == '2') {
		sif_i2c_scan(0);
		return l;
	}
	/* "echo 3 > ..." => EN7571 detect + register probe at 0x70 using the REAL
	 * 16-bit (wordAddrNum=2) register addressing the vendor phy_I2C_read uses.
	 * Detection (en7571_EN7571_detection @0x3425c): reg[0x170]==3 && reg[0x15c]>=3. */
	if (c == '3') {
		extern unsigned short SIF_X_Read(unsigned char, unsigned short,
			unsigned char, unsigned char, unsigned int,
			unsigned char *, unsigned short);
		unchar b[2];
		char ln[96];
		int p, got, r170 = -1, r15c = -1;
		unsigned int probe[] = { 0x170, 0x15c, 0x00, 0x02, 0x100, 0x110, 0x200 };
		unsigned int i;

		b[0] = 0xa5;
		if (SIF_X_Read(0, I2C_U2_CLK_DIV, 0x70, 2, 0x170, b, 1) == 1) r170 = b[0];
		b[0] = 0xa5;
		if (SIF_X_Read(0, I2C_U2_CLK_DIV, 0x70, 2, 0x15c, b, 1) == 1) r15c = b[0];
		printk(KERN_INFO "en7571-detect: reg[0x170]=%d reg[0x15c]=%d => %s\n",
		       r170, r15c,
		       (r170 == 3 && r15c >= 3) ? "EN7571 PRESENT" : "no/other");

		p = scnprintf(ln, sizeof(ln), "en7571 regprobe(16b):");
		for (i = 0; i < ARRAY_SIZE(probe); i++) {
			b[0] = b[1] = 0xa5;
			got = SIF_X_Read(0, I2C_U2_CLK_DIV, 0x70, 2, probe[i], b, 2);
			p += scnprintf(ln + p, sizeof(ln) - p, " [%03x]=%02x%02x%s",
				       probe[i], b[0], b[1], got == 2 ? "" : "?");
		}
		printk(KERN_INFO "%s\n", ln);
		return l;
	}
	if (c == '4') {				/* EN7571 RX bring-up + LOS/PWRADC read */
		en7571_rx_test();
		return l;
	}
	xpon_phy_rx_up = 0;			/* force a fresh bring-up on next read */
	xpon_phy_rx_bringup();
	return l;
}
static const struct proc_ops xpon_los_ops = {
	.proc_read  = xpon_los_read,
	.proc_write = xpon_los_write,
};

/* /proc/econet_xpon_optical (read): GPON downstream optical link status via the
 * EN7571 LOS comparator. Auto-brings-up the front-end on first read. */
static ssize_t xpon_optical_read(struct file *f, char __user *ub, size_t l, loff_t *o)
{
	char k[256]; int n, i, loss = 0, los;
	(void)f;
	if (*o > 0)
		return 0;
	en7571_optical_bringup();
	for (i = 0; i < 5; i++) {            /* majority vote over 5 samples */
		los = en7571_los_present();
		if (los == 1)
			loss++;
		udelay(2000);
	}
	n = scnprintf(k, sizeof(k),
		"GPON optical RX : LINK %s\n"
		"detector        : EN7571 LOS (reg 0x133 bit0; 1=loss, 0=signal)\n"
		"APD bias        : DAC 0x60 (~39V)\n"
		"loss samples    : %d/5\n",
		(loss >= 3) ? "DOWN (loss of signal)" : "UP (downstream signal present)",
		loss);
	return simple_read_from_buffer(ub, l, o, k, n);
}
static const struct proc_ops xpon_optical_ops = { .proc_read = xpon_optical_read };
static ssize_t xpon_regdump_read(struct file *f, char __user *ub, size_t l, loff_t *o)
{
	(void)f;
	return simple_read_from_buffer(ub, l, o, g_regdump, g_regdump_len);
}
static const struct proc_ops xpon_regdump_ops = { .proc_read = xpon_regdump_read };

/* /proc/econet_xpon_ddmi (read): optical transceiver DDM (SFF-8472 raw values)
 * from the MT7570/EN7571 BOSA over the internal SIF-I2C. Emits the raw 8472
 * encodings; userspace applies the standard scaling (temp 1/256 C signed,
 * VCC 100 uV/LSB, bias 2 uA/LSB, TX/RX power 0.1 uW/LSB -> dBm). The readers
 * self-guard the i2c bus, so no lock is taken here. Values are only meaningful
 * once the PHY/optics are up (calibrated during ranging); before that they may
 * read stale/zero. */
static ssize_t xpon_ddmi_read(struct file *f, char __user *ub, size_t l, loff_t *o)
{
	char k[256]; int n;
	short t; unsigned short vcc, bias, txp, rxp;
	(void)f;
	if (*o > 0)
		return 0;
	en7571_optical_bringup();
	t    = (short)mt7570_temperature_get_8472();   /* 1/256 C, signed */
	vcc  = mt7570_supply_voltage_get_8472();        /* 100 uV/LSB */
	bias = mt7570_bias_current_get_8472();          /* 2 uA/LSB */
	txp  = mt7570_TxPower_get_8472();               /* 0.1 uW/LSB */
	rxp  = mt7570_RxPower_get_8472();               /* 0.1 uW/LSB */
	n = scnprintf(k, sizeof(k),
		"temperature_8472 %d\n"
		"voltage_8472 %u\n"
		"tx_bias_8472 %u\n"
		"tx_power_8472 %u\n"
		"rx_power_8472 %u\n",
		(int)t, (unsigned)vcc, (unsigned)bias, (unsigned)txp, (unsigned)rxp);
	return simple_read_from_buffer(ub, l, o, k, n);
}
static const struct proc_ops xpon_ddmi_ops = { .proc_read = xpon_ddmi_read };

void xpon_phy_recon_register(void)
{
	proc_create("econet_xpon_phy_recon", 0644, NULL, &xpon_recon_ops);
	proc_create("econet_xpon_los", 0644, NULL, &xpon_los_ops);
	proc_create("econet_xpon_optical", 0444, NULL, &xpon_optical_ops);
	proc_create("econet_xpon_regdump", 0444, NULL, &xpon_regdump_ops);
	proc_create("econet_xpon_ddmi", 0444, NULL, &xpon_ddmi_ops);
}
/* Matching teardown (called from xpondrv_cleanup): remove the procs and undo
 * i2c_init()'s ioremap + /proc/i2c so a module reload is clean. */
void xpon_phy_recon_cleanup(void)
{
	remove_proc_entry("econet_xpon_phy_recon", NULL);
	remove_proc_entry("econet_xpon_los", NULL);
	remove_proc_entry("econet_xpon_optical", NULL);
	remove_proc_entry("econet_xpon_ddmi", NULL);
}

