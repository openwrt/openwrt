/*
* File Name: gpon_ploam.c
* Description: PLOAM message related function for GPON
*
******************************************************************/
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/random.h>

#include "common/drv_global.h"
#include "common/phy_if_wrapper.h"
#include "gpon/gpon.h"

#include "xmcs/xmcs_sdi.h"
#include "gpon/gpon_power_management.h"
#include "common/xpon_daemon.h"
#include "gpon/gpon_qos.h"

#ifdef TCSUPPORT_CPU_EN7521
#include <ecnt_hook/ecnt_hook_fe.h>
#endif 

#ifdef TCSUPPORT_AUTOBENCH
#include <linux/proc_fs.h>
#endif

#include <linux/module.h>
#include <linux/moduleparam.h>
/* SESSION 12 burst-TX-timing fix (stock@O5 vs port@O3 MAC-reg diff, 0xbfb64090 block):
 *   G_PLOu_GUARD_BIT (0x4094): stock=0x18(24), port=0x20(32) <- "FIX 3" copied OLT raw.gbits(0x20);
 *     stock IGNORES the OLT gbits and uses const 24. po_gbits default 24 = stock; -1 = old raw.gbits.
 *   G_RSP_TIME  (0x40ac): stock=0x577, port=0x58b. po_rsp default 0x577 = stock. */
int po_gbits = 24;      module_param(po_gbits, int, 0644);
int po_rsp   = 0x577;   module_param(po_rsp,   int, 0644);
/* sn_vs: if non-zero, override the last 4 (vendor-specific) SN bytes, keeping the "DSNW" vendor prefix,
 * to test a FRESH/random SN that can't collide with the stock firmware's active OLT session. */
int sn_vs    = 0;       module_param(sn_vs,   int, 0644);
/* 2026-07-07 TX-burst-timing sweep (ranging-arrival). po_finetune -> DBG_DLY.fine_int_dly[15:8] (0x1C default =
 * current init seed; MAC tx delay INVERSELY proportional, ~6.43ns/LSB byte-time). po_phytxdly -> DBG_DLY
 * phy_tx_dly[7:0] (-1 = leave HW default untouched). Applied at O2 entry (before the O3 SN burst). Defaults =
 * current behaviour so the build is behaviorally identical until swept via sysfs. */
int po_finetune = 0x1C; module_param(po_finetune, int, 0644);
int po_phytxdly = -1;   module_param(po_phytxdly, int, 0644);

/******************************************************************************
******************************************************************************/
void ploam_eqd_adjustment(uint newEqd)
{
	REG_G_EQD gponEqd ;
	uint phyRegData = 0 ;
	int deltaEqd = newEqd - gpGponPriv->gponCfg.eqd ;
	int K, A, B, a, intByteDelay ;
#ifdef TCSUPPORT_CPU_EN7521
	REG_DBG_TX_SYNC_OFFSET txSyncOffset;
#else
	REG_DBG_PROBE_LOW32 gponDebugProbe ;
#endif	
#ifdef TCSUPPORT_CUC
    if(gpGponPriv->gponCfg.dis_ranging_in_o5) {
        if(deltaEqd > 16 || deltaEqd < -16) {
            PON_MSG(MSG_OAM, "Delta Eqd is bigger than 16!\n");
            return;
        }
    }
#endif
	if(gpGponPriv->gponCfg.eqdO5Offset == 0) {
		gpGponPriv->gponCfg.eqd = newEqd ;
	} else {
		/* For O5 EqD offset test */
		if(gpGponPriv->gponCfg.flags.eqdOffsetFlag == GPON_EQD_OFFSET_FLAG_SUBTRACT) {
			deltaEqd -= gpGponPriv->gponCfg.eqdO5Offset ;
			gpGponPriv->gponCfg.eqd = newEqd - gpGponPriv->gponCfg.eqdO5Offset;
		} else {
			deltaEqd += gpGponPriv->gponCfg.eqdO5Offset ;
			gpGponPriv->gponCfg.eqd = newEqd + gpGponPriv->gponCfg.eqdO5Offset;
		}
	}

#ifdef TCSUPPORT_CPU_EN7521
	/* Get MAC internal byte delay*/
	txSyncOffset.Raw = IO_GREG(DBG_TX_SYNC_OFFSET) ;
	intByteDelay = txSyncOffset.Bits.dbg_tx_sync_offset ;
#else
	/* Get MAC internal byte delay from debug probe */				
	IO_SREG(DBG_PROBE_CTRL, 0x0000000C) ;
	gponDebugProbe.Raw = IO_GREG(DBG_PROBE_LOW32) ;
	intByteDelay = (gponDebugProbe.Raw>>23) & 3 ;
#endif
	
	if(deltaEqd > 0) {
		K = deltaEqd + gpGponPriv->gponCfg.bitDelay ;
		A = K >> 3 ;
		B = K & 7 ;
		//a = (((intByteDelay+(A&3)) <= 3) ? 0 : 8) + (A>>2) - (A&3) ;
		a=(((intByteDelay + (A&3)) <= 3) ? 0 : 8) + (A - (A&3)) - (A&3);
		gpGponPriv->gponCfg.byteDelay += (a<<3) ;
		gpGponPriv->gponCfg.bitDelay = B ;
	} else if(deltaEqd < 0) {
		K = 0 - deltaEqd - gpGponPriv->gponCfg.bitDelay ;
		A = (K>0) ? ((K>>3)+((K&7)?1:0)) : 0 ;
		B = (A<<3) - K ; 
		//a = ((intByteDelay >= (A&3)) ? 0 : 8) + (A>>2) - (A&3) ;
		a=((intByteDelay >= (A&3)) ? 0 : 8) + (A - (A&3)) - (A&3);
		gpGponPriv->gponCfg.byteDelay -= (a<<3) ;
		gpGponPriv->gponCfg.bitDelay = B ;
	} else {
		return ;
	}

	if(gpGponPriv->gponCfg.eqdO5Offset != 0) {
		/* For O5 EqD offset test */
		PON_MSG(MSG_OAM, "PLOAM: main path EqD in O5, %x, deltaEqd = %x.\n", gpGponPriv->gponCfg.eqd, deltaEqd) ;
	}
	PON_MSG(MSG_OAM, "PLOAM: Setting the main path EqD in O5, ByteDelay:%x, BitDelay:%x. (A:%x, B:%x, byte_dly:%x, a:%x)\n", gpGponPriv->gponCfg.byteDelay, gpGponPriv->gponCfg.bitDelay, A, B, intByteDelay, a) ;
	
	/* Setting the EqD to MAC register */
	gponEqd.Raw = IO_GREG(G_EQD) ;
	gponEqd.Bits.eqd = gpGponPriv->gponCfg.byteDelay ;
	IO_SREG(G_EQD, gponEqd.Raw) ;
	XPON_PHY_SET_BIT_DELAY(gpGponPriv->gponCfg.bitDelay);
}

/******************************************************************************
******************************************************************************/
int ploam_parser_down_message(PLOAM_RAW_General_T *pGenPloamMsg)
{
	int ret = -EFAULT ;
	uint msgId = pGenPloamMsg->raw.msg_id ;
	ploam_recv_handler_t ploam_recv_handler ;
	
	if(msgId>=PLOAM_DOWN_MAX_TYPE || gpGponPriv->ploamRecvHandler[msgId]==NULL) {
		PON_MSG((MSG_ERR|MSG_OAM), "PLOAM: receive unknow message.(%.8X %.8X %.8X)\n", pGenPloamMsg->value[0], pGenPloamMsg->value[1], pGenPloamMsg->value[2]) ;
		return -EFAULT ;
	}
	
	ploam_recv_handler = gpGponPriv->ploamRecvHandler[msgId] ;
	ret = ploam_recv_handler(pGenPloamMsg) ;

	return ret ;
}


/******************************************************************************
******************************************************************************/
static int ploam_send_passwd_msg(unchar *passwd, unchar len)
{
	PLOAM_RAW_Password_T pdMsg ;
		
	memset(&pdMsg, 0, sizeof(PLOAM_RAW_Password_T)) ;
	pdMsg.raw.dest_id = GPON_ONU_ID ;
	pdMsg.raw.msg_id = PLOAM_UP_MSG_PASSWORD_MESSAGE ;
	memcpy(pdMsg.raw.passwd, passwd,  len) ;

	/* Set the MAC registedr to send PLOAM message */
	PON_MSG(MSG_OAM, "PLOAM: Send password message.(%.8X %.8X %.8X)\n", pdMsg.value[0], pdMsg.value[1], pdMsg.value[2]) ; 
	gponDevSendPloamMsg((PLOAM_RAW_General_T *)&pdMsg, 3) ;
	
	return 0 ;
}

/******************************************************************************
******************************************************************************/
int ploam_send_dying_gasp(void)
{
	PLOAM_RAW_Dying_Gasp_T dgMsg ;

	memset(&dgMsg, 0, sizeof(PLOAM_RAW_Dying_Gasp_T)) ;
	dgMsg.raw.dest_id = GPON_ONU_ID ;
	dgMsg.raw.msg_id = PLOAM_UP_MSG_DYING_GASP ;
	
	/* Set the MAC registedr to send PLOAM message */
	PON_MSG(MSG_OAM, "PLOAM: Send dying gasp message.(%.8X %.8X %.8X)\n", dgMsg.value[0], dgMsg.value[1], dgMsg.value[2]) ; 
	gponDevSendPloamMsg((PLOAM_RAW_General_T *)&dgMsg, 3) ;

	return 0 ;
}


