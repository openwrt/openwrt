/*
* File Name: gpon_act.c
* Description: GPON ONU activation
*
******************************************************************/
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

#include <common/drv_global.h>
#include <common/phy_if_wrapper.h>
#include <common/xpon_led.h>
#include <gpon/gpon_qos.h>
#include <gpon/gpon.h>

#if defined(TCSUPPORT_XPON_LED)
#include <asm/tc3162/ledcetrl.h>
extern int led_xpon_status;
#endif

struct tasklet_struct taskLedConfig;
struct tasklet_struct gponRecvPloamMessageTask;

extern int is_hwnat_dont_clean;

#ifndef TCSUPPORT_CPU_EN7521
/* fe_reset_flag only be used in MT7520 for 
  * reseting PON PHY. 
  * It is used when traffic and then PHY link down.
  */
extern atomic_t fe_reset_flag;/*here_20150620*/
#endif /* TCSUPPORT_CPU_EN7521 */

#define TO1_TIMEOUT_MAX_CNT	20
atomic_t to1_timeout_cnt = ATOMIC_INIT(TO1_TIMEOUT_MAX_CNT);
atomic_t to1_rst_cnt = ATOMIC_INIT(0);
/*****************************************************************************
******************************************************************************/
void gpon_act_to1_timer_expires(struct timer_list *t)	/* PORT 6.18: timer_setup() callback sig */
{

#ifndef TCSUPPORT_CPU_EN7521
		if(atomic_read(&fe_reset_flag) ) {
			return;
		}
#endif	

	if(GPON_CURR_STATE==GPON_STATE_O3 || GPON_CURR_STATE==GPON_STATE_O4) {
		{ extern int ploam_verbose; if(ploam_verbose) printk(KERN_DEBUG "RNGDBG: TO1 (10s) timeout in O%d -> O2\n", GPON_CURR_STATE) ; }
		PON_MSG((MSG_ERR|MSG_ACT), "TO1 timer timeout. Current State:O%d, Next State:O2\n", G_ACTIVATION_ST->Bits.act_st) ;

		if(atomic_dec_and_test(&to1_timeout_cnt)){
			PON_MSG((MSG_ERR|MSG_ACT),"%s TO1_TIMEOUT_MAX_CNT:%d trigger mac & phy reset.\r\n", __FUNCTION__ , TO1_TIMEOUT_MAX_CNT);
			xmcs_set_connection_start(XPON_ENABLE);
			/*Reset the to1_timeout_cnt*/
			atomic_set(&to1_timeout_cnt,TO1_TIMEOUT_MAX_CNT);
			atomic_inc(&to1_rst_cnt);
		}

		/* Change the current state */		
		gpon_act_change_gpon_state(GPON_STATE_O2) ;
	}
}

/*****************************************************************************
******************************************************************************/
void gpon_act_to2_timer_expires(struct timer_list *t)	/* PORT 6.18: timer_setup() callback sig */
{
	if(GPON_STATE_O6 != GPON_CURR_STATE){
        return ;
    }
        
	PON_MSG((MSG_ERR|MSG_ACT), "TO2 timer timeout. Current State:O%d, Next State:O1\n", G_ACTIVATION_ST->Bits.act_st) ;
	/*enable qdma green drop function */
	xpon_set_qdma_qos(XPON_ENABLE);

	gpPonSysData->sysLinkStatus = PON_LINK_STATUS_OFF ;

	gpPonSysData->sysPrevLink = PON_LINK_STATUS_GPON ;
	
	if(likely(gpGponPriv->gponCfg.flags.hotplug == XPON_DISABLE)) {
    	is_hwnat_dont_clean = 0; /* For real unplug fiber */
	}
    gpPhyData->phy_link_status = PHY_LINK_STATUS_LOS;
    xmcs_report_event(XMCS_EVENT_TYPE_GPON, XMCS_EVENT_GPON_LOS, 0) ;
	
    stop_omci_oam_monitor();

#ifdef TCSUPPORT_CPU_EN7521
	gpon_disable() ;
#else
	gpon_disable_with_option(GPON_DEV_RESET_WITH_FE_RESET);
#endif
}

