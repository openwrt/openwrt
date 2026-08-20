#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <linux/kthread.h>
#include <linux/netdevice.h>


#include <ecnt_hook/ecnt_hook_pon_mac.h>

#include "../inc/common/drv_global.h"
#include "../inc/common/phy_if_wrapper.h"

#include "../inc/epon/epon.h"
#include "../inc/epon/epon_mpcp.h"
#include "../inc/epon/epon_reg.h"
#include "../inc/gpon/gpon.h"
#include "../inc/gpon/gpon_init.h"
#include "../inc/gpon/gpon_dvt.h"
#include "../inc/gpon/gpon_reg.h"
#include "../inc/pwan/xpon_netif.h"
#include "../inc/xmcs/xmcs_mci.h"
#include "../inc/xmcs/xmcs_sdi.h"
#include "../inc/xmcs/xmcs_if.h"
#include "../inc/pwan/gpon_wan.h"

#ifdef TCSUPPORT_CPU_EN7521
#include "gpon/gpon_power_management.h"
#include <ecnt_hook/ecnt_hook_fe.h>
#endif
#include "../inc/common/xpon_daemon.h"
#include "../inc/common/xpon_led.h"

#include <ecnt_hook/ecnt_hook_qdma_7510_20.h>

#include "gpon/gpon_recovery.h"

#include <linux/fs.h>

extern int xpon_los_status ;

/* TODO: gpon group should export this in a gpon header; local extern for now */
extern void gpon_detect_dying_gasp(void);

/* ------------------------------------------------------------------------
 * 6.18 proc_fs compat
 *
 * The 2.6.36 driver registered procfs files by setting the (now-removed)
 * ->read_proc / ->write_proc members of struct proc_dir_entry.  On 6.18
 * struct proc_dir_entry is opaque, so we register a single generic
 * struct proc_ops and forward to the legacy handlers (whose prototypes are
 * unchanged) via a small descriptor stored as the PDE private data.
 * Runtime semantics are an approximation (single-shot read of one page);
 * full seq_file conversion is deferred (functional bring-up).  TODO.
 * ------------------------------------------------------------------------ */
typedef int (*xpon_legacy_read_t)(char *page, char **start, off_t off,
				  int count, int *eof, void *data);
typedef int (*xpon_legacy_write_t)(struct file *file, const char *buffer,
				   unsigned long count, void *data);

struct xpon_legacy_proc {
	xpon_legacy_read_t  rd;
	xpon_legacy_write_t wr;
};

static ssize_t xpon_legacy_proc_read(struct file *f, char __user *ubuf,
				     size_t cnt, loff_t *ppos)
{
	struct xpon_legacy_proc *lp = pde_data(file_inode(f));
	char *page, *start = NULL;
	int len, eof = 0;

	if (*ppos > 0 || !lp || !lp->rd)
		return 0;

	page = (char *)__get_free_page(GFP_KERNEL);
	if (!page)
		return -ENOMEM;

	len = lp->rd(page, &start, 0, PAGE_SIZE - 1, &eof, NULL);
	if (len < 0)
		len = 0;
	if (len > (int)cnt)
		len = cnt;
	if (len && copy_to_user(ubuf, page, len))
		len = -EFAULT;
	free_page((unsigned long)page);

	if (len > 0)
		*ppos += len;
	return len;
}

static ssize_t xpon_legacy_proc_write(struct file *f, const char __user *ubuf,
				      size_t cnt, loff_t *ppos)
{
	struct xpon_legacy_proc *lp = pde_data(file_inode(f));

	if (!lp || !lp->wr)
		return -EIO;
	/* legacy handlers copy_from_user() internally, so pass the user ptr */
	return lp->wr(f, ubuf, cnt, NULL);
}

static const struct proc_ops xpon_legacy_proc_ops = {
	.proc_read  = xpon_legacy_proc_read,
	.proc_write = xpon_legacy_proc_write,
};

static struct proc_dir_entry *xpon_create_proc(const char *name,
		struct proc_dir_entry *parent,
		xpon_legacy_read_t rd, xpon_legacy_write_t wr)
{
	struct xpon_legacy_proc *lp;

	lp = kzalloc(sizeof(*lp), GFP_KERNEL);
	if (!lp)
		return NULL;
	lp->rd = rd;
	lp->wr = wr;
	return proc_create_data(name, 0, parent, &xpon_legacy_proc_ops, lp);
}

/*****************************************************************************
******************************************************************************
   Define the Global variable 
******************************************************************************
******************************************************************************/
#define PON_MODE_GPON					(1)
#define PON_MODE_EPON					(1<<1)
#define PHY_RD							(1<<2)

#define CONFIG_DYING_GASP_IRQ    		(19)
/* The dying-gasp IRQ is only requested when xpon_hw=1 (xpon_dying_gasp_init).
 * Track ownership so cleanup never free_irq()s an IRQ we never requested
 * (otherwise rmmod under the xpon_hw=0 safe-load warns "free already-free IRQ"). */
static bool g_dying_gasp_irq_owned;
#define PADD_HEADER_LEN     			(2)
#define DYING_GAPS_PKT_LEN  			(62)

static char dying_gasp_oam[]=
{
	0x00,0x00,0x01,0x80,0xC2,0x00,0x00,0x02,0x00,0x25,
	0x12,0xBB,0x25,0x0E,0x88,0x09,0x03,0x00,0x52,0x00,
	0x01,0x10,0x01,0x00,0x00,0x00,0x0F,0x05,0xEE,0x00,
	0x13,0x25,0x00,0x22,0x01,0x00,0x02,0x10,0x01,0x00,
	0x00,0x00,0x0F,0x05,0xEE,0x00,0x13,0x25,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

uint8 pon_mode;
static uint epon_proc_cnt = 0;
xpon_regs *xpon_regs_p = NULL;
static epon_stat *epon_stat_prev = NULL;
static struct proc_dir_entry *xpon_proc_dir = NULL;
static struct proc_dir_entry *gpon_proc_dir = NULL;
static struct proc_dir_entry *epon_proc_dir = NULL;
static struct proc_dir_entry *gpon_debug_proc_dir=NULL;
static struct proc_dir_entry *gpon_power_management_proc_dir=NULL;
/* Legacy /proc/tc3162 parent dir. The vendor 2.6.36 SoC platform created it;
 * mainline does not, so the WAN/PHY procs created as create_proc_entry("tc3162/x")
 * (service_change, pon_*, los_status, vlan_filter_enable, ...) WARN in
 * __xlate_proc_name and fail. Created once in xpondrv_init(), removed in cleanup. */
static struct proc_dir_entry *xpon_proc_tc3162 = NULL;

PON_SysData_T   * gpPonSysData = NULL ;
PWAN_GlbPriv_T  * gpWanPriv    = NULL ;
MCS_GlbPriv_T   * gpMcsPriv    = NULL ;
GPON_GlbPriv_T  * gpGponPriv   = NULL ;
EPON_GlbPriv_T  * gpEponPriv   = NULL ;

extern int xmcs_set_connection_start(XPON_Mode_t mode);

/* PORT 6.18: gsw_p6_rate_timer was init'd with the removed init_timer()/.data/.function
 * API -> the timer core reads .function at the wrong struct offset -> garbage ptr ->
 * Oops in call_timer_fn when the timer fires. timer_setup() + this trampoline fix it. */
void xpon_disable_cpu_protection(void);
static void xpon_gsw_p6_rate_timer_cb(struct timer_list *t)
{
	xpon_disable_cpu_protection();
}

void trans_status_refresh(struct timer_list *t)	/* PORT 6.18: timer_setup() callback sig */
{
    XPON_DAEMON_Job_data_t job;

    if(!gpPhyData->trans_status_refresh_pending ){
        job.id       = XPON_DAEMON_JOB_GET_PHY_PARAM ;
        job.priority = XPON_DAEMON_JOB_PRIORITY_LOW  ;
        gpPhyData->trans_status_refresh_pending = 1;
        xpon_daemon_job_enqueue(&job) ;
        wake_up_xpon_daemon() ;
    }

    if (
    #ifndef TCSUPPORT_CPU_EN7521
        0    == atomic_read(&fe_reset_flag)       &&
    #endif
        FALSE == gpPonSysData->ponMacPhyReset)
    {
        XPON_START_TIMER(gpPhyData->trans_status_refresh_timer, 1000) ;	/* was .data = 1000 ms */
    }
}

static void xpondrv_init_phy_data(void)
{
    gpPhyData->is_fix_mode   = FALSE;
    gpPhyData->calibrating   = FALSE;
    gpPhyData->working_mode  = PHY_UNKNOWN_CONFIG    ;
    gpPhyData->detect_mode   = PHY_UNKNOWN_CONFIG    ;
    gpPhyData->detect_status = XPON_PHY_SYNC_STATUS_NOT_START ;
    gpPhyData->phy_link_status     = PHY_LINK_STATUS_UNKNOWN ;

    spin_lock_init(&gpPhyData->trans_params_lock);
    memset(&gpPhyData->trans_params, 0 , sizeof(gpPhyData->trans_params));

	/* PORT 6.18: init_timer()+.data+.function -> timer_setup(); the per-timer ms
	 * interval that used to live in .data is now passed at the XPON_START_TIMER
	 * arm sites (trans:1000, delay_start:3000, check_sync:1000). */
	timer_setup(&gpPhyData->trans_status_refresh_timer, trans_status_refresh, 0) ;/*used for transceiver alarm*/
    gpPhyData->trans_status_refresh_pending = 0 ;

	timer_setup(&gpPhyData->delay_start_detect_timer, try_set_phy_mode, 0) ;

	timer_setup(&gpPhyData->check_sync_timer, check_sync_result, 0) ;
}

/*****************************************************************************
******************************************************************************/
static void xpondrv_init_global_data(void)
{
	gpPonSysData->sysOnuType = READ_FLASH_BYTE(flash_base + 0xff9c);
	gpPonSysData->sysLinkStatus = PON_LINK_STATUS_OFF ;
	gpPonSysData->sysPrevLink   = PON_LINK_STATUS_OFF ;
    gpPonSysData->sysStartup    = PON_WAN_STOP ;

	gpEponPriv = &gpPonSysData->eponPrivData ;
	gpGponPriv = &gpPonSysData->gponPrivData ;

	gpWanPriv = &gpPonSysData->wanPrivData ;
	gpMcsPriv = &gpPonSysData->mcsPrivData ;
    gpPhyData =  &gpPonSysData->phyCfg ;

	//gpPonSysData->debugLevel = (MSG_TRACE|MSG_ERR|MSG_INT|MSG_ACT|MSG_OAM|MSG_OMCI) ;//| (MSG_TRACE|MSG_CONTENT) ;
	//gpPonSysData->debugLevel = (MSG_INT | MSG_OAM | MSG_ACT) ;
	gpPonSysData->debugLevel = 0 ;

	if(gpPonSysData->sysOnuType != PON_ONU_TYPE_HGU)//set default onu type to SFU
		gpPonSysData->sysOnuType  = PON_ONU_TYPE_SFU;
	else
		gpPonSysData->sysOnuType = PON_ONU_TYPE_HGU;

	gpPonSysData->dyingGaspData.isEponHwFlag = 1 ;

	if (isMT7520E2E3) // for E2/E3 chip 
		gpPonSysData->dyingGaspData.isGponHwFlag = GPON_SW ;
	else // for E4 and after chip
		gpPonSysData->dyingGaspData.isGponHwFlag = GPON_HW ;
	
	gpPonSysData->sysOltType = 0;

    init_waitqueue_head(&gpPonSysData->xpon_daemon.wq) ;

    memset(&gpPonSysData->xpon_daemon.job_queue, 0, sizeof(gpPonSysData->xpon_daemon.job_queue));
    spin_lock_init(&gpPonSysData->xpon_daemon.job_queue.lock);

    
	gpPonSysData->xpon_daemon.task= kthread_run(xpon_daemon, NULL, "xpon_daemon");
	if(IS_ERR(gpPonSysData->xpon_daemon.task))
	{		
		panic("@%s>>%d--xpon_daemon init failed\n", __FUNCTION__, __LINE__);
	}
    gpPonSysData->rogue_state = XPON_ROUGE_STATE_FALSE ;
    gpPonSysData->fe_reset_happened = FALSE;
    xpondrv_init_phy_data();

#ifndef TCSUPPORT_CPU_EN7521
	/* PORT 6.18: init_timer()+.data+.function removed -> timer_setup(); the 5s interval
	 * is now passed at the XPON_START_TIMER call site (xpon_daemon.c). */
	timer_setup(&gpPonSysData->gsw_p6_rate_timer, xpon_gsw_p6_rate_timer_cb, 0) ;
    gpPonSysData->gswRateLimitFlag = FALSE ;
#endif
    gpPonSysData->isUpDataTraffic    = XPON_DISABLE ;
    gpPonSysData->isUpOmciOamTraffic = XPON_DISABLE ;
    gpPonSysData->ponMacPhyReset     = FALSE ;
}


/******************************************************************************
******************************************************************************/
static int gpon_debug_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index=0 ;
	int cpu_type = 0;
	chipInformation devices_table[] = 
	{
		{MT7520S, isMT7520S},
		{MT7520, isMT7520},
		{MT7520G, isMT7520G},
		{MT7525, isMT7525},
		{MT7525G, isMT7525G},
		{EN7521S, isEN7521S},
		{EN7521F, isEN7521F},
		{EN7526F, isEN7526F},
		{EN7526D, isEN7526D},
		{EN7526G, isEN7526G},
        {EN751221, isEN751221},
		{0, 0},
	};
	#if 0
	if (isMT7520S)
		cpu_type = 1;
	else if (isMT7520)
		cpu_type = 2;
	else if (isMT7520G)
		cpu_type = 3;
	else if (isMT7525)
		cpu_type = 4;
	else if (isMT7525G)
		cpu_type = 5;
	else if (isEN751221)
		cpu_type = 6;
	#endif
	int i;
	for ( i = 0; i <ARRAY_SIZE(devices_table); i++ )
	{
		if(devices_table[i].chipid)
		{
			cpu_type = devices_table[i].onu_type_id;
			break;
		}
	}

	index = sprintf(buf, "%d\n", cpu_type);

	index -= off;
	*start = buf + off;

	if (index > count)
		index = count;
	else
		*eof = 1;

	if (index < 0)
		index = 0;

	return index;

#if 0
	off_t pos=0, begin=0 ;

	/* need to add the debug information */
	//index += sprintf(buf+ index, "CSR: G_PLOAMu_FIFO_STS   	:%.8x\n", G_PLOAMu_FIFO_STS->Raw) ; 
	//CHK_BUF() ;
	
	*eof = 1 ;

done:
	*start = buf + (off - begin) ;
	index -= (off - begin) ;
	if(index<0)		index = 0 ;
	if(index>count)		index = count ;
#endif
	return index ;
}

///******************************************************************************
//******************************************************************************/
//int enableQdmaScheduling(void)
//{
//	QDMA_TrtcmParam_T trtcmParam ;
//	int i ;
//	
//	qdma_set_trtcm_param_unit(QDMA_TRTCM_UNIT_16BYTE) ;
//	qdma_set_trtcm_mode(QDMA_ENABLE) ;
//
//	trtcmParam.channel = 1 ;
//	trtcmParam.cir = 4096 ;
//	trtcmParam.cbs = 256 ;
//	trtcmParam.pir = 10240 ;
//	trtcmParam.pbs = 512 ;
//	if(qdma_set_trtcm_params(&trtcmParam) < 0) {
//		printk("===========>0000\n") ;
//	}
//	
//	memset(&trtcmParam, 0, sizeof(QDMA_TrtcmParam_T)) ;
//	
//	for(i=0 ; i<15 ; i++) {
//		trtcmParam.channel = i ;
//		if(qdma_get_trtcm_params(&trtcmParam) < 0) {
//			printk("===========>0001\n") ;
//		} else {
//			printk("========>%d: CIR:%d, CBS:%d, PIR:%d, PBS:%d\n", trtcmParam.channel,
//													 				trtcmParam.cir,
//													 				trtcmParam.cbs,
//													 				trtcmParam.pir,
//													 				trtcmParam.pbs) ;
//		}
//	}
//	
//	
//	/* Configure the QDMA scheduling mode */
//	if(qdma_set_scheduling_mode(QDMA_WAN_TYPE_GPON) < 0) {
//		printk("=======>1111\n") ;
//	}
//	return 0 ;
//}

/******************************************************************************
******************************************************************************/
unchar reiSeq = 0 ;
#if defined(TCSUPPORT_EPON_ATTACK_ENHANCE)
uint8 attackEnhance = 0;
unsigned long int dropThred = 500;
uint8 rxDropCheckTimer = 5;
#endif
#define isdigit(x)	((x)>='0'&&(x)<='9')
int atoi_temp(char *s)
{
	int i = 0;

	if(s == NULL)
	{
		printk("\r\ns is NULL ====> atoi in xpon drv");
		return -1;
	}
	
        while (isdigit(*s)) {
                i = i*10 + *(s++) - '0';
        }
        return i;
}



#ifndef TCSUPPORT_CPU_EN7521
#define FE_GDMA2_FWD_CFG 0xbfb51500
#endif

#ifndef TCSUPPORT_CPU_EN7521
static void print_debug_param(void)
{
    printk(" \t macPhyReset: %x\n \t rstDbgDly: %x\n"
           " \t reg4208: %x\n \t responseTime: %x\n", 
            gpGponPriv->gponCfg.macPhyReset, gpGponPriv->gponCfg.rstDbgDly,
            gpGponPriv->gponCfg.reg4208, gpGponPriv->gponCfg.onuResponseTime);
}
#endif /* TCSUPPORT_CPU_EN7521 */

uint8 *GetMacAddr(void);

void
xmit_dying_gasp_oam(void)
{
	struct sk_buff * skb =  NULL;
	struct net_device * dev = NULL;

	dev = dev_get_by_name(&init_net, "oam");

	if (!dev) goto end;

   
	skb = skbmgr_alloc_skb2k();

	if(unlikely(!skb))
		goto end;

	memcpy((dying_gasp_oam + (PADD_HEADER_LEN + ETH_ALEN) ), GetMacAddr(),ETH_ALEN);
	memcpy(skb->data, dying_gasp_oam, sizeof(dying_gasp_oam));
	skb_put(skb,DYING_GAPS_PKT_LEN);
	pwan_net_start_xmit(skb, dev);
   
	dev_put(dev);
	
end:
	return;
}
extern uint g_EponLaserTime;
extern __u8 g_silence_time;
extern epon_t eponDrv;

