/*
* File Name: gpon.c
* Description: 
*
******************************************************************/
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/types.h>

#include <gpon/gpon.h>
#include <gpon/gpon_init.h>	/* PORT: register_gpon_isr()/unregister_gpon_isr() prototypes */
#include <common/drv_global.h>
#include <common/xpon_daemon.h>
#include <common/phy_if_wrapper.h>
#include <common/xpon_led.h>


GPON_Emergence_Info gEmergenceRecord[GPON_EMERGENCE_STATE_MAX_NUM]={0};
atomic_t gEmerStateindex = ATOMIC_INIT(0);
extern void  dbgtoMem(__u32 debugLevel, char *fmt,...);

/* TODO: belongs in pwan/gpon_wan.h (pwan group); forward-declared here so the
 * gpon disable path resolves.  Defined in src/pwan/gpon_wan.c. */
extern int gwan_remove_all_gemport_for_disable(void);

/******************************************************************************
******************************************************************************/
void prepare_gpon(void)
{
#ifndef TCSUPPORT_CPU_EN7521
    if(TRUE == gpPonSysData->gswRateLimitFlag){
        XPON_STOP_TIMER(gpPonSysData->gsw_p6_rate_timer);
        xpon_disable_cpu_protection();
    }
#endif

    /*enable green drop function default*/
    xpon_set_qdma_qos(XPON_ENABLE);
	
    select_xpon_wan(PHY_GPON_CONFIG);
	XPON_PHY_SET_MODE(PHY_GPON_CONFIG);

    gpon_disable() ;
}

void schedule_gpon_dev_reset(GPON_DEV_RESET_TYPE_t type)
{
#ifndef TCSUPPORT_CPU_EN7521
    if( GPON_DEV_RESET_WITH_FE_RESET == type){
        schedule_fe_reset();
        return;
    }
#endif

    { extern int ploam_verbose; if(ploam_verbose || GPON_CURR_STATE==GPON_STATE_O5) printk(KERN_DEBUG "RNGDBG: schedule_gpon_dev_reset type=%d curr_state=O%d phy_link=%d (-> O1/O2, re-activate)\n", type, GPON_CURR_STATE, gpPhyData->phy_link_status) ; }
    if(PHY_LINK_STATUS_READY == gpPhyData->phy_link_status){
        gpon_act_change_gpon_state(GPON_STATE_O2);
    }else{
        gpon_act_change_gpon_state(GPON_STATE_O1);
    }
    
    XPON_DPRINT_MSG("#######About to do GPON MAC and XPON PHY Reset!#######\n");

    XPON_DAEMON_Job_data_t job     ;
    job.id       = XPON_DAEMON_JOB_GPON_DEV_RESET ;
    job.priority = XPON_DAEMON_JOB_PRIORITY_HIGH  ;

    xpon_daemon_job_enqueue(&job)  ;    
    wake_up_xpon_daemon() ;
}

int gpon_disable_with_option(GPON_DEV_RESET_TYPE_t reset_type)
{
	REG_G_GBL_CFG gponGlbCfg ;

	int ret = 0 ;

	unregister_gpon_isr() ;
	
	ret = gpon_qos_deinit() ;
	if(ret != 0) {
		PON_MSG(MSG_ERR, "GPON QoS deinitialization failed.\n") ;
		return ret ;
	}

	/* reset the onu_id and omcc id */
	gpGponPriv->gponCfg.onu_id = GPON_UNASSIGN_ONU_ID ;
	gpGponPriv->gponCfg.omcc = GPON_UNASSIGN_GEM_ID ;
	gpGponPriv->gponCfg.flags.isRequestKey = 0 ;
    gpGponPriv->typeBOnGoing = 0;

	/* Disable the MAC tx FEC */
	gponGlbCfg.Raw = IO_GREG(G_GBL_CFG) ;
	gponGlbCfg.Bits.us_fec_en = 0 ;
	IO_SREG(G_GBL_CFG, gponGlbCfg.Raw) ;

	gwan_remove_all_tcont() ;
	gwan_remove_all_gemport_for_disable() ;

    schedule_gpon_dev_reset(reset_type);
	return 0 ;
}

/*****************************************************************************
******************************************************************************/
int gpon_enable(void)
{
	int ret = 0 ;

	if((GPON_CURR_STATE!=GPON_STATE_O1) && 
        (GPON_CURR_STATE!=GPON_STATE_O2) &&
        (GPON_CURR_STATE!=GPON_STATE_O7)) {
		return -EFAULT ;
	}
	
	ret = gpon_dev_init() ;
	if(ret != 0) {
		PON_MSG(MSG_ERR, "GPON device initialization failed.\n") ;
		return ret ;
	}
	ret = gpon_qos_init() ;
	if(ret != 0) {
		PON_MSG(MSG_ERR, "GPON QoS initialization failed.\n") ;
		return ret ;
	}

	/* register QDMA ISR callback function */
	ret = register_gpon_isr();
	if(ret != 0) {
		PON_MSG(MSG_ERR, "Register GPON ISR handler function failed.\n") ;
		return ret ;
	}

	/* restore some configuration before gpon enable */
	gponDevSetSerialNumber(gpGponPriv->gponCfg.sn) ;
	gponDevSetEncryptKey(gpGponPriv->gponCfg.key) ;

	return 0 ;
}