/******************************************************************************
******************************************************************************/
static int ploam_send_encryption_key(unchar key_idx, unchar *key)
{
	PLOAM_RAW_Encryption_Key_T encryMsg ;
	int i ;

	memset(&encryMsg, 0, sizeof(PLOAM_RAW_Encryption_Key_T)) ;
	encryMsg.raw.dest_id = GPON_ONU_ID ;
	encryMsg.raw.msg_id = PLOAM_UP_MSG_ENCRYPTION_KEY ;
	for(i=0 ; i<2 ; i++) {
		encryMsg.raw.key_idx = key_idx ;
		encryMsg.raw.frag_idx = i ;
		memcpy(encryMsg.raw.key, key+(i*8),  8) ;
		
		/* Set the MAC registedr to send PLOAM message */
		PON_MSG(MSG_OAM, "PLOAM: Send encryption key message.(%.8X %.8X %.8X)\n", encryMsg.value[0], encryMsg.value[1], encryMsg.value[2]) ; 
		gponDevSendPloamMsg((PLOAM_RAW_General_T *)&encryMsg, 3) ;
	}
	
	return 0 ;
}

/******************************************************************************
******************************************************************************/
int ploam_send_pee_msg(void)
{
	PLOAM_RAW_PEE_T peeMsg ;
	
	memset(&peeMsg, 0, sizeof(PLOAM_RAW_PEE_T)) ;
	peeMsg.raw.dest_id = GPON_ONU_ID ;
	peeMsg.raw.msg_id = PLOAM_UP_MSG_PEE_MESSAGE ;

	/* Set the MAC registedr to send PLOAM message */
	PON_MSG(MSG_OAM, "PLOAM: Send PEE message.(%.8X %.8X %.8X)\n", peeMsg.value[0], peeMsg.value[1], peeMsg.value[2]) ; 
	gponDevSendPloamMsg((PLOAM_RAW_General_T *)&peeMsg, 1) ;

	return 0 ;
}

/******************************************************************************
******************************************************************************/
int ploam_send_pst_msg(unchar line_num, unchar k1_ctl, unchar k2_ctl)
{
	PLOAM_RAW_PST_T pstMsg ;
	
	memset(&pstMsg, 0, sizeof(PLOAM_RAW_PST_T)) ;
	pstMsg.raw.dest_id = GPON_ONU_ID ;
	pstMsg.raw.msg_id = PLOAM_UP_MSG_PST_MESSAGE ;
	pstMsg.raw.line_num = line_num ;
	pstMsg.raw.k1_ctrl = k1_ctl ;
	pstMsg.raw.k2_ctrl = k2_ctl ;

	/* Set the MAC registedr to send PLOAM message */
	PON_MSG(MSG_OAM, "PLOAM: Send PST message.(%.8X %.8X %.8X)\n", pstMsg.value[0], pstMsg.value[1], pstMsg.value[2]) ; 
	gponDevSendPloamMsg((PLOAM_RAW_General_T *)&pstMsg, 1) ;

	return 0 ;
}

/******************************************************************************
******************************************************************************/
int ploam_send_rei_msg(uint err_count, unchar *seq_num_p)
{
	PLOAM_RAW_REI_T reiMsg ;
	
	memset(&reiMsg, 0, sizeof(PLOAM_RAW_REI_T)) ;
	reiMsg.raw.dest_id = GPON_ONU_ID ;
	reiMsg.raw.msg_id = PLOAM_UP_MSG_REI_MESSAGE ;
	reiMsg.raw.counter[0] = err_count>>24 ;
	reiMsg.raw.counter[1] = err_count>>16 ;
	reiMsg.raw.counter[2] = err_count>>8 ;
	reiMsg.raw.counter[3] = err_count ;
	reiMsg.raw.seq_num = *seq_num_p ;

	/* Set the MAC registedr to send PLOAM message */
	PON_MSG(MSG_OAM, "PLOAM: Send REI message.(%.8X %.8X %.8X)\n", reiMsg.value[0], reiMsg.value[1], reiMsg.value[2]) ; 
	gponDevSendPloamMsg((PLOAM_RAW_General_T *)&reiMsg, 1) ;
	
	*seq_num_p = (*seq_num_p+1) & 0xF ;

	return 0 ;
}


/******************************************************************************
******************************************************************************/
static int ploam_send_acknowledge_msg(unchar dm_id, unchar *msg)
{
	PLOAM_RAW_Acknowledge_T ackMsg ;

	PON_MSG(MSG_OAM, "PLOAM: Send acknowledge PLOAM message.\n") ;
	
	ackMsg.raw.dest_id = GPON_ONU_ID ;
	ackMsg.raw.msg_id = PLOAM_UP_MSG_ACKNOWLEDGE ;
	ackMsg.raw.dm_id = dm_id ;
	memcpy(ackMsg.raw.dm_byte, msg, 9) ;
	
	/* Set the MAC registedr to send PLOAM message */
	PON_MSG(MSG_OAM, "PLOAM: Send acknowledge message.(%.8X %.8X %.8X)\n", ackMsg.value[0], ackMsg.value[1], ackMsg.value[2]) ; 
	gponDevSendPloamMsg((PLOAM_RAW_General_T *)&ackMsg, CONFIG_SEND_ACK_PLOAMU) ;
	
	return 0 ;
}

/******************************************************************************
 Descriptor:	To send sleep_request PLOAM.
 Input Args:	sleepMode: 0: Doze.
                                  1: Sleep.
                                  2: WSleep.
 Ret Value:		0: Success.
******************************************************************************/
int ploam_send_sleep_request_msg(GPON_PLOAMu_SLEEP_MODE_t sleepMode)
{
	PLOAM_RAW_Sleep_Request_T sleepRequestMsg ;
	
	memset(&sleepRequestMsg, 0, sizeof(PLOAM_RAW_Sleep_Request_T)) ;
	sleepRequestMsg.raw.dest_id = GPON_ONU_ID ;
	sleepRequestMsg.raw.msg_id = PLOAM_UP_MSG_SLEEP_REQUEST_MESSAGE ;
	sleepRequestMsg.raw.sleep_mode = sleepMode ;

	/* Set the MAC registedr to send PLOAM message */
	PON_MSG(MSG_OAM, "PLOAM: Send Sleep_Request message.(%.8X %.8X %.8X)\n", sleepRequestMsg.value[0], sleepRequestMsg.value[1], sleepRequestMsg.value[2]) ; 
	gponDevSendPloamMsg((PLOAM_RAW_General_T *)&sleepRequestMsg, 1) ;
	
	return 0 ;
}

#define PHY_TX_EN_BIT_LEN_CONST (24)
#define PHY_TX_EN_PATTERN (0xAA)

/******************************************************************************
******************************************************************************/
static int ploam_recv_upstream_overhead(PLOAM_RAW_Upstream_Overhead_T *pUpOverheadMsg)
{
	REG_G_PLOu_GUARD_BIT gponGuardBit ;
	REG_G_PLOu_PRMBL_TYPE1_2 gponPrmblType ;
	REG_G_PRE_ASSIGNED_DLY gponPreAssignDly ;
    PHY_GPON_Delimiter_Guard_t pat = {0} ;

	PON_MSG(MSG_OAM, "PLOAM: Rx Upstream_Overhead.(%.8X %.8X %.8X),state O%d\n", 
            pUpOverheadMsg->value[0], 
            pUpOverheadMsg->value[1], 
            pUpOverheadMsg->value[2],
            GPON_CURR_STATE) ; 
	//PON_MSG(MSG_OAM, "PLOAM: Current State O%d\n", GPON_CURR_STATE) ; 

	if(pUpOverheadMsg->raw.dest_id != PLOAM_BROADCAST_ADDR) {
		PON_MSG((MSG_OAM|MSG_ERR), "The dest. address(%.2x) of PLOAM message is incorrect.\n", pUpOverheadMsg->raw.dest_id) ;
		return -1 ;
	}

	if(GPON_CURR_STATE == GPON_STATE_O2) {

		/* Arm the upstream transceiver (session 7: was MT7520-only; enable for A60928 too).
		 * On A60928 this un-inverts the burst so the laser follows the GPON MAC's grant-timed
		 * burst-enable -> the MAC bursts the SN only in the OLT's allocated SN/ranging windows
		 * (standard GPON, not a forced-on laser). disableSnFlag keeps it quiet if set. */
		if (!gpGponPriv->disableSnFlag)
			XPON_PHY_TX_ENABLE();

		uint phyRegData;
		PHY_GponPreb_T pon_preb;
		PPHY_GponPreb_T p_pon_preb = & pon_preb;
		p_pon_preb->mask = PHY_GUARD_BIT_NUM_EN |  PHY_PRE_T1_NUM_EN | PHY_PRE_T2_NUM_EN | PHY_PRE_T3_PAT_EN;
		p_pon_preb->guard_bit_num = (po_gbits >= 0) ? po_gbits : pUpOverheadMsg->raw.gbits;   /* SESSION12: stock uses 24, not OLT's 0x20 */
		p_pon_preb->preamble_t1_num = pUpOverheadMsg->raw.t1_pbits;   /* FIX 5: was t2 (swapped) */
		p_pon_preb->preamble_t2_num = pUpOverheadMsg->raw.t2_pbits;
		p_pon_preb->preamble_t3_pat = pUpOverheadMsg->raw.t3_pbits;
        XPON_PHY_SET_API(PON_SET_PHY_GPON_PREAMBLE, p_pon_preb);
		phyRegData = ((pUpOverheadMsg->raw.delimiter[0]<<16) | (pUpOverheadMsg->raw.delimiter[1]<<8) | (pUpOverheadMsg->raw.delimiter[2]<<0)) ;

        pat.delimiter  = phyRegData;
        pat.guard_time = PHY_TX_EN_PATTERN;
        XPON_PHY_SET_API(PON_SET_PHY_GPON_DELIMITER_GUARD, &pat);

		/* Setting the MAC register */
		gponGuardBit.Raw = IO_GREG(G_PLOu_GUARD_BIT) ;
		gponGuardBit.Bits.guard_bit = (po_gbits >= 0) ? po_gbits : pUpOverheadMsg->raw.gbits ;   /* SESSION12: stock uses 24 */
		IO_SREG(G_PLOu_GUARD_BIT, gponGuardBit.Raw) ;
        gpGponPriv->gponCfg.gponGuardBit.Raw = gponGuardBit.Raw;

		gponPrmblType.Raw = IO_GREG(G_PLOu_PRMBL_TYPE1_2) ;
		gponPrmblType.Bits.prmb1_bit = pUpOverheadMsg->raw.t1_pbits ;
		gponPrmblType.Bits.prmb2_bit = pUpOverheadMsg->raw.t2_pbits ;
		IO_SREG(G_PLOu_PRMBL_TYPE1_2, gponPrmblType.Raw) ;
        gpGponPriv->gponCfg.gponPrmblType.Raw = gponPrmblType.Raw;

		gponPreAssignDly.Raw = IO_GREG(G_PRE_ASSIGNED_DLY) ;
		gponPreAssignDly.Bits.pre_dly_en = pUpOverheadMsg->raw.delay_mode ;
		gponPreAssignDly.Bits.pre_dly = ((pUpOverheadMsg->raw.delay_time[0]<<8) | pUpOverheadMsg->raw.delay_time[1]) ;
		IO_SREG(G_PRE_ASSIGNED_DLY, gponPreAssignDly.Raw) ;
        gpGponPriv->gponCfg.gponPreAssignDly.Raw = gponPreAssignDly.Raw;

        gpGponPriv->gponCfg.gponDelmBit.Raw  = IO_GREG(G_PLOu_DELM_BIT);
        
		//G_PLOu_DELM_BIT->Bits.delm_bit = ?? ;
		//G_PLOu_OVERHEAD->Bits.plou_overhead = ?? ;

		/* Change the current state */
		gpon_act_change_gpon_state(GPON_STATE_O3) ;
	}
    

	return 0 ;
}