/*****************************************************************************
******************************************************************************/
void gpon_act_change_gpon_state(const uint new_state)
{
    REG_G_ACTIVATION_ST gponActState ;
    PHY_GponPreb_T pon_preb;
    uint last_state;
    ulong flags = 0;
    char same_state = 0;
    spin_lock_irqsave(&gpGponPriv->act_lock, flags) ;
    
    if ( new_state == GPON_CURR_STATE )
    {
        PON_MSG(MSG_OAM, "PLOAM: State non-change: original = O%d  new = O%d\n",GPON_CURR_STATE, new_state) ; 
        same_state = 1;
    }

    /* ★★ SESSION 12 ROOT-CAUSE FIX (SN=0 at O3): the SN was programmed into G_VENDOR_ID/G_VS_SN
     * ONLY by gpon_enable() (gpon.c:134), which is AFTER register_gpon_isr(); on k6.18 that QDMA-hook
     * registration returns early, so gponDevSetSerialNumber() was SKIPPED and both SN registers stayed
     * 0x0. Activation still reached O3 via the PLOAM poll, but the MAC transmitted an ALL-ZERO Serial
     * Number => the OLT can never Assign_ONU-ID => O3 stall (the real reason, mistaken for "optical").
     * Re-program the SN here (idempotent) so it is always present before the autonomous O3 SN burst. */
    if (new_state == GPON_STATE_O2 || new_state == GPON_STATE_O3 || new_state == GPON_STATE_O4)
        gponDevSetSerialNumber(gpGponPriv->gponCfg.sn) ;

    /* SESSION 12: set/step the SN launch-power-mode before each O3 SN burst (the sn_req_crs_int ISR
     * is unreliable on k6.18, so drive it here too). sn_pwr=-1 steps 0->1->2 across O3 entries. */
    if (new_state == GPON_STATE_O3) {
        extern void gpon_sn_request_threshold_crossed(void) ;
        gpon_sn_request_threshold_crossed() ;
    }

    if(new_state == GPON_STATE_O5) {
        pon_preb.mask = PHY_OPER_RANG_EN;
        pon_preb.oper_ranged_st = 0x3;
        XPON_PHY_SET_API(PON_SET_PHY_GPON_EXTEND_PREAMBLE, &pon_preb);
#ifdef TCSUPPORT_CPU_EN7521
        /* Enable filter upstream overhead and extended burst length PLOAM */
        gponDevSetFilterExtBurstLengthPLOAM(XPON_ENABLE);
        gponDevSetFilterUpstreamOverheadPLOAM(XPON_ENABLE);

        /* Store first psync to sof delay */
        if(gpGponPriv->gponCfg.phy_psync_to_sof_delay == UNKNOW_PSYNC_TO_SOF_DELAY) {
            gpGponPriv->gponCfg.phy_psync_to_sof_delay = IO_GMASK(0xbfaf02d4, 0xffff, 0);
        }
#endif
    } else if (new_state == GPON_STATE_O4 || new_state == GPON_STATE_O3) {
        pon_preb.mask = PHY_OPER_RANG_EN;
        pon_preb.oper_ranged_st = 0x2;
        /* FIX 2 (2026-07-04): apply the cached extended T3 preamble immediately on O3/O4 entry — the
         * MAC fires SN autonomously here and the OLT's burst receiver needs the ~119B T3 preamble to
         * AGC/CDR-lock. Without this the first SN bursts go out with 0 preamble -> the clone's O3 stall. */
        if (gpGponPriv->gponCfg.extBurstValid) {
            REG_G_PLOu_PRMBL_TYPE3 t3p ;
            pon_preb.mask |= PHY_T3_O4_PRE_EN | PHY_T3_O5_PRE_EN | PHY_EXT_BUR_MODE_EN ;
            pon_preb.t3_O4_preamble  = gpGponPriv->gponCfg.o3T3Cache - gpGponPriv->gponCfg.t3PreambleOffset ;
            pon_preb.t3_O5_preamble  = gpGponPriv->gponCfg.o5T3Cache ;
            pon_preb.extend_burst_mode = 1 ;
            XPON_PHY_SET_API(PON_SET_PHY_GPON_EXTEND_PREAMBLE, &pon_preb);
            /* mirror into the MAC preamble register (as ploam_recv_extended_burst_length does) */
            t3p.Raw = IO_GREG(G_PLOu_PRMBL_TYPE3) ;
            t3p.Bits.ebl_en = 1 ;
            t3p.Bits.ext_prmb3_o5_num    = gpGponPriv->gponCfg.o5T3Cache ;
            t3p.Bits.ext_prmb3_o3_o4_num = gpGponPriv->gponCfg.o3T3Cache - gpGponPriv->gponCfg.t3PreambleOffset ;
            IO_SREG(G_PLOu_PRMBL_TYPE3, t3p.Raw) ;
            gpGponPriv->gponCfg.gponT3Prmbl.Raw = t3p.Raw ;
        } else {
            XPON_PHY_SET_API(PON_SET_PHY_GPON_EXTEND_PREAMBLE, &pon_preb);
        }
    } else if(new_state == GPON_STATE_O2) {
        pon_preb.mask = PHY_T3_O4_PRE_EN | PHY_T3_O5_PRE_EN | PHY_EXT_BUR_MODE_EN | PHY_OPER_RANG_EN;
        pon_preb.t3_O4_preamble = 0;
        pon_preb.t3_O5_preamble = 0;
        pon_preb.extend_burst_mode = 0;
        pon_preb.oper_ranged_st = 0;
        XPON_PHY_SET_API(PON_SET_PHY_GPON_EXTEND_PREAMBLE, &pon_preb);
        XPON_PHY_SET_BIT_DELAY(0);
        #if 0
        if (phy_rx_fec_status()==PHY_TRUE)
            IO_SREG(G_RSP_TIME,  gpGponPriv->gponCfg.onuResponseTime + 0x20);
        else
        #endif
        /* 2026-07-07 TX-timing sweep: make po_rsp LIVE (re-read sysfs each O2) + apply the fine/phy TX-delay
         * knobs BEFORE the O3 SN burst. Defaults (0x577/0x1C/-1) reproduce current behaviour exactly. */
        { extern int po_rsp, po_finetune, po_phytxdly;
          gpGponPriv->gponCfg.onuResponseTime = (ushort)po_rsp;
          gponDevSetResponseTime(gpGponPriv->gponCfg.onuResponseTime);
          if (po_finetune >= 0) { gpGponPriv->gponCfg.internalDelayFineTune = (unchar)po_finetune;
                                  gponDevSetInternalDelayFineTune((unchar)po_finetune); }
          if (po_phytxdly >= 0) { REG_DBG_DLY _d; _d.Raw = IO_GREG(DBG_DLY);
                                  _d.Bits.phy_tx_dly = (uint)po_phytxdly & 0xff; IO_SREG(DBG_DLY, _d.Raw); } }
    }
    
    /* Set the current state to MAC register */
    gponActState.Raw = IO_GREG(G_ACTIVATION_ST) ;
    last_state = gponActState.Bits.act_st;
    gponActState.Bits.act_st = new_state ;
    IO_SREG(G_ACTIVATION_ST, gponActState.Raw) ;

    /* Stop the TO1 or TO2 timer */
//  if((GPON_CURR_STATE == GPON_STATE_O3) || (GPON_CURR_STATE == GPON_STATE_O4)) {
//      GPON_STOP_TIMER(gpGponPriv->to1_timer) ;
//  } else if(GPON_CURR_STATE == GPON_STATE_O6) {
//      GPON_STOP_TIMER(gpGponPriv->to2_timer) ;
//  }
    PON_MSG(MSG_OAM, "PLOAM: State change O%d --->  O%d\n", GPON_CURR_STATE, new_state) ; 

    GPON_CURR_STATE = new_state;
    
    if( GPON_STATE_O5 == new_state ){
        /*Reset the to1_timeout_cnt*/
        atomic_set(&to1_timeout_cnt,TO1_TIMEOUT_MAX_CNT);
        gpGponPriv->bip_cnt_val = 0;
        /* clear bip error counter */
        XPON_PHY_COUNTER_CLEAR(PHY_BIP_CNT_CLR);
    }
    else if (new_state != GPON_STATE_O5) {
        /* Disable All upstream */
        gpWanPriv->devCfg.flags.isTxDropOmcc = 1;
        xpon_set_qdma_qos(XPON_ENABLE);
    }

    /* Start the TO1 or TO2 timer */
    if((GPON_CURR_STATE == GPON_STATE_O3) || (GPON_CURR_STATE == GPON_STATE_O4)) {
        PON_MSG(MSG_ACT, "Start the T1 timer. Current State:O%d\n", G_ACTIVATION_ST->Bits.act_st) ;
        /* PORT 6.18: timer_list lost .data; arm directly with the configured ms timeout */
        mod_timer(&gpGponPriv->to1_timer, jiffies + msecs_to_jiffies(gpGponPriv->gponCfg.to1Timer)) ;
    } else if(GPON_CURR_STATE == GPON_STATE_O6) {
        PON_MSG(MSG_ACT, "Start the T2 timer. Current State:O%d\n", G_ACTIVATION_ST->Bits.act_st) ;
        mod_timer(&gpGponPriv->to2_timer, jiffies + msecs_to_jiffies(gpGponPriv->gponCfg.to2Timer)) ;
    }
#if defined(TCSUPPORT_XPON_HAL_API) || defined(TCSUPPORT_XPON_HAL_API_EXT) || defined(TCSUPPORT_FWC_ENV)
    if(!same_state){
        xmcs_report_event(XMCS_EVENT_TYPE_GPON, XMCS_EVENT_GPON_STATE_CHANGE, new_state) ;
    }
#endif
    tasklet_schedule(&taskLedConfig);
    spin_unlock_irqrestore(&gpGponPriv->act_lock, flags) ;
    PON_MSG(MSG_ACT, "Change the current state: O%d. Last State:O%d\n", G_ACTIVATION_ST->Bits.act_st, last_state) ;     
}