/* PORT: defined unconditionally (vendor xpondrv.c defines these globals
 * outside any guard; the "show_flag" proc handler below reads them directly). */
int drop_print_flag= 0;
int xpon_mac_print_open=0;

#if defined(TCSUPPORT_FWC_ENV)
extern FHNet_Mapping_Result_Out_t gMappingRet;
extern uint fhNetMappingDebug;
extern unsigned char  gFhDrop;
#endif

extern __u8 green_drop_flag;
#ifdef TCSUPPORT_VNPTT
extern __u8 ignore_deactive_flag;
#endif
static int gpon_debug_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	extern __u8 eponStaticRptEnable;
#ifdef TCSUPPORT_PON_TEST
	extern __u32 eponDrvDbgMask;
	extern __u8 eponDrvTmpMacAddr[];
#endif
	char val_string[64], cmd[32], subcmd[32] ;
	uint action = 0 ;
	int i, j ;
	int ret;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;

	cmd[0] = subcmd[0] = '\0' ;
	sscanf(val_string, "%s %s %x", cmd, subcmd, &action) ;

	/* --- devmem-equivalent for PHY/TX register RE (2026-07-04) --- */
	if(!strcmp(cmd, "peek")) {
		unsigned long a = simple_strtoul(subcmd, NULL, 16);
		printk(KERN_DEBUG "PEEK %08lx = %08x\n", a, *(volatile unsigned int *)a);
		return count;
	}
	if(!strcmp(cmd, "poke")) {
		unsigned long a = simple_strtoul(subcmd, NULL, 16);
		*(volatile unsigned int *)a = action;
		printk(KERN_DEBUG "POKE %08lx <= %08x (rb %08x)\n", a, action, *(volatile unsigned int *)a);
		return count;
	}
	if(!strcmp(cmd, "usdiag")) {   /* usdiag: one-shot US-forward CSR snapshot + verdict (2026-07-15 in-driver analyzer) */
		unsigned int act, eqd, gnt, hec, invg, usext, gem0, bst0, gem1, bst1;
		act   = *(volatile unsigned int *)0xbfb640bcUL & 0xf;   /* G_ACTIVATION low4 = O-state */
		eqd   = *(volatile unsigned int *)0xbfb640a8UL;         /* G_EQD ranging delay */
		gnt   = *(volatile unsigned int *)0xbfb64234UL;         /* DBG_BWM_GNT_STS */
		hec   = *(volatile unsigned int *)0xbfb64330UL;         /* DBG_GEM_HEC_ONE_ERR_CNT */
		invg  = *(volatile unsigned int *)0xbfb6437cUL;         /* SNIFF_GTC_INVLD_GEM_BYTE */
		usext = *(volatile unsigned int *)0xbfb64378UL;         /* DBG_US_GTC_EXTR_ETH_CNT */
		gem0  = *(volatile unsigned int *)0xbfb6430cUL;         /* DBG_TX_GEM_CNT */
		bst0  = *(volatile unsigned int *)0xbfb64310UL;         /* DBG_TX_BST_CNT */
		msleep(1500);
		gem1  = *(volatile unsigned int *)0xbfb6430cUL;
		bst1  = *(volatile unsigned int *)0xbfb64310UL;
		printk(KERN_ERR "== GPON US-DIAG ==\n");
		printk(KERN_ERR " act_state      : O%u\n", act);
		printk(KERN_ERR " G_EQD          : 0x%08x  (nonzero+stable => ranged/locked)\n", eqd);
		printk(KERN_ERR " BWM_GNT_STS    : 0x%08x  (>0 => OLT granting our alloc)\n", gnt);
		printk(KERN_ERR " TX_GEM_CNT +   : %u in 1.5s  (>0 => US GEM frames egressing on our gem)\n", gem1 - gem0);
		printk(KERN_ERR " TX_BST_CNT +   : %u in 1.5s  (>0 => US bursts firing in grant)\n", bst1 - bst0);
		printk(KERN_ERR " GEM_HEC_1ERR   : 0x%08x  (0 => clean GEM headers)\n", hec);
		printk(KERN_ERR " US_GTC_EXTR    : 0x%08x\n", usext);
		printk(KERN_ERR " SNIFF_INVLD_GEM: 0x%08x  (neighbor GEMs filtered at GTC)\n", invg);
		if(act == 5 && gnt && hec == 0)
			printk(KERN_ERR " VERDICT: O5 + granted + no-HEC%s => OLT ACCEPTS our upstream burst.\n"
			                "          If PPPoE still gets no PADO, the US-forward gate is OFF-DEVICE (OLT/OSS).\n",
			                (gem1 - gem0) ? " + US-egressing" : "");
		else
			printk(KERN_ERR " VERDICT: not serving-ready (O%u gnt=0x%x hec=0x%x) -- check ranging/OMCI first.\n",
			                act, gnt, hec);
		return count;
	}
	if(!strcmp(cmd, "dumptx")) {   /* dumptx [base_hex] [len_hex]; default 0xbfaf0000 len 0x600 */
		unsigned long base = subcmd[0] ? simple_strtoul(subcmd, NULL, 16) : 0xbfaf0000UL;
		unsigned long len  = action ? action : 0x600, a;
		for(a = base; a < base + len; a += 16)
			printk(KERN_DEBUG "%08lx: %08x %08x %08x %08x\n", a,
				*(volatile unsigned int *)(a), *(volatile unsigned int *)(a+4),
				*(volatile unsigned int *)(a+8), *(volatile unsigned int *)(a+12));
		return count;
	}
	if(!strcmp(cmd, "dump70")) {   /* dump70 [reg_hex] [count]; en7571 dev-0x70 analog space (SIF) */
		extern void en7571_dump70(unsigned int base, unsigned int count);
		unsigned int b = subcmd[0] ? (unsigned int)simple_strtoul(subcmd, NULL, 16) : 0;
		en7571_dump70(b, action);   /* action=0 -> curated key-register set */
		return count;
	}
	if(!strcmp(cmd, "sff")) {   /* read the optical module SFF-8472 A0 identity (dev 0x50) */
		extern void en7571_read_sff(void);
		en7571_read_sff();
		return count;
	}
	if(!strcmp(cmd, "poke70") || !strcmp(cmd, "poke70r")) {  /* poke70[r] <reg_hex> <val_hex> */
		extern void en7571_poke70(unsigned int reg, unsigned int val, int raw);
		unsigned int r = (unsigned int)simple_strtoul(subcmd, NULL, 16);
		en7571_poke70(r, action, cmd[6] == 'r');  /* poke70r -> raw 4-byte; poke70 -> 12-bit DAC */
		return count;
	}

    if(!strcmp(cmd, "show_flag")) {
        printk("sysLinkStatus:\t %d\n",gpPonSysData->sysLinkStatus);
        printk("sysStartup:\t %d\n",gpPonSysData->sysStartup);
    #ifndef TCSUPPORT_CPU_EN7521
        printk("txPacketCount:\t %d\n",gpWanPriv->devCfg.txPacketCount);
    #endif
        printk("drop_print_flag:\t %d\n",drop_print_flag);
		printk("xpon_mac_print_open:\t %d\n",xpon_mac_print_open);
#ifndef TCSUPPORT_CPU_EN7521
        extern atomic_t fe_reset_flag;
        printk("fe_reset_flag:\t %d\n", atomic_read(&fe_reset_flag));
#endif /* TCSUPPORT_CPU_EN7521 */
	}
#ifdef TCSUPPORT_CPU_EN7521
	else if(!strcmp(cmd, "set")) {
		if(!strcmp(subcmd, "rx_ratelimit")) {
			gpon_enable_cpu_protection(action);
			printk("set rx ratelimit %d FPS success\n", action);
		}
	}
#endif
	
	else if(!strcmp(cmd, "gpon_recovery_dbg"))
	{
		if(!strcmp(subcmd, "1")){
			gponRecovery.dbgPrint = 1;
		}else{
			gponRecovery.dbgPrint = 0;
		}
	}
	
    else if(!strcmp(cmd, "olttype")) {
		if(!strcmp(subcmd, "set")) {
			gpPonSysData->sysOltType = action;
			printk("set olt type: %d\n",gpPonSysData->sysOltType);
		} else{
			printk("get olt type: %d\n",gpPonSysData->sysOltType);
		}
	} else	if (!strcmp(cmd, "pwan")){
		if (!strcmp(subcmd, "qos")){
			xpon_set_qdma_qos(action);
			ECNT_QDMA_GREEN_DROP_CTRL_HOOK(ECNT_QDMA_SET_QOS_FLAG,action);
		}else if (!strcmp(subcmd, "mapqos")){
			QDMA_TxQosScheduler_T txQosSch;

			if (action ==1 ){
				if (gpWanPriv->devCfg.flags.isQosUp){
					printk("xPon: Qos is aleady up.\n");
					return count;
				}
				xpon_set_qdma_qos(1);
			}else if (action == 0){
				xpon_set_qdma_qos(0);
			}
			
			// set qos scheduler to sp mode
			memset(&txQosSch, 0, sizeof(QDMA_TxQosScheduler_T));
			for (i = 0; i < CONFIG_QDMA_QUEUE; ++i){
				txQosSch.queue[i].weight= 255; // skip set queue weight
			}

			for (i = 0; i < CONFIG_QDMA_CHANNEL; ++i){
				txQosSch.channel = i;
				txQosSch.qosType = QDMA_TXQOS_TYPE_SP;
#ifdef TCSUPPORT_CPU_EN7521
				QDMA_API_SET_TX_QOS(ECNT_QDMA_WAN, &txQosSch);
#else /* TCSUPPORT_CPU_EN7521 */
				qdma_set_tx_qos(&txQosSch);
#endif /* TCSUPPORT_CPU_EN7521 */
			}
		}else if (!strcmp(subcmd, "drop")){
			if (action > 3000){
				printk("Set TxDropTimer = %u\n", action);
				/* PORT 6.18: timer_list lost .data; txDropTimer now re-arms at
				 * the fixed TXDROP_TIMER_MS in gpon_wan.c.  TODO: make the
				 * TXDROP interval runtime-settable again. */
				/* gpWanPriv->txDropTimer.data = action; */
			}
		}
	}else if (!strcmp(cmd, "epon")){
		if (!strcmp(subcmd, "staticrpt")){
			eponStaticRptEnable = ((action==0)?0:1);			
			printk("%s epon static report!\n", (action==0)?"Disable":"Enable");
			
			if (gpPonSysData->sysLinkStatus == PON_LINK_STATUS_EPON){
				if (action == 0){ // disable static report for epon
					e_glb_cfg_SET_tx_default_rpt(e_glb_cfg, 0);
					e_tx_cal_cnst_SET_default_ovrhd(e_tx_cal_cnst, 0xc);
				}else{ // enable static report & set def rpt value
					e_rpt_len_SET_df_rpt_fifo_clr(e_rpt_len, 1); // clear rpt buffer
					e_rpt_len_GET_df_rpt_fifo_clr(e_rpt_len);
					e_rpt_len_GET_df_rpt_fifo_clr(e_rpt_len);
					e_rpt_len_GET_df_rpt_fifo_clr(e_rpt_len);
					e_rpt_len_GET_df_rpt_fifo_clr(e_rpt_len);
					e_rpt_len_SET_df_rpt_fifo_clr(e_rpt_len, 0); // return normal
					e_rpt_len_SET_df_rpt_dt_len(e_rpt_len, 1);// set def rpt data length
					e_rpt_data_SET_df_rpt_data(e_rpt_data, 0x01018000); // static report value
					e_rpt_data_SET_df_rpt_data(e_rpt_data, 0);					
					e_glb_cfg_SET_tx_default_rpt(e_glb_cfg, 1);
					e_tx_cal_cnst_SET_default_ovrhd(e_tx_cal_cnst, 0x8);
				}
			}
		}
		else if (!strcmp(subcmd, "dying")){
			if(action == 0) {
				gpPonSysData->dyingGaspData.isEponHwFlag = 0 ;	
				eponHwDygaspCtrl(XPON_DISABLE);
				printk("The dying gasp message will be send by software\n") ;		
			} else if (action == 1) {
				gpPonSysData->dyingGaspData.isEponHwFlag = 1 ;	
				eponHwDygaspCtrl(XPON_ENABLE);	
				printk("The dying gasp message will be send by hardware\n") ;	
			}
		}
#ifdef TCSUPPORT_CPU_EN7521
		else if (!strcmp(subcmd, "txoamfavor") && isEN7526c){
			if(action == 0) {
				gpEponPriv->eponCfg.txOamFavorMode = 0;	
				printk("Tx OAM Favor mode is disabled\n") ;		
			} else if (action == 1) {
				gpEponPriv->eponCfg.txOamFavorMode = 1;	
				printk("Tx OAM Favor mode is enabled\n") ;	
			}
		}
#endif
#ifdef TCSUPPORT_PON_TEST
		else if (!strcmp(subcmd, "tmpmac")){
			int tmp[6]= {0}, i =0;

			if (action ==1){
				if (9==sscanf(val_string, "%s %s %x %02x:%02x:%02x:%02x:%02x:%02x", cmd, subcmd, &action, \
					&tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5])){
					for (i=0; i<6; i++){
						eponDrvTmpMacAddr[i] = tmp[i];
					}
					eponDrvDbgMask |= EPON_DBGMASK_TEMPMAC;
					printk("Enable epon tmp mac %02x:%02x:%02x:%02x:%02x:%02x!\n DBGMASK = 0x%08x\n", \
						eponDrvTmpMacAddr[0], eponDrvTmpMacAddr[1], eponDrvTmpMacAddr[2], \
						eponDrvTmpMacAddr[3], eponDrvTmpMacAddr[4], eponDrvTmpMacAddr[5], eponDrvDbgMask);
				}else{
					printk("Error: param is wrong!\n");
				}
			}else{
				eponDrvDbgMask &= ~EPON_DBGMASK_TEMPMAC;
				printk("Disable epon tmp mac!\n");
			}
		}
#endif		
	}
#if defined(TCSUPPORT_EPON_ATTACK_ENHANCE)
	else if (!strcmp(cmd, "drop")){
		if (!strcmp(subcmd, "1")){
			printk("Enable the attack enhance function!\n");	
			attackEnhance = 1;
		}
		else
		{	
			printk("Disable the attack enhance function!\n");	
			attackEnhance = 0;
		}

	}
	else if (!strcmp(cmd, "dropThreld")){
		int threld = atoi_temp(subcmd);
		if(threld>=1 && threld<=0x1000)
			dropThred = threld;
		printk("The threld for drop packets is 0x%08lx", dropThred);
	}
	else if (!strcmp(cmd, "dropTimer")){
		int dropTimer = atoi_temp(subcmd);
		if(dropTimer>=1 && dropTimer<=60)
			rxDropCheckTimer = dropTimer;
		printk("The rx drop check timer is 0x%08lx", rxDropCheckTimer);
	}		
#endif

#ifdef XPON_MAC_CONFIG_DEBUG
	if(!strcmp(cmd, "msg")) {
		int level = gpPonSysData->debugLevel ;
		
		if(!strcmp(subcmd, "act")) {
			gpPonSysData->debugLevel = (action) ? (level|MSG_ACT) : (level&~MSG_ACT) ;
		} else if(!strcmp(subcmd, "omci")) {
			gpPonSysData->debugLevel = (action) ? (level|MSG_OMCI) : (level&~MSG_OMCI) ;
		} else if(!strcmp(subcmd, "oam")) {
			gpPonSysData->debugLevel = (action) ? (level|MSG_OAM) : (level&~MSG_OAM) ;
		} else if(!strcmp(subcmd, "int")) {
			gpPonSysData->debugLevel = (action) ? (level|MSG_INT) : (level&~MSG_INT) ;
		} else if(!strcmp(subcmd, "trace")) {
			gpPonSysData->debugLevel = (action) ? (level|MSG_TRACE) : (level&~MSG_TRACE) ;
		} else if(!strcmp(subcmd, "warning")) {
			gpPonSysData->debugLevel = (action) ? (level|MSG_WARN) : (level&~MSG_WARN) ;
		} else if(!strcmp(subcmd, "content")) {
			gpPonSysData->debugLevel = (action) ? (level|MSG_CONTENT) : (level&~MSG_CONTENT) ;
		} else if(!strcmp(subcmd, "debug")) {
			gpPonSysData->debugLevel = (action) ? (level|MSG_DBG) : (level&~MSG_DBG) ;
		} else if(!strcmp(subcmd, "err")) {
			gpPonSysData->debugLevel = (action) ? (level|MSG_ERR) : (level&~MSG_ERR) ;
		} else if(!strcmp(subcmd, "eqd")) {
			gpPonSysData->debugLevel = (action) ? (level|MSG_EQD) : (level&~MSG_EQD) ;
		} else if(!strcmp(subcmd, "xmcs")) {
			gpPonSysData->debugLevel = (action) ? (level|MSG_XMCS) : (level&~MSG_XMCS) ;
		} 
		printk("Debug Level: %x\n", gpPonSysData->debugLevel) ; 
	}  