#ifdef TCSUPPORT_CPU_EN7521
static void adjust_mac_internal_delay_fine_tune(void)
{
	uint psync_to_sof_delay_curr;
	uint last_dly;
	int psync_to_sof_delay_diff;
	int internalDelayFineTune;
	int retry = 10;
	int i;
	int cnt = 0;

	/* Restore first psync to sof delay */
	if(gpGponPriv->gponCfg.phy_psync_to_sof_delay != UNKNOW_PSYNC_TO_SOF_DELAY) {
		PON_MSG(MSG_DBG, "Adjust EqD: first psync_to_sof_delay 0x%x\n", gpGponPriv->gponCfg.phy_psync_to_sof_delay) ;
		/* This check is used to make sure register 0xbfaf02d4 ready */
		for(i = 0; i < retry; i++) {
			mdelay(1);
			psync_to_sof_delay_curr = IO_GMASK(0xbfaf02d4, 0xffff, 0);
			if(i == 0) {
				last_dly = psync_to_sof_delay_curr;
			} else if(last_dly == psync_to_sof_delay_curr) {
				cnt++;
			} else {
				last_dly = psync_to_sof_delay_curr;
				cnt = 0;
		}
			if(cnt == 3) {
				break;
			}
		}
		/* psync_to_sof_delay_diff > 0:phy delay is more than first 
		 * psync_to_sof_delay_diff < 0:phy delay is less than first 
		 */
		psync_to_sof_delay_diff = psync_to_sof_delay_curr - gpGponPriv->gponCfg.phy_psync_to_sof_delay;
		PON_MSG(MSG_DBG, "Adjust EqD: psync_to_sof_delay_diff d'%d\n", psync_to_sof_delay_diff) ;

		if(abs(psync_to_sof_delay_diff) <= 15) {
			/* shift psync_to_sof_delay_diff * 4 bits.
			 * The GPON MAC resolution is byte,
			 * so internalDelayFineTune = first internalDelayFineTune + psync_to_sof_delay_diff * 4 / 8
			 *                  = first internalDelayFineTune + psync_to_sof_delay_diff / 2 (bytes).
			 */
			/* internalDelayFineTune unit is 1byte time.
			 * MAC tx delay is inversely proportional to internalDelayFineTune.
			 */
			internalDelayFineTune = gpGponPriv->gponCfg.internalDelayFineTune +
									(psync_to_sof_delay_diff / 2);
	
			/* Set internalDelayFineTune */
			gponDevSetInternalDelayFineTune(internalDelayFineTune);
			PON_MSG(MSG_DBG, "Adjust EqD: internal delay fine tune 0x%x\n", internalDelayFineTune) ;
        }
    }
}
#endif /* TCSUPPORT_CPU_EN7521 */

/*****************************************************************************
******************************************************************************/
void gpon_detect_phy_ready(void)
{    
#ifdef TCSUPPORT_CPU_EN7521
	/* addjust MAC internal delay for EqD */
	if(!isFPGA) {
		adjust_mac_internal_delay_fine_tune();
	}
#endif /* TCSUPPORT_CPU_EN7521 */

	PON_MSG(MSG_INT, "GPON IRQ: gpon detect phy ready.\n") ;
	
	if(gpPonSysData->sysPrevLink == PON_LINK_STATUS_EPON) {
		xmcs_report_event(XMCS_EVENT_TYPE_EPON, XMCS_EVENT_EPON_MODE_CHANGE, 0);
	}

    if ( (TRUE == XPON_PHY_GET(PON_GET_PHY_GET_TX_POWER_EN_FLAG) ) && \
         (XPON_ROUGE_STATE_FALSE == gpPonSysData->rogue_state ) ) {
    	if(GPON_STATE_O7 != GPON_CURR_STATE) {
            change_alarm_led_status(ALARM_LED_OFF);
        } else {
#if defined(TCSUPPORT_CT_PON_CN_CN)
						change_alarm_led_status(ALARM_LED_FLICKER);
#else
						change_alarm_led_status(ALARM_LED_ON);
#endif
        }
    }
    
	if(GPON_CURR_STATE == GPON_STATE_O1) {	
		/* Change the current state */
		if(gpGponPriv->emergencystate) {
            gpon_act_change_gpon_state(GPON_STATE_O7) ;
			gpon_record_emergence_info(GPON_PHY_READY_REPORT_O7);
		} else {
            gpon_act_change_gpon_state(GPON_STATE_O2) ;
        }
	}
	
	if(gpPonSysData->sysStartup == PON_WAN_START) {
		gpPonSysData->sysLinkStatus = PON_LINK_STATUS_GPON ;
		gpon_enable() ;
	}
	
	/* report GPON event message */
	xmcs_report_event(XMCS_EVENT_TYPE_GPON, XMCS_EVENT_GPON_PHY_READY, 0) ;
}