/******************************************************************************
******************************************************************************/
static int ploam_recv_assign_onu_id(PLOAM_RAW_Assign_OnuID_T *pAssignOnuIdMsg)
{
	REG_G_ONU_ID gponOnuId ;
	
	PON_MSG(MSG_OAM, "PLOAM: Receive Assign_ONU_ID message.(%.8X %.8X %.8X)\n", pAssignOnuIdMsg->value[0], pAssignOnuIdMsg->value[1], pAssignOnuIdMsg->value[2]) ; 

	if(pAssignOnuIdMsg->raw.dest_id != PLOAM_BROADCAST_ADDR) {
		PON_MSG((MSG_ERR|MSG_OAM), "The dest. address(%.2x) of PLOAM message is incorrect.\n", pAssignOnuIdMsg->raw.dest_id) ;
		return -1 ;
	}
		
	/* Compare the serial number of system and Assign_ONU-ID PLOAM message */
	if(memcmp(gpGponPriv->gponCfg.sn, pAssignOnuIdMsg->raw.sn, 8)) {
		PON_MSG((MSG_ERR|MSG_OAM), "The serial number of PLOAM message is incorrect\n") ;
		return -1 ;
	}

	{ extern int ploam_verbose; if(ploam_verbose) printk(KERN_DEBUG "RNGDBG: Assign_ONU_ID SN-MATCH, curr_state=O%d onu_id=0x%02x\n", GPON_CURR_STATE, pAssignOnuIdMsg->raw.onu_id) ; }

	/* FIX (2026-07-07): the OLT assigned OUR SN, but the O3->O2 flap (FE-reset/LOS) often drops us to O2
	 * before this Assign is processed, so the original `== O3` guard rejected a valid Assign -> we never
	 * reached O4/ranging. Accept the SN-matched Assign in O2 as well (the MAC did the SN handshake in HW). */
	if(GPON_CURR_STATE == GPON_STATE_O3 || GPON_CURR_STATE == GPON_STATE_O2) {
		/* Change the current state */
		/* modify for CIG IOT testing */
		gpon_act_change_gpon_state(GPON_STATE_O4) ;

		/* Setting the ONU ID to MAC register */
		gponOnuId.Raw = IO_GREG(G_ONU_ID) ;
		gponOnuId.Bits.onu_id_vld = 1 ;
		gponOnuId.Bits.onu_id = pAssignOnuIdMsg->raw.onu_id ;
		IO_SREG(G_ONU_ID, gponOnuId.Raw) ;

		/* Store the latest ONU ID (GPON_ONU_ID)*/
		gpGponPriv->gponCfg.onu_id = pAssignOnuIdMsg->raw.onu_id ;
		gpWanPriv->gpon.allocId[0] = gpGponPriv->gponCfg.onu_id;
		gpWanPriv->activeChannelNum = 1;
	}
	
	return 0 ;
}

/* ★ SESSION-11 DIAG: manually force the GPON activation SM O3->O4->O5 WITHOUT the OLT, to validate the
 * port's O4/O5 code path (confirm the ONLY real blocker is the optical burst decode, not a missing SM
 * step). Uses the SM's OWN transition fn gpon_act_change_gpon_state() (so the SM's internal state truly
 * advances and won't self-revert like a bare register poke) + replicates ploam_recv_assign_onu_id (O4)
 * and ploam_recv_ranging_time O4-branch (O5) with a SYNTHETIC onu_id + EqD=0. Diagnostic FICTION: no
 * real OLT session/grants. RUN WITH LASER OFF (tx_laser_off=1) => zero rogue emission even though MBI
 * is started. stage=4 -> O4, stage=5 -> O5. */
void gpon_force_state_test(int stage, int onu_id)
{
	REG_G_ONU_ID  gponOnuId ;
	REG_G_GBL_CFG gponGlbCfg ;
	REG_G_EQD     gponEqd ;

	if (stage == 4) {
		printk(KERN_DEBUG "gpon_force: current=O%d -> forcing O4 (synthetic assign onu_id=0x%02x)\n",
			GPON_CURR_STATE, onu_id) ;
		gpon_act_change_gpon_state(GPON_STATE_O4) ;      /* SM transition: act_st=O4 + O4 ranging preamble */
		gponOnuId.Raw = IO_GREG(G_ONU_ID) ;
		gponOnuId.Bits.onu_id_vld = 1 ;
		gponOnuId.Bits.onu_id = onu_id ;
		IO_SREG(G_ONU_ID, gponOnuId.Raw) ;
		gpGponPriv->gponCfg.onu_id = onu_id ;
		gpWanPriv->gpon.allocId[0] = onu_id ;
		gpWanPriv->activeChannelNum = 1 ;
		printk(KERN_DEBUG "gpon_force: O4 applied. act_st=O%u G_ONU_ID=%08x\n",
			IO_GREG(0xbfb640bcu) & 0xfu, IO_GREG(G_ONU_ID)) ;
	} else if (stage == 5) {
		printk(KERN_DEBUG "gpon_force: current=O%d -> forcing O5 (synthetic ranging EqD=0 + US-FEC + MBI)\n",
			GPON_CURR_STATE) ;
		gpGponPriv->gponCfg.eqd = 0 ;
		gpGponPriv->gponCfg.byteDelay = 0 ;
		gpGponPriv->gponCfg.bitDelay = 0 ;
		gponEqd.Raw = IO_GREG(G_EQD) ;
		gponEqd.Bits.eqd = 0 ;
		IO_SREG(G_EQD, gponEqd.Raw) ;
		XPON_PHY_SET_BIT_DELAY(0) ;
		gponGlbCfg.Raw = IO_GREG(G_GBL_CFG) ;
		gponGlbCfg.Bits.us_fec_en = 1 ;
		IO_SREG(G_GBL_CFG, gponGlbCfg.Raw) ;
		gponDevMbiStop(XPON_DISABLE) ;                    /* start MAC<->PHY MBI (laser-off => no emission) */
		gpon_act_change_gpon_state(GPON_STATE_O5) ;
		printk(KERN_DEBUG "gpon_force: O5 applied. act_st=O%u\n", IO_GREG(0xbfb640bcu) & 0xfu) ;
	}
}