#ifdef TCSUPPORT_WAN_GPON
	else if(!strcmp(cmd, "csr")) {
		if(!strcmp(subcmd, "gem")) {
			gpon_dvt_gemport(action) ;
		}
		else if(!strcmp(subcmd, "tcont")) {
			if(G_TCONT_ID_0_1->Bits.t_cont0_vld)
				printk("Channel 0 valid, ID:%d\n", G_TCONT_ID_0_1->Bits.t_cont0_id) ; 
			if(G_TCONT_ID_0_1->Bits.t_cont1_vld)
				printk("Channel 1 valid, ID:%d\n", G_TCONT_ID_0_1->Bits.t_cont1_id) ; 
			if(G_TCONT_ID_2_3->Bits.t_cont2_vld)
				printk("Channel 2 valid, ID:%d\n", G_TCONT_ID_2_3->Bits.t_cont2_id) ; 
			if(G_TCONT_ID_2_3->Bits.t_cont3_vld)
				printk("Channel 3 valid, ID:%d\n", G_TCONT_ID_2_3->Bits.t_cont3_id) ; 
			if(G_TCONT_ID_4_5->Bits.t_cont4_vld)
				printk("Channel 4 valid, ID:%d\n", G_TCONT_ID_4_5->Bits.t_cont4_id) ; 
			if(G_TCONT_ID_4_5->Bits.t_cont5_vld)
				printk("Channel 5 valid, ID:%d\n", G_TCONT_ID_4_5->Bits.t_cont5_id) ; 
			if(G_TCONT_ID_6_7->Bits.t_cont6_vld)
				printk("Channel 6 valid, ID:%d\n", G_TCONT_ID_6_7->Bits.t_cont6_id) ; 
			if(G_TCONT_ID_6_7->Bits.t_cont7_vld)
				printk("Channel 7 valid, ID:%d\n", G_TCONT_ID_6_7->Bits.t_cont7_id) ; 
			if(G_TCONT_ID_8_9->Bits.t_cont8_vld)
				printk("Channel 8 valid, ID:%d\n", G_TCONT_ID_8_9->Bits.t_cont8_id) ; 
			if(G_TCONT_ID_8_9->Bits.t_cont9_vld)
				printk("Channel 9 valid, ID:%d\n", G_TCONT_ID_8_9->Bits.t_cont9_id) ; 
			if(G_TCONT_ID_10_11->Bits.t_cont10_vld)
				printk("Channel 10 valid, ID:%d\n", G_TCONT_ID_10_11->Bits.t_cont10_id) ; 
			if(G_TCONT_ID_10_11->Bits.t_cont11_vld)
				printk("Channel 11 valid, ID:%d\n", G_TCONT_ID_10_11->Bits.t_cont11_id) ; 
			if(G_TCONT_ID_12_13->Bits.t_cont12_vld)
				printk("Channel 12 valid, ID:%d\n", G_TCONT_ID_12_13->Bits.t_cont12_id) ; 
			if(G_TCONT_ID_12_13->Bits.t_cont13_vld)
				printk("Channel 13 valid, ID:%d\n", G_TCONT_ID_12_13->Bits.t_cont13_id) ; 
			if(G_TCONT_ID_14_15->Bits.t_cont14_vld)
				printk("Channel 14 valid, ID:%d\n", G_TCONT_ID_14_15->Bits.t_cont14_id) ; 
			if(G_TCONT_ID_14_15->Bits.t_cont15_vld)
				printk("Channel 15 valid, ID:%d\n", G_TCONT_ID_14_15->Bits.t_cont15_id) ; 
		}
	} else if(!strcmp(cmd, "test")) {
		if(!strcmp(subcmd, "loopback")) {
			for(i=0 ; i<CONFIG_GPON_MAX_GEMPORT ; i++) {
				if(gpWanPriv->gpon.gemPort[i].info.valid) {
					gwan_config_gemport(gpWanPriv->gpon.gemPort[i].info.portId, ENUM_CFG_LOOPBACK, action) ;
				}
			}
			gpWanPriv->devCfg.flags.isRandomLb = (action==2) ? 1 : 0 ;
			printk("Test Loopback Mode: %d\n", action) ;
		} else if(!strcmp(subcmd, "gpon")) {
			gpPonSysData->sysLinkStatus = PON_LINK_STATUS_GPON ;
		} else if(!strcmp(subcmd, "epon")) {
			gpPonSysData->sysLinkStatus = PON_LINK_STATUS_EPON ;
		} else if(!strcmp(subcmd, "eqd")) {
			gpon_dvt_eqd_adjustment(action) ;
			printk("Modify the EqD to %d in O5 state\n", action) ;
        } else if(!strcmp(subcmd, "eqd_dis")) {
            if(action==1) {
                gpPonSysData->gponPrivData.gponCfg.dis_ranging_in_o5=1;
                printk("\nDisable Ranging in O5!\n");
            } else {
                gpPonSysData->gponPrivData.gponCfg.dis_ranging_in_o5=0;
                printk("\nEnable Ranging in O5!\n");
            }    
		} else if(!strcmp(subcmd, "report")) {
			//enableQdmaScheduling() ;	
		} else if(!strcmp(subcmd, "eqdOffsetFlag")) {
			gpGponPriv->gponCfg.flags.eqdOffsetFlag = (action) ? GPON_EQD_OFFSET_FLAG_SUBTRACT : GPON_EQD_OFFSET_FLAG_ADD ;
			printk("%s eqdOffset %s the EqD\n", (action)?"Subtract":"Add",
												(action)?"from":"to") ;		
		} else if(!strcmp(subcmd, "eqdO4Offset")) {
			gpGponPriv->gponCfg.eqdO4Offset = action ;
			printk("Modify the EqD offset = %x at ranging_time PLOAM in O4\n", action) ;
		} else if(!strcmp(subcmd, "eqdO5Offset")) {
			gpGponPriv->gponCfg.eqdO5Offset = action ;
			printk("Modify the EqD offset = %x in O5\n", gpGponPriv->gponCfg.eqdO5Offset) ;
			gpon_dvt_eqd_adjustment(gpGponPriv->gponCfg.eqd) ;
		} 
		if(!strcmp(subcmd, "connection")) {
			gpon_dvt_onu_connection(action) ;
			printk("Finish to test the gpon ONU connection.\n") ;
		}
		if(!strcmp(subcmd, "hotplug")) {
			gpon_dvt_transceiver_on_off_connection(action) ;
			printk("Finish to test the gpon ONU hotplug connection.\n") ;
		}
	} else if(!strcmp(cmd, "ploam")) {
		if(!strcmp(subcmd, "dying")) {
			if(action == 0) {
				gpPonSysData->dyingGaspData.isGponHwFlag = GPON_SW ;	
				gponDevHardwareDyingGasp(GPON_SW) ;
				printk("The dying gasp message will be send by software\n") ;		
			} else if (action == 1) {
				gpPonSysData->dyingGaspData.isGponHwFlag = GPON_HW ;	
				gponDevHardwareDyingGasp(GPON_HW) ;
				printk("The dying gasp message will be send by hardware\n") ;		
			} else if (action == 0x10) {
				ploam_send_dying_gasp() ;
			} else if (action == 0x11 ) {
				IO_SREG(G_PLOAMu_WDATA, 0) ;
				IO_SREG(G_PLOAMu_WDATA, 0) ;
				IO_SREG(G_PLOAMu_WDATA, 0) ;				
				DBG_US_DYING_GASP_CTRL->Bits.dying_gasp_test = 1 ;
			}
		}
	} else if(!strcmp(cmd, "sn")) {
		struct XMCS_GponOnuInfo_S onuInfo ;
		struct XMCS_GponSnPasswd_S gponSnPasswd ;
		
		memset(&gponSnPasswd, 0, sizeof(struct XMCS_GponSnPasswd_S)) ;
		gpon_cmd_proc(GPON_IOG_ONU_INFO, (ulong)&onuInfo) ;
		memcpy(gponSnPasswd.passwd, onuInfo.passwd, GPON_PASSWD_LENS);
		
		memcpy(gponSnPasswd.sn, subcmd, 4) ;
		gponSnPasswd.sn[4] = (unchar)(action>>24) ;
		gponSnPasswd.sn[5] = (unchar)(action>>16) ;
		gponSnPasswd.sn[6] = (unchar)(action>>8) ;
		gponSnPasswd.sn[7] = (unchar)(action) ;
		gpon_cmd_proc(GPON_IOS_SN_PASSWD, (ulong)&gponSnPasswd) ;
	} else if(!strcmp(cmd, "passwd")) {
		if(strlen(subcmd) <= GPON_PASSWD_LENS) {
			memset(gpGponPriv->gponCfg.passwd, 0, GPON_PASSWD_LENS) ;
			strcpy(gpGponPriv->gponCfg.passwd, subcmd) ;
			printk("Password: %s\n", gpGponPriv->gponCfg.passwd) ;
		}
	} else if(!strcmp(cmd, "show")) {
		if(!strcmp(subcmd, "xpon_print")) {
        	xpon_mac_print_open = action;
		}else if(!strcmp(subcmd, "pwan_drop")) {
        	drop_print_flag = action;
		}
		else if(!strcmp(subcmd, "gem")) {
			struct XMCS_GemPortInfo_S gemInfo ;
			uint gemPortId, gemIdx ;
			if(xmcs_get_gem_port_info(&gemInfo) < 0) {
				printk("exec failed") ;
			} else {
				for(i=0 ; i<gemInfo.entryNum ; i++) {
					gemPortId = gemInfo.info[i].gemPortId;
					gemIdx = (gpWanPriv->gpon.gemIdToIndex[gemPortId] & GPON_GEM_IDX_MASK);		
					printk("GEM ID:%d, Alloc ID:%d, If Idx:%d, Loopback:%s, Encryption:%s, valid is %d, chn:%d\n", 
								gemInfo.info[i].gemPortId, 
								gemInfo.info[i].allocId, 
								gemInfo.info[i].aniIdx, 
								(gemInfo.info[i].lbMode)?"ENABLE":"DISABLE", 
								(gemInfo.info[i].enMode)?"ENABLE":"DISABLE",
								gpWanPriv->gpon.gemPort[gemIdx].info.valid,
								gpWanPriv->gpon.gemPort[gemIdx].info.channel) ;
				}
			}
		}
		else if(!strcmp(subcmd, "llid"))
		{
			struct XMCS_EponLlidInfo_S Info;
			memset(&Info, 0, sizeof(Info));
			if(xmcs_get_llid_info(&Info) < 0)
			{
		    	printk("exec failed") ;
			}
			else
			{
				for(i=0 ; i<Info.entryNum ; i++) 
				{
					printk("idx = %d, llid = %d, Channel = %d, rxMode = %d, txMode = %d.\n", Info.info[i].idx, Info.info[i].llid, Info.info[i].channel, Info.info[i].rxMode, Info.info[i].txMode) ;
				}
			}
		}
		else if(!strcmp(subcmd, "tcont")) {
			struct XMCS_TcontInfo_S tcontInfo ;
			if(xmcs_get_tcont_info(&tcontInfo) < 0) {
				printk("exec failed") ;
			} else {
				for(i=0 ; i<tcontInfo.entryNum ; i++) {
					printk("TCONT ID:%d, Channel:%d\n", tcontInfo.info[i].allocId, tcontInfo.info[i].channel) ;
				}
			}
		} else if(!strcmp(subcmd, "superframe")) {
			uint counter;
			gponDevGetSuperframe(&counter);
			printk("Super frame Counter = %x\n", counter);
		} else if(!strcmp(subcmd, "status")) {
			struct XMCS_GponOnuInfo_S onuInfo ;
			gpon_cmd_proc(GPON_IOG_ONU_INFO, (ulong)&onuInfo) ;
			printk("ONU Info:\n") ;
			printk("ONU ID: 	%d\n", onuInfo.onuId) ;
			printk("ONU State: 	%d\n", onuInfo.state) ;
			printk("SN: 		") ;
			for(i=0 ; i<4 ; i++) {
				printk("%c", onuInfo.sn[i]) ;
			}
			for(i=4 ; i<8 ; i++) {
				printk("%.2x", onuInfo.sn[i]) ;
			}
			printk("\nPASSWD: 	%s\n", onuInfo.passwd) ;
			printk("Key Idx: 	%d\n", onuInfo.keyIdx) ;
			printk("Key: 		") ;
			for(i=0 ; i<16 ; i++) {
				printk("%.2x", onuInfo.key[i]) ;
			}
			printk("\nO1 Timer: 	%d\n", onuInfo.actTo1Timer) ;
			printk("O2 Timer: 	%d\n", onuInfo.actTo2Timer) ;
			printk("OMCC ID: 	%d\n", onuInfo.omcc) ;
		} else if(!strcmp(subcmd, "counter")) {
			uint tmp, rdata, tdata ;
			
			for(i=0 ; i<CONFIG_GPON_MAX_GEMPORT ; i++) {
				if(gpWanPriv->gpon.gemPort[i].info.valid) {
					if(action == gpWanPriv->gpon.gemPort[i].info.portId || action==0x10000) {
						gponDevGetGemPortCounter(gpWanPriv->gpon.gemPort[i].info.portId, GEMPORT_RX_FRAME_CNT, &tmp, &rdata) ;
						gponDevGetGemPortCounter(gpWanPriv->gpon.gemPort[i].info.portId, GEMPORT_TX_FRAME_CNT, &tmp, &tdata) ;
						printk("Gem Port:%d, SW Tx:%u, SW Rx:%u, HW Tx:%u, HW Rx:%u\n", 
										gpWanPriv->gpon.gemPort[i].info.portId,
										(uint)gpWanPriv->gpon.gemPort[i].stats.tx_packets, 
										(uint)gpWanPriv->gpon.gemPort[i].stats.rx_packets, 
										tdata, rdata) ;
					}
				}
			}
		} else if(!strcmp(subcmd, "gemcounter")) {
			struct XMCS_GponGemCounter_S gponGemCounter ;
			memset(&gponGemCounter, 0, sizeof(struct XMCS_GponGemCounter_S)) ;
			gponGemCounter.gemPortId = action ;

			get_counter_from_reg(&gponGemCounter) ;
			printk("Gem Port:%d\n", gponGemCounter.gemPortId);
			printk("GemFrame Rx High:%u, GemFrame Rx Low:%u\n", gponGemCounter.rxGemFrameH, gponGemCounter.rxGemFrameL) ;
			printk("GemPayload Rx High:%u, GemPayload Rx Low:%u\n", gponGemCounter.rxGemPayloadH, gponGemCounter.rxGemPayloadL) ;
			printk("GemFrame Tx High:%u, GemFrame Tx Low:%u\n", gponGemCounter.txGemFrameH, gponGemCounter.txGemFrameL) ;
			printk("GemPayload Tx High:%u, GemPayload Tx Low:%u\n", gponGemCounter.txGemPayloadH, gponGemCounter.txGemPayloadL) ;
		}else if(!strcmp(subcmd, "gpon_recovery_backup")){
			printk("------ bakAllocId ------\n");
			for(i = 0; i < gpGponPriv->pGponRecovery->allocIdNum; i++)
			{
				printk("gponRecovery.allocId[%d] = %d\n", i, gpGponPriv->pGponRecovery->allocId[i]);
			}
			printk("\n");
			printk("------ bakGemport ------\n");
			for(i = 0; i < gpGponPriv->pGponRecovery->gemPortNum; i++)
			{
				printk("gponRecovery.gemPort[%d].allocId = %d\n", i, gpGponPriv->pGponRecovery->gemPort[i].allocId);
				printk("gponRecovery.gemPort[%d].gemPortId = %d\n", i, gpGponPriv->pGponRecovery->gemPort[i].gemPortId);
				printk("gponRecovery.gemPort[%d].channel = %d\n", i, gpGponPriv->pGponRecovery->gemPort[i].channel);
				printk("gponRecovery.gemPort[%d].encryption = %d\n", i, gpGponPriv->pGponRecovery->gemPort[i].encryption);
				printk("\n");
			}
		}
	} else if(!strcmp(cmd, "gpon")) {
		if(!strcmp(subcmd, "startup")) {
			if((action == 0) || (action == 2)) { /*Disable or PowerDown*/
				if_cmd_proc(IO_IOS_WAN_LINK_START, action) ;
				//gpPonSysData->sysStartup = PON_WAN_STOP ;
				//gpon_disable() ;
				printk("Disable GPON ONU\n") ;
			} else {
				if_cmd_proc(IO_IOS_WAN_LINK_START, action) ;
				//gpPonSysData->sysStartup = PON_WAN_START ;
				//gpon_enable() ;
				printk("Enable GPON ONU\n") ;
			}
		} else if(!strcmp(subcmd, "clear")) {
			gponDevClearSwCounter();
			printk("Successful to clear the GEM counter\n") ;
		} else if(!strcmp(subcmd, "cntreset")) {
			gponDevClearHwCounter();
		} else if(!strcmp(subcmd, "rei")) {
			int errCount = action ;
			ploam_send_rei_msg(errCount, &reiSeq) ;
			printk("Send REI PLOAM message successful\n") ;
		} else if(!strcmp(subcmd, "responseTime")) {
			 gpGponPriv->gponCfg.onuResponseTime = action ;
			printk("Set ONU Response Time to %x\n", gpGponPriv->gponCfg.onuResponseTime) ;
		} else if(!strcmp(subcmd, "preambleoffset")) {
			gpGponPriv->gponCfg.t3PreambleOffset = action ;
			printk("Set the O3/O4 type 3 preamble offset %d successful.\n", gpGponPriv->gponCfg.t3PreambleOffset) ;
		} else if(!strcmp(subcmd, "preambleFlag")) {
			gpGponPriv->gponCfg.flags.preambleFlag = action ;
			printk("%s the preambe length over 128 byte\n", (action)?"Enable":"Disable") ;		
		} else if(!strcmp(subcmd, "sniffer")) {
			int i = 0;
			uint gemIdx = 0;
			uint value = 0;
			gpGponPriv->gponCfg.flags.sniffer = action ;
			if (gpGponPriv->gponCfg.flags.sniffer == XPON_ENABLE){
				#ifdef TCSUPPORT_CPU_EN7521
				FE_API_SET_DROP_CRC_ERR_ENABLE(FE_GDM_SEL_GDMA2, FE_DISABLE);
				FE_API_SET_DROP_RUNT_ENABLE(FE_GDM_SEL_GDMA2, FE_DISABLE);
				#else
				value = IO_GREG(FE_GDMA2_FWD_CFG);
				value &= ~(0x1800000);
				IO_SREG(FE_GDMA2_FWD_CFG, value);
				#endif
				for(i=0 ; i<GPON_MAX_GEM_ID ; i++) {
					gemIdx = gpWanPriv->gpon.gemIdToIndex[i] & GPON_GEM_IDX_MASK;
					if((gemIdx >= CONFIG_GPON_MAX_GEMPORT) ||(!gpWanPriv->gpon.gemPort[gemIdx].info.valid)) {	
						gponDevSetGemInfo(i, XPON_ENABLE, XPON_DISABLE) ;
					}
				}
			} else {
				#ifdef TCSUPPORT_CPU_EN7521
				FE_API_SET_DROP_CRC_ERR_ENABLE(FE_GDM_SEL_GDMA2, FE_ENABLE);
				FE_API_SET_DROP_RUNT_ENABLE(FE_GDM_SEL_GDMA2, FE_ENABLE);
				#else
				value = IO_GREG(FE_GDMA2_FWD_CFG);
				value |= (0x1800000);
				IO_SREG(FE_GDMA2_FWD_CFG, value);
				#endif
				for(i=0 ; i<GPON_MAX_GEM_ID ; i++) {
					gemIdx = gpWanPriv->gpon.gemIdToIndex[i] & GPON_GEM_IDX_MASK;
					if((gemIdx >= CONFIG_GPON_MAX_GEMPORT) ||(!gpWanPriv->gpon.gemPort[gemIdx].info.valid)) {	
						gponDevSetGemInfo(i, XPON_DISABLE, XPON_DISABLE) ;
					}
				}
			}
			printk("%s sniffer all gemport downstream data\n", (action)?"Enable":"Disable") ;		
#ifndef TCSUPPORT_CPU_EN7521
		} else if(!strcmp(subcmd, "dly_reset")) {
            gpGponPriv->gponCfg.rstDbgDly = action;
		} else if(!strcmp(subcmd, "dev_reset")) {
            gpGponPriv->gponCfg.macPhyReset = action;
		} else if(!strcmp(subcmd, "mac4208")) {
            gpGponPriv->gponCfg.reg4208 = action;
		} else if(!strcmp(subcmd, "showDbgParam")) {
            print_debug_param();
#endif /* TCSUPPORT_CPU_EN7521 */
		} else if(!strcmp(subcmd, "xmit_dying_gasp")) {
			if (action > 0)
			{
				while (action)
				{
					xmit_dying_gasp_oam();
					action--;
				}
			}
		}
	} else if(!strcmp(cmd, "retire")) {
#ifdef TCSUPPORT_CPU_EN7521
		ret = FE_API_SET_CHANNEL_RETIRE(FE_GDM_SEL_GDMA2, action, FE_LINKDOWN) ;
#else
		ret = qdma_set_channel_retire(action);
#endif
		if(ret == 0) {
			printk("Successful to set the channel %d retire\n", action) ;
		} else {
			printk("Failed to set the channel %d retire\n", action) ;
		}
	} else if(!strcmp(cmd, "pcp")) {
		if(!strcmp(subcmd, "auto")) {
			gpon_dvt_pcp_auto_testing() ;
			printk("Finished to auto test PCP function\n") ;
		} else {
			if(gpGponPriv->gponCfg.dvtPcpCounter) {
				printk("Last PCP check counter is %d, %d\n", gpGponPriv->gponCfg.dvtPcpCounter, gpGponPriv->gponCfg.dvtPcpCheckErr) ;
				gpGponPriv->gponCfg.dvtPcpCounter = 0 ;
			}
			
			if(!strcmp(subcmd, "disable")) {	
				gpGponPriv->gponCfg.dvtPcpCheck = 0 ;
				IO_SREG(0xbfb51514, 0) ;
			} else if(action==0x1 || action==0x2 || action==0x4 || action==0x8) {
				if(!strcmp(subcmd, "gdmrx")) {
					gpGponPriv->gponCfg.dvtPcpCheck = (0x40 | action) ;
					IO_SREG(0xbfb51514, (action<<8)) ;
				} else if(!strcmp(subcmd, "cdmrx")) {
					gpGponPriv->gponCfg.dvtPcpCheck = (0x20 | action) ;
					IO_SREG(0xbfb51514, (action<<4)) ;
				} else if(!strcmp(subcmd, "cdmtx")) {
					gpGponPriv->gponCfg.dvtPcpCheck = (0x10 | action) ;
					IO_SREG(0xbfb51514, (action)) ;
				}
			}
			printk("Successful to set the PCP %.8x\n", IO_GREG(0xbfb51514)) ;
		} 
	}
	else if(!strcmp(cmd, "vlan_trans"))
	{
		uint vlan1,vlan2,vlanCnt;
	}
	else if(!strcmp(cmd, "epon"))
	{
		if(!strcmp(subcmd, "laser_time"))
		{
			g_EponLaserTime = action;
			printk("epon laser time del %d\n",g_EponLaserTime);
		}
#ifndef TCSUPPORT_CPU_EN7521
		if(!strcmp(subcmd, "mpcp_fwd"))
		{
			setMpcpFwd(action);
			printk("set epon mpcp fwd %s\n",(action)?"Enable":"Disable");
		}
#endif
		else if(!strcmp(subcmd, "set_llid"))
		{
			printk("epon set llid %d traffic up\n",action);
			xmcs_set_epon_llid_config(action);
		}
		if(!strcmp(subcmd, "silent_time"))
		{
			sscanf(val_string, "%s %s %d", cmd, subcmd, &action) ;
			g_silence_time = action;
			printk("epon set silent time %d\n",g_silence_time);
		}		
	}
	else if(!strcmp(cmd, "assign"))
	{
		ushort gemportId = 0;
		if(!strcmp(subcmd, "gemport"))
		{
			gemportId = action;
			assignGemportId(gemportId);
		}
	}	
	else if(!strcmp(cmd, "bip_cnt")) 
	{
			if(!strcmp(subcmd, "show")) 
			{
				unsigned int bip_cnt_tmp = 0;
				bip_cnt_tmp = XPON_PHY_GET(PON_GET_PHY_BIP_COUNTER);
				gpGponPriv->bip_cnt_val += bip_cnt_tmp;
				printk("current_bip_cnt_counter = %u, total_bip_cnt_counter = %u\n",bip_cnt_tmp,gpGponPriv->bip_cnt_val);
			}
			else if(!strcmp(subcmd, "clear")) 
			{
				gpGponPriv->bip_cnt_val = 0;
			}
	}
	else if(!strcmp(cmd, "emergence_state"))
	{
		if(!strcmp(subcmd, "show"))
		{
			printk("show GPON emergence_state begin\n");
			gpon_show_emergence_info();
		}
	}
	else if(!strcmp(cmd, "green_drop"))
	{
		if(!strcmp(subcmd, "disable"))
		{
			printk("disable green_drop config \n");
			green_drop_flag = FALSE;
			xpon_set_qdma_qos(XPON_DISABLE);
		}
		else if(!strcmp(subcmd, "enable"))
		{
			printk("enable green_drop config \n");
			green_drop_flag = TRUE;
			xpon_set_qdma_qos(XPON_ENABLE);
		}
	}