#ifndef TCSUPPORT_CPU_EN7521
/*****************************************************************************
******************************************************************************/
void gpon_reset_dbg_delay(void)
{
    REG_DBG_DLY readVal  = {.Raw = IO_GREG(DBG_DLY)};
    REG_DBG_DLY writeVal = {.Raw = gpGponPriv->gponCfg.reg4208};
   
    if( gpGponPriv->gponCfg.rstDbgDly & MAC_4208_PHY_RX_FIX ){
        readVal.Bits.phy_rx_dly_sel = writeVal.Bits.phy_rx_dly_sel;
    }else if( gpGponPriv->gponCfg.rstDbgDly & MAC_4208_PHY_RX_DLY ){
        readVal.Bits.fix_phy_rx_dly = writeVal.Bits.fix_phy_rx_dly;
    }else if( gpGponPriv->gponCfg.rstDbgDly & MAC_4208_INTERNAL_DLY ){
        readVal.Bits.fine_int_dly = writeVal.Bits.fine_int_dly;
    }else if( gpGponPriv->gponCfg.rstDbgDly & MAC_4208_PHY_TX_DLY ){
        readVal.Bits.phy_tx_dly = writeVal.Bits.phy_tx_dly;
    }

    IO_SREG(DBG_DLY,  writeVal.Raw);
}
#endif /* TCSUPPORT_CPU_EN7521 */

#ifdef TCSUPPORT_CPU_EN7521
static void disable_sniffer_mode(void)
{
	/* store sniffer mode status */
	gponDevGetSniffMode(&gpGponPriv->gponCfg.sniffer_mode);
	/* close sniffer mode */
	gponDevEnableSniffMode(XPON_DISABLE);
}
#endif


/*****************************************************************************
******************************************************************************/
void gpon_detect_los_lof(void)
{
	PON_MSG(MSG_INT, "GPON IRQ: gpon detect los lof.\n") ;
	PON_MSG(MSG_ERR, "one bit err cnt:0x%08x\n", IO_GREG(DBG_GEM_HEC_ONE_ERR_CNT)) ; 
	PON_MSG(MSG_ERR, "two bits err cnt:0x%08x\n", IO_GREG(DBG_GEM_HEC_TWO_ERR_CNT)) ; 
	PON_MSG(MSG_ERR, "uncorrectable err cnt:0x%08x\n", IO_GREG(DBG_GEM_HEC_UC_ERR_CNT)) ; 
  
    /*turn on LED*/
    if (GPON_CURR_STATE == GPON_STATE_O7) {
#if defined(TCSUPPORT_CT_PON_CN_CN)
					change_alarm_led_status(ALARM_LED_FLICKER);
#else
					change_alarm_led_status(ALARM_LED_ON);
#endif
    }

	if((GPON_STATE_O2 == GPON_CURR_STATE) || 
	   (GPON_STATE_O3 == GPON_CURR_STATE) || 
	   (GPON_STATE_O4 == GPON_CURR_STATE)) {
		gpPonSysData->sysLinkStatus = PON_LINK_STATUS_OFF ;
		gpPhyData->phy_link_status  = PHY_LINK_STATUS_LOS ;
		/* report GPON event message */
		xmcs_report_event(XMCS_EVENT_TYPE_GPON, XMCS_EVENT_GPON_LOS, 0) ;


		//xmcs_set_connection_start(PHY_ENABLE);
		/* Change the current state */
#ifdef TCSUPPORT_CPU_EN7521
		gpon_disable() ;
#else
		gpon_disable_with_option(GPON_DEV_RESET_WITH_FE_RESET);
#endif
	} else if( GPON_STATE_O5 == GPON_CURR_STATE) {
#if defined(TCSUPPORT_RA_HWNAT) && defined(TCSUPPORT_RA_HWNAT_ENHANCE_HOOK)
		extern int is_hwnat_dont_clean;
		is_hwnat_dont_clean = 1;
#endif

#ifdef TCSUPPORT_CPU_EN7521
		/* addjust MAC internal delay for EqD */
		disable_sniffer_mode();
#else /* TCSUPPORT_CPU_EN7521 */
    	if ( (gpGponPriv->gponCfg.macPhyReset & (GPON_MAC_RESET_BIT | PHY_ANALOG_RESET_BIT | PHY_DIGITAL_RESET_BIT) ) )
    	{
    	   gpon_dev_typeB_reset();
    	}

    	if (gpGponPriv->gponCfg.rstDbgDly ){
    	   gpon_reset_dbg_delay();
    	}
#endif /* TCSUPPORT_CPU_EN7521 */
		/* Change the current state */
		gpon_act_change_gpon_state(GPON_STATE_O6) ;
	}
	
	gpGponPriv->gponCfg.flags.dvtGponLosFlag = 0 ;
}