#ifdef TCSUPPORT_AUTOBENCH
int gpon_slt_test =0;
EXPORT_SYMBOL(gpon_slt_test);
#endif
/******************************************************************************
******************************************************************************/
static int ploam_recv_ranging_time(PLOAM_RAW_Ranging_Time_T *pRangingMsg)
{
    REG_G_EQD gponEqd ;
    REG_G_GBL_CFG gponGlbCfg ;
    uint phyRegData = 0 ;
    uint newEqd ;
#ifdef TCSUPPORT_CPU_EN7521
	QDMA_RxLowThreshold_T rxLowThrhPtr;
#endif
    
    PON_MSG(MSG_OAM, "PLOAM: Receive Ranging_Time message.(%.8X %.8X %.8X)\n", pRangingMsg->value[0], pRangingMsg->value[1], pRangingMsg->value[2]) ; 

    if(pRangingMsg->raw.dest_id != GPON_ONU_ID) {
        PON_MSG((MSG_ERR|MSG_OAM), "The dest. address(%.2x) of PLOAM message is incorrect.\n", pRangingMsg->raw.dest_id) ;
        return -1 ;
    }

    if((GPON_CURR_STATE == GPON_STATE_O4) || (GPON_CURR_STATE == GPON_STATE_O5)) {
        /* Setting the EqD to MAC register */
        if(pRangingMsg->raw.eqd_type == 0) {
            newEqd = ((pRangingMsg->raw.delay[0]<<24) | (pRangingMsg->raw.delay[1]<<16) | (pRangingMsg->raw.delay[2]<<8) | (pRangingMsg->raw.delay[3])) ;   
            PON_MSG((MSG_OAM|MSG_EQD), "PLOAM: Receive the main path EqD, %x.\n", newEqd) ;

            if(GPON_CURR_STATE == GPON_STATE_O4) {
                gpGponPriv->gponCfg.eqd = newEqd ; /* record the eqd value before adjust the offset */
        
                /* Adjust the eqd value for eqd debug */
                if(gpGponPriv->gponCfg.eqdO4Offset) {
                    if(gpGponPriv->gponCfg.flags.eqdOffsetFlag == GPON_EQD_OFFSET_FLAG_SUBTRACT) {
                        newEqd -= gpGponPriv->gponCfg.eqdO4Offset ;
                        PON_MSG(MSG_OAM, "PLOAM: Adjust the main path EqD in O4, Offset:%x.\n", gpGponPriv->gponCfg.eqdO4Offset) ;
                    } else {
                        newEqd += gpGponPriv->gponCfg.eqdO4Offset ;
                        PON_MSG(MSG_OAM, "PLOAM: Adjust the main path EqD in O4, Offset:%x.\n", gpGponPriv->gponCfg.eqdO4Offset) ;
                    }
                }
                
                gpGponPriv->gponCfg.byteDelay = newEqd & BYTE_DELAY_MASK ;
                gpGponPriv->gponCfg.bitDelay = newEqd & BIT_DELAY_MASK ;
                PON_MSG(MSG_OAM, "PLOAM: Setting the main path EqD in O4, ByteDelay:%x, BitDelay:%x.\n", gpGponPriv->gponCfg.byteDelay, gpGponPriv->gponCfg.bitDelay) ;
                /* Setting the EqD to MAC register */
                gponEqd.Raw = IO_GREG(G_EQD) ;
                gponEqd.Bits.eqd = gpGponPriv->gponCfg.byteDelay ;
                IO_SREG(G_EQD, gponEqd.Raw) ;

                XPON_PHY_SET_BIT_DELAY(gpGponPriv->gponCfg.bitDelay);
                
            } else if(GPON_CURR_STATE == GPON_STATE_O5) {
                ploam_eqd_adjustment(newEqd) ;
            }
        } else if (pRangingMsg->raw.eqd_type == 1) {
            PON_MSG(MSG_OAM, "PLOAM: Setting the protection path EqD.\n") ;
        }
    }

    if(GPON_CURR_STATE == GPON_STATE_O4) {
        /* Enable the MAC tx FEC */
        gponGlbCfg.Raw = IO_GREG(G_GBL_CFG) ;
        gponGlbCfg.Bits.us_fec_en = 1 ;
        IO_SREG(G_GBL_CFG, gponGlbCfg.Raw) ;
        if (!gpGponPriv->typeBOnGoing){
			/* START GPON/PSE MBI Interface */
			gponDevMbiStop(XPON_DISABLE) ;
#ifndef TCSUPPORT_CPU_EN7521
			enable_cpu_us_omci_oam_traffic() ;
            disable_cpu_us_data_traffic()    ;
#else
			enable_cpu_us_traffic(); /* enable cpu traffic */
#endif
	        start_omci_oam_monitor();
		}
        /* Change the current state */
        gpon_act_change_gpon_state(GPON_STATE_O5) ;
		gpGponPriv->typeBOnGoing = 0;

#if defined(TCSUPPORT_FWC_ENV)
		gpon_recover_create_allocId();
		gpon_recover_create_gemport();
#endif

#ifdef TCSUPPORT_AUTOBENCH
		if(gpon_slt_test!=1){
	        printk("gpon O5 pass\n");
	        gpon_slt_test = 1;
		}
#endif
        
        /* report GPON event message */
        xmcs_report_event(XMCS_EVENT_TYPE_GPON, XMCS_EVENT_GPON_ACTIVATE, 0) ;
    }
    
    return 0 ;
}

/******************************************************************************
******************************************************************************/
#ifdef TCSUPPORT_VNPTT
	int ignore_deactive_flag = 0;
#endif
/* 2026-07-08: dump GPON-MAC (base 0xBFB64000) DS RX + GEM-extraction counters, to answer whether the OLT
 * sends us OMCI GEM in the O5 window (before deactivating) and whether it reaches CPU extraction. Read-only. */
static void gpon_dump_mac_rxcnt(const char *w)
{
	printk(KERN_DEBUG "OMCIDBG-RXCNT[%s]: rxGEM=%08x crcErr=%08x rxGTC=%08x txGEM=%08x txBST=%08x hec1=%08x hec2=%08x hecUC=%08x dsSPF=%08x gtcExtr=%08x dsExtrEth=%08x usExtrEth=%08x\n", w,
		*(volatile unsigned int *)0xBFB64300u, *(volatile unsigned int *)0xBFB64304u,
		*(volatile unsigned int *)0xBFB64308u, *(volatile unsigned int *)0xBFB6430Cu,
		*(volatile unsigned int *)0xBFB64310u, *(volatile unsigned int *)0xBFB64330u,
		*(volatile unsigned int *)0xBFB64334u, *(volatile unsigned int *)0xBFB64338u,
		*(volatile unsigned int *)0xBFB64358u, *(volatile unsigned int *)0xBFB64368u,
		*(volatile unsigned int *)0xBFB64374u, *(volatile unsigned int *)0xBFB64378u);
}

static int ploam_recv_deactivate_onu(PLOAM_RAW_Deactivate_OnuID_T *pDeactMsg)
{
	uint spf;

	gponDevGetSuperframe(&spf);
	PON_MSG(MSG_OAM, "PLOAM: Receive Deactivate_OnuID message.(%.8X %.8X %.8X), SPF:0x%08x\n", pDeactMsg->value[0], pDeactMsg->value[1], pDeactMsg->value[2], spf) ; 
	PON_MSG(MSG_ERR, "one bit err cnt:0x%08x\n", IO_GREG(DBG_GEM_HEC_ONE_ERR_CNT)) ; 
	PON_MSG(MSG_ERR, "two bits err cnt:0x%08x\n", IO_GREG(DBG_GEM_HEC_TWO_ERR_CNT)) ; 
	PON_MSG(MSG_ERR, "uncorrectable err cnt:0x%08x\n", IO_GREG(DBG_GEM_HEC_UC_ERR_CNT)) ; 
        
	if((pDeactMsg->raw.dest_id != GPON_ONU_ID) && (pDeactMsg->raw.dest_id != PLOAM_BROADCAST_ADDR)) {
		PON_MSG((MSG_ERR|MSG_OAM), "The dest. address(%.2x) of PLOAM message is incorrect.\n", pDeactMsg->raw.dest_id) ;
		return -1 ;
	}
	{ printk(KERN_DEBUG "RNGDBG: Deactivate_ONU_ID recv dest=0x%02x in O%d\n", pDeactMsg->raw.dest_id, GPON_CURR_STATE) ; }
	gpon_dump_mac_rxcnt("DEACT") ;
	{ extern int ignore_deact; if(ignore_deact && (GPON_CURR_STATE==GPON_STATE_O4 || (ignore_deact>=2 && GPON_CURR_STATE==GPON_STATE_O5))) { printk(KERN_DEBUG "RNGDBG: IGNORING Deactivate in O%d (ignore_deact=%d) -> hold state\n", GPON_CURR_STATE, ignore_deact) ; return 0 ; } }
#ifdef TCSUPPORT_VNPTT
	if(ignore_deactive_flag)
		return 0;
#endif			
	if(GPON_CURR_STATE==GPON_STATE_O4 || GPON_CURR_STATE==GPON_STATE_O5 || 
	   GPON_CURR_STATE==GPON_STATE_O6) {
		/*enable qdma green drop function */
		xpon_set_qdma_qos(XPON_ENABLE);
		
        xmcs_report_event(XMCS_EVENT_TYPE_GPON, XMCS_EVNET_GPON_DEACTIVATE, 0) ;

        stop_omci_oam_monitor() ;
        
#ifdef TCSUPPORT_CPU_EN7521
		gpon_disable();
#else
		if (XPON_PHY_GET(PON_GET_PHY_LOS_STATUS) == PHY_LOS_HAPPEN)
		{
			gpon_act_change_gpon_state(GPON_STATE_O1) ;
            gpPhyData->phy_link_status = PHY_LINK_STATUS_LOS;
			gpon_disable_with_option(GPON_DEV_RESET_WITH_FE_RESET) ;

			PON_MSG(MSG_ERR,"ploam_recv_deactivate_onu and PHY_LOS_HAPPEN report los interrupt \n");

        
		}else{
		    gpon_disable_with_option(GPON_DEV_RESET_WITH_FE_RESET);
        }
#endif
    }
	
	return 0 ;
}