#if defined(TCSUPPORT_FWC_ENV)
	else if(!strcmp(cmd, "fhnet_mapping") ) 
	{
		uint value1, value2=0, value3 ;
		sscanf(val_string, "%s %x %d %x %x", cmd, &value1, &value2, &value3, &gFhDrop) ;
		fhNetMappingDebug = value1;
		gMappingRet.gemportId = value2;
		gMappingRet.queueId = value3;
		fhnet_set_vlan_mapping_hook();
		printk("fhnet_mapping debug %s gemport %d queueId %d\n",value1?"enable":"disbale",value2,value3);
	}
#endif
#ifdef TCSUPPORT_VNPTT
	else if(!strcmp(cmd, "ignore_deactive"))
	{
		if(!strcmp(subcmd, "disable"))
		{
			printk("no ignore deactive \n");
			ignore_deactive_flag = FALSE;
		}
		else if(!strcmp(subcmd, "enable"))
		{
			printk("ignore deactive \n");
			ignore_deactive_flag = TRUE;
		}
	}
#endif
#endif /* TCSUPPORT_WAN_GPON */
#endif /* XPON_MAC_CONFIG_DEBUG */
	else if(!strcmp(cmd, "epon_test") )
	{
		if(!strcmp(subcmd, "holdoverflag")){
			eponDrv.hldoverEnable = action;
			printk("hldOverFlag = %d\n",eponDrv.hldoverEnable);
		}
		else if(!strcmp(subcmd, "typeBOnGoing")){
			eponDrv.typeBOnGoing = action;
			printk("typeBOnGoing = %d\n",eponDrv.typeBOnGoing);
		}
		else if(!strcmp(subcmd, "time")){
			eponDrv.hldOverTime= action;
			printk("hldOverTime = %d\n",eponDrv.hldOverTime);
		}
	}

	return count ;
}

/*****************************************************************************
******************************************************************************/
#if defined(TCSUPPORT_CT_PON) || defined(TCSUPPORT_XPON_HAL_API_QOS) || defined(TCSUPPORT_XPON_HAL_API_EXT)
	static int qdma_reg_init(void)
	{
		int i;
		int ret;
#ifdef TCSUPPORT_CPU_EN7521
		QDMA_txCngstCfg_t pTxCngstCfg;
		QDMA_TxQDynCngstThrh_T txqDynCngstThrh;
#else
		QDMA_TxQueueCongestThreshold_T txqThreshold;
		QDMA_TxQueueCongestScale_T txqScale;
		QDMA_Mode_t qdmaMode = QDMA_ENABLE;
#endif
		QDMA_TxBufCtrl_T txBufUsage;
		
#if defined (QOS_REMARKING) || defined (TCSUPPORT_HW_QOS)
#ifdef TCSUPPORT_CPU_EN7521
		pTxCngstCfg.txCngstMode = QDMA_TXCNGST_DYNAMIC_NORMAL;
		QDMA_API_SET_TXQ_CNGST_AUTO_CONFIG(ECNT_QDMA_WAN, &pTxCngstCfg);
		QDMA_API_SET_TXQ_DEI_DROP_MODE(ECNT_QDMA_WAN, QDMA_ENABLE);

		memset(&txqDynCngstThrh, 0, sizeof(QDMA_TxQDynCngstThrh_T));
		QDMA_API_GET_TXQ_CNGST_DYNAMIC_THRESHOLD(ECNT_QDMA_WAN, &txqDynCngstThrh);
		/* default for single Tcont */
		txqDynCngstThrh.dynCngstChnlMinThrh = 0xA0;
		QDMA_API_SET_TXQ_CNGST_DYNAMIC_THRESHOLD(ECNT_QDMA_WAN, &txqDynCngstThrh);
#else
		/* do QDMA congestion configuration, otherwise low
		 * priority packets may use all tx descriptors. 
		 * We suppose each queue (for all channels) can 
		 * use 16*5==80 dscps at most! */
		memset(&txqScale, 0, sizeof(QDMA_TxQueueCongestScale_T));
		txqScale.maxScale = QDMA_TXQUEUE_SCALE_16_DSCP;
		qdma_set_congestion_scale(&txqScale);
		
		memset(&txqThreshold, 0, sizeof(QDMA_TxQueueCongestThreshold_T));
		for (i = 0; i < TX_QUEUE_NUM; i++)
		{
			txqThreshold.queueIdx = i;
			txqThreshold.grnMaxThreshold = 5;
			qdma_set_congest_threshold(&txqThreshold);
		}

#endif
	
		/* limite PSE buffer usage for each channel, then
		 * low priority packes won't block high priority
		 * packets (in PSE buffer) for too long, in order
		 * to prevent highest priority packets from dropping */
		memset(&txBufUsage, 0, sizeof(QDMA_TxBufCtrl_T));
		txBufUsage.mode = QDMA_ENABLE;
		txBufUsage.chnThreshold = 0x20; //PSE blocks
		txBufUsage.totalThreshold = 0x20;
#ifdef TCSUPPORT_CPU_EN7521
		ret = QDMA_API_SET_TXBUF_THRESHOLD(ECNT_QDMA_WAN, &txBufUsage);
#else /* TCSUPPORT_CPU_EN7521 */
		ret = qdma_set_txbuf_threshold (&txBufUsage);
#endif /* TCSUPPORT_CPU_EN7521 */
		if (ret) {
			printk("\nFAILED(%s): qdma setting for txBufUsage\n" , __FUNCTION__);
			return -1;
		}
#endif
	
		return 0;
	}

#endif
static int xpondrv_qdma_init(void)
{
	QDMA_InitCfg_t qdmaInitCfg ;
	QDMA_TxBufCtrl_T txBufCtrl ;
	uint dbg=0 ;
	int ret ;
	
#if defined(TCSUPPORT_CT_PON) || defined(TCSUPPORT_XPON_HAL_API_QOS) || defined(TCSUPPORT_XPON_HAL_API_EXT)
	/* HW QDMA QoS init */
	qdma_reg_init();
#endif
		
	memset(&qdmaInitCfg, 0, sizeof(QDMA_InitCfg_t)) ;
#ifndef TCSUPPORT_CPU_EN7521
	qdmaInitCfg.txRecycleMode = QDMA_TX_INTERRUPT ;
#ifdef CONFIG_USE_RX_NAPI
	qdmaInitCfg.rxRecvMode = QDMA_RX_NAPI ;
#else
	qdmaInitCfg.rxRecvMode = QDMA_RX_INTERRUPT ;
#endif /* CONFIG_USE_RX_NAPI */
	qdmaInitCfg.txIrqThreshold = 32 ;
	qdmaInitCfg.txIrqPTime = 128 ;
	qdmaInitCfg.rxDelayInt = 0 ;
	qdmaInitCfg.rxDelayPTime = 0 ;
#ifdef TCSUPPORT_MERGED_DSCP_FORMAT
	qdmaInitCfg.cbXmitMsg = pwan_cb_prepare_tx_message ;
#endif /* TCSUPPORT_MERGED_DSCP_FORMAT */
	qdmaInitCfg.cbXmitFinish = pwan_cb_tx_finished ;
#endif
	qdmaInitCfg.cbRecvPkts = pwan_cb_rx_packet ;
	qdmaInitCfg.cbEventHandler = pwan_cb_event_handler ;
#ifdef TCSUPPORT_CPU_EN7521
	QDMA_API_INIT(ECNT_QDMA_WAN, &qdmaInitCfg);
	QDMA_API_TX_DMA_MODE(ECNT_QDMA_WAN, QDMA_ENABLE);
	QDMA_API_RX_DMA_MODE(ECNT_QDMA_WAN, QDMA_ENABLE);
	QDMA_API_LOOPBACK_MODE(ECNT_QDMA_WAN, QDMA_LOOPBACK_DISABLE) ;
#else /* TCSUPPORT_CPU_EN7521 */
	qdma_init(&qdmaInitCfg) ;

	do {
 		if(pwan_prepare_rx_buffer(NULL, NULL) != 0) {
			break ;
		}
		dbg++ ;
	} while(qdma_has_free_rxdscp()) ;

	qdma_dma_mode(QDMA_ENABLE, QDMA_ENABLE, QDMA_ENABLE) ;
	qdma_loopback_mode(QDMA_LOOPBACK_DISABLE) ;
#endif /* TCSUPPORT_CPU_EN7521 */

	//txBufCtrl.mode = QDMA_ENABLE ;
	//txBufCtrl.chnThreshold = 0xC0 ;
	//txBufCtrl.totalThreshold = 0xC0 ;
	//ret = qdma_set_txbuf_threshold(&txBufCtrl) ;
	//if(ret < 0) {
	//	return ret ;
	//}
	xpon_set_qdma_qos(0);

    PON_MSG(MSG_TRACE, "Prepare %d receive packet buffers in QDMA init stage.\n", dbg) ;

	return 0 ;
}

/*****************************************************************************
******************************************************************************/
irqreturn_t xpon_dying_gasp_interrupt(int irq, void *dev_id)
{

#ifdef TCSUPPORT_DYING_GASP_OPTIMIZATION
         if(isEN751221)
         {      
                /* power cost from large to small */
                VPint(CR_INTC_IMR) = 0x0; /* stop all interrupt.*/		
	       VPint(0xbfb40050) = 0x0;  /* stop all interrupt.*/
               
                /*shutdown zarlink slic*/
                /*
                uint8 temp_val =0x04;
                VpMpiCmd(0,0x1,0x04,1, &temp_val);
                */
               
                /*PCIe gen  POWER DOWN */
                VPint(0xbfaf2000) = 0x7f7f0010; /* PCIe gen1 */
                VPint(0xbfac030c) = 0x80000000; /*  PCIe gen2 */

                VPint(0xbfb000c4) = 0x1000603;/* usb port0 + p1 phy clock bit[25][26][27]*/
                VPint(0xbfb000ec) = 0xe0; /* usb port0 + p1 phy reset [6][7][8]*/

                /*SCU reset*/
                VPint(0xbfb00834) = ~((1<<8) | (1<<13) | (1<<31)); /*reset all block exclude timer, GPIO , pon*/   

                /*Disable PPE*/
                VPint(0xbfb50e00) &= ~0x1; 
                
                
                timerSet(5, 2000, ENABLE, TIMER_TOGGLEMODE, TIMER_HALTDISABLE);
                timer_WatchDogConfigure(ENABLE, ENABLE);      
         }
#endif

#ifdef TCSUPPORT_WAN_GPON
	if (gpPonSysData->sysLinkStatus == PON_LINK_STATUS_GPON) {
		if (gpPonSysData->dyingGaspData.isGponHwFlag == GPON_SW) {
			/* Clear pre dying gasp interrupt status */
			IO_SBITS(0xBFB00084, SCU_DYING_GASP_STATUS) ; 
			gpon_detect_dying_gasp();
		}
	}
#endif 	/* TCSUPPORT_WAN_GPON */

#ifdef TCSUPPORT_WAN_EPON
	else	if (gpPonSysData->sysLinkStatus == PON_LINK_STATUS_EPON) {	
		/* Clear pre dying gasp interrupt status */
		IO_SBITS(0xBFB00084, SCU_DYING_GASP_STATUS) ; 
		if (!gpPonSysData->dyingGaspData.isEponHwFlag) {
			xmit_dying_gasp_oam();
		}
	}
#endif /* TCSUPPORT_WAN_EPON */


#ifdef TCSUPPORT_DYING_GASP_OPTIMIZATION
        dying_gasp_setup_mem_cpu();
#endif

	return IRQ_HANDLED;
}

/*****************************************************************************
******************************************************************************/
static int xpon_dying_gasp_init(void)
{
	 /* Register Dying Gasp interrupt */
	if(request_irq(CONFIG_DYING_GASP_IRQ, xpon_dying_gasp_interrupt, 0, "dying gasp", NULL) != 0) {
		printk("Request the interrupt service routine fail, irq:%d.\n", CONFIG_DYING_GASP_IRQ) ;
		return -ENODEV ;
	}
	g_dying_gasp_irq_owned = true;

	return 0;
}


void schedule_fe_reset(void)
{
#ifndef TCSUPPORT_CPU_EN7521
    XPON_DPRINT_MSG("#######About to do FE Reset!#######\n");

    XPON_DAEMON_Job_data_t job     ;
    job.id       = XPON_DAEMON_JOB_FE_RESET ;
    job.priority = XPON_DAEMON_JOB_PRIORITY_HIGH  ;
    gpPonSysData->sysStartup = PON_WAN_STOP;

    xpon_daemon_job_enqueue(&job)  ;
    
    wake_up_xpon_daemon() ;
#endif
}

extern atomic_t to1_timeout_cnt;
extern atomic_t to1_rst_cnt;
static int gpon_stage_change_chk_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int index=0 ;
	off_t pos=0, begin=0 ;

	index += sprintf(buf+ index, "to1_timeout_cnt:%d rst:%d\n", atomic_read(&to1_timeout_cnt),  atomic_read(&to1_rst_cnt)) ;
	CHK_BUF() ;
	*eof = 1 ;