/*****************************************************************************
******************************************************************************/
void gpon_detect_dying_gasp(void)
{
	int i ;
	
	for(i=0 ; i<GPON_REPEAT_DYING_GASP ; i++) {
		ploam_send_dying_gasp() ;
	}

	//report GPON event message
	//gpon_event_report(...) ;
}

/*****************************************************************************
******************************************************************************/
/* SESSION 12: SN launch-power-mode leveling. The vendor left this STUBBED and never wired to the
 * sn_req_crs_int ISR, so tx_power_mode (G_SN_MSG_CFG[17:16]) was frozen at 0 and the un-ranged ONU
 * could never step 0->1->2 to find a launch level the OLT's burst receiver can lock. sn_pwr: -1 =
 * step (the GPON mechanism); 0/1/2 = force that mode (A/B test which the OLT accepts). */
int sn_pwr = -1; module_param(sn_pwr, int, 0644);
/* 2026-07-07: the OLT assigns our SN an ONU-ID then Deactivates ~31ms later with no ranging. ignore_deact=1
 * makes us stay in O4 through the Deactivate, to test whether a premature deactivate (vs a genuine reject)
 * is the wall — i.e. give the OLT time to range us. Toggle live: /sys/module/econet_xpon/parameters/ignore_deact */
int ignore_deact = 0; module_param(ignore_deact, int, 0644);
/* 2026-07-08 (decisive rogue-US test): de939ca1 showed the OLT Deactivates ~6ms after
 * gwan_create_new_tcont enables the GDM2 upstream TX channel (0xBFB51524 |= (1<<ch)) at Config_PortID.
 * no_tcont_tx=1 gates OUT that TX-enable write: if the 6ms deactivate then DISAPPEARS, the port's
 * upstream TX egress is rogue/malformed (confirms the rogue-US hypothesis). If it STILL deactivates,
 * the cause is elsewhere (AES key-switch / OMCI provisioning). Toggle: params/no_tcont_tx. */
int no_tcont_tx = 0; module_param(no_tcont_tx, int, 0644);
/* 2026-07-13: mirror stock CDMA2_HWF_CHN_EN(0xBFB5140C) |= (1<<ch) in gwan_create_new_tcont (the
 * FE_API_SET_HWFWD_CHANNEL(CDMA2) is a no-op stub with no fallback). no_hwfwd_en=1 gates it OUT for A/B. */
int no_hwfwd_en = 0; module_param(no_hwfwd_en, int, 0644);
/* 2026-07-08 (no_tcont_tx REFUTED rogue-tcont-TX: OLT deactivates +8ms after Config_PortID even with the GDM2
 * TX-enable skipped). pid_skip bisects which action in ploam_recv_config_port_id's activate-body provokes the
 * OLT Deactivate. Bitmask (runtime-toggle before O5 entry): bit0=skip G_OMCI_ID program, bit1=skip
 * gwan_create_new_tcont, bit2=skip gwan_create_new_gemport, bit3=skip gponDevSetKeySwithTime (AES key-switch).
 * The Config_PortID PLOAM ACK (gpon_ploam.c:887) is ALWAYS sent. If pid_skip=0xF still deactivates -> cause is
 * upstream of Config_PortID (Request_Key/encryption) or OLT policy. */
int pid_skip = 0; module_param(pid_skip, int, 0644);
/* 2026-07-08 (PLOAMu byte-order A/B): STOCK writes the upstream PLOAMu word DIRECTLY (gpon_dev.c:87, no swab);
 * the PORT applies swab32 (gpon_dev.c:104) "symmetric" to the VALIDATED RX swab — but that TX swab is UNVALIDATED
 * and first exercised at the O5 Password/ACK, exactly where the OLT deactivates (ranging uses G_SN_MSG_CFG hw, not
 * this path). tx_ploam_swab=1 = current swab; 0 = stock-direct (no swab). If 0 stops the +6ms Config_PortID
 * deactivate -> the TX swab was garbling our O5 PLOAMu = THE bug. Runtime: params/tx_ploam_swab. */
int tx_ploam_swab = 1; module_param(tx_ploam_swab, int, 0644);
void gpon_sn_request_threshold_crossed(void)
{
	uint v = IO_GREG(G_SN_MSG_CFG) ;
	uint mode = (v >> 16) & 0x3 ;
	uint newmode = (sn_pwr >= 0) ? ((uint)sn_pwr % 3) : ((mode + 1) % 3) ;
	v = (v & ~(0x3u << 16)) | (newmode << 16) ;
	IO_SREG(G_SN_MSG_CFG, v) ;
	{ extern int ploam_verbose; if (ploam_verbose || sn_pwr >= 0)
		printk(KERN_DEBUG "gpon: SN tx_power_mode %u -> %u (G_SN_MSG_CFG=%08x)\n", mode, newmode, v) ; }
}