/******************************************************************************
******************************************************************************/
static int ploam_recv_disable_serial_number(PLOAM_RAW_Disable_SN_T *pDisSnMsg)
{
	PON_MSG(MSG_OAM, "PLOAM: Receive Disable_SN message.(%.8X %.8X %.8X)\n", pDisSnMsg->value[0], pDisSnMsg->value[1], pDisSnMsg->value[2]) ; 

	if(pDisSnMsg->raw.dest_id != PLOAM_BROADCAST_ADDR) {
		PON_MSG((MSG_ERR|MSG_OAM), "The dest. address(%.2x) of PLOAM message is incorrect.\n", pDisSnMsg->raw.dest_id) ;
		return -1 ;
	}
	
	if(GPON_CURR_STATE == GPON_STATE_O7) {
		if((pDisSnMsg->raw.mode == PLOAM_DISABLE_PARTICIPATE_ALL) || 
		  ((pDisSnMsg->raw.mode == PLOAM_DISABLE_PARTICIPATE) && !memcmp(gpGponPriv->gponCfg.sn, pDisSnMsg->raw.sn, 8))) {		
		  
			gpGponPriv->emergencystate = 0;
			gpon_act_change_gpon_state(GPON_STATE_O2) ;
			XPON_PHY_TX_ENABLE();

	        xmcs_report_event(XMCS_EVENT_TYPE_GPON, XMCS_EVENT_GPON_ENABLE, 0) ;
		}
	} else if(GPON_CURR_STATE != GPON_STATE_O1) {
		if((pDisSnMsg->raw.mode == PLOAM_DISABLE_DENIED_ALL) || 
          ((pDisSnMsg->raw.mode == PLOAM_DISABLE_DENIED) && !memcmp(gpGponPriv->gponCfg.sn, pDisSnMsg->raw.sn, 8))) {
			/* Change the current state */
			gpon_act_change_gpon_state(GPON_STATE_O7) ;
            XPON_PHY_TX_DISABLE();
	        xmcs_report_event(XMCS_EVENT_TYPE_GPON, XMCS_EVENT_GPON_DISABLE, 0) ;
			gpGponPriv->emergencystate = 1;
			gpon_record_emergence_info(GPON_DISABLE_SN_REPORT_O7);
			gpon_record_emergence_info(GPON_DISABLE_SN_SET_EMERGNCE_STATE);
		}
	}
	
	return 0 ;
}

/******************************************************************************
******************************************************************************/
static int ploam_recv_encrypt_port_id(PLOAM_RAW_Encrypted_PortID_T *pEncryMsg)
{
	ushort port_id ;
	
	PON_MSG(MSG_OAM, "PLOAM: Receive Encrypted_PortID message.(%.8X %.8X %.8X)\n", pEncryMsg->value[0], pEncryMsg->value[1], pEncryMsg->value[2]) ;
	printk(KERN_DEBUG "OMCIDBG: Encrypted_PortID RECV in O%d dest=0x%02x (%.8X %.8X %.8X)\n", GPON_CURR_STATE, pEncryMsg->raw.dest_id, pEncryMsg->value[0], pEncryMsg->value[1], pEncryMsg->value[2]) ;

	if(pEncryMsg->raw.dest_id != GPON_ONU_ID) {
		PON_MSG((MSG_ERR|MSG_OAM), "The dest. address(%.2x) of PLOAM message is incorrect.\n", pEncryMsg->raw.dest_id) ;
		return -1 ;
	}
	
	port_id = ((pEncryMsg->raw.port_id_m<<4) | (pEncryMsg->raw.port_id_l)) ;
				
	/* Check the Port-ID is valid or not */
	/* In some case, the OLT could send the Encrypted_PortID PLOAM 
	   before creating GEM port via OMCI */

	/* only record the GEM port encry info here*/
	gpWanPriv->gpon.gemIdToIndex[port_id] &= GPON_GEM_IDX_MASK ;
	gpWanPriv->gpon.gemIdToIndex[port_id] |= ((pEncryMsg->raw.encrypt==0b11)?GPON_GEM_ENCRY_MASK:0) ;
	if(gwan_is_gemport_valid(port_id) == 1) 		
		gwan_config_gemport(port_id, ENUM_CFG_ENCRYPTION, ((pEncryMsg->raw.encrypt==0b11)?1:0)) ;
#if 0
	if(gwan_is_gemport_valid(port_id) != 1) {
		gwan_create_new_gemport(port_id, GPON_UNKNOWN_CHANNEL, ((pEncryMsg->raw.encrypt==0b11)?1:0)) ;
	} else {
		gwan_config_gemport(port_id, ENUM_CFG_ENCRYPTION, ((pEncryMsg->raw.encrypt==0b11)?1:0)) ;
	}
#endif


	/* Send acknowledge PLOAM message */
	ploam_send_acknowledge_msg(pEncryMsg->raw.msg_id, (unchar *)pEncryMsg) ;
	
	return 0 ;
}


/******************************************************************************
******************************************************************************/
static int ploam_recv_request_passwd(PLOAM_RAW_Request_Password_T *pReqPasswdMsg)
{
	PON_MSG(MSG_OAM, "PLOAM: Receive Request_Password message.(%.8X %.8X %.8X)\n", pReqPasswdMsg->value[0], pReqPasswdMsg->value[1], pReqPasswdMsg->value[2]) ;
	printk(KERN_DEBUG "OMCIDBG: Request_Password RECV in O%d dest=0x%02x\n", GPON_CURR_STATE, pReqPasswdMsg->raw.dest_id) ;
	gpon_dump_mac_rxcnt("REQ_PW") ;

	if(pReqPasswdMsg->raw.dest_id != GPON_ONU_ID) {
		PON_MSG((MSG_ERR|MSG_OAM), "The dest. address(%.2x) of PLOAM message is incorrect.\n", pReqPasswdMsg->raw.dest_id) ;
		return -1 ;
	}
	
	if(GPON_CURR_STATE == GPON_STATE_O5) {
		/* three Password PLOAM messages */
		ploam_send_passwd_msg(gpGponPriv->gponCfg.passwd, 10) ;
	}
	
	return 0 ;
}

/******************************************************************************
******************************************************************************/
static int ploam_recv_assign_alloc_id(PLOAM_RAW_Assign_AllocID_T *pAssAllocIdMsg)
{
	ushort allocId ;
	
	PON_MSG(MSG_OAM, "PLOAM: Receive Assign_AllocID message.(%.8X %.8X %.8X)\n", pAssAllocIdMsg->value[0], pAssAllocIdMsg->value[1], pAssAllocIdMsg->value[2]) ; 

	if(pAssAllocIdMsg->raw.dest_id != GPON_ONU_ID) {
		PON_MSG((MSG_ERR|MSG_OAM), "The dest. address(%.2x) of PLOAM message is incorrect.\n", pAssAllocIdMsg->raw.dest_id) ;
		return -1 ;
	}
	
	if(GPON_CURR_STATE == GPON_STATE_O5) {	
		allocId = ((pAssAllocIdMsg->raw.alloc_id_m<<4) | (pAssAllocIdMsg->raw.alloc_id_l)) ;

		if(allocId != gpGponPriv->gponCfg.onu_id) {
			/* Setting the MAC register, set the Alloc-ID valid */
			if(pAssAllocIdMsg->raw.type == 0x01 /*Allocate in GEM*/) 
			{
				if(FALSE == gwanCheckAllocIdExist(allocId))
				{
					if (0 ==gwan_create_new_tcont(allocId)){
						gpWanPriv->activeChannelNum ++;
						if (gpWanPriv->activeChannelNum > CONFIG_GPON_MAX_TCONT) {
							printk("AllocId has exceed the max num %d\n", CONFIG_GPON_MAX_TCONT);
							gpWanPriv->activeChannelNum = CONFIG_GPON_MAX_TCONT;
						}
						xpon_reset_qdma_tx_buf();
					}
				}
				/* report GPON event message */
				xmcs_report_event(XMCS_EVENT_TYPE_GPON, XMCS_EVENT_GPON_TCONT_ALLOCED, allocId) ;
			} else if(pAssAllocIdMsg->raw.type == 0xFF /*Deallocate*/) {
				if ( 0 == gwan_remove_tcont(allocId)){
					gpWanPriv->activeChannelNum --;
					if (gpWanPriv->activeChannelNum < 1)
						gpWanPriv->activeChannelNum = 1;
					xpon_reset_qdma_tx_buf();
				}
			}
		}
		PON_MSG(MSG_OAM, "PLOAM: OLT Assign ALLOC_ID %d\n", allocId) ;
	}
	
	/* Send acknowledge PLOAM message */
	ploam_send_acknowledge_msg(pAssAllocIdMsg->raw.msg_id, (unchar *)pAssAllocIdMsg) ;	

	return 0 ;
}

/******************************************************************************
******************************************************************************/
static int ploam_recv_popup_msg(PLOAM_RAW_Popup_T *pPopupMsg)
{
	PON_MSG(MSG_OAM, "PLOAM: Receive Popup message.(%.8X %.8X %.8X)\n", pPopupMsg->value[0], pPopupMsg->value[1], pPopupMsg->value[2]) ; 

	if((pPopupMsg->raw.dest_id != GPON_ONU_ID) && (pPopupMsg->raw.dest_id != PLOAM_BROADCAST_ADDR)) {
		PON_MSG((MSG_ERR|MSG_OAM), "The dest. address(%.2x) of PLOAM message is incorrect.\n", pPopupMsg->raw.dest_id) ;
		return -1 ;
	}

	if(GPON_CURR_STATE == GPON_STATE_O6) {
        gpGponPriv->typeBOnGoing = 1;
		if(pPopupMsg->raw.dest_id == PLOAM_BROADCAST_ADDR) {
			/* Change the current state */
			gpon_act_change_gpon_state(GPON_STATE_O4) ;
		} else {
			/* Change the current state */
			gpon_act_change_gpon_state(GPON_STATE_O5) ;
		}
	} 
	
	return 0 ;
}