done:
	*start = buf + (off - begin) ;
	index -= (off - begin) ;
	if(index<0)		index = 0 ;
	if(index>count)		index = count ;
	return index ;
}

static int gpon_stage_change_chk_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char val_string[64]={0};
	int timerout_cnt = 0, rst_cnt = 0;
	
	if (count > sizeof(val_string) - 1)
		return -EINVAL ;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT ;
	
	sscanf(val_string, "%d %d", &timerout_cnt, &rst_cnt);
	atomic_set(&to1_timeout_cnt, timerout_cnt);
	atomic_set(&to1_rst_cnt, rst_cnt);
	
	return count ;
}

void xpon_dump_raw_data(char *buf, int *len)
{
	*len += sprintf((*len+buf), "\n");
	*len += sprintf((*len+buf), "**************************************\n");
	*len += sprintf((*len+buf), "**             Raw data             **\n");
	*len += sprintf((*len+buf), "**************************************\n");
	*len += sprintf((*len+buf), "\n");

	*len += sprintf((*len+buf), " SCU_WAN_CONF_REG         (0xBFB00070): %#03x\n", xpon_regs_p->wan_mode_val);
	if(pon_mode & PON_MODE_GPON) {
		*len += sprintf((*len+buf), " G_ACTIVATION     (0xBFB640BC): %#03x\n", xpon_regs_p->gpon_ploam_stat_val);
		*len += sprintf((*len+buf), " G_EQD            (0xBFB640A8): %#03x\n", xpon_regs_p->gpon_eqd_val);
		*len += sprintf((*len+buf), " G_RSP_TIME       (0xBFB640BC): %#03x\n", xpon_regs_p->gpon_resp_time_val);
	}
	if(pon_mode & PON_MODE_EPON) {
		*len += sprintf((*len+buf), " E_MPCP_STAT      (0xBFB66120): %#03x\n", xpon_regs_p->epon_rx_mpcp_cnt);
		*len += sprintf((*len+buf), " E_MPCP_RGST_STAT (0xBFB66124): %#03x\n", xpon_regs_p->epon_tx_mpcp_rgst_cnt);
		*len += sprintf((*len+buf), " E_SYNC_TIME      (0xBFB660D4): %#03x\n", xpon_regs_p->epon_sync_time_val);
	}

	return;
}

void get_epon_stat(char *buf, int *len, epon_stat *epon_stat_cur)
{
	epon_stat_cur->jiffies_val = jiffies;
		
	xpon_regs_p->epon_rx_mpcp_cnt = IO_GREG(e_mpcp_stat);
	epon_stat_cur->dscvr_gate_cnt = (xpon_regs_p->epon_rx_mpcp_cnt)&0xffff;
	epon_stat_cur->rx_rgst_cnt = (((xpon_regs_p->epon_rx_mpcp_cnt)&0x00ff0000)>>16);
				
	xpon_regs_p->epon_tx_mpcp_rgst_cnt = IO_GREG(e_mpcp_rgst_stat);
	epon_stat_cur->tx_rgst_cnt = (((xpon_regs_p->epon_tx_mpcp_rgst_cnt)&0x0000ff00)>>8);
	epon_stat_cur->tx_rgst_ack_cnt = ((xpon_regs_p->epon_tx_mpcp_rgst_cnt)&0xff);
				
	xpon_regs_p->epon_sync_time_val = IO_GREG(e_sync_time);

	if(pon_mode & PHY_RD) {
		if(epon_proc_cnt && !(epon_stat_cur->rx_rgst_cnt-epon_stat_prev->rx_rgst_cnt) && !(epon_stat_cur->tx_rgst_cnt-epon_stat_prev->tx_rgst_cnt) && !(epon_stat_cur->tx_rgst_ack_cnt-epon_stat_prev->tx_rgst_ack_cnt))
			*len += sprintf((*len+buf), " MPCP Status: Registered");
		else if(epon_proc_cnt)
			*len += sprintf((*len+buf), " MPCP Status: Registering");
		else
			*len += sprintf((*len+buf), " MPCP Status: Unknown");
	
		*len += sprintf((*len+buf), "	Jiffies: %#lx(+%#lx +%#ld)\n", epon_stat_cur->jiffies_val, (epon_proc_cnt ? (epon_stat_cur->jiffies_val-epon_stat_prev->jiffies_val) : epon_proc_cnt), (epon_proc_cnt ? (epon_stat_cur->jiffies_val-epon_stat_prev->jiffies_val) : epon_proc_cnt));
		*len += sprintf((*len+buf), " Discover Gate Count:	 %#x(+%#x +%#d)\n", epon_stat_cur->dscvr_gate_cnt, (epon_proc_cnt ? (epon_stat_cur->dscvr_gate_cnt-epon_stat_prev->dscvr_gate_cnt) : epon_proc_cnt), (epon_proc_cnt ? (epon_stat_cur->dscvr_gate_cnt-epon_stat_prev->dscvr_gate_cnt) : epon_proc_cnt));
		*len += sprintf((*len+buf), " RX Register Count: 	 %#x(+%#d)\n", epon_stat_cur->rx_rgst_cnt, (epon_proc_cnt ? (epon_stat_cur->rx_rgst_cnt-epon_stat_prev->rx_rgst_cnt) : epon_proc_cnt));
		*len += sprintf((*len+buf), " TX Register Count: 	 %#x(+%#d)\n", epon_stat_cur->tx_rgst_cnt, (epon_proc_cnt ? (epon_stat_cur->tx_rgst_cnt-epon_stat_prev->tx_rgst_cnt) : epon_proc_cnt));
		*len += sprintf((*len+buf), " TX Register ACK Count:  %#x(+%#d)\n", epon_stat_cur->tx_rgst_ack_cnt, (epon_proc_cnt ? (epon_stat_cur->tx_rgst_ack_cnt-epon_stat_prev->tx_rgst_ack_cnt) : epon_proc_cnt));
		*len += sprintf((*len+buf), " Sync Time: %#x\n", (xpon_regs_p->epon_sync_time_val)&0xffff);

		epon_proc_cnt++;
	}
	
	epon_stat_prev->jiffies_val = epon_stat_cur->jiffies_val;
	epon_stat_prev->dscvr_gate_cnt = epon_stat_cur->dscvr_gate_cnt;
	epon_stat_prev->rx_rgst_cnt = epon_stat_cur->rx_rgst_cnt;
	epon_stat_prev->tx_rgst_cnt = epon_stat_cur->tx_rgst_cnt;
	epon_stat_prev->tx_rgst_ack_cnt = epon_stat_cur->tx_rgst_ack_cnt;
	
	return;
}

/* TODO EN7528: EPON dropped for GPON-only build (rgstAgainTimeout needs
 * TCSUPPORT_EPON_RGST_SILENT). Guarded out so xpondrv.c builds GPON-only. */
#ifdef TCSUPPORT_WAN_EPON
extern epon_t eponDrv;
char mpcpStateStr[MAX_MPCP_STATE_NUM][MAX_MPCP_STATE_STR_LEN] =
{
	"WAIT",
	"REGISTERING",
	"REGISTER_REQUEST",
	"REGISTER_PENDING",
	"RETRY",
	"DENIED",
	"REGISTER_ACK",
	"NACK",
	"REGISTERED",
	"REMOTE_DEREGISTER",
	"LOCAL_DEREGISTER"
};

static void epon_dump_llid_info(char *buf, int *len)
{
	__u8 llidIndex=0;
	eponLlid_t *llidEntry_p = NULL;
	char state[20] = {0};
	__u8 discStateIndex = 0;

	for(llidIndex=0;llidIndex < EPON_LLID_MAX_NUM;llidIndex++)
	{
		if((eponDrv.llidMask)&(1<<llidIndex))//this LLID enable
		{
			llidEntry_p = &(eponDrv.eponLlidEntry[llidIndex]);
			discStateIndex = llidEntry_p->eponMpcp.eponDiscFsm.mpcpDiscState;
			if(discStateIndex >= MAX_MPCP_STATE_NUM)
			{
				printk("MPCP disc State Index %d exceed max Num\n",discStateIndex);
				return;
			}
			strcpy(state,mpcpStateStr[discStateIndex]);
			*len += sprintf((*len+buf)," EPON LLID %d MPCP Disc State: %s \n",llidIndex,state);
			*len += sprintf((*len+buf)," EPON LLID %d is in  %s State \n",llidIndex, ((MPCP_STATE_DENIED == discStateIndex) ? "Silent":"Normal"));
			if(MPCP_STATE_DENIED == discStateIndex)
				*len += sprintf((*len+buf)," EPON LLID %d Silent Time left %d s(Total %d s) \n",llidIndex,llidEntry_p->eponMpcp.eponDiscFsm.rgstAgainTimeout, g_silence_time);
		}
	}
}
#endif /* TCSUPPORT_WAN_EPON */


static int xpon_info_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int len;
	pon_mode = 0;

	if(!epon_proc_cnt) {
		epon_stat_prev = (epon_stat *)kmalloc(sizeof(epon_stat), GFP_KERNEL);
		if(epon_stat_prev == NULL){
			printk("Error: can't allocate buffer for epon_stat_prev.\n");
			return -1;
		}
	}
	epon_stat *epon_stat_cur = (epon_stat *)kmalloc(sizeof(epon_stat), GFP_KERNEL);
	if(epon_stat_cur == NULL){
		printk("Error: can't allocate buffer for epon_stat_cur.\n");
		return -1;
	}
	xpon_regs_p = (xpon_regs *)kmalloc(sizeof(xpon_regs), GFP_KERNEL);
	if(xpon_regs_p == NULL){
		printk("Error: can't allocate buffer for xpon_regs_p.\n");
		return -1;
	}

	len = sprintf(buf, "\n");
	len += sprintf(buf+len, "%s\n", "**************************************");
	len += sprintf(buf+len, "%s\n", "**        XPON MAC Status           **");
	len += sprintf(buf+len, "%s\n", "**************************************");
	len += sprintf(buf+len, "\n");

	// Get PON Mode
	xpon_regs_p->wan_mode_val = IO_GREG(SCU_WAN_CONF_REG) & 0x1;
	
	if(WAN_GPON_MODE == xpon_regs_p->wan_mode_val  && PHY_GPON_CONFIG == gpPhyData->working_mode) {
		pon_mode |= PON_MODE_GPON;
		len += sprintf(buf+len, " Mode: GPON\n");
	}else if(WAN_EPON_MODE == xpon_regs_p->wan_mode_val && PHY_EPON_CONFIG == gpPhyData->working_mode) {
		pon_mode |= PON_MODE_EPON;
		len += sprintf(buf+len, " Mode: EPON\n");
	}else
		len += sprintf(buf+len, " Mode: Error\n");
	
	// Get GPON Status
	if(pon_mode & PON_MODE_GPON) {
		xpon_regs_p->gpon_ploam_stat_val = IO_GREG(G_ACTIVATION_ST);
		xpon_regs_p->gpon_eqd_val        = IO_GREG(G_EQD);
		xpon_regs_p->gpon_resp_time_val  = IO_GREG(G_RSP_TIME);
		if(pon_mode & PHY_RD) {
			len += sprintf(buf+len, " Ploam state: O%d\n", (xpon_regs_p->gpon_ploam_stat_val)&0x7);
			len += sprintf(buf+len, " Equalization Delay: %#x\n", xpon_regs_p->gpon_eqd_val);			
			len += sprintf(buf+len, " Response Time: %#x\n", xpon_regs_p->gpon_resp_time_val);
		}
	}

	// Get EPON Status
#ifdef TCSUPPORT_WAN_EPON
	if(pon_mode & PON_MODE_EPON)
	{
		get_epon_stat(buf, &len, epon_stat_cur);
		epon_dump_llid_info(buf, &len);
	}
#endif /* TCSUPPORT_WAN_EPON */


	// Dump raw data
	xpon_dump_raw_data(buf, &len);

	kfree(epon_stat_cur);
	kfree(xpon_regs_p);

	len -= off;
	*start = buf + off;

	if(len > count)
		len = count;
	else
		*eof = 1;

	if(len < 0)
		len = 0;

	return len;
}

static inline int should_ignore_phy_los(void)
{
    return ((PHY_LINK_STATUS_READY != gpPhyData->phy_link_status ) || (PHY_UNKNOWN_CONFIG == gpPhyData->working_mode) );
}

static void xpon_phy_los_illegal_handler(PHY_Event_Source_t src, PHY_Event_Type_t id)
{

    XPON_DPRINT_MSG("id:%d\n", id);
    
    if(should_ignore_phy_los() ) {
        XPON_DPRINT_MSG("[%s] ignore phy los!\n", __FUNCTION__);
        return;
    }

    change_alarm_led_status(ALARM_LED_FLICKER);
	xpon_los_status = 0;

    switch(gpPhyData->working_mode)
    {
        case PHY_GPON_CONFIG:
            gpon_detect_los_lof();
            break;

        case PHY_EPON_CONFIG:
            eponDetectPhyLosLof();
            break;

        default:
            dump_stack();
            printk("Illegal value of gpPhyData->working_mode: %d! %s:%d\n", 
                gpPhyData->working_mode, __FUNCTION__, __LINE__);
            break;
    }
}

static inline int should_ignore_phy_ready(void)
{
    return (PHY_LINK_STATUS_READY == gpPhyData->phy_link_status);
}

/*prepare the cmdline args, make it similar to 'main' function's arg*/
/*ret val: 1 success, 0 fail*/
int xpon_prepare_params(char * cmdline, size_t * argc, char ** argv)
{
    size_t cnt = 0, pos = 0;
    enum{SEARCH_ARG, HIT_ARG} state = SEARCH_ARG;
    
    for(pos = 0; cmdline[pos] != 0; ++pos)
    {
        switch(state)
        {
            case SEARCH_ARG:
                if('\x20' != cmdline[pos] && '\n' != cmdline[pos])
                {
                    state = HIT_ARG;
                    argv[cnt] = & cmdline[pos];
                }
                break;

            case HIT_ARG:
                if('\x20' == cmdline[pos] || '\n' == cmdline[pos])
                {
                    cmdline[pos] = 0;
                    state = SEARCH_ARG;
                    ++ cnt;
                    if(cnt > MAX_CMD_LINE_ARGC)
                    {
                        return 1;
                    }
                }
                break;

            default:
                return 0;
        }
    }

    *argc = cnt;
    return 1;
}

static void xpon_phy_ready_handler(PHY_Event_Source_t src)
{
    uint   job_done = 0;

#ifdef TCSUPPORT_CPU_EN7521
	if(PHY_GPON_CONFIG == gpPhyData->working_mode ) {
		/* restore sniffer mode */
		gponDevSetSniffMode(&gpGponPriv->gponCfg.sniffer_mode);
	}
#endif /* TCSUPPORT_CPU_EN7521 */

    if(should_ignore_phy_ready() ){
        XPON_DPRINT_MSG("[%s] ignore phy ready!\n", __FUNCTION__);
        return;
    }
      
    xpon_los_status = 1;
    do{
        
        if(PHY_UNKNOWN_CONFIG == gpPhyData->working_mode ) {
            gpPhyData->detect_status = XPON_PHY_SYNC_STATUS_SYNCED    ;
            gpPhyData->working_mode  = XPON_PHY_GET(PON_GET_PHY_MODE) ;
            continue;
        }

        gpPhyData->phy_link_status = PHY_LINK_STATUS_READY ;
        
        switch(gpPhyData->working_mode)
        {
            case PHY_GPON_CONFIG:
                gpon_detect_phy_ready();
                job_done = 1;
                break;

            case PHY_EPON_CONFIG:
				enable_cpu_us_traffic();
                eponDetectPhyReady();
                job_done = 1;
                break;
                
            default:
                dump_stack();
                printk("Illegal value of gpPhyData->working_mode: %d! %s:%d\n", 
                    gpPhyData->working_mode, __FUNCTION__, __LINE__);
                break;
        }
    } while(!job_done) ;
}

static void xpon_phy_start_rouge_handle(void)
{
    gpPonSysData->rogue_state = XPON_ROUGE_STATE_TRUE ;
}

static void xpon_phy_stop_rouge_handle(void)
{
    if( PHY_GPON_CONFIG == gpPhyData->working_mode   && \
        GPON_STATE_O7   == GPON_CURR_STATE ) {
        	gpGponPriv->emergencystate = 0;
            gpon_disable();
			XPON_PHY_TX_ENABLE();
	        xmcs_report_event(XMCS_EVENT_TYPE_GPON, XMCS_EVENT_GPON_ENABLE, 0) ;
            change_alarm_led_status(ALARM_LED_OFF) ;
        } 
        
    gpPonSysData->rogue_state = XPON_ROUGE_STATE_FALSE ;
}

static void xpon_tx_power_handle(unchar opt)
{
	__u8 llidIndex = 0;
	
}

static int xpon_phy_event_dispatch(PON_PHY_Event_data_t * pEvent)
{
    if(PON_WAN_START != gpPonSysData->sysStartup){
        if(TRUE == gpPhyData->calibrating  && PHY_EVENT_CALIBRATION_STOP == pEvent->id){
            gpPhyData->calibrating = FALSE;
            xmcs_set_connection_start(XPON_ENABLE);
        }else{
            XPON_DPRINT_MSG("PON WAN STOP! Ignore phy event:%d!\n", pEvent->id);
        }
        return 0;
    }else{
        XPON_DPRINT_MSG("Phy event:%d!\n", pEvent->id);
    }
    
    switch (pEvent->id)
    {
        case PHY_EVENT_TRANS_LOS_INT      :
        case PHY_EVENT_PHY_ILLG_INT       :
        case PHY_EVENT_TRANS_LOS_ILLG_INT :
            xpon_phy_los_illegal_handler(pEvent->src, pEvent->id);
            break;

        case PHY_EVENT_PHYRDY_INT:
            xpon_phy_ready_handler(pEvent->src);
            break;

        case PHY_EVENT_PHY_LOF_INT:
            break;
            
        case PHY_EVENT_TF_INT:
            break;
            
        case PHY_EVENT_TRANS_INT:
            break;
            
        case PHY_EVENT_TRANS_SD_FAIL_INT:
            break;
            
        case PHY_EVENT_I2CM_INT:
            break;

        case PHY_EVENT_START_ROGUE_MODE:
            xpon_phy_start_rouge_handle();
            break;

        case PHY_EVENT_STOP_ROGUE_MODE:
            xpon_phy_stop_rouge_handle();
            break;

        case PHY_EVENT_CALIBRATION_START:
            gpPhyData->calibrating = TRUE;
            xmcs_set_connection_start(XPON_DISABLE);
            break;
            
		case PHY_EVENT_TX_POWER_ON:
        	xpon_tx_power_handle(XPON_PHY_TX_POWER_ON);
			break;
		case PHY_EVENT_TX_POWER_OFF:
			xpon_tx_power_handle(XPON_PHY_TX_POWER_OFF);
			break;
        default:
            printk("Event %d handler not implemented! %s:%d\n", pEvent->id, __FUNCTION__, __LINE__);
            break;
    }

    return 0;
}