/*****************************************************************************
******************************************************************************/
void gpon_ber_interval_expires(struct timer_list *t)	/* PORT 6.18: timer_setup() sig */
{
	uint bip_cnt ;

	if(GPON_CURR_STATE == GPON_STATE_O5) {
		/* PORT 6.18: .data removed; re-arm with the configured ms BER interval */
		mod_timer(&gpGponPriv->ber_timer, jiffies + msecs_to_jiffies(gpGponPriv->gponCfg.berInterval)) ;
	
		bip_cnt = XPON_PHY_GET(PON_GET_PHY_BIP_COUNTER) ;
        gpGponPriv->bip_cnt_val += bip_cnt ;
		//Send a REI PLOAM message
		ploam_send_rei_msg(bip_cnt, &gpGponPriv->gponCfg.reiSeq) ;
	}
}

/*****************************************************************************
******************************************************************************/

static inline int is_same_ploam_msg_as_previous( PLOAM_RAW_General_T * pMsg)
{
    switch( pMsg->raw.msg_id ){
        case PLOAM_DOWN_MSG_RANGING_TIME:
            return !memcmp(&gpGponPriv->prePloamMsg, pMsg, 7) ;
            
        default:
            return !memcmp(&gpGponPriv->prePloamMsg, pMsg, sizeof(PLOAM_RAW_General_T)) ;
        }
}

static inline int should_ignore_ploam_msg(PLOAM_RAW_General_T * pPloamMsg)
{
    static int same_cnt = 0;
    int is_same_msg = 0;
    if(PON_WAN_START != gpPonSysData->sysStartup){
        PON_MSG(MSG_OAM, "WAN not start, ignore ploam msg:%8X\n", pPloamMsg->value[0]) ;
        return TRUE;
    }


    /* 3 consecutivly same ploam msg, only process once */
    is_same_msg = is_same_ploam_msg_as_previous(pPloamMsg);
    
    if(is_same_msg) same_cnt ++;

    if(is_same_msg && (same_cnt % 3) != 0 ){
	    PON_MSG(MSG_OAM, "Receive the PLOAM message same:%8X\n", pPloamMsg->value[0]) ;
        return TRUE;
    }else{
        memcpy(&gpGponPriv->prePloamMsg, pPloamMsg, sizeof(PLOAM_RAW_General_T)) ;
        same_cnt = 0;
    }

    return FALSE;
}

void gpon_recv_ploam_message(void)
{
	PLOAM_RAW_General_T ploamMsg;
	int depth=0;

	do {
		if((depth = gponDevGetPloamMsg(&ploamMsg)) <= 0) {
			PON_MSG(MSG_ERR, "Receive PLOAM message failed.\n") ;
            return;
		} else if(FALSE == should_ignore_ploam_msg(&ploamMsg) ){
            ploam_parser_down_message(&ploamMsg) ;
        }
	} while(depth>3) ;
}

/*****************************************************************************
******************************************************************************/
int gpon_update_time_to_switch(void)
{
	uint sec, nanosec ;
	
	/* get the current tod from MAC */
	gponDevGetCurrentTod(&sec, &nanosec) ;
//	PON_MSG(MSG_TRACE, "Get the current TOD from GPON MAC. sec:%d, nanosec:%d\n", sec, nanosec) ;
	printk("Update the TOD to switch. sec:%d, nanosec:%d.\n\n", sec, nanosec) ;
	
	/* set the current second+1 to switch and enable 1pps update */
	//Set to Switch
	
	return 0 ;	
}

