/*
* File Name: gpon_init.c
* Description: Initialization for GPON
*
******************************************************************/
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/timer.h>

#include <ecnt_hook/ecnt_hook_net.h>
#include "common/drv_global.h"
#include "gpon/gpon_act.h"
#include "gpon/gpon_dev.h"
#include "gpon/gpon_qos.h"
#include "gpon/gpon_power_management.h"
#include "gpon/gpon_recovery.h"

#ifdef TCSUPPORT_CPU_EN7521
#include <ecnt_hook/ecnt_hook_fe.h>
#endif


Pgpon_mac_reg_REGS g_gpon_mac_reg_BASE = NULL ;


	
   
/*****************************************************************************
******************************************************************************/
static int gpon_init_private_data(GPON_GlbPriv_T *gpon_priv_p) 
{
    int ret = 0 ;
    /* GPON identity comes from module_params (declared in phy_init.c); the driver
     * hardcodes no serial or password. gpon_sn = 4 vendor ASCII chars + 8 hex digits;
     * gpon_pw = PLOAM password (ASCII). Empty => the generic vendor default below.
     * Source the real values from the unit's own factory flash at insmod. */
    extern char *gpon_sn, *gpon_pw;
    unchar sn[GPON_SN_LENS]         = {0x4D,0x54,0x4B,0x47,0x00,0x00,0x00,0x01}; /* "MTKG00000001" default */
    unchar passwd[GPON_PASSWD_LENS] = {0};
    if (gpon_sn && strlen(gpon_sn) >= 12) {
        sn[0]=gpon_sn[0]; sn[1]=gpon_sn[1]; sn[2]=gpon_sn[2]; sn[3]=gpon_sn[3];
        if (hex2bin(&sn[4], gpon_sn + 4, 4))
            printk(KERN_DEBUG "gpon_init: gpon_sn has bad hex; keeping default VS bytes\n");
    }
    if (gpon_pw && *gpon_pw) {
        size_t n = strlen(gpon_pw); if (n > GPON_PASSWD_LENS) n = GPON_PASSWD_LENS;
        memcpy(passwd, gpon_pw, n);
    }
    /* optional: override the 4 vendor-specific SN bytes (fresh/random SN for testing) */
    { extern int sn_vs; if (sn_vs != 0) { sn[4]=(sn_vs>>24)&0xff; sn[5]=(sn_vs>>16)&0xff; sn[6]=(sn_vs>>8)&0xff; sn[7]=sn_vs&0xff; } }
    printk(KERN_DEBUG "gpon_init: SN=%c%c%c%c%02X%02X%02X%02X pw[0]=%02x\n",
        sn[0],sn[1],sn[2],sn[3], sn[4],sn[5],sn[6],sn[7], passwd[0]) ;

	gpon_priv_p->bip_cnt_val = 0;
    memset(gpon_priv_p->gponCfg.sn, 0, sizeof(gpon_priv_p->gponCfg.sn)) ;
    memset(gpon_priv_p->gponCfg.passwd, 0, sizeof(gpon_priv_p->gponCfg.passwd)) ;
    
    /* SESSION 7: default 1 = do NOT transmit the SN / arm the laser. The full activation
     * runs to O3 (SN handshake) but this uncalibrated mesh node can't emit a registrable
     * optical burst (phy_gpon_tx_power stub + no factory laser cal), so keep the laser OFF
     * on the live PON. Set 0 (rebuild) only to deliberately transmit (accepting the risk). */
    gpon_priv_p->disableSnFlag      =           0 ;   /* default 1 = don't transmit SN (safe). 0 to transmit. */
    gpon_priv_p->emergencystate     =           0 ;
    
    gpon_priv_p->gponCfg.onu_id =                       GPON_UNASSIGN_ONU_ID ;
    gpon_priv_p->gponCfg.keyIdx =                       0 ;
    gpon_priv_p->gponCfg.omcc =                         GPON_UNASSIGN_GEM_ID ;
    gpon_priv_p->gponCfg.reiSeq =                       0 ;
    gpon_priv_p->gponCfg.to1Timer =                     GPON_ACT_TO1_TIMER ;
    gpon_priv_p->gponCfg.to2Timer =                     GPON_ACT_TO2_TIMER ;
    gpon_priv_p->gponCfg.berInterval =                  0 ;
    gpon_priv_p->gponCfg.eqd =                          0 ;
    gpon_priv_p->gponCfg.byteDelay =                    0 ;
    gpon_priv_p->gponCfg.bitDelay =                     0 ;
    gpon_priv_p->gponCfg.eqdO4Offset =                  0 ;
    gpon_priv_p->gponCfg.eqdO5Offset =                  0 ;
    gpon_priv_p->gponCfg.t3PreambleOffset =             0 ;
    gpon_priv_p->gponCfg.dvtPcpCheck =                  0 ;
    gpon_priv_p->gponCfg.dvtPcpCounter =                0 ;
    gpon_priv_p->gponCfg.dvtPcpCheckErr =               0 ;
    gpon_priv_p->gponCfg.counter_type   =               GPON_COUNTER_TYPE_GEM ;
    gpon_priv_p->gponCfg.flags.isRequestKey =           0 ;
    gpon_priv_p->gponCfg.flags.isTodUpdate =            0 ;
    gpon_priv_p->gponCfg.flags.preambleFlag =           GPON_BURST_MODE_OVERHEAD_LEN_DEFAULT ;
    gpon_priv_p->gponCfg.flags.sniffer =                0 ;
	gpon_priv_p->gponCfg.flags.hotplug =                XPON_DISABLE;
    gpon_priv_p->gponCfg.internalDelayFineTune =        0x1C;
    { extern int po_rsp; gpon_priv_p->gponCfg.onuResponseTime = po_rsp; }   /* SESSION12: stock@O5=0x577, port was 0x58b */

#ifdef TCSUPPORT_CUC
    gpon_priv_p->gponCfg.dis_ranging_in_o5 =1;
#else
    gpon_priv_p->gponCfg.dis_ranging_in_o5 =0;
#endif

#ifdef TCSUPPORT_CPU_EN7521
    gpon_priv_p->gponCfg.flags.isPloamFilter =          1;
    gpon_priv_p->gponCfg.flags.isBwmStopTimeErrInt =    XPON_DISABLE;
    gpon_priv_p->gponCfg.phy_psync_to_sof_delay =       UNKNOW_PSYNC_TO_SOF_DELAY;
	/* sniffer mode */
	gpon_priv_p->gponCfg.sniffer_mode.enable = XPON_DISABLE;
	gpon_priv_p->gponCfg.sniffer_mode.lan_port = SNIFFER_MODE_LAN0;
	gpon_priv_p->gponCfg.sniffer_mode.packet_padding = XPON_ENABLE;
    /* Sleep count */
    gpon_priv_p->gponCfg.sleep_count =      POWER_MANAGEMENT_DEFAULT_SLEEP_COUNT;
    
    spin_lock_init(&gpon_priv_p->swReset_lock) ;
#else
    gpon_priv_p->gponCfg.reg4208 = 0x80800F00;
    gpon_priv_p->gponCfg.rstDbgDly = MAC_4208_PHY_RX_FIX | MAC_4208_PHY_RX_DLY | 
                                     MAC_4208_INTERNAL_DLY | MAC_4208_PHY_TX_DLY;
    /* do Mac Reset and Phy whole reset */
    gpon_priv_p->gponCfg.macPhyReset = GPON_MAC_RESET_BIT | PHY_ANALOG_RESET_BIT;
#endif  

    /* idle gem threshold */
    gpon_priv_p->gponCfg.idle_gem_thld =    GPON_IDLE_GEM_THLD;

	spin_lock_init(&gpon_priv_p->mac_reset_lock) ;

    spin_lock_init(&gpon_priv_p->act_lock) ;
    init_waitqueue_head(&gpon_priv_p->wq_gpon_reset_done);

    gpon_priv_p->typeBOnGoing = 0;

    /* Set ONU password */
    memcpy(gpon_priv_p->gponCfg.passwd, passwd, GPON_PASSWD_LENS) ;
    
    /* Set the serial number */
    memcpy(gpon_priv_p->gponCfg.sn, sn, GPON_SN_LENS) ;

	gpon_priv_p->pGponRecovery = &gponRecovery;
	gpon_recovery_init();
	
    return ret ;
}