static int xpon_mac_hook_dispatch(struct ecnt_data  * in_data)
{
    xpon_mac_hook_data_t * data = (xpon_mac_hook_data_t *)in_data;
    struct XMCS_GponOnuInfo_S onuInfo ;
    struct XMCS_GponSnPasswd_S gponSnPasswd ;
    
    switch (data->src_module)
    {
        case XPON_PHY_MODULE:
            xpon_phy_event_dispatch(data->pEvent);
            break;
        case XPON_SN_SET:
            {
                memset(&gponSnPasswd, 0, sizeof(struct XMCS_GponSnPasswd_S)) ;
                gpon_cmd_proc(GPON_IOG_ONU_INFO, (ulong)&onuInfo) ;
                memcpy(gponSnPasswd.passwd, onuInfo.passwd, GPON_PASSWD_LENS);
                memcpy(gponSnPasswd.sn, data->sn, 8) ;

                gpon_cmd_proc(GPON_IOS_SN_PASSWD, (ulong)&gponSnPasswd) ;
            }
            break;
        default:
            printk("XPON MAC Driver not ready to receive event from module with id:%d,", data->src_module);
            return ECNT_HOOK_ERROR;
    }

    return ECNT_RETURN;
}


#ifndef TCSUPPORT_XPON_LED
static void xpon_alarm_led_init(void)
{
	/* PORT 6.18: init_timer()+.data+.function -> timer_setup(); the real
	 * xpon_los_led_flicker() lives in xpon_led.c and re-arms at 500 ms. */
	timer_setup(&gpPonSysData->led_timer, xpon_los_led_flicker, 0) ;/*used for lec flicker*/
    gpPonSysData->flicker_stop       = 0 ;
}
#endif


static int max_dscv_gate_proc_init(void)
{
    if(NULL == xpon_proc_dir){
        return -1;
    }

	if (NULL == xpon_create_proc("max_dscv_gate", xpon_proc_dir,
				     max_dscv_gate_read_proc, max_dscv_gate_write_proc))
	{
		printk("\ncreate max_dscv_gate proc fail.");
		return -1;
	}

	return 0;
}

static int xpon_rogue_mode_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	int len;

	len = sprintf(page, "%d\n", gpPonSysData->rogue_state);

	len -= off;
	*start = page + off;

	if (len > count)
		len = count;
	else
		*eof = 1;

	if (len < 0)
		len = 0;

	return len;
}


static int xpon_fe_reset_happened_read_proc(char *page, char **start, off_t off,
	int count, int *eof, void *data)
{
	int len = 0;

	len = sprintf(page, "%d\n", gpPonSysData->fe_reset_happened);

	len -= off;
	*start = page + off;

	if (len > count)
		len = count;
	else
		*eof = 1;

	if (len < 0)
		len = 0;

	return len;
}


static int xpon_fe_reset_happened_write_proc(struct file *file, const char *buffer,
	unsigned long count, void *data)
{
	char val_string[8] = {0};
	unsigned long val = 0;

	if (count > sizeof(val_string) - 1)
		return -EINVAL;

	if (copy_from_user(val_string, buffer, count))
		return -EFAULT;

	val_string[count] = '\0';

	gpPonSysData->fe_reset_happened = simple_strtoul(val_string, NULL, 10);
	
	return count;
}


static int xpon_proc_init(void)
{
    if(gpon_proc_dir || xpon_proc_dir){
        return 0;
    }

    gpon_proc_dir=proc_mkdir("gpon", NULL);
    if(NULL == gpon_proc_dir){
        return -1;
    }

	if(NULL == xpon_create_proc("debug", gpon_proc_dir,
			gpon_debug_read_proc, gpon_debug_write_proc))
		return -1;

	if(NULL == xpon_create_proc("stage_chk_cnt", gpon_proc_dir,
			gpon_stage_change_chk_read_proc, gpon_stage_change_chk_write_proc))
		return -1;

#ifdef TCSUPPORT_CPU_EN7521
	if(NULL == xpon_create_proc("power_management", gpon_proc_dir,
			gpon_power_management_read_proc, gpon_power_management_write_proc))
		return -1;
#endif

	xpon_proc_dir = proc_mkdir("xpon", NULL);
    if(NULL == xpon_proc_dir){
        return -1;
    }

	if(NULL == xpon_create_proc("ponInfo", xpon_proc_dir,
			xpon_info_read_proc, NULL))
		return -1;

	if(NULL == xpon_create_proc("omci_oam_monitor", xpon_proc_dir,
			xpon_omai_oam_monitor_read_proc, xpon_omci_oam_monitor_write_proc))
		return -1;

	if(NULL == xpon_create_proc("los_led", xpon_proc_dir,
			xpon_los_led_read_proc, xpon_los_led_write_proc))
		return -1;

	if(NULL == xpon_create_proc("rogue_mode", xpon_proc_dir,
			xpon_rogue_mode_read_proc, NULL))
		return -1;

	if(NULL == xpon_create_proc("fe_reset_happened", xpon_proc_dir,
			xpon_fe_reset_happened_read_proc, xpon_fe_reset_happened_write_proc))
		return -1;

    epon_proc_dir = proc_mkdir("epon", NULL);
    if(NULL == epon_proc_dir){
        return -1;
    }

	if(NULL == xpon_create_proc("max_dscv_gate", epon_proc_dir,
			max_dscv_gate_read_proc, max_dscv_gate_write_proc))
		return -1;


    return 0;
}


struct ecnt_hook_ops xpondrv_hook_dispatch_ops = {
    .name = "xpon_mac_hook_dispatch",
    .hookfn = xpon_mac_hook_dispatch,
    .is_execute = 1,
    .maintype = ECNT_XPON_MAC,
    .subtype = ECNT_XPON_MAC_HOOK,
};

/*****************************************************************************
******************************************************************************/
static void xpon_qdma_seam_teardown(void);

void xpondrv_cleanup(void)
{
    /* Stop econet-eth from dispatching PON IRQ/RX into us before we unload. */
    xpon_qdma_seam_teardown();

    XPON_STOP_TIMER(gpPhyData->delay_start_detect_timer)   ;
    XPON_STOP_TIMER(gpPhyData->check_sync_timer)           ;
    
    gpPonSysData->sysStartup = PON_WAN_STOP                ;
    XPON_STOP_TIMER(gpPhyData->trans_status_refresh_timer) ;
#ifndef TCSUPPORT_CPU_EN7521
    XPON_STOP_TIMER(gpPonSysData->gsw_p6_rate_timer);
#endif

	xpon_daemon_quit();
	

#ifdef TCSUPPORT_WAN_GPON
    gpon_deinit() ;
#endif /* TCSUPPORT_WAN_GPON */

#ifdef TCSUPPORT_WAN_EPON
    eponExit(); 
#endif /* TCSUPPORT_WAN_EPON */

    pon_mci_destroy() ;
    
    pwan_destroy() ;
    if (g_dying_gasp_irq_owned) {
        free_irq(CONFIG_DYING_GASP_IRQ, NULL);
        g_dying_gasp_irq_owned = false;
    }

    stop_omci_oam_monitor();
	
    if(gpPonSysData != NULL) {
        kfree(gpPonSysData) ;
        gpPonSysData = NULL ;
    }
    
    if(gpon_proc_dir){
#ifdef TCSUPPORT_CPU_EN7521
        remove_proc_entry("power_management", gpon_proc_dir);
#endif
        remove_proc_entry("debug", gpon_proc_dir);
	remove_proc_entry("stage_chk_cnt", gpon_proc_dir);
    }
    remove_proc_entry("gpon",NULL);
	
    if (xpon_proc_dir) {
        remove_proc_entry("ponInfo",xpon_proc_dir);
	remove_proc_entry("fe_reset_happened",xpon_proc_dir);
        remove_proc_entry("los_led",xpon_proc_dir);
        remove_proc_entry("rogue_mode",xpon_proc_dir);
        remove_proc_entry("omci_oam_monitor",xpon_proc_dir);
    }
    remove_proc_entry("xpon",NULL);

    if (epon_proc_dir){
        remove_proc_entry("max_dscv_gate",epon_proc_dir);
    }
    remove_proc_entry("epon",NULL);


    if(epon_stat_prev)
        kfree(epon_stat_prev);

    ecnt_unregister_hook(&xpondrv_hook_dispatch_ops);

    /* Remove the PHY-recon proc and the legacy tc3162 parent (after pwan_destroy()
     * above removed all tc3162/<x> children) so reload does not warn on re-create. */
    { extern void xpon_phy_recon_cleanup(void); xpon_phy_recon_cleanup(); }
    if (xpon_proc_tc3162) {
        remove_proc_entry("tc3162", NULL);
        xpon_proc_tc3162 = NULL;
    }

}

/*****************************************************************************
******************************************************************************/

int xpon_check_emergence_state(void)
{
	return FALSE;
}



/*****************************************************************************
******************************************************************************/
#include <econet_eth_seam.h>

/* When 0 (default) the module loads WITHOUT touching GPON MAC/PHY hardware
 * (safe; the EN7528 register bring-up needs recon-probe data + a lab OLT).
 * Set xpon_hw=1 only on a bench with an OLT. */
static int xpon_hw = 1;   /* default 1: a deployed ONT does the GPON MAC/PHY bring-up on load
			   * (auto-lock via xpon_los_poll_fn). Pass xpon_hw=0 for a safe bench load. */
module_param(xpon_hw, int, 0444);
MODULE_PARM_DESC(xpon_hw, "1=bring up GPON MAC/PHY hardware (needs OLT); 0=safe load (default)");

/* The econet-eth PON QDMA engine (qdma[1]) bound at init. */
static struct en75_qdma *xpon_pon_qdma;

/* PON IRQ callback registered with econet-eth. Runs in IRQ/atomic context
 * (under econet-eth's irq->lock_irq) -> must stay non-blocking.
 * Defers gpon_isr/PLOAM-drain to a workqueue (see gpon_isr_work_fn). */

/* GPON MAC interrupt (PLOAM recv / SN / ranging) -> gpon_isr(), deferred to a workqueue.
 * The QDMA delivers IPS_GPON_INT to xpon_pon_irq_cb (hard IRQ ctx) then econet-eth MASKS the
 * bit to avoid a storm and never re-unmasks it -> so a pure IRQ dispatch fires only once. We
 * therefore ALSO schedule this from the 2s LOS poll as a catch-all: gpon_isr reads+clears the
 * GPON MAC status (G_INT_STATUS) directly, independent of the masked QDMA bit. schedule_work()
 * coalesces, so the IRQ and poll paths never run gpon_isr concurrently. */
extern void gpon_isr(void);
static int gpon_sm_active;                    /* 1 after the activation SM is driven (gpon_enable done) */
static struct work_struct gpon_isr_work;
static void gpon_isr_work_fn(struct work_struct *w)
{
	extern void gpon_recv_ploam_message(void);
	uint st = IO_GREG(0xbfb64008u) & IO_GREG(0xbfb6400cu);   /* G_INT_STATUS & G_INT_ENABLE */
	if (st) {
		{ extern int ploam_verbose;
		if (ploam_verbose)
			pr_emerg("econet-xpon: GPON INT status=%08x act_st=O%u\n", st, IO_GREG(0xbfb640bcu) & 0xfu); }
		gpon_isr();                           /* SN/ranging/error ints (each status-bit gated) */
	}
	/* ★ k6.18 FIX: the ploamd_recv_int STATUS bit is unreliable — G_INT_STATUS reads 0 even while the
	 * downstream PLOAM FIFO holds msg01/msg14, so gpon_isr's `if(ploamd_recv_int)` never drains it and
	 * the ONU stalls at O2 (never processes Upstream_Overhead->O3 / Assign_ONU_ID->O4). Drain the DS
	 * PLOAM FIFO DIRECTLY here, keyed on the FIFO status (gponDevGetPloamMsg checks ploamd_fifo_used),
	 * independent of G_INT_STATUS. Empty FIFO -> no-op. PROVEN: the `e` manual poll drained the FIFO and
	 * advanced O2->O3 this way. Runs from the 2s LOS-poll + the (masked) QDMA IRQ; schedule_work coalesces. */
	gpon_recv_ploam_message();
}

/* ★2026-07-09 FAST PLOAM POLLER — the missed root cause. The GPON MAC IRQ is masked-after-first-fire by
 * econet-eth and never re-unmasked, so the DS PLOAM FIFO was drained only by the 2s LOS poll. GPON runs on a
 * 125us frame with ms-scale PLOAM timing: at 2s the FIFO overflows with broadcast msg01/msg14 between polls,
 * so the OLT's DIRECTED messages (Assign_ONU_ID, Ranging_Time) are LOST and time-critical response windows are
 * missed -> TO1 timeouts / deactivates (systematic on both EN7528 ports/OLTs; the rare O5 was a lucky poll
 * alignment). This kthread schedules the coalesced gpon_isr_work every fast_ploam_ms so PLOAM is drained at
 * ms-scale. fast_ploam_ms=0 reverts to 2s-poll-only (A/B). */
int fast_ploam_ms = 2;  module_param(fast_ploam_ms, int, 0644);
static struct task_struct *gpon_fastpoll_task;
static int gpon_fastpoll_fn(void *arg)
{
	(void)arg;
	while (!kthread_should_stop()) {
		int ms = (fast_ploam_ms > 0) ? fast_ploam_ms : 50;
		if (gpon_sm_active && fast_ploam_ms > 0)
			schedule_work(&gpon_isr_work);
		usleep_range((unsigned)ms * 1000u, (unsigned)ms * 1000u + 300u);
	}
	return 0;
}

static void xpon_pon_irq_cb(void *arg, enum en75_irq_purpose_type type)
{
	pr_info_ratelimited("econet-xpon: PON IRQ (type=%d) dispatched to driver\n", (int)type);
	/* IPS_GPON_INT (6) = the GPON MAC interrupt (PLOAM/SN/ranging). Defer to process ctx;
	 * gpon_isr can't run under econet-eth's irq->lock_irq. Only after gpon_enable(). */
	if (type == IPS_GPON_INT && gpon_sm_active)
		schedule_work(&gpon_isr_work);
}

/* TX round-trip test: `echo 1 > /proc/econet_xpon_tx` submits a dummy 64-byte
 * frame to the econet-eth PON QDMA engine (qdma[1]) egressing to GDMA2. Proves
 * the TX datapath seam end-to-end (no fiber/MAC needed: the QDMA completes the
 * descriptor regardless of physical egress). Result is logged to dmesg. */
#include <linux/proc_fs.h>
static int xpon_tx_test_count;
static ssize_t xpon_tx_test_write(struct file *f, const char __user *buf,
				  size_t len, loff_t *off)
{
	struct en75_qdma *q = en75_xpon_pon_qdma();
	union desc_msg msg = { 0 };
	struct sk_buff *skb;
	char kbuf[16] = { 0 };
	unsigned int gem = 0;
	int ret;

	/* `echo <gem> > /proc/econet_xpon_tx` tags the test frame with that GEM. */
	if (len && copy_from_user(kbuf, buf, min(len, sizeof(kbuf) - 1)) == 0)
		(void)kstrtouint(strim(kbuf), 0, &gem);

	if (!q) {
		pr_info("econet-xpon: tx-test: PON qdma unavailable\n");
		return len;
	}
	skb = alloc_skb(128, GFP_KERNEL);
	if (!skb)
		return len;
	skb_reserve(skb, 2);
	memset(skb_put(skb, 64), 0, 64);		/* dummy 64-byte payload */
	set_etx_sp_tag(&msg.etx, gem & 0xFFF);		/* GEM = sp_tag bits 23:12 */
	set_etx_fport(&msg.etx, ETX_FPORT_GDM2);	/* GPON WAN egress */
	ret = en75_qdma_xmit(q, skb, &msg, 0);
	if (ret < 0)
		dev_kfree_skb_any(skb);
	pr_info("econet-xpon: tx-test #%d gem=%u -> en75_qdma_xmit(qdma1,fport=GDM2) ret=%d\n",
		++xpon_tx_test_count, gem, ret);
	return len;
}
static const struct proc_ops xpon_tx_test_ops = {
	.proc_write = xpon_tx_test_write,
};

/* ---- Minimal PON WAN netdev (ponwan0) ----
 * One Ethernet netdev bound to a single GEM (default 0; OMCI would provision
 * per-GEM netdevs in M2). TX: ndo_start_xmit -> en75_qdma_xmit(qdma1, GDM2,
 * gem) with BQL. RX frames are delivered here from the PON RX callback via
 * netif_rx(), so they reach the Linux network stack (tcpdump-able). */
#include <linux/etherdevice.h>
#include <linux/if_ether.h>
#include <linux/if_vlan.h>			/* bab1f510: skb_vlan_tag_*, struct vlan_ethhdr, VLAN_VID_MASK */
static struct net_device *xpon_wan_dev;
static u16 xpon_wan_gem;			/* GEM bound to ponwan0 (default 0) */
static u8  xpon_wan_chan;			/* US T-CONT channel stamped into the TX desc (default 0; set to the gem's alloc channel, e.g. gem225/alloc351 -> 1) */
static u8  xpon_wan_queue = 6;			/* ★ US QoS queue stamped into the TX desc (ETX_QUEUE 2:0). Stock stamps PPPoE->queue6 ("ppp/dhcp/high prio", xpon_netif.h); the shim previously left queue0. Settable via /proc/econet_xpon_wanqueue for the US-forward queue-sweep. */

/* bab1f510 "activate mapped GPON data services": per-VLAN US service selection,
 * mirroring stock gwan_prepare_tx_message (gpon_wan.c:696) which picks the GEM by the
 * frame's service VLAN and reads the T-CONT channel from the programmed gem table.
 * The statics above remain the fallback when there is no tag / no matching entry. */
#define XPON_WAN_MAP_MAX 8
static struct xpon_wan_map { u16 vlan; u16 gem; u8 chan; u8 queue; u8 valid; }
	xpon_wan_vlan_map[XPON_WAN_MAP_MAX];