/******************************************************************************
******************************************************************************/
static int ploam_recv_request_key(PLOAM_RAW_Request_Key_T *pReqKeyMsg)
{
	PON_MSG(MSG_OAM, "PLOAM: Receive Request_Key message.(%.8X %.8X %.8X)\n", pReqKeyMsg->value[0], pReqKeyMsg->value[1], pReqKeyMsg->value[2]) ;
	printk(KERN_DEBUG "OMCIDBG: Request_Key RECV in O%d dest=0x%02x\n", GPON_CURR_STATE, pReqKeyMsg->raw.dest_id) ;

	if(pReqKeyMsg->raw.dest_id != GPON_ONU_ID) {
		PON_MSG((MSG_ERR|MSG_OAM), "The dest. address(%.2x) of PLOAM message is incorrect.\n", pReqKeyMsg->raw.dest_id) ;
		return -1 ;
	}
		
	if(GPON_CURR_STATE == GPON_STATE_O5) {
		/* Generate a random number for encryption key */
		get_random_bytes(gpGponPriv->gponCfg.key, GPON_ENCRYPT_KEY_LENS) ;
		gpGponPriv->gponCfg.keyIdx = (gpGponPriv->gponCfg.keyIdx)?0:1 ;
		
		/* Set the new encryption to device */
		gponDevSetEncryptKey(gpGponPriv->gponCfg.key) ;
		
		/* Send three encryption key PLOAM messages */
		ploam_send_encryption_key(gpGponPriv->gponCfg.keyIdx, gpGponPriv->gponCfg.key) ;
		
		/* Set the flag to indicate the key request in activation state */
		gpGponPriv->gponCfg.flags.isRequestKey = 1 ;
	}
	
	return 0 ;
}


/******************************************************************************
******************************************************************************/
static int ploam_recv_config_port_id(PLOAM_RAW_Configure_PortID_T *pConfigPortIdMsg)
{
	REG_G_OMCI_ID gponOmciId ;
	ushort port_id ;
	
	PON_MSG(MSG_OAM, "PLOAM: Receive Configure_PortID message.(%.8X %.8X %.8X)\n", pConfigPortIdMsg->value[0], pConfigPortIdMsg->value[1], pConfigPortIdMsg->value[2]) ;
	printk(KERN_DEBUG "OMCIDBG: Configure_PortID RECV in O%d dest=0x%02x act=%d (%.8X %.8X %.8X)\n", GPON_CURR_STATE, pConfigPortIdMsg->raw.dest_id, pConfigPortIdMsg->raw.activate, pConfigPortIdMsg->value[0], pConfigPortIdMsg->value[1], pConfigPortIdMsg->value[2]) ;

	if(pConfigPortIdMsg->raw.dest_id != GPON_ONU_ID) {
		PON_MSG((MSG_ERR|MSG_OAM), "The dest. address(%.2x) of PLOAM message is incorrect.\n", pConfigPortIdMsg->raw.dest_id) ;
		return -1 ;
	}

	if(GPON_CURR_STATE == GPON_STATE_O5 || GPON_CURR_STATE == GPON_STATE_O4) {
		port_id = ((pConfigPortIdMsg->raw.port_id_m<<4) | (pConfigPortIdMsg->raw.port_id_l)) ;

		if(pConfigPortIdMsg->raw.activate == 1 /*activate*/) {
			extern int pid_skip ;
			gpGponPriv->gponCfg.omcc = port_id ;
			printk(KERN_DEBUG "OMCIDBG: OMCC LATCHED port_id=0x%x pid_skip=0x%x\n", port_id, pid_skip) ;
			gpon_dump_mac_rxcnt("CFG_PID") ;

			/* Setting the MAC registger */
			/* activate the port id of OMCI channel  (pid_skip bit0) */
			if(!(pid_skip & 0x1)) {
				gponOmciId.Raw = IO_GREG(G_OMCI_ID) ;
				gponOmciId.Bits.omci_port_id_vld = 1 ;
				gponOmciId.Bits.omci_gpid = port_id ;
				IO_SREG(G_OMCI_ID, gponOmciId.Raw) ;
			}

			if(!(pid_skip & 0x2)) gwan_create_new_tcont(GPON_ONU_ID) ;                    /* bit1 */
			if(!(pid_skip & 0x4)) gwan_create_new_gemport(port_id, 0, XPON_DISABLE) ;     /* bit2 */

			/* reload the AES key to MAC  (pid_skip bit3) */
			if(!(pid_skip & 0x8) && !gpGponPriv->gponCfg.flags.isRequestKey) {
				gponDevSetKeySwithTime(0) ;
				gpGponPriv->gponCfg.flags.isRequestKey = 1 ;
			}
			printk(KERN_DEBUG "OMCIDBG: config_port_id activate-body DONE (pid_skip=0x%x)\n", pid_skip) ;
			/* report GPON event message */
			xmcs_report_event(XMCS_EVENT_TYPE_GPON, XMCS_EVENT_GPON_TCONT_ALLOCED, GPON_ONU_ID) ;
		} else {
			gpGponPriv->gponCfg.omcc = GPON_UNASSIGN_GEM_ID ;
			
			/* Setting the MAC registger */
			/* deactivate the port id of OMCI channel */
			gponOmciId.Raw = IO_GREG(G_OMCI_ID) ;
			gponOmciId.Bits.omci_port_id_vld = 0 ;
			IO_SREG(G_OMCI_ID, gponOmciId.Raw) ;

			gwan_remove_gemport(port_id) ;
			gwan_remove_tcont(GPON_ONU_ID) ;
		}
	}
	
	/* Send acknowledge PLOAM message */
	ploam_send_acknowledge_msg(pConfigPortIdMsg->raw.msg_id, (unchar *)pConfigPortIdMsg) ;
	
	return 0 ;
}


/******************************************************************************
******************************************************************************/
static int ploam_recv_pee_msg(PLOAM_RAW_PEE_T *pPeeMsg)
{
	PON_MSG(MSG_OAM, "PLOAM: Receive PEE message.(%.8X %.8X %.8X)\n", pPeeMsg->value[0], pPeeMsg->value[1], pPeeMsg->value[2]) ; 

	if(pPeeMsg->raw.dest_id != PLOAM_BROADCAST_ADDR) {
		PON_MSG((MSG_ERR|MSG_OAM), "The dest. address(%.2x) of PLOAM message is incorrect.\n", pPeeMsg->raw.dest_id) ;
		return -1 ;
	}
	
	//Setting the system alarm status
	//alert the PEE alarm
	
	return 0 ;
}


/******************************************************************************
******************************************************************************/
static int ploam_recv_cpl_msg(PLOAM_RAW_CPL_T *pCplMsg)
{
	PON_MSG(MSG_OAM, "PLOAM: Receive CPL message.(%.8X %.8X %.8X)\n", pCplMsg->value[0], pCplMsg->value[1], pCplMsg->value[2]) ; 

	if((pCplMsg->raw.dest_id!=PLOAM_BROADCAST_ADDR) && (pCplMsg->raw.dest_id!=GPON_ONU_ID)) {
		PON_MSG((MSG_ERR|MSG_OAM), "The dest. address(%.2x) of PLOAM message is incorrect.\n", pCplMsg->raw.dest_id) ;
		return -1 ;
	}
		
	if(GPON_CURR_STATE==GPON_STATE_O4 || GPON_CURR_STATE==GPON_STATE_O5) {
		if(pCplMsg->raw.power_level == 0b10 /*Increase*/) {
			//Setting the PHY register
			//increase the transmitted power
		} else if(pCplMsg->raw.power_level == 0b10 /*Decrease*/) {
			//Setting the PHY register
			//decrease the transmitted power
		}
	}
	
	return 0 ;
}

/******************************************************************************
******************************************************************************/
static int ploam_recv_pst_msg(PLOAM_RAW_PST_T *pPstMsg)
{
	PON_MSG(MSG_OAM, "PLOAM: Receive PST message.(%.8X %.8X %.8X)\n", pPstMsg->value[0], pPstMsg->value[1], pPstMsg->value[2]) ; 

	if((pPstMsg->raw.dest_id!=PLOAM_BROADCAST_ADDR) && (pPstMsg->raw.dest_id!=GPON_ONU_ID)) {
		PON_MSG((MSG_ERR|MSG_OAM), "The dest. address(%.2x) of PLOAM message is incorrect.\n", pPstMsg->raw.dest_id) ;
		return -1 ;
	}
	
	//Active upstream APS command
	//gpon_active_aps_command
	
	return 0 ;
}

/******************************************************************************
******************************************************************************/
static int ploam_recv_ber_interval(PLOAM_RAW_BER_Interval_T *pBerMsg)
{
	uint interval ;
	
	PON_MSG(MSG_OAM, "PLOAM: Receive BER_Interval message.(%.8X %.8X %.8X)\n", pBerMsg->value[0], pBerMsg->value[1], pBerMsg->value[2]) ; 

	if((pBerMsg->raw.dest_id!=PLOAM_BROADCAST_ADDR) && (pBerMsg->raw.dest_id!=GPON_ONU_ID)) {
		PON_MSG((MSG_ERR|MSG_OAM), "The dest. address(%.2x) of PLOAM message is incorrect.\n", pBerMsg->raw.dest_id) ;
		return -1 ;
	}
		
	if(GPON_CURR_STATE == GPON_STATE_O5) {
		interval = ((pBerMsg->raw.interval[0]<<24) | (pBerMsg->raw.interval[1]<<16) | (pBerMsg->raw.interval[2]<<8) | (pBerMsg->raw.interval[3])) ;
		gpGponPriv->gponCfg.berInterval = interval>>3 ; /* translation the frame number to ms */
		/* PORT 6.18: timer_list lost .data (it held the ms interval read below) */

		if(gpGponPriv->gponCfg.berInterval) {
			mod_timer(&gpGponPriv->ber_timer, jiffies + msecs_to_jiffies(gpGponPriv->gponCfg.berInterval)) ;
			XPON_PHY_COUNTER_CLEAR(PHY_BIP_CNT_CLR);
		}
	}
	
	/* Send acknowledge PLOAM message */
	ploam_send_acknowledge_msg(pBerMsg->raw.msg_id, (unchar *)pBerMsg) ;
	
	return 0 ;	
}