/*****************************************************************************
******************************************************************************/
uint gpon_act_get_gpon_state(void)
{
	return GPON_CURR_STATE;
}

void gpon_act_led_config(void)
{
	if(isFPGA)  return ;
#if defined(TCSUPPORT_XPON_LED)
#if !defined(TCSUPPORT_C5_XPON_AUTH_LED) 
    if(GPON_CURR_STATE == GPON_STATE_O1 || GPON_CURR_STATE == GPON_STATE_O6 || GPON_CURR_STATE == GPON_STATE_O7)
#endif
	{
        	if(GPON_CURR_STATE == GPON_STATE_O7)
        	{
#if defined(TCSUPPORT_CT_PON_CN_CN)
			change_alarm_led_status(ALARM_LED_FLICKER);
#else
            change_alarm_led_status(ALARM_LED_ON);
#endif
       		}
		ledTurnOff(LED_XPON_STATUS);		
		ledTurnOff(LED_XPON_TRYING_STATUS);
#if defined(TCSUPPORT_C9_ROST_LED)
		ledTurnOn(LED_XPON_UNREG_STATUS);
#endif

		led_xpon_status = 0;
	} else if (GPON_CURR_STATE == GPON_STATE_O5) {
#if defined(TCSUPPORT_C9_ROST_LED)
		ledTurnOff(LED_XPON_STATUS);
		ledTurnOff(LED_XPON_UNREG_STATUS);
		ledTurnOn(LED_XPON_TRYING_STATUS); 				
#else	
		ledTurnOff(LED_XPON_TRYING_STATUS);		
		ledTurnOn(LED_XPON_STATUS);
		change_alarm_led_status(ALARM_LED_OFF);
#endif
		led_xpon_status = 2;		
	}else{

#if defined(TCSUPPORT_C9_ROST_LED)
		ledTurnOff(LED_XPON_STATUS);
		ledTurnOff(LED_XPON_TRYING_STATUS);
		ledTurnOn(LED_XPON_UNREG_STATUS); 				
#else		
#if !defined(TCSUPPORT_TW_BOARD_CDS) 
		ledTurnOff(LED_XPON_STATUS);	
		ledTurnOn(LED_XPON_TRYING_STATUS);
#endif
#endif
		led_xpon_status = 1;
	}
#endif
}