static DEFINE_SPINLOCK(xpon_wan_map_lock);

/* Read the frame's service VLAN id: accelerated skb tag OR an inline 802.1Q/AD header
 * (skb_header_pointer copies into vhbuf if non-linear, so it is safe either way). */
static int xpon_wan_frame_vid(struct sk_buff *skb)
{
	struct vlan_ethhdr vhbuf;
	const struct vlan_ethhdr *vh;
	if (skb_vlan_tag_present(skb))
		return skb_vlan_tag_get(skb) & VLAN_VID_MASK;
	vh = skb_header_pointer(skb, 0, sizeof(vhbuf), &vhbuf);
	if (vh && (vh->h_vlan_proto == htons(ETH_P_8021Q) ||
		   vh->h_vlan_proto == htons(ETH_P_8021AD)))
		return ntohs(vh->h_vlan_TCI) & VLAN_VID_MASK;
	return -1;
}

static int xpon_wan_open(struct net_device *dev)  { netif_start_queue(dev); return 0; }
static int xpon_wan_stop(struct net_device *dev)  { netif_stop_queue(dev);  return 0; }

static netdev_tx_t xpon_wan_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct en75_qdma *q = en75_xpon_pon_qdma();
	struct netdev_queue *txq = netdev_get_tx_queue(dev, skb_get_queue_mapping(skb));
	union desc_msg msg = { 0 };
	int xlen = skb->len;
	u16 gem = xpon_wan_gem;			/* bab1f510: static fallback ... */
	u8  chan = xpon_wan_chan, queue = xpon_wan_queue;
	int vid;

	if (!q || skb_linearize(skb))
		goto drop;
	/* RUNT FIX: pad short frames up to the 60-byte Ethernet minimum (ETH_ZLEN).
	 * The GDM2/GEM-framer TX path does NOT auto-pad (the HW-pad hook is a stub) and
	 * stock pads in SW (xpon_netif.c macSend skb_padto). Without this, a PADI egresses
	 * as a ~36B runt -> the OLT/BNG L2 drops the undersized frame -> 0 PADO.
	 * skb_put_padto frees the skb on failure, so return directly (no goto drop). */
	if (skb_put_padto(skb, ETH_ZLEN)) {
		dev->stats.tx_dropped++;
		return NETDEV_TX_OK;
	}
	xlen = skb->len;
	/* bab1f510: per-VLAN service selection; on no tag / no match keep the statics. */
	vid = xpon_wan_frame_vid(skb);
	if (vid >= 0) {
		unsigned long fl;
		int i;
		spin_lock_irqsave(&xpon_wan_map_lock, fl);
		for (i = 0; i < XPON_WAN_MAP_MAX; i++)
			if (xpon_wan_vlan_map[i].valid && xpon_wan_vlan_map[i].vlan == vid) {
				gem   = xpon_wan_vlan_map[i].gem;
				chan  = xpon_wan_vlan_map[i].chan;
				queue = xpon_wan_vlan_map[i].queue;
				break;
			}
		spin_unlock_irqrestore(&xpon_wan_map_lock, fl);
	}
	set_etx_sp_tag(&msg.etx, gem & 0xFFF);		/* GEM tag (sp_tag 23:12) */
	set_etx_channel(&msg.etx, chan);		/* US T-CONT channel (alloc). gem225/alloc351 -> ch1; without this the MAC drops the US frame (gem/channel mismatch) */
	set_etx_queue(&msg.etx, queue & 0x7);		/* ★ US QoS queue (ETX_QUEUE 2:0). stock: PPPoE->queue6. */
	set_etx_fport(&msg.etx, ETX_FPORT_GDM2);	/* GPON WAN egress */
	netdev_tx_sent_queue(txq, xlen);
	if (en75_qdma_xmit(q, skb, &msg, 0) < 0) {
		netdev_tx_completed_queue(txq, 1, xlen);
		goto drop;
	}
	dev->stats.tx_packets++;
	dev->stats.tx_bytes += xlen;
	return NETDEV_TX_OK;
drop:
	dev->stats.tx_dropped++;
	dev_kfree_skb_any(skb);
	return NETDEV_TX_OK;
}

static const struct net_device_ops xpon_wan_netdev_ops = {
	.ndo_open	= xpon_wan_open,
	.ndo_stop	= xpon_wan_stop,
	.ndo_start_xmit	= xpon_wan_xmit,
	.ndo_set_mac_address = eth_mac_addr,	/* allow `ip link set ponwan0 address ...` (matched-set MAC) */
};

/* --- GPON optical carrier: reflect the EN7571 LOS comparator on ponwan0's carrier
 * so OpenWrt (ifstatus/LuCI/mwan3) natively sees fiber link up/down. --- */
extern void en7571_optical_bringup(void);   /* phy_init.c: front-end + APD bias */
extern int  en7571_los_present(void);       /* phy_init.c: reg 0x133 bit0 (1=loss) */
extern int  gpon_do_lock(int quiet);        /* phy_init.c: the proven GPON CDR lock sequence */
extern int  gpon_is_locked(void);           /* phy_init.c: 1 if PHYSTA1 rdy==6 */
extern int  auto_lock;                       /* phy_init.c: enable auto-lock */
extern void gpon_mbi_start_probe(void);      /* phy_init.c: O1->O2 MBI-datapath start + DS_SPF probe */
extern int  mbi_autoprobe;                   /* phy_init.c: run the MBI probe once after first lock */

static struct delayed_work xpon_los_poll_work;
static int xpon_los_poll_active;

/* When 1 (default) the LOS poll drives the GPON software activation SM O1->O2 once, the first
 * time the CDR locks (see the one-shot in xpon_los_poll_fn). Set 0 to disable (e.g. if it
 * crashes) via `insmod ... mac_activate=0` or `echo 0 > /sys/module/econet_xpon/parameters/mac_activate`. */
static int mac_activate = 1; module_param(mac_activate, int, 0644);
static int rx_dedup = 0; module_param(rx_dedup, int, 0644);  /* 1=drop OMCI retransmits (old behaviour); 0=deliver ALL frames (deadlock test) */
extern void gpon_detect_phy_ready(void);     /* gpon.c: phy-ready -> act O1->O2 + gpon_enable */

static void xpon_los_poll_fn(struct work_struct *w)
{
	int i, loss = 0, up;

	if (!xpon_wan_dev)
		return;
	en7571_optical_bringup();               /* idempotent; re-inits after an apd-off */
	for (i = 0; i < 5; i++) {               /* majority vote */
		if (en7571_los_present() == 1)
			loss++;
		udelay(2000);
	}
	up = (loss < 3);
	if (up && !netif_carrier_ok(xpon_wan_dev)) {
		netif_carrier_on(xpon_wan_dev);
		pr_info("econet-xpon: ponwan0 GPON optical LINK UP\n");
	} else if (!up && netif_carrier_ok(xpon_wan_dev)) {
		netif_carrier_off(xpon_wan_dev);
		pr_info("econet-xpon: ponwan0 GPON optical LINK DOWN (LOS)\n");
	}
	/* ★ auto-lock the GPON CDR: when light is present but not locked, run the proven lock
	 * sequence (SOLVED 2026-07-03: NCPO seed + arm + IMMEDIATE finalize). Re-acquires on drop. */
	if (up && auto_lock && !gpon_is_locked()) {
		if (gpon_do_lock(1) == 6)
			pr_info("econet-xpon: GPON CDR LOCKED (rdy=6, RX_SYNC=0xa)\n");
	}
	/* ★ O1->O2: drive the SOFTWARE activation SM once after the CDR first locks. The auto-lock
	 * path only locks the PHY CDR; act_st (G_ACTIVATION[3:0]) is SW-written and never advances
	 * unless we run the phy-ready path. Replicate xpon_phy_ready_handler's GPON branch:
	 * working_mode=GPON, sysStartup=WAN_START, phy_link_status=READY -> gpon_detect_phy_ready()
	 * -> gpon_act_change_gpon_state(O2) + gpon_enable() (register_gpon_isr). O1->O2 is ONU-side;
	 * O2->O5 then needs the OLT to range our SN. Gated by mac_activate; runs once per boot. */
	if (mac_activate && gpon_is_locked() && gpPhyData && gpPonSysData) {
		static int mac_activated;
		if (!mac_activated) {
			uint act0, act1;
			mac_activated = 1;
			gpPonSysData->debugLevel = 0xFFFF;   /* verbose: show PLOAM/SN/ranging/activation logs */
			act0 = IO_GREG(0xbfb640bcu) & 0xfu;
			gpPhyData->working_mode    = PHY_GPON_CONFIG;
			gpPonSysData->sysStartup   = PON_WAN_START;
			gpPhyData->phy_link_status = PHY_LINK_STATUS_READY;
			pr_emerg("econet-xpon: GPON activation: driving phy_ready (act_st=O%u) -> O2 + gpon_enable\n", act0);
			gpon_detect_phy_ready();
			gpon_sm_active = 1;                   /* arm the gpon_isr work (IRQ + poll paths) */
			act1 = IO_GREG(0xbfb640bcu) & 0xfu;
			pr_emerg("econet-xpon: GPON activation: act_st O%u -> O%u after phy_ready\n", act0, act1);
		}
	}
	/* Drain the GPON MAC interrupt (PLOAM/SN/ranging) each poll once the SM is up. The QDMA
	 * GPON int is masked after its first fire, so this 2s poll is the reliable drain path. */
	if (gpon_sm_active)
		schedule_work(&gpon_isr_work);
	/* ★ O1->O2 one-shot: the first time the CDR is locked, START the MAC<->PHY MBI datapath
	 * and log DS_SPF (gpon_init leaves MBI stopped; nothing here runs gpon_enable to start it).
	 * If DS_SPF then increments, the un-started MBI is the O1 blocker. Runs once per boot. */
	if (mbi_autoprobe && gpon_is_locked()) {
		static int mbi_probed;
		if (!mbi_probed) {
			mbi_probed = 1;
			gpon_mbi_start_probe();
		}
	}
	if (xpon_los_poll_active)
		schedule_delayed_work(&xpon_los_poll_work, msecs_to_jiffies(2000));
}

static void xpon_wan_netdev_create(void)
{
	int err;

	xpon_wan_dev = alloc_etherdev(0);
	if (!xpon_wan_dev)
		return;
	strscpy(xpon_wan_dev->name, "ponwan0", IFNAMSIZ);
	xpon_wan_dev->netdev_ops = &xpon_wan_netdev_ops;
	/* WAN MAC from the wan_mac module_param if set, else a random locally-administered
	 * address. No device MAC is hardcoded in the driver. */
	{ extern char *wan_mac; u8 mac[ETH_ALEN];
	  if (wan_mac && *wan_mac && mac_pton(wan_mac, mac)) eth_hw_addr_set(xpon_wan_dev, mac);
	  else eth_hw_addr_random(xpon_wan_dev); }
	err = register_netdev(xpon_wan_dev);
	if (err) {
		pr_info("econet-xpon: register ponwan0 failed (%d)\n", err);
		free_netdev(xpon_wan_dev);
		xpon_wan_dev = NULL;
		return;
	}
	pr_info("econet-xpon: registered WAN netdev ponwan0 (gem %u)\n", xpon_wan_gem);

	/* start carrier down; the LOS poll raises it when the fiber has signal */
	netif_carrier_off(xpon_wan_dev);
	INIT_WORK(&gpon_isr_work, gpon_isr_work_fn);
	INIT_DELAYED_WORK(&xpon_los_poll_work, xpon_los_poll_fn);
	xpon_los_poll_active = 1;
	schedule_delayed_work(&xpon_los_poll_work, msecs_to_jiffies(1500));
	gpon_fastpoll_task = kthread_run(gpon_fastpoll_fn, NULL, "gpon_fastpoll");  /* ★ ms-scale PLOAM drain */
}

/* `echo <gem> > /proc/econet_xpon_wantx` sends a dummy frame THROUGH ponwan0
 * (exercises ndo_start_xmit). With loopback on it returns via the RX path. */
static ssize_t xpon_wantx_write(struct file *f, const char __user *buf,
				size_t len, loff_t *off)
{
	char kbuf[16] = { 0 };
	unsigned int gem = 0;
	struct sk_buff *skb;

	if (len && copy_from_user(kbuf, buf, min(len, sizeof(kbuf) - 1)) == 0)
		(void)kstrtouint(strim(kbuf), 0, &gem);
	xpon_wan_gem = gem & 0xFFF;
	if (!xpon_wan_dev)
		return len;
	skb = netdev_alloc_skb(xpon_wan_dev, 64);
	if (!skb)
		return len;
	memset(skb_put(skb, 64), 0, 64);
	skb->dev = xpon_wan_dev;
	skb->protocol = htons(ETH_P_802_2);
	dev_queue_xmit(skb);
	pr_info("econet-xpon: wantx gem=%u via ponwan0\n", xpon_wan_gem);
	return len;
}
static const struct proc_ops xpon_wantx_ops = { .proc_write = xpon_wantx_write };

/* `echo <chan> > /proc/econet_xpon_wanchan` sets the US T-CONT channel stamped into
 * the ponwan0 TX descriptor via set_etx_channel(). Must equal the bound gem's alloc
 * channel (gem225/alloc351 -> 1), else the GPON MAC drops the upstream frame. */
static ssize_t xpon_wanchan_write(struct file *f, const char __user *buf,
				size_t len, loff_t *off)
{
	char kbuf[16] = { 0 };
	unsigned int ch = 0;

	if (len && copy_from_user(kbuf, buf, min(len, sizeof(kbuf) - 1)) == 0)
		(void)kstrtouint(strim(kbuf), 0, &ch);
	xpon_wan_chan = ch & 0xFF;
	pr_info("econet-xpon: wanchan chan=%u for ponwan0 US\n", xpon_wan_chan);
	return len;
}
static const struct proc_ops xpon_wanchan_ops = { .proc_write = xpon_wanchan_write };

/* `echo <q> > /proc/econet_xpon_wanqueue` sets the US QoS queue (ETX_QUEUE 2:0) stamped
 * into the ponwan0 TX descriptor. Stock's gwan datapath stamps PPPoE US into queue6
 * ("ppp/dhcp/high prio"); the shim defaulted to 0. Sweep 0..7 to isolate the OLT's
 * US-forward gate (does queue6 unlock PADO?). */
static ssize_t xpon_wanqueue_write(struct file *f, const char __user *buf,
				size_t len, loff_t *off)
{
	char kbuf[16] = { 0 };
	unsigned int q = 0;

	if (len && copy_from_user(kbuf, buf, min(len, sizeof(kbuf) - 1)) == 0)
		(void)kstrtouint(strim(kbuf), 0, &q);
	xpon_wan_queue = q & 0x7;
	pr_info("econet-xpon: wanqueue queue=%u for ponwan0 US\n", xpon_wan_queue);
	return len;
}
static const struct proc_ops xpon_wanqueue_ops = { .proc_write = xpon_wanqueue_write };

/* `echo 1 > /proc/econet_xpon_usdbg` dumps the GPON MAC CSRs (T-CONT Alloc-IDs @0x4020,
 * BWM grant status @0x4234, TX GEM/BST counters @0x430C/0x4310, DBA backdoor @0x4340) to
 * the kernel log; read with `dmesg`. Emission-path evidence: are US bursts physically
 * transmitted (TX_BST climbing) and is the OLT granting our alloc (BWM_GNT)? */
extern int gponDevDumpCsr(void);
static ssize_t xpon_usdbg_write(struct file *f, const char __user *buf,
				size_t len, loff_t *off)
{
	pr_info("econet-xpon: === usdbg gponDevDumpCsr (wan_gem=%u chan=%u queue=%u) ===\n",
		xpon_wan_gem, xpon_wan_chan, xpon_wan_queue);
	gponDevDumpCsr();
	return len;
}
static const struct proc_ops xpon_usdbg_ops = { .proc_write = xpon_usdbg_write };

/* bab1f510: `echo "<vlan> <gem> <chan> <queue>" > /proc/econet_xpon_wanmap` adds/updates
 * one VLAN->service entry; `echo "del <vlan>"` (or a 0 gem) clears it. If <chan> is 0 the
 * T-CONT channel is derived from the programmed gem table (gemPort[gemIdToIndex[gem]].info),
 * the same source mkgem uses, so it can never disagree with the alloc. Populated from
 * userspace (native_omcid, which knows VLAN 102 -> GEM 225 / T-CONT alloc 351 / queue). */
static ssize_t xpon_wanmap_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
	char kbuf[64] = { 0 };
	unsigned int vlan = 0, gem = 0, chan = 0, queue = 0;
	int i, slot = -1, del = 0;
	unsigned long fl;

	if (len && copy_from_user(kbuf, buf, min(len, sizeof(kbuf) - 1)))
		return -EFAULT;
	if (!strncmp(kbuf, "del", 3)) { del = 1; sscanf(kbuf + 3, "%u", &vlan); }
	else sscanf(kbuf, "%u %u %u %u", &vlan, &gem, &chan, &queue);
	if (!vlan) return len;
	/* chan==0 -> derive from the programmed gem table (mirror stock gpon_wan.c:889) */
	if (!del && gem && chan == 0 && gpWanPriv) {
		u32 gi = gpWanPriv->gpon.gemIdToIndex[gem] & GPON_GEM_IDX_MASK;
		if (gi < CONFIG_GPON_MAX_GEMPORT && gpWanPriv->gpon.gemPort[gi].info.valid)
			chan = gpWanPriv->gpon.gemPort[gi].info.channel;
	}
	spin_lock_irqsave(&xpon_wan_map_lock, fl);
	for (i = 0; i < XPON_WAN_MAP_MAX; i++) {
		if (xpon_wan_vlan_map[i].valid && xpon_wan_vlan_map[i].vlan == vlan) { slot = i; break; }
		if (slot < 0 && !xpon_wan_vlan_map[i].valid) slot = i;
	}
	if (slot >= 0) {
		if (del || gem == 0) {
			if (xpon_wan_vlan_map[slot].vlan == vlan)
				xpon_wan_vlan_map[slot].valid = 0;
		} else {
			xpon_wan_vlan_map[slot].vlan  = vlan;
			xpon_wan_vlan_map[slot].gem   = gem & 0xFFF;
			xpon_wan_vlan_map[slot].chan  = (u8)chan;
			xpon_wan_vlan_map[slot].queue = queue & 0x7;
			xpon_wan_vlan_map[slot].valid = 1;
		}
	}
	spin_unlock_irqrestore(&xpon_wan_map_lock, fl);
	pr_info("econet-xpon: wanmap %s vlan=%u gem=%u chan=%u queue=%u\n",
		(del || gem == 0) ? "del" : "set", vlan, gem, chan, queue);
	return len;
}
static const struct proc_ops xpon_wanmap_ops = { .proc_write = xpon_wanmap_write };