/*****************************************************************************
******************************************************************************/
int register_gpon_isr(void)
{
#ifdef TCSUPPORT_CPU_EN7521
	return QDMA_API_REGISTER_HOOKFUNC(ECNT_QDMA_WAN, QDMA_CALLBACK_GPON_MAC_HANDLER, gpon_isr);
#else
	return qdma_register_callback_function(QDMA_CALLBACK_GPON_MAC_HANDLER, gpon_isr) ;
#endif
}

int unregister_gpon_isr(void)
{
#ifdef TCSUPPORT_CPU_EN7521
	return QDMA_API_UNREGISTER_HOOKFUNC(ECNT_QDMA_WAN, QDMA_CALLBACK_GPON_MAC_HANDLER);
#else
	return qdma_unregister_callback_function(QDMA_CALLBACK_GPON_MAC_HANDLER) ;
#endif
}


int gpon_init(void)
{
	int ret = 0 ;
	
	/* Base Register of  GPON */
	g_gpon_mac_reg_BASE = (Pgpon_mac_reg_REGS)(ioremap(CONFIG_GPON_BASE_ADDR, CONFIG_GPON_REG_RANGE)) ;	/* PORT 6.18: ioremap_nocache() removed; ioremap() is uncached by default */
	if(!g_gpon_mac_reg_BASE) {
		PON_MSG(MSG_ERR, "ioremap the GPON base address failed.\n") ;
		return -ENOMEM ;
	}
	ret = gpon_init_private_data(gpGponPriv) ;
	if(ret != 0) {
		PON_MSG(MSG_ERR, "GPON private data initialization failed.\n") ;
		return ret ;
	}

	ret = ploam_init() ;
	if(ret) {
		PON_MSG(MSG_ERR, "GPON PLOAM initialization failed.\n") ;
		return ret ;
	}

	ret = gpon_act_init() ;
	if(ret) {
		PON_MSG(MSG_ERR, "GPON ONU activation initialization failed.\n") ;
		return ret ;
	}

	ret = gpon_dev_init() ;
	if(ret != 0) {
		PON_MSG(MSG_ERR, "GPON device initialization failed.\n") ;
		return ret ;
	}

	/* stop MBI interface */
	gponDevMbiStop(XPON_ENABLE) ;

#ifdef TCSUPPORT_CPU_EN7521
	ret = gpon_power_management_init() ;
	if(ret) {
		PON_MSG(MSG_ERR, "GPON power management initialization failed.\n") ;
		return ret ;
	}
	/* Set internalDelayFineTune */
	gponDevSetInternalDelayFineTune(gpGponPriv->gponCfg.internalDelayFineTune);
	/* disable sniff mode */
	gponDevSetSniffMode(&gpGponPriv->gponCfg.sniffer_mode);
	/* DBA backdoor */
	gponDevSetDbaBackdoor(XPON_DISABLE);
	/* DBA slight modify */
	gponDevSetDbaSlightModify(XPON_DISABLE);

	/* enable 1pps signal output from GPIO22  */
	IO_SBITS(0xbfa20104, 0x04);

	/* enable ToD clock  */
	IO_SBITS(0xbfa200e4, 0x100);
	IO_SBITS(0xbfa200ec, 0x02);
#endif
    PON_MSG(MSG_TRACE, "GPON initialization done\n") ;

	return ret ;
}

/*****************************************************************************
******************************************************************************/
void gpon_deinit(void)
{
#if TCSUPPORT_CPU_EN7521
	xpon_power_management_deinit();
#endif
	/* Stop the TO1 or TO2 timer */
	GPON_STOP_TIMER(gpGponPriv->to1_timer) ;
	GPON_STOP_TIMER(gpGponPriv->to2_timer) ;

	GPON_STOP_TIMER(gpGponPriv->ber_timer) ;

	if(g_gpon_mac_reg_BASE) {
		iounmap(g_gpon_mac_reg_BASE) ;
		g_gpon_mac_reg_BASE = NULL ;
	}
	gpon_act_deinit();
	
}