/*****************************************************************************
******************************************************************************/
void gpon_isr(void)
{
	REG_G_INT_STATUS intStatus ;
	REG_G_INT_ENABLE intEnable ;
	uint sec, nanosec, spfCnt;

	intStatus.Raw = IO_GREG(G_INT_STATUS) ;
	intEnable.Raw = IO_GREG(G_INT_ENABLE) ;

		//Clear the interrupt status
		intStatus.Raw &= intEnable.Raw ;
		IO_SREG(G_INT_STATUS, intStatus.Raw) ;
		
	if(intStatus.Raw & intEnable.Raw) {
		if(intStatus.Raw & GPON_INT_INDICATION) {
			if(intStatus.Bits.dying_gasp_send_int) {
				/* Clear pre dying gasp interrupt status */
				IO_SBITS(0xBFB00084, SCU_DYING_GASP_STATUS) ; 
				PON_MSG(MSG_INT, "GPON IRQ: send a dying gasp ploam message interrupt.\n") ;
			} 
			if(intStatus.Bits.aes_key_switch_done_int) {
				PON_MSG(MSG_INT, "GPON IRQ: AES key switch done interrupt.\n") ;
			}
			if(intStatus.Bits.ranging_req_recv_int) {
				extern int ploam_verbose;
				PON_MSG(MSG_INT, "GPON IRQ: Ranging_Request received interrupt.\n") ;
				if(ploam_verbose) printk(KERN_DEBUG "RNGDBG: Ranging_Request RECEIVED in O%d (OLT wants ranging burst)\n", GPON_CURR_STATE) ;
			}
            if(intStatus.Bits.sn_onu_send_o4_int) {
				extern int ploam_verbose;
				PON_MSG(MSG_INT, "GPON IRQ: SN_ONU send in O4 state interrupt.\n") ;
				if(ploam_verbose) printk(KERN_DEBUG "RNGDBG: SN_ONU/ranging burst SENT in O4 G_ONU_ID=%08x | DBG_DLY=%08x RSP=%08x PRE_DLY=%08x GNT=%08x psync=%04x\n", IO_GREG(G_ONU_ID), IO_GREG(DBG_DLY), IO_GREG(G_RSP_TIME), IO_GREG(G_PRE_ASSIGNED_DLY), IO_GREG(DBG_BWM_GNT_STS), IO_GMASK(0xbfaf02d4u,0xffff,0)) ;
			}
			if(intStatus.Bits.sn_req_recv_int) {
				extern int ploam_verbose;
				PON_MSG(MSG_INT, "GPON IRQ: SN_Request received interrupt.\n") ;
				if(ploam_verbose) printk(KERN_DEBUG "SNDBG: SN_Request RECEIVED (OLT is asking for our SN)\n") ;
			}
			if(intStatus.Bits.sn_onu_send_o3_int) {
				extern int ploam_verbose;
				PON_MSG(MSG_INT, "GPON IRQ: SN_ONU sent in O3 state interrupt.\n") ;
				if(ploam_verbose) printk(KERN_DEBUG "SNDBG: SN_ONU SENT in O3 (MAC transmitted our SN burst)\n") ;
			}
#ifdef TCSUPPORT_CPU_EN7521
			if(intStatus.Bits.o5_eqd_adj_done_int) {
				{ extern int ploam_verbose; if(ploam_verbose) printk(KERN_DEBUG "RNGDBG: o5_eqd_adj_done_int -- EqD ACCEPTED G_EQD=%08x (RANGING SUCCEEDED)\n", IO_GREG(G_EQD)) ; }
				PON_MSG(MSG_INT, "GPON IRQ: EqD adjustment done interrupt in O5 state interrupt.\n") ;
			}
			if(intStatus.Bits.olt_ds_fec_chg_int) {
				PON_MSG(MSG_INT, "GPON IRQ: DS fec change interrupt.\n") ;
			}
			if(intStatus.Bits.onu_us_fec_chg_int) {
				PON_MSG(MSG_INT, "GPON IRQ: US fec change interrupt.\n") ;
			}
			if(intStatus.Bits.lwi_int) {
				PON_MSG(MSG_INT, "GPON IRQ: LWI interrupt.\n") ;
			}
			if(intStatus.Bits.fwi_int) {
				PON_MSG(MSG_INT, "GPON IRQ: FWI interrupt.\n") ;
			}
			if(intStatus.Bits.pop_up_recv_in_O6_int) {
				PON_MSG(MSG_INT, "GPON IRQ: POP_UP receive in O6 interrupt.\n") ;
			}
#endif			
		} 
		if(intStatus.Raw & GPON_INT_TOD) {
			if(intStatus.Bits.tod_1pps_int) {
				if(gpGponPriv->gponCfg.flags.isTodUpdate) {
					gpGponPriv->gponCfg.flags.isTodUpdate = 0 ;
					gpon_update_time_to_switch() ;
					gponDevSet1PPSInt(XPON_DISABLE) ;
				}
				PON_MSG(MSG_INT, "GPON IRQ: ToD 1PPS interrupt.\n") ;
			} 
			if(intStatus.Bits.tod_update_done_int) {
					gponDevGetCurrentTod(&sec, &nanosec) ;
					printk("Get the current TOD from GPON MAC. sec:%d, nanosec:%d, interval:%d\n", sec, nanosec, (uint)((sec-G_NEW_TOD_SEC_L32->Bits.new_tod_sec_l32)*1000000000 + (int)(nanosec-G_NEW_TOD_NANO_SEC->Bits.new_tod_nano_sec))) ;
				gpGponPriv->gponCfg.flags.isTodUpdate = 1 ;
				gponDevSet1PPSInt(XPON_ENABLE) ;
				PON_MSG(MSG_INT, "GPON IRQ: ToD updated done interrupt.\n") ;
			}
		}
		if(intStatus.Raw & GPON_INT_PLOAM) {
			if(intStatus.Bits.ploamu_send_int) {
				PON_MSG(MSG_INT, "GPON IRQ: A PLOAM message sent interrupt.\n") ;
			}
			if(intStatus.Bits.ploamd_recv_int) {
				PON_MSG(MSG_INT, "GPON IRQ: A PLOAM message received interrupt.\n") ;
				gpon_recv_ploam_message();
			}
		}
		if(intStatus.Raw & GPON_INT_ERROR) {
			if(intStatus.Bits.los_gem_del_int) {
				PON_MSG((MSG_INT|MSG_ERR), "GPON IRQ: Loss of GEM delineation interrupt.\n") ;
			}
			if(intStatus.Bits.sn_req_crs_int) {
				PON_MSG((MSG_INT|MSG_ERR), "GPON IRQ: SN_Request_Threshold crossed interrupt.\n") ;
				gpon_sn_request_threshold_crossed() ;   /* SESSION12: step SN launch power mode */
			}
			if(intStatus.Bits.rx_eof_err_int) {
				PON_MSG((MSG_INT|MSG_ERR), "GPON IRQ: PHY_RX_EOF signal error interrupt.\n") ;
			}
			if(intStatus.Bits.tx_late_start_int) {
				{ extern int ploam_verbose; if(ploam_verbose) printk(KERN_DEBUG "RNGDBG: tx_late_start_int FIRED in O%d -- TX BURST LATE vs OLT grant (DBG_DLY=%08x RSP=%08x PRE_DLY=%08x EQD=%08x GNT=%08x)\n", GPON_CURR_STATE, IO_GREG(DBG_DLY), IO_GREG(G_RSP_TIME), IO_GREG(G_PRE_ASSIGNED_DLY), IO_GREG(G_EQD), IO_GREG(DBG_BWM_GNT_STS)) ; }
				PON_MSG((MSG_INT|MSG_ERR), "GPON IRQ: Tx local time late interrupt.\n") ;
			}
			if(intStatus.Bits.bst_sgl_diff_int) {
				{ extern int ploam_verbose; if(ploam_verbose) printk(KERN_DEBUG "RNGDBG: bst_sgl_diff_int in O%d -- burst/single grant buffer mismatch\n", GPON_CURR_STATE) ; }
				PON_MSG((MSG_INT|MSG_ERR), "GPON IRQ: Burst and single grant buffer mismatch interrupt.\n") ;
				gponDevGetSuperframe(&spfCnt);
				PON_MSG((MSG_INT|MSG_ERR), "GPON IRQ: Superframe counter:%08x\n", spfCnt) ;
			}
			if(intStatus.Bits.fifo_err_int) {
				dbgtoMem(0, "GPON IRQ: FIFO error interrupt.\n") ;
			}
#ifdef TCSUPPORT_CPU_EN7521
			if(intStatus.Bits.bwm_stop_time_err_int) {
				{ extern int ploam_verbose; if(ploam_verbose) printk(KERN_DEBUG "RNGDBG: bwm_stop_time_err_int in O%d -- OLT stop-time/US-FEC window mismatch (GNT=%08x)\n", GPON_CURR_STATE, IO_GREG(DBG_BWM_GNT_STS)) ; }
				if(gpGponPriv->gponCfg.flags.isBwmStopTimeErrInt == XPON_DISABLE) {
					gponDevSetBwmStopTimeInt(XPON_DISABLE);
				}
				gponDevGetSuperframe(&spfCnt);
				PON_MSG((MSG_INT|MSG_ERR), "GPON IRQ: OLT assigned stop time error when US FEC on interrupt.\n") ;
				PON_MSG((MSG_INT|MSG_ERR), "GPON IRQ: Superframe counter:%08x\n", spfCnt) ;
			}
			if(intStatus.Bits.rx_gem_intlv_err_int) {
				PON_MSG((MSG_INT|MSG_ERR), "GPON IRQ: Rx more than two interleaved GEM fragment interrupt.\n") ;
			}
			if(intStatus.Bits.bfifo_full_int) {
				PON_MSG((MSG_INT|MSG_ERR), "GPON IRQ: bfifo full interrupt.\n") ;
			}
			if(intStatus.Bits.sfifo_full_int) {
				PON_MSG((MSG_INT|MSG_ERR), "GPON IRQ: sfifo full interrupt.\n") ;
			}
#endif
			if(intStatus.Bits.rx_err_int) {
				PON_MSG((MSG_INT|MSG_ERR), "GPON IRQ: Rx error interrupt.\n") ;
			}
		}
#ifdef XPON_MAC_CONFIG_DEBUG
		if((gpPonSysData->debugLevel&MSG_DBG) && (intStatus.Raw&GPON_INT_ERROR)) {
#ifdef TCSUPPORT_CPU_EN7521
			PON_MSG(MSG_DBG, "[4220:%.8x],[4224:%.8x],[4228:%.8x],[422C:%.8x],[4240:%.8x]\n", DBG_BWM_FILTER_CTRL->Raw, DBG_BWM_SFIFO_STS->Raw, DBG_GRP_0->Raw, DBG_GRP_1->Raw, DBG_PROBE_CTRL->Raw) ;
			IO_SBITS(DBG_BWM_FILTER_CTRL, (0x07 << 29));
#else
			PON_MSG(MSG_DBG, "[4224:%.8x],[4228:%.8x],[422C:%.8x],[4240:%.8x]\n", DBG_BWM_FIFO_STS->Raw, DBG_GRP_0->Raw, DBG_GRP_1->Raw, DBG_PROBE_CTRL->Raw) ;
#endif
			PON_MSG(MSG_DBG, "[4300:%.8x],[4304:%.8x],[4308:%.8x],[430C:%.8x],[4310:%.8x]\n\n", DBG_RX_GEM_CNT->Raw, DBG_RX_CRC_ERR_CNT->Raw, DBG_RX_GTC_CNT->Raw, DBG_TX_GEM_CNT->Raw, DBG_TX_BST_CNT->Raw) ;
			DBG_GRP_0->Raw = 0xFFFFFFFF ;
			DBG_GRP_1->Raw = 0xFFFFFFFF ;
		}
#endif
	} else {
		PON_MSG(MSG_INT, "GPON IRQ: NULL GPON ISR, Status:%.8x, Mask:%.8x\n", intStatus.Raw, intEnable.Raw) ;
	}	
}