/* `echo "<gem> <alloc>" > /proc/econet_xpon_mkgem` creates the data GEM's HW upstream
 * map by calling gwan_create_new_gemport() DIRECTLY (the xmcs_create_gem_port ioctl path
 * silently no-ops on this port). channel = T-CONT index whose allocId matches <alloc>.
 * Logs the before/after gem table state so we can see why the create takes. */
static ssize_t xpon_mkgem_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
	char kbuf[32] = { 0 };
	unsigned int gem = 0, alloc = 0, gi;
	int i, channel = 0, ret;

	if (len && copy_from_user(kbuf, buf, min(len, sizeof(kbuf) - 1)) == 0)
		sscanf(kbuf, "%u %u", &gem, &alloc);
	if (!gpWanPriv) { pr_info("econet-xpon: mkgem: no gpWanPriv\n"); return len; }
	for (i = 0; i < CONFIG_GPON_MAX_TCONT; i++)
		if (gpWanPriv->gpon.allocId[i] == (ushort)alloc) { channel = i; break; }
	pr_info("econet-xpon: mkgem IN  gem=%u alloc=%u ch=%d idx[gem]=0x%x num=%u\n",
		gem, alloc, channel, gpWanPriv->gpon.gemIdToIndex[gem], gpWanPriv->gpon.gemNumbers);
	ret = gwan_create_new_gemport((ushort)gem, (unchar)channel, 0);
	/* bab1f510 auto-complete: fold the US TX-desc binding into mkgem so even a no-poke
	 * reboot auto-selects the OMCI-provisioned gem/chan (queue already defaults to 6). */
	xpon_wan_gem  = gem & 0xFFF;
	xpon_wan_chan = (u8)channel;
	gi = gpWanPriv->gpon.gemIdToIndex[gem] & GPON_GEM_IDX_MASK;
	pr_info("econet-xpon: mkgem OUT ret=%d idx[gem]=0x%x num=%u port[%u].valid=%d portId=%u\n",
		ret, gpWanPriv->gpon.gemIdToIndex[gem], gpWanPriv->gpon.gemNumbers, gi,
		(gi < CONFIG_GPON_MAX_GEMPORT) ? gpWanPriv->gpon.gemPort[gi].info.valid : -1,
		(gi < CONFIG_GPON_MAX_GEMPORT) ? gpWanPriv->gpon.gemPort[gi].info.portId : 0);
	return len;
}
static const struct proc_ops xpon_mkgem_ops = { .proc_write = xpon_mkgem_write };

/* PON RX path: frames from qdma[1] (no GDM netdev) are handed here by econet-eth.
 * Deliver to ponwan0 via netif_rx() so they reach the network stack. */
#include <linux/uaccess.h>
static u32 xpon_rx_count;
static u16 xpon_rx_last_gem;
static u8  xpon_rx_last_sport;
/* GO/NO-GO PROBE (2026-07-07): per-gem histogram of frames reaching the raw PON RX
 * callback, so we can see at O5 whether an OMCC-tagged downstream OMCI frame arrives
 * here at all (=> demux+TX suffice) or never (=> GDM2/PSE GEM->CPU classify needed). */
static struct { u16 gem; u32 cnt; } xpon_rx_gem_hist[16];
static int xpon_rx_gem_n;
static void xpon_pon_rx_cb(void *arg, struct sk_buff *skb, u8 sport, u16 gem)
{
	struct net_device *dev = xpon_wan_dev;
	int _i;

	xpon_rx_count++;
	xpon_rx_last_gem = gem;
	xpon_rx_last_sport = sport;
	for (_i = 0; _i < xpon_rx_gem_n; _i++)
		if (xpon_rx_gem_hist[_i].gem == gem) { xpon_rx_gem_hist[_i].cnt++; break; }
	if (_i == xpon_rx_gem_n && xpon_rx_gem_n < 16) {
		xpon_rx_gem_hist[_i].gem = gem; xpon_rx_gem_hist[_i].cnt = 1; xpon_rx_gem_n++;
	}

	/* ★ OMCI RX DEMUX: route the OMCC-GEM downstream OMCI to the omci netdev (omcid).
	 * ★★ RAW delivery (2026-07-09): the stock omci daemon's omci_lib_adpt_socket_receive
	 * does NOT strip any L2 header; its recv thread reads the OMCI DevID at buf[3] (must be
	 * 0x0a baseline). So deliver the frame STARTING AT THE OMCI TCI (no 14-byte 0x88b5 wrap).
	 * eth_type_trans leaves mac_header at byte0, so the daemon's AF_PACKET SOCK_RAW recvfrom
	 * returns buf[0]=TCI, buf[3]=DevID. (The old 88b5-wrap made buf[3]=0x00 -> daemon dropped
	 * every real OLT frame; only ponstub raw injects answered.) */
	if (gpGponPriv && gpWanPriv && gpGponPriv->gponCfg.omcc != 0xFFFF &&
	    gem == gpGponPriv->gponCfg.omcc && gpWanPriv->pPonNetDev[PWAN_IF_OMCI]) {
		struct net_device *odev = gpWanPriv->pPonNetDev[PWAN_IF_OMCI];
		{ u8 *d = skb->data;   /* raw OMCI (baseline): [0..1]TCI [2]msgType(bit6=AR) [3]devID [4..5]MEclass [6..7]MEinst */
		  printk(KERN_DEBUG "OMCIDBG: RX OMCC gem=%u len=%u | OMCI tci=%02x%02x mt=%02x(mt=%u AR=%u) did=%02x class=%u inst=%u | %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
			gem, skb->len, d[0], d[1], d[2], d[2] & 0x1f, (d[2] >> 6) & 1, d[3],
			(d[4] << 8) | d[5], (d[6] << 8) | d[7],
			d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7], d[8], d[9]); }
		/* ★ OMCI RX DEDUP (2026-07-10): the stock omci daemon replies ~15 txns behind
		 * (it processes ~2/cycle and never drains its RX backlog). The OLT retransmits
		 * each unanswered request; delivering those retransmits keeps the backlog full
		 * so the daemon never catches up and MIB-Upload/Create never complete. Drop any
		 * OMCI whose TCI was already delivered in the last second (a retransmit) — then
		 * while the OLT is stuck retransmitting one request the daemon drains its backlog,
		 * reaches the current request, and the sequence advances. (omcifix.ko's TCI-fix
		 * handles the residual small lag.) */
		{
			static struct { u16 tci; unsigned long j; } seen[16];
			static int si;
			u16 rtci = (skb->data[0] << 8) | skb->data[1];
			unsigned long now = jiffies; int _k;
			for (_k = 0; _k < 16; _k++)
				if (rx_dedup && seen[_k].j && seen[_k].tci == rtci && (now - seen[_k].j) < HZ) {
					dev_kfree_skb_any(skb);
					return;    /* retransmit -> drop so the daemon can drain */
				}
			seen[si].tci = rtci; seen[si].j = now ? now : 1; si = (si + 1) & 15;
		}
		skb->dev = odev;
		skb->protocol = eth_type_trans(skb, odev);	/* raw OMCI at byte0; mac_header stays at byte0 */
		odev->stats.rx_packets++;
		odev->stats.rx_bytes += skb->len;
		netif_rx(skb);
		return;
	}

	if (!dev) {				/* no netdev yet: count + consume */
		dev_kfree_skb_any(skb);
		return;
	}
	skb->dev = dev;
	skb->protocol = eth_type_trans(skb, dev);
	dev->stats.rx_packets++;
	dev->stats.rx_bytes += skb->len;
	netif_rx(skb);				/* deliver to the network stack */
}
static ssize_t xpon_rx_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
	char tmp[512];
	int i, n = scnprintf(tmp, sizeof(tmp), "rx_count %u last_gem %u last_sport %u omcc_id %u\ngems:",
			  xpon_rx_count, xpon_rx_last_gem, xpon_rx_last_sport,
			  gpGponPriv ? (u32)gpGponPriv->gponCfg.omcc : 0);
	for (i = 0; i < xpon_rx_gem_n; i++)
		n += scnprintf(tmp + n, sizeof(tmp) - n, " %u:%u", xpon_rx_gem_hist[i].gem, xpon_rx_gem_hist[i].cnt);
	n += scnprintf(tmp + n, sizeof(tmp) - n, "\n");
	return simple_read_from_buffer(buf, len, off, tmp, n);
}
static const struct proc_ops xpon_rx_ops = { .proc_read = xpon_rx_read };

/* `echo 1 > /proc/econet_xpon_lb` toggles qdma[1] internal loopback (TX->RX),
 * so the RX path can be exercised without the GPON MAC/optics. */
static ssize_t xpon_lb_write(struct file *f, const char __user *buf,
			     size_t len, loff_t *off)
{
	struct en75_qdma *q = en75_xpon_pon_qdma();
	char c = '1';

	if (len && copy_from_user(&c, buf, 1))
		return -EFAULT;
	if (q) {
		en75_qdma_set_loopback(q, c != '0');
		pr_info("econet-xpon: qdma1 loopback %s\n", c != '0' ? "ON" : "OFF");
	}
	return len;
}
static const struct proc_ops xpon_lb_ops = { .proc_write = xpon_lb_write };

/* Tear down the econet-eth QDMA seam + ponwan0 + TX/RX test procs that
 * xpondrv_init() registered. The callbacks MUST be cleared in econet-eth before
 * this module unloads, otherwise econet-eth dispatches the next PON IRQ/RX frame
 * into freed module memory. Idempotent (guards + remove_proc_entry no-ops), so
 * it is also safe on the xpondrv_init() error path. */
static void xpon_qdma_seam_teardown(void)
{
	if (xpon_pon_qdma) {
		en75_qdma_register_pon_rx(xpon_pon_qdma, NULL, NULL);
		en75_qdma_register_pon(xpon_pon_qdma, NULL, NULL);
		en75_qdma_unuse(xpon_pon_qdma);
		xpon_pon_qdma = NULL;
	}
	if (xpon_wan_dev) {
		xpon_los_poll_active = 0;
		gpon_sm_active = 0;
		if (gpon_fastpoll_task) { kthread_stop(gpon_fastpoll_task); gpon_fastpoll_task = NULL; }
		cancel_delayed_work_sync(&xpon_los_poll_work);
		cancel_work_sync(&gpon_isr_work);
		unregister_netdev(xpon_wan_dev);
		free_netdev(xpon_wan_dev);
		xpon_wan_dev = NULL;
	}
	remove_proc_entry("econet_xpon_tx", NULL);
	remove_proc_entry("econet_xpon_rx", NULL);
	remove_proc_entry("econet_xpon_lb", NULL);
	remove_proc_entry("econet_xpon_wantx", NULL);
	remove_proc_entry("econet_xpon_wanchan", NULL);
	remove_proc_entry("econet_xpon_wanqueue", NULL);
	remove_proc_entry("econet_xpon_usdbg", NULL);
	remove_proc_entry("econet_xpon_wanmap", NULL);
	remove_proc_entry("econet_xpon_mkgem", NULL);
}

int xpondrv_init(void)
{
	printk("xPON driver initialization\n") ;

	/* initial the global data memory */
	gpPonSysData = (PON_SysData_T *)kmalloc(sizeof(PON_SysData_T), GFP_KERNEL) ;
	if(gpPonSysData == NULL) {
		printk("Alloc data struct memory failed\n") ;
		goto ret ;
	} else {
		memset(gpPonSysData, 0, sizeof(PON_SysData_T));
		printk("Alloc data struct memory successful, %d\n", sizeof(PON_SysData_T)) ;
		xpondrv_init_global_data() ;
	}

	/* Create the legacy /proc/tc3162 parent before any tc3162/<x> child proc. */
	if (!xpon_proc_tc3162)
		xpon_proc_tc3162 = proc_mkdir("tc3162", NULL);

	/* PHY-only optical recon trigger (no fiber/MAC/laser); see phy_init.c. */
	{ extern void xpon_phy_recon_register(void); xpon_phy_recon_register(); }

    omci_oam_monitor_init(&gpPonSysData->Omci_Oam_Monitor) ;

	/* WAN/OAM/OMCI interface driver initizliation */
	if(pwan_init() != 0) {
		printk("PON WAN/OAM/OMCI interface driver initialization failed\n") ;
		goto ret ;
	}
 
	/* XPON character device driver initizliation */
	if(pon_mci_init() != 0) {
		printk("PON monitor and control interface driver initialization failed\n") ;
		goto ret ;
	}
	
	/* EPON initialization (stub in this GPON-only build) */
	if(eponInit() != 0) {
		printk("EPON init failed\n") ;
		goto ret ;
	}

	/* ---- GPON MAC / QDMA / dying-gasp hardware bring-up: GATED behind
	 * xpon_hw (default 0). These poke the GPON MAC (ioremap 0x1FB60000),
	 * the QDMA engine and GPIO directly; doing that blind on EN7528 (no
	 * recon-probe register data, no OLT) can hang the bus. Keep the module
	 * loadable without touching PON hardware. ---- */
	if (xpon_hw) {
		if(gpon_init() != 0) {
			printk("GPON initialization failed\n") ;
			goto ret ;
		}
		if(xpondrv_qdma_init() != 0) {
			printk("QDMA buffer manager initialization failed\n") ;
			goto ret ;
		}
		/* PORT 6.18: dying-gasp IRQ (CONFIG_DYING_GASP_IRQ=19) IS a kernel per-CPU IPI on
		 * this platform. Even the request_irq() ATTEMPT corrupts the IPI's irqaction ->
		 * intermittent "Fatal exception in interrupt" (garbage handler) when the IPI fires
		 * (e.g. during smp_call_function). Not needed for GPON activation -> SKIP entirely. */
		printk("econet-xpon: dying-gasp IRQ skipped (irq19 = kernel IPI on 6.18)\n") ;
		/* if(xpon_dying_gasp_init() != 0) { ... }  -- intentionally disabled */
	} else {
		printk("econet-xpon: xpon_hw=0 -> GPON MAC/PHY bring-up SKIPPED (safe load)\n") ;
	}

	if(xpon_proc_init() != 0) {
		printk("xpon vlan filter initialization failed\n") ;
		goto ret ;
	}

    if (ECNT_REGISTER_SUCCESS != ecnt_register_hook(&xpondrv_hook_dispatch_ops) ){
        panic("Register hook function failed! %s:%d", __FUNCTION__, __LINE__);
    }

	/* ---- Bind to the econet-eth PON QDMA seam (exported GPL symbols).
	 * Takes over the PON IRQ callback from the bench-spike stub and enables
	 * the dedicated PON QDMA engine (qdma[1]). Safe: no MAC access. ---- */
	xpon_pon_qdma = en75_xpon_pon_qdma();
	if (xpon_pon_qdma) {
		en75_qdma_use(xpon_pon_qdma);
		en75_qdma_register_pon(xpon_pon_qdma, xpon_pon_irq_cb, NULL);
		printk("econet-xpon: bound to econet-eth PON QDMA engine (qdma[1])\n") ;
		en75_qdma_register_pon_rx(xpon_pon_qdma, xpon_pon_rx_cb, NULL) ;
		xpon_wan_netdev_create() ;
		proc_create("econet_xpon_tx", 0220, NULL, &xpon_tx_test_ops) ;
		proc_create("econet_xpon_rx", 0444, NULL, &xpon_rx_ops) ;
		proc_create("econet_xpon_lb", 0220, NULL, &xpon_lb_ops) ;
		proc_create("econet_xpon_wantx", 0220, NULL, &xpon_wantx_ops) ;
		proc_create("econet_xpon_wanchan", 0220, NULL, &xpon_wanchan_ops) ;
		proc_create("econet_xpon_mkgem", 0220, NULL, &xpon_mkgem_ops) ;
		proc_create("econet_xpon_wanqueue", 0220, NULL, &xpon_wanqueue_ops) ;
		proc_create("econet_xpon_usdbg", 0220, NULL, &xpon_usdbg_ops) ;
		proc_create("econet_xpon_wanmap", 0220, NULL, &xpon_wanmap_ops) ;	/* bab1f510 per-VLAN service table */
	} else {
		printk("econet-xpon: econet-eth PON QDMA engine unavailable (load econet-eth first)\n") ;
	}

	if (xpon_hw) {
#ifndef TCSUPPORT_XPON_LED
		xpon_alarm_led_init();
#endif
		change_alarm_led_status(ALARM_LED_FLICKER);
		XPON_START_TIMER(gpPhyData->trans_status_refresh_timer, 1000) ;
	}

	return 0 ;

ret:
	xpondrv_cleanup() ;
	return -EFAULT ;
}

void xpon_stop_timer(void)
{
    /*Don't access phy register, so timer don't stop*/
    /*XPON_STOP_TIMER(gpPhyData->trans_status_refresh_timer);*/

    if(PHY_GPON_CONFIG == gpPhyData->working_mode){
        gpon_stop_timer();
    }else if(PHY_EPON_CONFIG == gpPhyData->working_mode){
        /* todo: addd epon_stop_timer */
    }
}

#ifndef TCSUPPORT_CPU_EN7521
void enable_cpu_us_omci_oam_traffic(void)
{
    gpPonSysData->isUpOmciOamTraffic = XPON_ENABLE;
}

void enable_cpu_us_data_traffic(void)
{
    gpPonSysData->isUpDataTraffic = XPON_ENABLE;
}

void disable_cpu_us_data_traffic(void)
{
    gpPonSysData->isUpDataTraffic = XPON_DISABLE;
}

void xpon_enable_cpu_protection(void)
{
    attack_protect_set(1, 1);
    gpPonSysData->gswRateLimitFlag = TRUE;
    /* set p6 egress traffic limit (cir=10mbps,cbs=4kbyte) */
    IO_SREG(0xbfb59640, 0x1408488) ;
}

void xpon_disable_cpu_protection(void)
{
    attack_protect_set(0, 1) ;
    gpPonSysData->gswRateLimitFlag = FALSE;
    /*close p6 egress traffic limit */
	regWrite32(0xbfb59640,0x0) ;
}
#endif /* TCSUPPORT_CPU_EN7521 */

module_init(xpondrv_init)
module_exit(xpondrv_cleanup)
MODULE_LICENSE("GPL");