/*****************************************************************************
******************************************************************************/
/* PORT 6.18: tasklet_init() callback is void(unsigned long); gpon_act_led_config
 * is void(void).  Use tasklet_setup() + a trampoline to keep led_config intact. */
/* ★ PON status LED (2026-07-08): the port's ledTurnOn/change_alarm_led_status are stubbed on EN7528, so the
 * front LEDs never reflect status. Drive the front PON LED directly. From H660 stock DS_CONFIG_LED_PON it is a
 * dual-color LED on GPIO3 (color A / "green") + GPIO32 (color B / "red"). EN7528 GPIO regs (phy_reg.h): bank0
 * DATA 0xbfbf0204 / OE 0xbfbf0214 (GPIO0-31); bank1 DATA1 0xbfbf0270 / OE1 0xbfbf0278 (GPIO32+). Behaviour:
 * O5 = green solid (registered); O2/O3/O4 = red (ranging/searching); O1/other = off. If the physical colours
 * come out swapped, flip the GPIO3<->GPIO32 assignment (polarity from stock config, not re-verified live). */
static void gpon_pon_led_update(void)
{
	unsigned int d0 = IO_GREG(0xbfbf0204u), oe0 = IO_GREG(0xbfbf0214u);
	unsigned int d1 = IO_GREG(0xbfbf0270u), oe1 = IO_GREG(0xbfbf0278u);
	int green = (GPON_CURR_STATE == GPON_STATE_O5);
	int red   = (GPON_CURR_STATE == GPON_STATE_O2 || GPON_CURR_STATE == GPON_STATE_O3 ||
		     GPON_CURR_STATE == GPON_STATE_O4);
	/* Live-verified 2026-07-08: GPIO32 = physically GREEN, GPIO3 = physically RED (opposite of the stock-config
	 * guess). So: registered(O5) -> GPIO32 green; ranging -> GPIO3 red. */
	oe0 |= (1u << 3);   d0 = red   ? (d0 | (1u << 3)) : (d0 & ~(1u << 3));   /* GPIO3  = red   */
	oe1 |= (1u << 0);   d1 = green ? (d1 | (1u << 0)) : (d1 & ~(1u << 0));   /* GPIO32 = green */
	IO_SREG(0xbfbf0214u, oe0);  IO_SREG(0xbfbf0204u, d0);
	IO_SREG(0xbfbf0278u, oe1);  IO_SREG(0xbfbf0270u, d1);
	printk(KERN_DEBUG "PONLED: O%d -> %s\n", GPON_CURR_STATE, green ? "GREEN(registered)" : red ? "RED(ranging)" : "OFF");
}

static void gpon_act_led_config_tasklet(struct tasklet_struct *t)
{
    gpon_pon_led_update();
    gpon_act_led_config();
}

int gpon_act_init(void)
{
    tasklet_setup(&taskLedConfig, gpon_act_led_config_tasklet);
	gpGponPriv->state = GPON_STATE_O1;
	
	/* PORT 6.18: init_timer()+.data+.function -> timer_setup(); the per-timer
	 * ms timeout (was stashed in .data) is now read from gponCfg at arm time. */
	timer_setup(&gpGponPriv->to1_timer, gpon_act_to1_timer_expires, 0) ;
	timer_setup(&gpGponPriv->to2_timer, gpon_act_to2_timer_expires, 0) ;

	return 0 ;	
}

/*****************************************************************************
*function :
*		gpon_act_deinit
*description : 
*		This function is used to deinit gpon act.  
*input :
*		N/A
*output :
*		return 0 on success
******************************************************************************/
int gpon_act_deinit(void)
{
	/* kill led task */
	tasklet_kill(&taskLedConfig);
	return 0 ;
}