void gpon_record_emergence_info(unchar event)
{
	GPON_Emergence_Info *info = NULL;

	if(event >= GPON_EMERGENCE_STATE_MAX_NUM)
	{
		PON_MSG(MSG_ERR,"%s event exceed max num\n",__FUNCTION__);
		return;
	}
	
	if(atomic_read(&gEmerStateindex) >= GPON_EMERGENCE_STATE_MAX_NUM)
	{
		atomic_set(&gEmerStateindex,0);
		PON_MSG(MSG_DBG, "gEmerStateindex exceed max num set 0\n");
	}
	PON_MSG(MSG_DBG, "record gEmerStateindex %d \n",atomic_read(&gEmerStateindex));
	
	info = &gEmergenceRecord[atomic_read(&gEmerStateindex)];
	
	memset(info, 0, sizeof(GPON_Emergence_Info));
	
	info->isHappen = TRUE;
	info->time = ktime_get();
	info->event = event;
	info->emergenceState = gpGponPriv->gponCfg.emergencystate;
	atomic_inc(&gEmerStateindex);
}

static inline void gpon_print_time_information(ktime_t * time)
{    
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,36)
	u32 nsec = time->tv.nsec % 1000;
	u32 usec = (time->tv.nsec / 1000) % 1000;    
	u32 msec = (time->tv.nsec / 1000000) % 1000;   
	u32 sec = time->tv.sec % 60;
	u32 min = time->tv.sec / 60;
	printk("[%03dmin : %03ds : %03dms: %03dus : %03dns] ", min, sec, msec, usec, nsec);