/******************************************************************************
******************************************************************************/
static int ploam_recv_key_switch_time(PLOAM_RAW_Key_Switching_Time_T *pKeySwitchMsg)
{
	uint counter ;
	
	PON_MSG(MSG_OAM, "PLOAM: Receive Key_Switching_Time message.(%.8X %.8X %.8X)\n", pKeySwitchMsg->value[0], pKeySwitchMsg->value[1], pKeySwitchMsg->value[2]) ;
	printk(KERN_DEBUG "OMCIDBG: Key_Switch_Time RECV in O%d dest=0x%02x\n", GPON_CURR_STATE, pKeySwitchMsg->raw.dest_id) ;

	if(pKeySwitchMsg->raw.dest_id != GPON_ONU_ID) {
		PON_MSG((MSG_ERR|MSG_OAM), "The dest. address(%.2x) of PLOAM message is incorrect.\n", pKeySwitchMsg->raw.dest_id) ;
		return -1 ;
	}
		
	if(GPON_CURR_STATE == GPON_STATE_O5) {
		counter = ((pKeySwitchMsg->raw.counter[0]<<24) | (pKeySwitchMsg->raw.counter[1]<<16) | (pKeySwitchMsg->raw.counter[2]<<8) | (pKeySwitchMsg->raw.counter[3])) ;
		
		/* Setting the MAC register */
		/* set the super counter to start key switching */
		gponDevSetKeySwithTime(counter) ;
	}
		
	/* Send acknowledge PLOAM message */
	ploam_send_acknowledge_msg(pKeySwitchMsg->raw.msg_id, (unchar *)pKeySwitchMsg) ;
	
	return 0 ;	
}

/******************************************************************************
******************************************************************************/
static int ploam_recv_extended_burst_length(PLOAM_RAW_Extended_Burst_Length_T *pExtBurstLenMsg)
{
    REG_G_PLOu_PRMBL_TYPE3 gponT3Prmbl ;
    REG_G_PLOu_GUARD_BIT gponBuardBit ;
    REG_G_PLOu_PRMBL_TYPE1_2 gponPrmblType ;
    REG_G_PLOu_DELM_BIT gponDelmBit ;
    uint overhead, o3T3Preamble, o5T3Preamble ;
    int t3Offset = gpGponPriv->gponCfg.t3PreambleOffset ;
    
    PON_MSG(MSG_OAM, "PLOAM: Receive Extended_Burst_Length message.(%.8X %.8X %.8X)\n", pExtBurstLenMsg->value[0], pExtBurstLenMsg->value[1], pExtBurstLenMsg->value[2]) ;
    printk(KERN_DEBUG "PREB: ExtBurst handler ENTERED msg=%08x dest=%02x raw_o3t3=%d raw_o5t3=%d state=O%d\n",
        pExtBurstLenMsg->value[0], pExtBurstLenMsg->raw.dest_id,
        pExtBurstLenMsg->raw.o3_t3_preamble, pExtBurstLenMsg->raw.o5_t3_preamble, GPON_CURR_STATE) ;

    if(pExtBurstLenMsg->raw.dest_id != PLOAM_BROADCAST_ADDR) {
        PON_MSG((MSG_ERR|MSG_OAM), "The dest. address(%.2x) of PLOAM message is incorrect.\n", pExtBurstLenMsg->raw.dest_id) ;
        return -1 ;
    }

    /* FIX 2 (2026-07-04): cache the T3 preamble lengths in ANY state. The OLT broadcasts
     * Extended_Burst_Length while the ONU may still be in O2; the original O3-only path drops it,
     * then O3-entry fires SN with a 0-byte preamble -> OLT can't lock. gpon_act applies this cache
     * on O3/O4 entry. (H660GM-A: o3_t3=0x77=119B, o5_t3=0x0F.) */
    gpGponPriv->gponCfg.o3T3Cache = pExtBurstLenMsg->raw.o3_t3_preamble ;
    gpGponPriv->gponCfg.o5T3Cache = pExtBurstLenMsg->raw.o5_t3_preamble ;
    gpGponPriv->gponCfg.extBurstValid = 1 ;

    if(GPON_CURR_STATE == GPON_STATE_O3) {
        /* Check the guard bit, t1, t2, t3 preamble and delimitation bit > 128 byte or not */
        gponBuardBit.Raw = IO_GREG(G_PLOu_GUARD_BIT) ;
        gponPrmblType.Raw = IO_GREG(G_PLOu_PRMBL_TYPE1_2) ;
        gponDelmBit.Raw = IO_GREG(G_PLOu_DELM_BIT) ;
        
        overhead = (gponBuardBit.Bits.guard_bit + gponPrmblType.Bits.prmb1_bit + gponPrmblType.Bits.prmb2_bit + gponDelmBit.Bits.delm_bit) >> 3 ;
        
        if(gpGponPriv->gponCfg.flags.preambleFlag == GPON_BURST_MODE_OVERHEAD_LEN_MORE_THAN_128)
        {
            o3T3Preamble = pExtBurstLenMsg->raw.o3_t3_preamble;
            o5T3Preamble = pExtBurstLenMsg->raw.o5_t3_preamble;
        }
        else
        {
            o3T3Preamble = ((overhead+pExtBurstLenMsg->raw.o3_t3_preamble)>128) ? (128-overhead) : pExtBurstLenMsg->raw.o3_t3_preamble ;
            o5T3Preamble = ((overhead+pExtBurstLenMsg->raw.o5_t3_preamble)>128) ? (128-overhead) : pExtBurstLenMsg->raw.o5_t3_preamble ;
        }
            
        PHY_GponPreb_T pon_preb;
        PPHY_GponPreb_T p_pon_preb = & pon_preb;
        p_pon_preb->mask = PHY_T3_O4_PRE_EN | PHY_T3_O5_PRE_EN | PHY_EXT_BUR_MODE_EN | PHY_OPER_RANG_EN;
        p_pon_preb->t3_O4_preamble = o3T3Preamble-t3Offset;
        p_pon_preb->t3_O5_preamble = o5T3Preamble;
        p_pon_preb->extend_burst_mode = 1;
        p_pon_preb->oper_ranged_st = 0x2;
        if(gpGponPriv->gponCfg.flags.preambleFlag == GPON_BURST_MODE_OVERHEAD_LEN_MORE_THAN_128)
        {
        #if defined(CONFIG_USE_MT7520_ASIC) || defined(CONFIG_USE_A60928)
            p_pon_preb->mask |= PHY_EXTB_LENG_SEL_EN;
            p_pon_preb->extb_length_sel = 1;
        #endif
            PON_MSG(MSG_OAM, "PLOAM: Setting the O3/O4 type 3 preamble. preambleFlag:%s, o3Overhead:%d, O5Overhead:%d\n", 
                (gpGponPriv->gponCfg.flags.preambleFlag) ? "Enable" : "Disable", (overhead+pExtBurstLenMsg->raw.o3_t3_preamble), (overhead+pExtBurstLenMsg->raw.o5_t3_preamble)) ;
        }
        XPON_PHY_SET_API(PON_SET_PHY_GPON_EXTEND_PREAMBLE, p_pon_preb);
        /* Setting the MAC register */
        gponT3Prmbl.Raw = IO_GREG(G_PLOu_PRMBL_TYPE3) ;
        gponT3Prmbl.Bits.ebl_en = 1 ;
        gponT3Prmbl.Bits.ext_prmb3_o5_num = o5T3Preamble ;
        gponT3Prmbl.Bits.ext_prmb3_o3_o4_num = (o3T3Preamble-t3Offset) ;
        IO_SREG(G_PLOu_PRMBL_TYPE3, gponT3Prmbl.Raw) ;
        gpGponPriv->gponCfg.gponT3Prmbl.Raw = gponT3Prmbl.Raw;
        printk(KERN_DEBUG "PREB: O3 apply o3T3=%d o5T3=%d t3off=%d ovh=%d -> G_PLOu_PRMBL_TYPE3 wrote=%08x readback=%08x (ebl=%d o3o4num=%d)\n",
            o3T3Preamble, o5T3Preamble, t3Offset, overhead, gponT3Prmbl.Raw, IO_GREG(G_PLOu_PRMBL_TYPE3),
            (int)gponT3Prmbl.Bits.ebl_en, (int)gponT3Prmbl.Bits.ext_prmb3_o3_o4_num) ;

        PON_MSG(MSG_OAM, "PLOAM: Setting the O3/O4 type 3 preamble. Overflow: %d, Rec. Value: %d, Set Value: %d\n", ((overhead+pExtBurstLenMsg->raw.o3_t3_preamble)>128) ? 1 : 0, pExtBurstLenMsg->raw.o3_t3_preamble, o3T3Preamble-t3Offset) ;
    }
    return 0 ;  
}

/******************************************************************************
******************************************************************************/
static int ploam_recv_pon_id_msg(PLOAM_RAW_PonID_T *pPonIdMsg)
{
	PON_MSG(MSG_OAM, "PLOAM: Receive PonID message.(%.8X %.8X %.8X)\n", pPonIdMsg->value[0], pPonIdMsg->value[1], pPonIdMsg->value[2]) ; 

	if(pPonIdMsg->raw.dest_id != PLOAM_BROADCAST_ADDR) {
		PON_MSG((MSG_ERR|MSG_OAM), "The dest. address(%.2x) of PLOAM message is incorrect.\n", pPonIdMsg->raw.dest_id) ;
		return -1 ;
	}
	
	//Get the PON identifier value
	//store the PON ID information if necessary
	
	return 0 ;
}

/******************************************************************************
******************************************************************************/
static int ploam_recv_swift_popup(PLOAM_RAW_Swift_Popup_T *pSwiftPopupMsg)
{
	PON_MSG(MSG_OAM, "PLOAM: Receive Swift_Popup message.(%.8X %.8X %.8X)\n", pSwiftPopupMsg->value[0], pSwiftPopupMsg->value[1], pSwiftPopupMsg->value[2]) ; 

	if(pSwiftPopupMsg->raw.dest_id != PLOAM_BROADCAST_ADDR) {
		PON_MSG((MSG_ERR|MSG_OAM), "The dest. address(%.2x) of PLOAM message is incorrect.\n", pSwiftPopupMsg->raw.dest_id) ;
		return -1 ;
	}
	
	if(GPON_CURR_STATE == GPON_STATE_O6) {
		/* Change the current state */
		gpon_act_change_gpon_state(GPON_STATE_O5) ;
	} 
	
	return 0 ;
}

/******************************************************************************
******************************************************************************/
static int ploam_recv_ranging_adjustment(PLOAM_RAW_Ranging_Adjustment_T *pRangingAdjustMsg)
{
	uint eqd_offset ;
	
	PON_MSG(MSG_OAM, "PLOAM: Receive Ranging_Adjustment message.(%.8X %.8X %.8X)\n", pRangingAdjustMsg->value[0], pRangingAdjustMsg->value[1], pRangingAdjustMsg->value[2]) ; 

	if((pRangingAdjustMsg->raw.dest_id!=GPON_ONU_ID) && (pRangingAdjustMsg->raw.dest_id!=PLOAM_BROADCAST_ADDR)) {
		PON_MSG((MSG_ERR|MSG_OAM), "The dest. address(%.2x) of PLOAM message is incorrect.\n", pRangingAdjustMsg->raw.dest_id) ;
		return -1 ;
	}
		
	if(GPON_CURR_STATE == GPON_STATE_O5) {
		eqd_offset = ((pRangingAdjustMsg->raw.eqd_value[0]<<24) | (pRangingAdjustMsg->raw.eqd_value[1]<<16) | (pRangingAdjustMsg->raw.eqd_value[2]<<8) | (pRangingAdjustMsg->raw.eqd_value[3])) ;
		
		if(eqd_offset != 0) {
			if(pRangingAdjustMsg->raw.s_bit == 0b0) {
				/* increase eqd_offset value to the eqd */
				ploam_eqd_adjustment(gpGponPriv->gponCfg.eqd + eqd_offset) ;
			} else {
				/* decrease eqd_offset value to the eqd */
				ploam_eqd_adjustment(gpGponPriv->gponCfg.eqd - eqd_offset) ;
			}
		}
	}
				
	/* Send acknowledge PLOAM message */
	ploam_send_acknowledge_msg(pRangingAdjustMsg->raw.msg_id, (unchar *)pRangingAdjustMsg) ;
	
	return 0 ;
}

/******************************************************************************
 Descriptor:	To receive sleep_allow PLOAM.
 Input Args:	pSleepAllow: sleep_allow PLOAM structure.
 Ret Value:		0: Success.
               	    -1: The PLOAM destination ID is neither ONU ID nor broadcast.
******************************************************************************/
static int ploam_recv_Sleep_Allow(PLOAM_RAW_Sleep_Allow_T *pSleepAllow)
{
	uint eqd_offset ;
	
	PON_MSG(MSG_OAM, "PLOAM: Receive Sleep_Allow message.(%.8X %.8X %.8X)\n", pSleepAllow->value[0], pSleepAllow->value[1], pSleepAllow->value[2]) ; 

	if((pSleepAllow->raw.dest_id!=GPON_ONU_ID) && (pSleepAllow->raw.dest_id!=PLOAM_BROADCAST_ADDR)) {
		PON_MSG((MSG_ERR|MSG_OAM), "The dest. address(%.2x) of PLOAM message is incorrect.\n", pSleepAllow->raw.dest_id) ;
		return -1 ;
	}

	return 0 ;
}

/******************************************************************************
******************************************************************************/
int ploam_init(void)
{
	memset(gpGponPriv->ploamRecvHandler, 0, sizeof(ploam_recv_handler_t)*PLOAM_DOWN_MAX_TYPE) ;
	gpGponPriv->ploamRecvHandler[PLOAM_DOWN_MSG_UPSTREAM_OVERHEAD] 		= (ploam_recv_handler_t)ploam_recv_upstream_overhead ;
	gpGponPriv->ploamRecvHandler[PLOAM_DOWN_MSG_ASSIGN_ONUID] 			= (ploam_recv_handler_t)ploam_recv_assign_onu_id ;
	gpGponPriv->ploamRecvHandler[PLOAM_DOWN_MSG_RANGING_TIME] 			= (ploam_recv_handler_t)ploam_recv_ranging_time ;	
	gpGponPriv->ploamRecvHandler[PLOAM_DOWN_MSG_DEACTIVATE_ONUID] 		= (ploam_recv_handler_t)ploam_recv_deactivate_onu ;	
	gpGponPriv->ploamRecvHandler[PLOAM_DOWN_MSG_DISABLE_SERIAL_NUM] 	= (ploam_recv_handler_t)ploam_recv_disable_serial_number ;
	gpGponPriv->ploamRecvHandler[PLOAM_DOWN_MSG_ENCRYPTED_PORTID] 		= (ploam_recv_handler_t)ploam_recv_encrypt_port_id ;
	gpGponPriv->ploamRecvHandler[PLOAM_DOWN_MSG_REQUEST_PASSWORD] 		= (ploam_recv_handler_t)ploam_recv_request_passwd ;
	gpGponPriv->ploamRecvHandler[PLOAM_DOWN_MSG_ASSIGN_ALLOCID] 		= (ploam_recv_handler_t)ploam_recv_assign_alloc_id ;
	gpGponPriv->ploamRecvHandler[PLOAM_DOWN_MSG_POPUP_MESSAGE] 			= (ploam_recv_handler_t)ploam_recv_popup_msg ;
	gpGponPriv->ploamRecvHandler[PLOAM_DOWN_MSG_REQUEST_KEY] 			= (ploam_recv_handler_t)ploam_recv_request_key ;
	gpGponPriv->ploamRecvHandler[PLOAM_DOWN_MSG_CONFIG_PORTID] 			= (ploam_recv_handler_t)ploam_recv_config_port_id ;
	gpGponPriv->ploamRecvHandler[PLOAM_DOWN_MSG_PEE_MESSAGE] 			= (ploam_recv_handler_t)ploam_recv_pee_msg ;
	gpGponPriv->ploamRecvHandler[PLOAM_DOWN_MSG_CPL_MESSAGE] 			= (ploam_recv_handler_t)ploam_recv_cpl_msg ;
	gpGponPriv->ploamRecvHandler[PLOAM_DOWN_MSG_PST_MESSAGE] 			= (ploam_recv_handler_t)ploam_recv_pst_msg ;
	gpGponPriv->ploamRecvHandler[PLOAM_DOWN_MSG_BER_INTERVAL] 			= (ploam_recv_handler_t)ploam_recv_ber_interval ;
	gpGponPriv->ploamRecvHandler[PLOAM_DOWN_MSG_KEY_SWITCHING_TIME] 	= (ploam_recv_handler_t)ploam_recv_key_switch_time ;
	gpGponPriv->ploamRecvHandler[PLOAM_DOWN_MSG_EXTENDED_BURST_LENGTH] 	= (ploam_recv_handler_t)ploam_recv_extended_burst_length ;
	gpGponPriv->ploamRecvHandler[PLOAM_DOWN_MSG_PONID_MESSAGE] 			= (ploam_recv_handler_t)ploam_recv_pon_id_msg ;
	gpGponPriv->ploamRecvHandler[PLOAM_DOWN_MSG_SWIFT_POPUP] 			= (ploam_recv_handler_t)ploam_recv_swift_popup ;
	gpGponPriv->ploamRecvHandler[PLOAM_DOWN_MSG_RANGING_ADJUSTMENT] 	= (ploam_recv_handler_t)ploam_recv_ranging_adjustment ; 
	gpGponPriv->ploamRecvHandler[PLOAM_DOWN_MSG_SLEEP_ALLOW_MESSAGE] 	= (ploam_recv_handler_t)ploam_recv_Sleep_Allow ; 
	
	memset(&gpGponPriv->prePloamMsg, 0, sizeof(PLOAM_RAW_General_T)) ;

	/* PORT 6.18: init_timer()+.data+.function -> timer_setup(); ms interval read
	 * from gponCfg.berInterval at arm time (was stashed in .data). */
	timer_setup(&gpGponPriv->ber_timer, gpon_ber_interval_expires, 0) ;

	return 0 ;
}