#endif
}

void gpon_show_emergence_info(void)
{
	int i = 0;
	int printId = 0;
	GPON_Emergence_Info *info = NULL;
	
	for(i = 0; i < GPON_EMERGENCE_STATE_MAX_NUM; i++)
	{
		printId = (i + atomic_read(&gEmerStateindex)) % GPON_EMERGENCE_STATE_MAX_NUM;
		info = &gEmergenceRecord[printId];
		
		PON_MSG(MSG_DBG, "show printId %d, event %u ishappen %u\n",printId,info->event,info->isHappen);
		
		if(info->isHappen == TRUE)
		{
			gpon_print_time_information(&info->time);
			switch(info->event)
			{
				case GPON_DISABLE_SN_REPORT_O7:
					printk("GPON RCV DISABLE_SN REPORTO7          ");
					break;
				case GPON_DISABLE_SN_SET_EMERGNCE_STATE:
					printk("GPON RCV DISABLE_SN SET_EMERGNCE_STATE");
					break;
				case GPON_OMCI_SET_EMERGNCE_STATE:
					printk("GPON OMCI SET EMERGNCE_STATE          ");
					break;
				case GPON_PHY_READY_REPORT_O7:
					printk("GPON PHY READY REPORT_O7              ");
					break;
				case GPON_SET_CONNECTION_REPORT_O7:
					printk("GPON SET CONNECTION START REPORT_O7   ");
					break;	
				default:
					printk("No Info   ");
					break;
			}
			printk("  emergence state %s\n",(info->emergenceState)?"ON":"OFF");
		}
	}
}

void gpon_stop_timer(void)
{
	GPON_STOP_TIMER(gpGponPriv->to1_timer) ;
	GPON_STOP_TIMER(gpGponPriv->to2_timer) ;
	GPON_STOP_TIMER(gpGponPriv->ber_timer) ;
	GPON_STOP_TIMER(gpWanPriv->gpon.gemMibTimer) ;
    GPON_STOP_TIMER(gpWanPriv->txDropTimer) ;    
    stop_omci_oam_monitor();
}



