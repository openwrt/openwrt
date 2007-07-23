/* ============================================================================
 * Copyright (C) 2004 -Infineon Technologies AG.
 *
 * All rights reserved.
 * ============================================================================
 *
 *============================================================================
 * Licensed under GNU GPL v2
 * ============================================================================
 */

 
/* ===========================================================================
 *
 * File Name:   amazon_mei.c
 * Author :     Ou Ke
 *
 * ===========================================================================
 *
 * Project: Amazon
 *
 * ===========================================================================
 * Contents:This file implements the MEI driver for Amazon ADSL/ADSL2+
 *  controller.
 *  
 * ===========================================================================
 * References: 
 *
 */


/* ===========================================================================
 * Revision History:
 * 		12/1/2005 : Ritesh Banerjee
 * 			- Create a kernel thread kmibpoll to poll for periodic RFC 2662
 * 			and RFC 3440 counters. Removes the need for user space 
 * 			adsl_mibpoll_daemon and saves atleast 30KB of RAM.
 *
 * $Log$
 * ===========================================================================
 */

/*
 * ===========================================================================
 *                           INCLUDE FILES
 * ===========================================================================
 */
//000002:fchang 2005/6/2 joelin 04/27/2005 for pcm clock
//000003:fchang 2005/6/2 Henry added for Amazon-E support
//165001:henryhsu 2005/9/6 Modify for adsl firmware version 1.2.1.2.0.1 DATA_LED can't flash.
// 509221:tc.chen 2005/09/22 Reset DFE added when MEI_TO_ARC_CS_DONE not cleared by ARC
// 603221:tc.chen 2006/03/21 added APIs to support the WEB related parameters for ADSL Statistics

#ifndef EXPORT_SYMTAB
#define EXPORT_SYMTAB
#endif
#define AMAZON_MEI_MIB_RFC3440

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/module.h>               
#include <linux/version.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <asm/irq.h>
#include <asm/segment.h>
#include <asm/semaphore.h>
#include <linux/init.h>                                  
#include <linux/ioport.h>
#include <asm/uaccess.h>                       
#include <linux/proc_fs.h>
#include <asm/io.h>
#include <linux/vmalloc.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/list.h>
#include <linux/time.h>

#include <asm/amazon/amazon.h>
#include <asm/irq.h>
#include <asm/amazon/irq.h>
#include <asm/amazon/amazon_mei.h>
#include <asm/amazon/amazon_mei_app.h>
#include <asm/amazon/amazon_mei_ioctl.h>
#include <asm/amazon/amazon_mei_app_ioctl.h>

#define SET_BIT(reg, mask)                  reg |= (mask)
#define CLEAR_BIT(reg, mask)                reg &= (~mask)
#define CLEAR_BITS(reg, mask)               CLEAR_BIT(reg, mask)
#define SET_BITS(reg, mask)                 SET_BIT(reg, mask)
#define SET_BITFIELD(reg, mask, off, val)   {reg &= (~mask); reg |= (val << off);}

extern void mask_and_ack_amazon_irq(unsigned int irq_nr);

#ifdef AMAZON_CHECK_LINK
//amazon_tpe.c
extern int (*adsl_link_notify)(int);
#endif //AMAZON_CHECK_LINK

// for ARC memory access
#define WHILE_DELAY 20000
#define AMAZON_DMA_DEBUG_MUTEX


//TODO
#undef DFE_LOOPBACK
#define ARC_READY_ACK

static amazon_mei_mib * current_intvl;
static struct list_head interval_list;
static amazon_mei_mib * mei_mib;

static int reboot_firsttime=1;//000002:fchang

	//PCM
#define PCM_CHANNEL_NUM		2	//1 rx, 1 tx
static pcm_data_struct	pcm_data[PCM_CHANNEL_NUM]__attribute__ ((aligned(4)));	//0=tx0, 1=rx0, 2=tx1, 3=rx1
static u32 pcm_start_addr;
//#define PCM_HRT_TIME_HZ		4000	//?us
#define PCM_ACCESS_DEBUG
static int irqtimes=0;
#undef DATA_LED_ON_MODE
#define ADSL_LED_SUPPORT	//joelin for adsl led
#ifdef ADSL_LED_SUPPORT
static int firmware_support_led=0; //joelin version check 	for adsl led	
static int stop_led_module=0;	//wakeup and clean led module
static int led_support_check=0;	//1.1.2.7.1.1
#endif //ADSL_LED_SUPPORT
#define IFX_DYING_GASP
#ifdef IFX_DYING_GASP
static wait_queue_head_t wait_queue_dying_gasp;	//dying gasp
//struct tq_struct dying_gasp_task; 		//dying gasp
static wait_queue_head_t wait_queue_uas_poll;	//joelin 04/16/2005
static u16 unavailable_seconds=0;		//joelin 04/16/2005
static meidebug lop_debugwr;				//dying gasp
#endif //IFX_DYING_GASP
static int dbg_int=0;
//#define DEBUG_ACCESS_DELAY	for(dbg_int=0;dbg_int<100;dbg_int++){;}
#define DEBUG_ACCESS_DELAY
static u8 sampledata[512];
static int firsttime[PCM_CHANNEL_NUM]={0,1};
static int num_cmp[PCM_CHANNEL_NUM]={0,0};
static int pcm_start_loc[PCM_CHANNEL_NUM]={0,0}; 

	// for clearEoC 
//#define MEI_CLREOC_BUFF_SIZE	512	//double the receive fifo size, bytes
//static u8 clreoc[MEI_CLREOC_BUFF_SIZE]__attribute__ ((aligned(4)));	//buffer to hold clearEoC data in bytes
#undef AMAZON_CLEAR_EOC
#ifdef AMAZON_CLEAR_EOC
extern void ifx_push_eoc(struct sk_buff * pkt);
#endif
static int meiResetArc(void); 
#define IFX_POP_EOC_DONE	0
#define IFX_POP_EOC_FAIL	-1
static struct list_head clreoc_list;
static amazon_clreoc_pkt * clreoc_pkt;
#define CLREOC_BUFF_SIZE	12	//number of clreoc commands being buffered
//static int clreoc_wr=0;
//static int clreoc_rd=0;		//used to control clreoc circular buffer 
static wait_queue_head_t wait_queue_clreoc;
#ifdef ADSL_LED_SUPPORT
static wait_queue_head_t wait_queue_led;	//adsl led
static wait_queue_head_t wait_queue_led_polling;// adsl led
struct tq_struct led_task; 			// adsl led
static DECLARE_TASK_QUEUE(tq_ifx_led);		// task
int adsl_led_flash_task(void *ptr);		// adsl led
#endif	//ADSL_LED_SUPPORT
static void * clreoc_command_pkt=NULL;
static int clreoc_max_tx_len=0;

// 603221:tc.chen start
#define ME_HDLC_IDLE 0
#define ME_HDLC_INVALID_MSG 1
#define ME_HDLC_MSG_QUEUED 2
#define ME_HDLC_MSG_SENT 3
#define ME_HDLC_RESP_RCVD 4
#define ME_HDLC_RESP_TIMEOUT 5
#define ME_HDLC_RX_BUF_OVERFLOW 6
#define ME_HDLC_UNRESOLVED 1
#define ME_HDLC_RESOLVED 2
// 603221:tc.chen end

#ifdef LOCK_RETRY
static int reboot_lock=0;
#endif

static mib_previous_read mib_pread={0,0,0,0,0,0,0,0,0,0,0,0};
static mib_flags_pretime mib_pflagtime;// initialized when module loaded

	static u32 ATUC_PERF_LOFS=0;
	static u32 ATUC_PERF_LOSS=0;
	static u32 ATUC_PERF_ESS=0;
	static u32 ATUC_PERF_INITS=0;
	static u32 ATUR_PERF_LOFS=0;
	static u32 ATUR_PERF_LOSS=0;
	static u32 ATUR_PERF_LPR=0;
	static u32 ATUR_PERF_ESS=0;
	static u32 ATUR_CHAN_RECV_BLK=0;
	static u32 ATUR_CHAN_TX_BLK=0;
	static u32 ATUR_CHAN_CORR_BLK=0;
	static u32 ATUR_CHAN_UNCORR_BLK=0;
	//RFC-3440
	static u32 ATUC_PERF_STAT_FASTR=0;
	static u32 ATUC_PERF_STAT_FAILED_FASTR=0;
	static u32 ATUC_PERF_STAT_SESL=0;
	static u32 ATUC_PERF_STAT_UASL=0;
	static u32 ATUR_PERF_STAT_SESL=0;
	static u32 ATUR_PERF_STAT_UASL=0;

	static adslChanPrevTxRate PrevTxRate={0,0};
	static adslPhysCurrStatus CurrStatus={0,0};
	static ChanType chantype={0,0};
	static adslLineAlarmConfProfileEntry AlarmConfProfile={"No Name\0",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
// 603221:tc.chen start
	static adslFarEndPerfStats FarendStatsData;
	struct timeval FarendData_acquire_time={0};
	static u32 adsl_mode,adsl_mode_extend; // adsl mode : adsl/ 2/ 2+
	static adslInitStats AdslInitStatsData;
//603221:tc.chen end
static u32 loop_diagnostics_mode=0;
static wait_queue_head_t wait_queue_loop_diagnostic;
#ifdef AMAZON_MEI_MIB_RFC3440
	static adslLineAlarmConfProfileExtEntry AlarmConfProfileExt={"No Name\0",0,0,0,0,0,0};
#endif

static int showtime=0;
static int loop_diagnostics_completed=0;
//////////////////////////////////////////////////////////////////////////////////
static int phy_mei_net_init(struct net_device * dev);
static int interleave_mei_net_init(struct net_device * dev);
static int fast_mei_net_init(struct net_device * dev);
static struct net_device_stats * phy_mei_net_get_stats(struct net_device * dev);
static struct net_device_stats * interleave_mei_net_get_stats(struct net_device * dev);
static struct net_device_stats * fast_mei_net_get_stats(struct net_device * dev);

typedef struct mei_priv{
        struct net_device_stats stats;
}mei_priv;

static struct net_device phy_mei_net = { init: phy_mei_net_init, name: "MEI_PHY"};
static struct net_device interleave_mei_net = { init: interleave_mei_net_init, name: "MEI_INTL"};
static struct net_device fast_mei_net = { init: fast_mei_net_init, name: "MEI_FAST"};
///////////////////////////////////////////////////////////////////////////////////

static int major=AMAZON_MEI_MAJOR;

static struct semaphore mei_sema;

// Mei to ARC CMV count, reply count, ARC Indicator count
static int indicator_count=0;
static int cmv_count=0;
static int reply_count=0;
static u16 Recent_indicator[MSG_LENGTH];

// Used in interrupt handler as flags
static int arcmsgav=0;
static int cmv_reply=0;
static int cmv_waiting=0;

#define PROC_ITEMS 8

long mei_debug_mode = 0; //509221:tc.chen for adsl firmware debug

//  to wait for arc cmv reply, sleep on wait_queue_arcmsgav;
static wait_queue_head_t wait_queue_arcmsgav;
static wait_queue_head_t wait_queue_codeswap;
static wait_queue_head_t wait_queue_mibdaemon;
static wait_queue_head_t wait_queue_reboot;
static u32 * image_buffer=NULL;        	// holding adsl firmware image
static u16 RxMessage[MSG_LENGTH]__attribute__ ((aligned(4)));
static u16 TxMessage[MSG_LENGTH]__attribute__ ((aligned(4)));                                                                                                               
static u32 * mei_arc_swap_buff=NULL;   	//  holding swap pages
static ARC_IMG_HDR * img_hdr;
static int reboot_flag;

#ifdef DFE_LOOPBACK
#include "arc_pm.h"
#endif


/////////////////               net device                              ///////////////////////////////////////////////////
static int phy_mei_net_init(struct net_device * dev)
{
        //ether_setup(dev);
        dev->get_stats = phy_mei_net_get_stats;
        dev->ip_ptr = NULL;
	dev->type = 94;
	
//	dev->mtu=12345;
	dev->flags=IFF_UP;
	
        dev->priv = kmalloc(sizeof(struct mei_priv), GFP_KERNEL);
        if(dev->priv == NULL)
                return -ENOMEM;
        memset(dev->priv, 0, sizeof(struct mei_priv));
        return 0;
}

static int interleave_mei_net_init(struct net_device * dev)
{
        //ether_setup(dev);
        dev->get_stats = interleave_mei_net_get_stats;
        dev->ip_ptr = NULL;
	dev->type = 124;
	dev->flags=IFF_UP;
        dev->priv = kmalloc(sizeof(struct mei_priv), GFP_KERNEL);
        if(dev->priv == NULL)
                return -ENOMEM;
        memset(dev->priv, 0, sizeof(struct mei_priv));
        return 0;
}

static int fast_mei_net_init(struct net_device * dev)
{
        //ether_setup(dev);
        dev->get_stats = fast_mei_net_get_stats;
        dev->ip_ptr = NULL;
	dev->type = 125;
	dev->flags=IFF_UP;
        dev->priv = kmalloc(sizeof(struct mei_priv), GFP_KERNEL);
        if(dev->priv == NULL)
                return -ENOMEM;
        memset(dev->priv, 0, sizeof(struct mei_priv));
        return 0;
}

static struct net_device_stats * phy_mei_net_get_stats(struct net_device * dev)
{
        struct mei_priv * priv;
        priv = (struct mei_priv *)dev->priv;
	// update statistics
	(priv->stats).rx_packets = ATUR_CHAN_RECV_BLK;
	(priv->stats).tx_packets = ATUR_CHAN_TX_BLK;
	(priv->stats).rx_errors = ATUR_CHAN_CORR_BLK + ATUR_CHAN_UNCORR_BLK;
	(priv->stats).rx_dropped = ATUR_CHAN_UNCORR_BLK;
	
        return &(priv->stats);
}

static struct net_device_stats * interleave_mei_net_get_stats(struct net_device * dev)
{
        struct mei_priv * priv;
        priv = (struct mei_priv *)dev->priv;
	// update statistics
	(priv->stats).rx_packets = ATUR_CHAN_RECV_BLK;
	(priv->stats).tx_packets = ATUR_CHAN_TX_BLK;
	(priv->stats).rx_errors = ATUR_CHAN_CORR_BLK + ATUR_CHAN_UNCORR_BLK;
	(priv->stats).rx_dropped = ATUR_CHAN_UNCORR_BLK;
	
        return &(priv->stats);
}

static struct net_device_stats * fast_mei_net_get_stats(struct net_device * dev)
{
        struct mei_priv * priv;
        priv = (struct mei_priv *)dev->priv;
	// update statistics
	(priv->stats).rx_packets = ATUR_CHAN_RECV_BLK;
	(priv->stats).tx_packets = ATUR_CHAN_TX_BLK;
	(priv->stats).rx_errors = ATUR_CHAN_CORR_BLK + ATUR_CHAN_UNCORR_BLK;
	(priv->stats).rx_dropped = ATUR_CHAN_UNCORR_BLK;
	
        return &(priv->stats);
}
/////////////////               mei access Rd/Wr methods       ///////////////////////////////////////////////////
void meiLongwordWrite(u32 ul_address, u32 ul_data)
{
	*((volatile u32 *)ul_address) = ul_data;
	asm("SYNC");
	return;
} //	end of "meiLongwordWrite(..."

void meiLongwordRead(u32 ul_address, u32 *pul_data)
{
	*pul_data = *((volatile u32 *)ul_address);
	asm("SYNC");
	return;
} //	end of "meiLongwordRead(..."

MEI_ERROR meiDMAWrite(u32 destaddr, u32 *databuff, u32 databuffsize)
{
	u32 *p = databuff;
	u32 temp;
	u32 flags;

	if( destaddr & 3)
		return MEI_FAILURE;

#ifdef AMAZON_DMA_DEBUG_MUTEX
	save_flags(flags);
	cli();
#endif
		

	//	Set the write transfer address
	meiLongwordWrite(MEI_XFR_ADDR, destaddr);

	//	Write the data pushed across DMA
	while (databuffsize--)
	{
		temp = *p;
		if(databuff==(u32 *)TxMessage)	// swap half word
			temp = ((temp & 0xffff)<<16) + ((temp & 0xffff0000)>>16);
		meiLongwordWrite(MEI_DATA_XFR, temp);
		p++;
	} // 	end of "while(..."

#ifdef AMAZON_DMA_DEBUG_MUTEX
	restore_flags(flags);	
#endif
	
	return MEI_SUCCESS;

} //	end of "meiDMAWrite(..."

MEI_ERROR meiDMAWrite_16(u32 destaddr, u32 *databuff, u32 databuffsize)
{
	u32 *p = databuff;
	u32 temp;
	u32 flags;

	if( destaddr & 3)
		return MEI_FAILURE;
		
#ifdef AMAZON_DMA_DEBUG_MUTEX
	save_flags(flags);
	cli();
#endif
		

	//	Set the write transfer address
	meiLongwordWrite(MEI_XFR_ADDR, destaddr);

	//	Write the data pushed across DMA
	while (databuffsize--)
	{
		temp = *p;
		temp = ((temp & 0xffff)<<16) + ((temp & 0xffff0000)>>16);//swap half word
		meiLongwordWrite(MEI_DATA_XFR, temp);
		p++;
	} // 	end of "while(..."
	
#ifdef AMAZON_DMA_DEBUG_MUTEX
	restore_flags(flags);
#endif

	return MEI_SUCCESS;

} //	end of "meiDMAWrite_16(..."

MEI_ERROR meiDMAWrite_8(u32 destaddr, u32 *databuff, u32 databuffsize)
{
	u32 *p = databuff;
	u32 temp;
	u32 flags;

	if( destaddr & 3)
		return MEI_FAILURE;
		
#ifdef AMAZON_DMA_DEBUG_MUTEX
	save_flags(flags);
	cli();
#endif
		

	//	Set the write transfer address
	meiLongwordWrite(MEI_XFR_ADDR, destaddr);

	//	Write the data pushed across DMA
	while (databuffsize--)
	{
		temp = *p;
		temp = ((temp & 0xff)<<24) + ((temp & 0xff00)<<8)+ ((temp & 0xff0000)>>8)+ ((temp & 0xff000000)>>24);//swap byte
		meiLongwordWrite(MEI_DATA_XFR, temp);
		p++;
	} // 	end of "while(..."
	
#ifdef AMAZON_DMA_DEBUG_MUTEX
	restore_flags(flags);
#endif

	return MEI_SUCCESS;

} //	end of "meiDMAWrite_8(..."

MEI_ERROR meiDMARead(u32 srcaddr, u32 *databuff, u32 databuffsize)
{
	u32 *p = databuff;
	u32 temp;
	u32 flags;
	
	if( srcaddr & 3)
		return MEI_FAILURE;

#ifdef AMAZON_DMA_DEBUG_MUTEX
	save_flags(flags);
	cli();
#endif
	

	//	Set the read transfer address
	meiLongwordWrite(MEI_XFR_ADDR, srcaddr);

	//	Read the data popped across DMA
	while (databuffsize--)
	{
		meiLongwordRead(MEI_DATA_XFR, &temp);
		if(databuff==(u32 *)RxMessage)	// swap half word
			temp = ((temp & 0xffff)<<16) + ((temp & 0xffff0000)>>16);
		*p=temp;
		p++;
	} // 	end of "while(..."

#ifdef AMAZON_DMA_DEBUG_MUTEX
	restore_flags(flags);
#endif

	return MEI_SUCCESS;

} //	end of "meiDMARead(..."

MEI_ERROR meiDMARead_16(u32 srcaddr, u32 *databuff, u32 databuffsize)
{
	u32 *p = databuff;
	u32 temp;
	u32 flags;
	
	if( srcaddr & 3)
		return MEI_FAILURE;
		
#ifdef AMAZON_DMA_DEBUG_MUTEX
	save_flags(flags);
	cli();
#endif
		

	//	Set the read transfer address
	meiLongwordWrite(MEI_XFR_ADDR, srcaddr);

	//	Read the data popped across DMA
	while (databuffsize--)
	{
		meiLongwordRead(MEI_DATA_XFR, &temp);
		temp = ((temp & 0xffff)<<16) + ((temp & 0xffff0000)>>16);
		*p=temp;
		p++;
	} // 	end of "while(..."
	
#ifdef AMAZON_DMA_DEBUG_MUTEX
	restore_flags(flags);
#endif

	return MEI_SUCCESS;

} //	end of "meiDMARead_16(..."

MEI_ERROR meiDMARead_8(u32 srcaddr, u32 *databuff, u32 databuffsize)
{
	u32 *p = databuff;
	u32 temp;
	u32 flags;
	
	if( srcaddr & 3)
		return MEI_FAILURE;
		
#ifdef AMAZON_DMA_DEBUG_MUTEX
	save_flags(flags);
	cli();
#endif
		

	//	Set the read transfer address
	meiLongwordWrite(MEI_XFR_ADDR, srcaddr);

	//	Read the data popped across DMA
	while (databuffsize--)
	{
		meiLongwordRead(MEI_DATA_XFR, &temp);
		temp = ((temp & 0xff)<<24) + ((temp & 0xff00)<<8)+ ((temp & 0xff0000)>>8)+ ((temp & 0xff000000)>>24);//swap byte
		*p=temp;
		p++;
	} // 	end of "while(..."
	
#ifdef AMAZON_DMA_DEBUG_MUTEX
	restore_flags(flags);
#endif

	return MEI_SUCCESS;

} //	end of "meiDMARead_8(..."

void meiPollForDbgDone(void)
{
	u32	query = 0;
	int 	i=0;
	while (i<WHILE_DELAY)
	{
		meiLongwordRead(ARC_TO_MEI_INT, &query);
		query &= (ARC_TO_MEI_DBG_DONE);
		if(query)
			break;
		i++;
		if(i==WHILE_DELAY){
#ifdef AMAZON_MEI_DEBUG_ON
			printk("\n\n PollforDbg fail");
#endif
		}
			DEBUG_ACCESS_DELAY;
	} 
   	meiLongwordWrite(ARC_TO_MEI_INT,  ARC_TO_MEI_DBG_DONE);  // to clear this interrupt
} //	end of "meiPollForDbgDone(..."

MEI_ERROR meiDebugWrite_8(u32 destaddr, u32 *databuff, u32 databuffsize)
{
	u32 i;
	u32 temp = 0x0;
	u32 address = 0x0;
	u32 *buffer = 0x0;
	u32 flags;
	
#ifdef AMAZON_DMA_DEBUG_MUTEX
	save_flags(flags);
	cli();
#endif
	

	//	Open the debug port before DMP memory write
	meiLongwordRead(MEI_CONTROL, &temp);
		DEBUG_ACCESS_DELAY;	
	temp |= (HOST_MSTR);
	meiLongwordWrite(MEI_CONTROL, temp);
		DEBUG_ACCESS_DELAY;
	meiLongwordWrite(MEI_DEBUG_DEC, MEI_DEBUG_DEC_DMP1_MASK);
		DEBUG_ACCESS_DELAY;

	//	For the requested length, write the address and write the data
	address = destaddr;
	buffer = databuff;
	for (i=0; i < databuffsize; i++)
	{
		meiLongwordWrite(MEI_DEBUG_WAD, address);
			DEBUG_ACCESS_DELAY;
		temp=*buffer;
		temp = ((temp & 0xff)<<24) + ((temp & 0xff00)<<8)+ ((temp & 0xff0000)>>8)+ ((temp & 0xff000000)>>24);//swap byte
		meiLongwordWrite(MEI_DEBUG_DATA, temp);
			DEBUG_ACCESS_DELAY;
		meiPollForDbgDone();
		address += 4;
		buffer++;
	} //	end of "for(..."

	//	Close the debug port after DMP memory write
	meiLongwordRead(MEI_CONTROL, &temp);
		DEBUG_ACCESS_DELAY;
	temp &= ~(HOST_MSTR);
	meiLongwordWrite(MEI_CONTROL, temp);
		DEBUG_ACCESS_DELAY;
		
#ifdef AMAZON_DMA_DEBUG_MUTEX
	restore_flags(flags);
#endif

	//	Return
	return MEI_SUCCESS;

} //	end of "meiDebugWrite_8(..."

MEI_ERROR meiDebugRead_8(u32 srcaddr, u32 *databuff, u32 databuffsize)
{
	u32 i;
	u32 temp = 0x0;
	u32 address = 0x0;
	u32 *buffer = 0x0;
	u32 flags;
	
#ifdef AMAZON_DMA_DEBUG_MUTEX
	save_flags(flags);
	cli();
#endif
	

	//	Open the debug port before DMP memory read
	meiLongwordRead(MEI_CONTROL, &temp);
		DEBUG_ACCESS_DELAY;
	temp |= (HOST_MSTR);
	meiLongwordWrite(MEI_CONTROL, temp);
		DEBUG_ACCESS_DELAY;
	meiLongwordWrite(MEI_DEBUG_DEC, MEI_DEBUG_DEC_DMP2_MASK);
		DEBUG_ACCESS_DELAY;

	//	For the requested length, write the address and read the data
	address = srcaddr;
	buffer = databuff;
	for (i=0; i<databuffsize; i++)
	{
		meiLongwordWrite(MEI_DEBUG_RAD, address);
			DEBUG_ACCESS_DELAY;
		meiPollForDbgDone();
		meiLongwordRead(MEI_DEBUG_DATA, &temp);
			DEBUG_ACCESS_DELAY;
		temp = ((temp & 0xff)<<24) + ((temp & 0xff00)<<8)+ ((temp & 0xff0000)>>8)+ ((temp & 0xff000000)>>24);//swap byte
		*buffer=temp;
		address += 4;
		buffer++;
	} //	end of "for(..."

	//	Close the debug port after DMP memory read
	meiLongwordRead(MEI_CONTROL, &temp);
		DEBUG_ACCESS_DELAY;
	temp &= ~(HOST_MSTR);
	meiLongwordWrite(MEI_CONTROL, temp);
		DEBUG_ACCESS_DELAY;
		
#ifdef AMAZON_DMA_DEBUG_MUTEX
	restore_flags(flags);
#endif

	//	Return
	return MEI_SUCCESS;

} //	end of "meiDebugRead_8(..."

MEI_ERROR meiDebugWrite_16(u32 destaddr, u32 *databuff, u32 databuffsize)
{
	u32 i;
	u32 temp = 0x0;
	u32 address = 0x0;
	u32 *buffer = 0x0;
	u32 flags;
	
#ifdef AMAZON_DMA_DEBUG_MUTEX
	save_flags(flags);
	cli();
#endif
	

	//	Open the debug port before DMP memory write
	meiLongwordRead(MEI_CONTROL, &temp);
		DEBUG_ACCESS_DELAY;
	temp |= (HOST_MSTR);
	meiLongwordWrite(MEI_CONTROL, temp);
		DEBUG_ACCESS_DELAY;
	meiLongwordWrite(MEI_DEBUG_DEC, MEI_DEBUG_DEC_DMP1_MASK);
		DEBUG_ACCESS_DELAY;

	//	For the requested length, write the address and write the data
	address = destaddr;
	buffer = databuff;
	for (i=0; i < databuffsize; i++)
	{
		meiLongwordWrite(MEI_DEBUG_WAD, address);
			DEBUG_ACCESS_DELAY;
		temp=*buffer;
		temp = ((temp & 0xffff)<<16) + ((temp & 0xffff0000)>>16);//swap half word
		meiLongwordWrite(MEI_DEBUG_DATA, temp);
			DEBUG_ACCESS_DELAY;
		meiPollForDbgDone();
		address += 4;
		buffer++;
	} //	end of "for(..."

	//	Close the debug port after DMP memory write
	meiLongwordRead(MEI_CONTROL, &temp);
		DEBUG_ACCESS_DELAY;
	temp &= ~(HOST_MSTR);
	meiLongwordWrite(MEI_CONTROL, temp);
		DEBUG_ACCESS_DELAY;
		
#ifdef AMAZON_DMA_DEBUG_MUTEX
	restore_flags(flags);
#endif

	//	Return
	return MEI_SUCCESS;

} //	end of "meiDebugWrite_16(..."

MEI_ERROR meiDebugRead_16(u32 srcaddr, u32 *databuff, u32 databuffsize)
{
	u32 i;
	u32 temp = 0x0;
	u32 address = 0x0;
	u32 *buffer = 0x0;
	u32 flags;
	
#ifdef AMAZON_DMA_DEBUG_MUTEX
	save_flags(flags);
	cli();
#endif
	

	//	Open the debug port before DMP memory read
	meiLongwordRead(MEI_CONTROL, &temp);
		DEBUG_ACCESS_DELAY;
	temp |= (HOST_MSTR);
	meiLongwordWrite(MEI_CONTROL, temp);
		DEBUG_ACCESS_DELAY;
	meiLongwordWrite(MEI_DEBUG_DEC, MEI_DEBUG_DEC_DMP2_MASK);
		DEBUG_ACCESS_DELAY;

	//	For the requested length, write the address and read the data
	address = srcaddr;
	buffer = databuff;
	for (i=0; i<databuffsize; i++)
	{
		meiLongwordWrite(MEI_DEBUG_RAD, address);
			DEBUG_ACCESS_DELAY;
		meiPollForDbgDone();
		meiLongwordRead(MEI_DEBUG_DATA, &temp);
			DEBUG_ACCESS_DELAY;
		temp = ((temp & 0xffff)<<16) + ((temp & 0xffff0000)>>16);//swap half word
		*buffer=temp;
		address += 4;
		buffer++;
	} //	end of "for(..."

	//	Close the debug port after DMP memory read
	meiLongwordRead(MEI_CONTROL, &temp);
		DEBUG_ACCESS_DELAY;
	temp &= ~(HOST_MSTR);
	meiLongwordWrite(MEI_CONTROL, temp);
		DEBUG_ACCESS_DELAY;
		
#ifdef AMAZON_DMA_DEBUG_MUTEX
	restore_flags(flags);
#endif

	//	Return
	return MEI_SUCCESS;

} //	end of "meiDebugRead_16(..."

MEI_ERROR meiDebugWrite(u32 destaddr, u32 *databuff, u32 databuffsize)
{
	u32 i;
	u32 temp = 0x0;
	u32 address = 0x0;
	u32 *buffer = 0x0;
	u32 flags;
	
#ifdef AMAZON_DMA_DEBUG_MUTEX
	save_flags(flags);
	cli();
#endif
	

	//	Open the debug port before DMP memory write
	meiLongwordRead(MEI_CONTROL, &temp);
		DEBUG_ACCESS_DELAY;
	temp |= (HOST_MSTR);
	meiLongwordWrite(MEI_CONTROL, temp);
		DEBUG_ACCESS_DELAY;
	meiLongwordWrite(MEI_DEBUG_DEC, MEI_DEBUG_DEC_DMP1_MASK);
		DEBUG_ACCESS_DELAY;

	//	For the requested length, write the address and write the data
	address = destaddr;
	buffer = databuff;
	for (i=0; i < databuffsize; i++)
	{
		meiLongwordWrite(MEI_DEBUG_WAD, address);
			DEBUG_ACCESS_DELAY;
		temp=*buffer;
		meiLongwordWrite(MEI_DEBUG_DATA, temp);
			DEBUG_ACCESS_DELAY;
		meiPollForDbgDone();
		address += 4;
		buffer++;
	} //	end of "for(..."

	//	Close the debug port after DMP memory write
	meiLongwordRead(MEI_CONTROL, &temp);
		DEBUG_ACCESS_DELAY;
	temp &= ~(HOST_MSTR);
	meiLongwordWrite(MEI_CONTROL, temp);
		DEBUG_ACCESS_DELAY;

#ifdef AMAZON_DMA_DEBUG_MUTEX
	restore_flags(flags);
#endif

	//	Return
	return MEI_SUCCESS;

} //	end of "meiDebugWrite(..."

MEI_ERROR meiDebugRead(u32 srcaddr, u32 *databuff, u32 databuffsize)
{
	u32 i;
	u32 temp = 0x0;
	u32 address = 0x0;
	u32 *buffer = 0x0;
	u32 flags;
	
#ifdef AMAZON_DMA_DEBUG_MUTEX
	save_flags(flags);
	cli();
#endif
	

	//	Open the debug port before DMP memory read
	meiLongwordRead(MEI_CONTROL, &temp);
		DEBUG_ACCESS_DELAY;
	temp |= (HOST_MSTR);
	meiLongwordWrite(MEI_CONTROL, temp);
		DEBUG_ACCESS_DELAY;
	meiLongwordWrite(MEI_DEBUG_DEC, MEI_DEBUG_DEC_DMP2_MASK);
		DEBUG_ACCESS_DELAY;

	//	For the requested length, write the address and read the data
	address = srcaddr;
	buffer = databuff;
	for (i=0; i<databuffsize; i++)
	{
		meiLongwordWrite(MEI_DEBUG_RAD, address);
			DEBUG_ACCESS_DELAY;
		meiPollForDbgDone();
		meiLongwordRead(MEI_DEBUG_DATA, &temp);
			DEBUG_ACCESS_DELAY;
		*buffer=temp;
		address += 4;
		buffer++;
	} //	end of "for(..."

	//	Close the debug port after DMP memory read
	meiLongwordRead(MEI_CONTROL, &temp);
		DEBUG_ACCESS_DELAY;
	temp &= ~(HOST_MSTR);
	meiLongwordWrite(MEI_CONTROL, temp);
		DEBUG_ACCESS_DELAY;
		
#ifdef AMAZON_DMA_DEBUG_MUTEX
	restore_flags(flags);
#endif

	//	Return
	return MEI_SUCCESS;

} //	end of "meiDebugRead(..."
EXPORT_SYMBOL(meiDebugRead);

void meiMailboxInterruptsDisable(void)
{
	meiLongwordWrite(ARC_TO_MEI_INT_MASK, 0x0);
} //	end of "meiMailboxInterruptsDisable(..."

void meiMailboxInterruptsEnable(void)
{
	meiLongwordWrite(ARC_TO_MEI_INT_MASK, MSGAV_EN); 
} //	end of "meiMailboxInterruptsEnable(..."

MEI_ERROR meiMailboxWrite(u16 *msgsrcbuffer, u16 msgsize)
{
	int i;
	u32 arc_mailbox_status = 0x0;
	u32 temp=0;
	MEI_ERROR meiMailboxError = MEI_SUCCESS;

	//	Check arc if mailbox write can be initiated
/*	meiLongwordRead(MEI_TO_ARC_INT, &arc_mailbox_status);
	if ((arc_mailbox_status & MEI_TO_ARC_MSGAV))
	{
		return MEI_MAILBOX_FULL;
	}
*/
	//	Write to mailbox
	meiMailboxError = meiDMAWrite(MEI_TO_ARC_MAILBOX, (u32*)msgsrcbuffer, msgsize/2);
	meiMailboxError = meiDMAWrite(MEI_TO_ARC_MAILBOXR, (u32 *)(&temp), 1); 

	//	Notify arc that mailbox write completed
	cmv_waiting=1;
	meiLongwordWrite(MEI_TO_ARC_INT, MEI_TO_ARC_MSGAV);
	
	i=0;
        while(i<WHILE_DELAY){ // wait for ARC to clear the bit
		meiLongwordRead(MEI_TO_ARC_INT, &arc_mailbox_status);
		if((arc_mailbox_status & MEI_TO_ARC_MSGAV) != MEI_TO_ARC_MSGAV)
			break;
		i++;
		if(i==WHILE_DELAY){
#ifdef AMAZON_MEI_DEBUG_ON
			printk("\n\n MEI_TO_ARC_MSGAV not cleared by ARC");
#endif
			meiMailboxError = MEI_FAILURE;
#if 0			
			for(i=0;i<msgsize;i++)
				printk("\n %8x", (*(msgsrcbuffer+i)));
#endif
		}	
	}      
		
	//	Return
	return meiMailboxError;

} //	end of "meiMailboxWrite(..."

MEI_ERROR meiMailboxRead(u16 *msgdestbuffer, u16 msgsize)
{
	//u32 arc_mailbox_status = 0x0;
	//u32 *mei_arc_msgbuff = 0x0;
	MEI_ERROR meiMailboxError = MEI_SUCCESS;

            /*
            //	Check arc if mailbox read can be initiated
	meiLongwordRead(ARC_TO_MEI_INT, &arc_mailbox_status);
	if ((arc_mailbox_status & ARC_TO_MEI_MSGAV) == 0)
	{
		return MEI_MAILBOX_EMPTY;
	} //	end of "if(..."
            */
            
	//	Read from mailbox
	meiMailboxError = meiDMARead(ARC_TO_MEI_MAILBOX, (u32*)msgdestbuffer, msgsize/2);

	//	Notify arc that mailbox read completed
	meiLongwordWrite(ARC_TO_MEI_INT, ARC_TO_MEI_MSGAV);

	//	Return
	return meiMailboxError;

} //	end of "meiMailboxRead(..."

MEI_ERROR meiHaltArc(void)
{
	u32 arc_control_mode = 0x0;
	u32 arc_debug_addr = 0x5;
	u32 arc_debug_data = 0x0;

	//	Switch arc control from JTAG mode to MEI mode- write '1' to bit0
	meiLongwordRead(MEI_CONTROL, &arc_control_mode);
	arc_control_mode |= (HOST_MSTR);
	meiLongwordWrite(MEI_CONTROL, arc_control_mode);

	//	Write arc aux reg access mask (0x0) into debug addr decode reg
	meiLongwordWrite(MEI_DEBUG_DEC, MEI_DEBUG_DEC_AUX_MASK);

	//	Write arc debug reg addr (0x5) into debug read addr reg
	meiLongwordWrite(MEI_DEBUG_RAD, arc_debug_addr);
	meiPollForDbgDone();

	//	Read debug data reg and save content
	meiLongwordRead(MEI_DEBUG_DATA, &arc_debug_data);

	//	Write arc debug reg addr (0x5) into debug write addr reg
	meiLongwordWrite(MEI_DEBUG_WAD, arc_debug_addr);

	//	Write debug data reg with content ORd with 0x2 (halt bit set)
	arc_debug_data |= (BIT1);
	meiLongwordWrite(MEI_DEBUG_DATA, arc_debug_data);
       	meiPollForDbgDone();

	//	Switch arc control from MEI mode to JTAG mode- write '0' to bit0
	meiLongwordRead(MEI_CONTROL, &arc_control_mode);
	arc_control_mode &= ~(HOST_MSTR);
	meiLongwordWrite(MEI_CONTROL, arc_control_mode);

	//	Return
	return MEI_SUCCESS;

} //	end of "meiHalt(..."

MEI_ERROR meiDownloadBootCode(void)
{
	u32 arc_control_mode;
	u32 boot_loop;
	u32 page_size;
	u32 dest_addr;

	u32 arc_debug_addr = 0x31F00;
	u32 arc_debug_data = 0x10;
	u32 temp;
//	int i;

	//MEI_ERROR meiDMAError = MEI_SUCCESS;

	//	Disable mask for arc codeswap interrupts
	meiMailboxInterruptsDisable();

	//	Switch arc control from JTAG mode to MEI mode- write '1' to bit0
	meiLongwordRead(MEI_CONTROL, &arc_control_mode);
	arc_control_mode |= (HOST_MSTR);
	meiLongwordWrite(MEI_CONTROL, arc_control_mode);

	//	Write (0x10) to CRI_CCR0(0x31F00) to enable ac_clk signal	
	meiLongwordWrite(MEI_DEBUG_DEC, MEI_DEBUG_DEC_DMP1_MASK);
	meiLongwordWrite(MEI_DEBUG_RAD, arc_debug_addr);
	meiPollForDbgDone();
	meiLongwordRead(MEI_DEBUG_DATA, &temp);
	temp |=arc_debug_data;
	
	meiLongwordWrite(MEI_DEBUG_WAD, arc_debug_addr);
	meiLongwordWrite(MEI_DEBUG_DATA, temp);
        meiPollForDbgDone();
            //meiLongwordWrite(MEI_DEBUG_DEC, MEI_DEBUG_DEC_AUX_MASK);

	//	Switch arc control from MEI mode to JTAG mode- write '0' to bit0
	meiLongwordRead(MEI_CONTROL, &arc_control_mode);
	arc_control_mode &= ~(HOST_MSTR);
	meiLongwordWrite(MEI_CONTROL, arc_control_mode);
	
#ifdef 	AMAZON_MEI_DEBUG_ON   //to test ac_clk setting correctness
	meiLongwordRead(MEI_CONTROL, &arc_control_mode);
	arc_control_mode |= (HOST_MSTR);
	meiLongwordWrite(MEI_CONTROL, arc_control_mode);
	
	meiLongwordWrite(MEI_DEBUG_DEC, MEI_DEBUG_DEC_DMP1_MASK);
	meiLongwordWrite(MEI_DEBUG_RAD, arc_debug_addr);
	meiPollForDbgDone();
	meiLongwordRead(MEI_DEBUG_DATA, &arc_debug_data);

	meiLongwordRead(MEI_CONTROL, &arc_control_mode);
	arc_control_mode &= ~(HOST_MSTR);
	meiLongwordWrite(MEI_CONTROL, arc_control_mode);
	
//	printk("\n\n ac_clk is %8x\n", arc_debug_data);
#endif

	/*
	**	DMA the boot code page(s)
	*/
#ifdef AMAZON_MEI_DEBUG_ON
//	printk("\n\n start download pages");
#endif
	for( boot_loop = 0; boot_loop < img_hdr->count; boot_loop++)
	{
		if( img_hdr->page[boot_loop].p_size & BOOT_FLAG)
		{
			page_size = meiGetPage( boot_loop, GET_PROG, MAXSWAPSIZE, mei_arc_swap_buff, &dest_addr);
			if( page_size > 0)
			{
				meiDMAWrite(dest_addr, mei_arc_swap_buff, page_size);
			}
		}
		if( img_hdr->page[boot_loop].d_size & BOOT_FLAG)
		{
			page_size = meiGetPage( boot_loop, GET_DATA, MAXSWAPSIZE, mei_arc_swap_buff, &dest_addr);
			if( page_size > 0)
			{
				meiDMAWrite( dest_addr, mei_arc_swap_buff, page_size);
			}
		}
	}
#ifdef AMAZON_MEI_DEBUG_ON
//	printk("\n\n pages downloaded");
#endif
	return MEI_SUCCESS;

} //	end of "meiDownloadBootCode(..."

MEI_ERROR meiRunArc(void)
{
	u32 arc_control_mode = 0x0;
	u32 arc_debug_addr = 0x0;
	u32 arc_debug_data = 0x0;

	//	Switch arc control from JTAG mode to MEI mode- write '1' to bit0
	meiLongwordRead(MEI_CONTROL, &arc_control_mode);
	arc_control_mode |= (HOST_MSTR);
	meiLongwordWrite(MEI_CONTROL, arc_control_mode);

	//	Write arc aux reg access mask (0x0) into debug addr decode reg
	meiLongwordWrite(MEI_DEBUG_DEC, MEI_DEBUG_DEC_AUX_MASK);

	//	Write arc status aux reg addr (0x0) into debug read addr reg
	meiLongwordWrite(MEI_DEBUG_RAD, arc_debug_addr);
	meiPollForDbgDone();

	//	Read debug data reg and save content
	meiLongwordRead(MEI_DEBUG_DATA, &arc_debug_data);

	//	Write arc status aux reg addr (0x0) into debug write addr reg
	meiLongwordWrite(MEI_DEBUG_WAD, arc_debug_addr);

	//	Write debug data reg with content ANDd with 0xFDFFFFFF (halt bit cleared)
	arc_debug_data &= ~(BIT25);
	meiLongwordWrite(MEI_DEBUG_DATA, arc_debug_data);
        meiPollForDbgDone();

	//	Switch arc control from MEI mode to JTAG mode- write '0' to bit0
	meiLongwordRead(MEI_CONTROL, &arc_control_mode);
	arc_control_mode &= ~(HOST_MSTR);
	meiLongwordWrite(MEI_CONTROL, arc_control_mode);

	//	Enable mask for arc codeswap interrupts
	meiMailboxInterruptsEnable();

	//	Return
	return MEI_SUCCESS;

} //	end of "meiActivate(..."

int meiGetPage( u32 Page, u32 data, u32 MaxSize, u32 *Buffer, u32 *Dest)
{
	u32	size;
	u32	i;
	u32	*p;

	if( Page > img_hdr->count)
		return -2;

	/*
	**	Get program or data size, depending on "data" flag
	*/
	size = (data == GET_DATA) ? img_hdr->page[ Page].d_size : img_hdr->page[ Page].p_size;

	size &= BOOT_FLAG_MASK; 	//	Clear boot bit!
	if( size > MaxSize)
		return -1;

	if( size == 0)
		return 0;
	/*
	**	Get program or data offset, depending on "data" flag
	*/
	i = data ? img_hdr->page[ Page].d_offset : img_hdr->page[ Page].p_offset;

	/*
	**	Copy data/program to buffer
	*/

	i /= 4;	//	Adjust offset for byte-to-UINT for array operation

	p = (u32 *)img_hdr + i;
	for(i = 0; i < size; i++)
		Buffer[i] = *p++;
	/*
	**	Pass back data/program destination address
	*/
	*Dest = data ? img_hdr->page[Page].d_dest : img_hdr->page[Page].p_dest;

	return size;
}

MEI_ERROR meiCMV(u16 * request, int reply)            // write cmv to arc, if reply needed, wait for reply
{
        MEI_ERROR meierror;
        wait_queue_t wait;
        
        cmv_reply=reply;
        
        meierror = meiMailboxWrite(request, MSG_LENGTH);
	
        if(meierror != MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
                printk("\n\n MailboxWrite Fail.");
#endif
                return meierror;
        }
        else{
                cmv_count++;
	}

        if(cmv_reply == NO_REPLY)
                return MEI_SUCCESS;

        init_waitqueue_entry(&wait, current);
        add_wait_queue(&wait_queue_arcmsgav, &wait);
        set_current_state(TASK_INTERRUPTIBLE);
//	cmv_waiting=1;
	
        if(arcmsgav==1){
                set_current_state(TASK_RUNNING);
                remove_wait_queue(&wait_queue_arcmsgav, &wait);
        }
        else{
                schedule_timeout(CMV_TIMEOUT);
                remove_wait_queue(&wait_queue_arcmsgav, &wait);
        }
	if(arcmsgav==0){//CMV_timeout
		cmv_waiting=0;
		arcmsgav=0;
#ifdef AMAZON_MEI_DEBUG_ON
		printk("\nmeiCMV: MEI_MAILBOX_TIMEOUT\n");
#endif
		return MEI_MAILBOX_TIMEOUT; 	
	}
	else{
        	arcmsgav=0;
       	 	reply_count++;
        	return MEI_SUCCESS;
	}
}

//TODO, for loopback test
#ifdef DFE_LOOPBACK
#define mte_reg_base                    (0x4800*4+0x20000)
 
/* Iridia Registers Address Constants */
#define MTE_Reg(r)                              (int)(mte_reg_base + (r*4))
 
#define IT_AMODE                                MTE_Reg(0x0004)


#define OMBOX_BASE 0x15F80
#define IMBOX_BASE 0x15FC0

#define TIMER_DELAY   (1024)
#define BC0_BYTES     (32)
#define BC1_BYTES     (30)
#define NUM_MB        (12)
#define TIMEOUT_VALUE 2000

void BFMWait (u32 cycle) {
  u32 i;
  for (i = 0 ; i< cycle ; i++); 
}

void WriteRegLong(u32 addr, u32 data){
  //printk("[%8x] <= %8x \n\n", addr, data);
  *((volatile u32 *)(addr)) =  data; 
}

u32 ReadRegLong (u32 addr) {
  u32	rd_val;
  
  rd_val = *((volatile u32 *)(addr));
  //printk("[%8x] => %8x \n\n", addr, rd_val);
  return rd_val;

}

/* This routine writes the mailbox with the data in an input array */
void WriteMbox(u32 *mboxarray,u32 size) {
  u32 i;
  
  WriteRegLong(MEI_XFR_ADDR,IMBOX_BASE);
  for (i=0;i<size;i++) {
    WriteRegLong(MEI_DATA_XFR,*(mboxarray+i));
  }
}

/* This routine reads the output mailbox and places the results into an array */
void ReadMbox(u32 *mboxarray,u32 size) {
  u32 i;
  
  WriteRegLong(MEI_XFR_ADDR,OMBOX_BASE);
  for (i=0;i<size;i++) {
    mboxarray[i] = ReadRegLong(MEI_DATA_XFR);
  }
}

void MEIWriteARCValue(u32 address, u32 value)
{
  u32 i,check = 0;
  /* Write address register */
  *((volatile u32 *)MEI_DEBUG_WAD) =  address;

  /* Write data register */
  *((volatile u32 *)MEI_DEBUG_DATA) =  value;

  /* wait until complete - timeout at 40*/
  for (i=0;i<40;i++) {
    check = *((volatile u32 *)ARC_TO_MEI_INT);
    if ((check & 0x20)) break;
    //printk("MEIWriteARCValue: check:%8x\n\n", check);
    }

  /* clear the flag */
  *((volatile u32 *)ARC_TO_MEI_INT)  = 0x20;

}


void post_mei_init(void)
{
u32 mailbox[NUM_MB];

  mailbox[0] = TIMER_DELAY;
 
  /* set bytes per bearer channel */
  mailbox[1] = BC0_BYTES;
  mailbox[2] = BC1_BYTES;
  WriteMbox(mailbox, 3);

  WriteRegLong(AAI_ACCESS, 0x00000001);

  /* enable ADSL block clock, ac_clk */
  WriteRegLong(MEI_CONTROL, 0x01);
  WriteRegLong(MEI_DEBUG_DEC, 0x00000001); // select ld/st space
  MEIWriteARCValue(0x31F00,   0x00000010); // write CRI_CCR0 to enable ac_clk

  /* set the MTE to register start */
  MEIWriteARCValue(IT_AMODE, 0xF);
  BFMWait(10); 
}


int wait_sync(void)
{
u32 mailbox[NUM_MB];
  /* wait for ATM sync to be achieved on both BC0 and BC1 */
  u32 timeout=0;
  ReadMbox(mailbox, 1);
  u32 readval = mailbox[0];
  while( ((readval & 0xFFFFFFFF) == 0) && (timeout < TIMEOUT_VALUE) ) {
    BFMWait(1);
    //printk("wait_sync\n\n");
    ReadMbox(mailbox, 1);
    readval = mailbox[0];
    timeout++;
  }
  if(timeout == TIMEOUT_VALUE)return 0;
  else return 1;
}
#endif //DFE_LOOPBACK
//end of TODO, for loopback test

MEI_ERROR meiForceRebootAdslModem(void)
{
#if 0
//#ifdef	ARC_READY_ACK
		if(down_interruptible(&mei_sema))	//disable CMV access until ARC ready
		{
                	return -ERESTARTSYS;
		}
#endif
	if(reboot_firsttime==1){//000002:fchang Start
		// reset ARC
		*((volatile u32 *)0xB0100910) = 0x80;	//reset DFE
		asm("SYNC");
		*((volatile u32 *)0xB0100910) = 0x0;
		asm("SYNC");
		if((*((volatile u32 *)0xB0100910))!=0x0)
#ifdef AMAZON_MEI_DEBUG_ON
			printk("\n reset DFE fail");
#endif
		
		// reset ARC
		meiLongwordWrite(MEI_CONTROL, SOFT_RESET);
		asm("SYNC");
		meiLongwordWrite(MEI_CONTROL, 0);
		asm("SYNC");	

	}	//000002:fchang End		
#ifdef DFE_LOOPBACK
                img_hdr=(ARC_IMG_HDR *)lp_image;
#else
		img_hdr=(ARC_IMG_HDR *)image_buffer;
#endif		
//		printk("\n\n enter haltarc");
                meiHaltArc();
//		printk("\n\n haltarc done");
//000002:fchang Start
	if(reboot_firsttime==0){
		printk("\n\n new reboot");
		meiResetArc();
		meiResetCore();
	}
	if(reboot_firsttime==1)
                meiDownloadBootCode();
	else
		mei_ioctl((struct inode *)NULL, (struct file *)NULL, AMAZON_MEI_DOWNLOAD, (unsigned long)NULL);

//000002:fchang End
#ifdef AMAZON_MEI_DEBUG_ON
//		printk("\n\n Download Done");
#endif

#ifdef	DFE_LOOPBACK
		post_mei_init();
#endif		

//		sema_init(&mei_sema, 1);
		//up(&mei_sema);

//		enable_irq(AMAZON_MEI_INT);
					
		meiRunArc();
//000002:fchang Start		
	if(reboot_firsttime==0){
		meiEnalbeMailboxInt();
	}		
//000002:fchang End
		
#ifdef AMAZON_MEI_DEBUG_ON
//		printk("\n\n ARC Running");
#endif
		
#ifdef 	AMAZON_MEI_DEBUG_ON   //to test ac_clk setting correctness
	{
	u32 arc_control_mode;
	u32 arc_debug_addr = 0x31F00;
	u32 arc_debug_data = 0x10;
	meiLongwordRead(MEI_CONTROL, &arc_control_mode);
	arc_control_mode |= (HOST_MSTR);
	meiLongwordWrite(MEI_CONTROL, arc_control_mode);
	
	meiLongwordWrite(MEI_DEBUG_DEC, MEI_DEBUG_DEC_DMP1_MASK);
	meiLongwordWrite(MEI_DEBUG_RAD, arc_debug_addr);
	meiPollForDbgDone();
	meiLongwordRead(MEI_DEBUG_DATA, &arc_debug_data);

	meiLongwordRead(MEI_CONTROL, &arc_control_mode);
	arc_control_mode &= ~(HOST_MSTR);
	meiLongwordWrite(MEI_CONTROL, arc_control_mode);
	
//	printk("\n\n ac_clk is %8x\n", arc_debug_data);
	}
#endif		
			
		
#ifdef DFE_LOOPBACK
		if (wait_sync() == 0){
			printk("ARC fails to run: time out\n\n");
		}else{
//			printk("ARC is ready\n\n");
		}
#endif		
		if(reboot_firsttime==1)	//000002:fchang
			reboot_firsttime=0;	//000002:fchang
		
                return MEI_SUCCESS;
}

////////////////////            procfs debug            ////////////////////////////////////////////////////////
#define MEI_DIRNAME     "mei"
static struct proc_dir_entry *meidir;

static ssize_t proc_write(struct file *, const char *, size_t, loff_t *);
static ssize_t proc_read(struct file *, char *, size_t, loff_t *);

static struct file_operations proc_operations = {
	read:	proc_read,
	write:	proc_write,
};

typedef struct reg_entry {
	int * flag;
	char name[30];          // big enough to hold names
	char description[100];      // big enough to hold description
	unsigned short low_ino;
} reg_entry_t;

static reg_entry_t regs[PROC_ITEMS];       // total items to be monitored by /proc/mei

#define NUM_OF_REG_ENTRY	(sizeof(regs)/sizeof(reg_entry_t))

static int proc_read(struct file * file, char * buf, size_t nbytes, loff_t *ppos)
{
        int i_ino = (file->f_dentry->d_inode)->i_ino;
	char outputbuf[64];
	int count=0;
	int i;
	u32 version=0;
	reg_entry_t* current_reg=NULL;
	
	for (i=0;i<NUM_OF_REG_ENTRY;i++) {
		if (regs[i].low_ino==i_ino) {
			current_reg = &regs[i];
			break;
		}
	}
	if (current_reg==NULL)
		return -EINVAL;
	
	if (current_reg->flag == (int *) 8){
	///proc/mei/version
	//format:
	//Firmware version: major.minor.sub_version.int_version.rel_state.spl_appl
	//Firmware Date Time Code: date/month min:hour
		if (*ppos>0) /* Assume reading completed in previous read*/
			return 0;               // indicates end of file
		if(down_interruptible(&mei_sema))
                	return -ERESTARTSYS;
		
		//if (indicator_count != 1){
		if (indicator_count < 1){
			up(&mei_sema);
			return -EAGAIN;
		}
		//major:bits 0-7 
		//minor:bits 8-15
		makeCMV(H2D_CMV_READ, INFO, 54, 0, 1, NULL);
		if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#if 0
#ifdef AMAZON_MEI_DEBUG_ON
			printk("\n\n WINHOST CMV fail");
#endif	
#endif
			up(&mei_sema);
			return -EIO;
		}
		version = RxMessage[4];
		count = sprintf(outputbuf, "%d.%d.",(version)&0xff,(version>>8)&0xff);
		
		//sub_version:bits 4-7
		//int_version:bits 0-3
		//spl_appl:bits 8-13
		//rel_state:bits 14-15
		makeCMV(H2D_CMV_READ, INFO, 54, 1, 1, NULL);
		if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#if 0
#ifdef AMAZON_MEI_DEBUG_ON
			printk("\n\n WINHOST CMV fail");	
#endif
#endif
			up(&mei_sema);
			return -EFAULT;
		}
		version =RxMessage[4];
		count += sprintf(outputbuf+count, "%d.%d.%d.%d",
				(version>>4)&0xf, 
				version&0xf,
				(version>>14)&0x3, 
				(version>>8)&0x3f);	
#ifdef ADSL_LED_SUPPORT				
// version check -start	for adsl led			
		if ((((version>>4)&0xf)==2)&&((version&0xf)>=3)&&((version&0xf)<7)) firmware_support_led=1;
		else if ((((version>>4)&0xf)==2)&&((version&0xf)>=7)) firmware_support_led=2;
		else if (((version>>4)&0xf)>2) firmware_support_led=2;

//165001:henryhsu:20050906:Modify for adsl firmware version 1.2.1.2.0.1 DATA_LED can't flash.
	//else firmware_support_led=0;
	else firmware_support_led=2;
//165001		


// version check -end	
#endif	
		//Date:bits 0-7
		//Month:bits 8-15
		makeCMV(H2D_CMV_READ, INFO, 55, 0, 1, NULL);
		if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#if 0
#ifdef AMAZON_MEI_DEBUG_ON
			printk("\n\n WINHOST CMV fail");
#endif	
#endif
			up(&mei_sema);
			return -EIO;
		}
		version = RxMessage[4];
		
		//Hour:bits 0-7
		//Minute:bits 8-15
		makeCMV(H2D_CMV_READ, INFO, 55, 1, 1, NULL);
		if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#if 0
#ifdef AMAZON_MEI_DEBUG_ON
			printk("\n\n WINHOST CMV fail");
#endif	
#endif
			up(&mei_sema);
			return -EFAULT;
		}
		version += (RxMessage[4]<<16);
		count += sprintf(outputbuf+count, " %d/%d %d:%d\n"
				,version&0xff
				,(version>>8)&0xff
				,(version>>25)&0xff
				,(version>>16)&0xff);
				
		up(&mei_sema);	
		
		*ppos+=count;
	}else if(current_reg->flag != (int *)Recent_indicator){
        	if (*ppos>0) /* Assume reading completed in previous read*/
			return 0;               // indicates end of file
		count = sprintf(outputbuf, "0x%08X\n\n", *(current_reg->flag));
	        *ppos+=count;
	        if (count>nbytes)  /* Assume output can be read at one time */
			return -EINVAL;
        }else{
        	if((int)(*ppos)/((int)7)==16)
                	return 0;  // indicate end of the message
        	count = sprintf(outputbuf, "0x%04X\n\n", *(((u16 *)(current_reg->flag))+ (int)(*ppos)/((int)7)));
                *ppos+=count;
	}
	if (copy_to_user(buf, outputbuf, count))
		return -EFAULT;
	return count;
}

static ssize_t proc_write(struct file * file, const char * buffer, size_t count, loff_t *ppos)
{
	int i_ino = (file->f_dentry->d_inode)->i_ino;
	reg_entry_t* current_reg=NULL;
	int i;
	unsigned long newRegValue;
	char *endp;

	for (i=0;i<NUM_OF_REG_ENTRY;i++) {
		if (regs[i].low_ino==i_ino) {
			current_reg = &regs[i];
			break;
		}
	}
	if ((current_reg==NULL) || (current_reg->flag == (int *)Recent_indicator))
		return -EINVAL;

	newRegValue = simple_strtoul(buffer,&endp,0);
	*(current_reg->flag)=(int)newRegValue;
	return (count+endp-buffer);
}
////////////////makeCMV(Opcode, Group, Address, Index, Size, Data), CMV in u16 TxMessage[MSG_LENGTH]///////////////////////////
void makeCMV(u8 opcode, u8 group, u16 address, u16 index, int size, u16 * data)
{
	memset(TxMessage, 0, MSG_LENGTH*2);
	TxMessage[0]= (opcode<<4) + (size&0xf);
	TxMessage[1]= (((index==0)?0:1)<<7) + (group&0x7f);
	TxMessage[2]= address;
	TxMessage[3]= index;
	if(opcode == H2D_CMV_WRITE)
		memcpy(TxMessage+4, data, size*2);
	return;
}

////////////////makeCMV(Opcode, Group, Address, Index, Size, Data), CMV in u16 TxMessage[MSG_LENGTH]///////////////////////////
void makeCMV_local(u8 opcode, u8 group, u16 address, u16 index, int size, u16 * data,u16 *CMVMSG)
{
	memset(CMVMSG, 0, MSG_LENGTH*2);
	CMVMSG[0]= (opcode<<4) + (size&0xf);
	CMVMSG[1]= (((index==0)?0:1)<<7) + (group&0x7f);
	CMVMSG[2]= address;
	CMVMSG[3]= index;
	if(opcode == H2D_CMV_WRITE)
		memcpy(CMVMSG+4, data, size*2);
	return;
}

////////////////                Driver Structure                /////////////////////////////////////////////////////////////////////////////
static ssize_t mei_write(struct file *, const char *, size_t, loff_t *);
static int mei_ioctl(struct inode *, struct file *, unsigned int, unsigned long);

static struct file_operations mei_operations = {
        write:         	mei_write,
        ioctl:         	mei_ioctl,
};


static ssize_t mei_write(struct file * filp, const char * buf, size_t size, loff_t * loff)
{
//	printk("\n\n mei_write entered");
//      image_buffer = (u32 *)kmalloc(size, GFP_KERNEL);
	image_buffer = (u32 *)vmalloc(size);
//	printk("\n\n image_buffer kmalloc done");
	if(image_buffer == NULL){
#ifdef AMAZON_MEI_DEBUG_ON
//		printk("\n\n kmalloc for firmware image fail");
		printk("\n\n vmalloc for firmware image fail");
#endif
		return -1;
	}
	copy_from_user((char *)image_buffer, buf, size);
//	printk("\n\n copy_from_user done");
        return size;
}

	////////// ISR GPTU Timer 6 for high resolution timer /////////////
void amazon_timer6_interrupt_MEI(int irq, void *dev_id, struct pt_regs *regs)
{
	int i,j;
	u32 temp;
	u16 temp16;
	u16 rdindex, wrindex;
	u16 num_rd=0;	//num of byte can be read
	u16 bytes_to_wr=0;
	
//	printk("\n\nenter timer\n\n");
	irqtimes++;
//	printk("\n%d\n",irqtimes);


/*
#ifdef	PCM_ACCESS_DEBUG
		meiDebugRead_8(0x30f20, &temp, 1);
#else
		meiDMARead_8(0x30f20, &temp, 1);
#endif		
	if((temp&0x4000)!=0){
		printk("\nER_ERR");
#ifdef	PCM_ACCESS_DEBUG
		meiDebugWrite_8(0x30f20, &temp, 1);
#else
		meiDMAWrite_8(0x30f20, &temp, 1);
#endif
#ifdef	PCM_ACCESS_DEBUG
		meiDebugRead_8(0x30f20, &temp, 1);
#else
		meiDMARead_8(0x30f20, &temp, 1);
#endif
		if((temp&0x4000)!=0)
			printk("\nER_ERR not cleared");
	}
*/	
	
	for(i=PCM_CHANNEL_NUM-1;i>=0;i--){// start from last channel, which is rx  
#ifdef	PCM_ACCESS_DEBUG
		meiDebugRead_16(pcm_start_addr+i*16+12, &temp, 1);
#else
		meiDMARead_16(pcm_start_addr+i*16+12, &temp, 1);
#endif
		wrindex = (u16)((temp & 0xffff0000)>>16);
//		printk(" %d",wrindex);
#ifdef	PCM_ACCESS_DEBUG
		meiDebugRead_16(pcm_start_addr+i*16+8, &temp, 1);
#else
		meiDMARead_16(pcm_start_addr+i*16+8, &temp, 1);
#endif
		rdindex = (u16)(temp & 0xffff);
//		printk(" %d",rdindex);
		if(rdindex<=wrindex)
			num_rd=((wrindex-rdindex)/4)*4;	//read multiply of 4 bytes
		else
			num_rd=((pcm_data[i].len-(rdindex-wrindex))/4)*4;	//read multiply of 4 bytes
		
		if(i%2!=0){//rx channel
			pcm_data[i].point=0;
			for(j=0;j<num_rd/4;j++){
				if(pcm_data[i].finish!=1){
					if((rdindex+j*4)>=pcm_data[i].len)
						temp16=(rdindex+j*4) - pcm_data[i].len;
					else
						temp16=rdindex+j*4;
#ifdef	PCM_ACCESS_DEBUG
					meiDebugRead_8((((u32)(pcm_data[i].LSW))+(((u32)(pcm_data[i].MSW))<<16))+temp16, (u32*)(pcm_data[i].buff+pcm_data[i].point), 1);
#else
					meiDMARead_8((((u32)(pcm_data[i].LSW))+(((u32)(pcm_data[i].MSW))<<16))+temp16, (u32*)(pcm_data[i].buff+pcm_data[i].point), 1);
#endif
				//	printk(" %8x", *((u32*)(pcm_data[i].buff+pcm_data[i].point))); 	
				/*	if(pcm_data[i].point==0){
						if(pcm_data[i].buff[0]==0xA5){// start of loopback data
							pcm_data[i].point+=4;	
							printk("\nstart receive data");
						}	
					} 
					else*/
						pcm_data[i].point+=4;
				/*	if(pcm_data[i].point==PCM_BUFF_SIZE){	//finish rx
						pcm_data[i].finish=1;
						printk("\nchannel[%d] finished", i);
					}	*/
				}
			}
			if(firsttime[i]==1){
				for(j=0;j<num_rd;j++){
					if(pcm_data[i].buff[j]==0x1){
						num_cmp[i]=num_rd-j;
						firsttime[i]=0;
						break;
					}
				}
				if(memcmp(sampledata+1, pcm_data[i].buff+j, num_cmp[i])!=0)
					printk("\n\ndata wrong,1st\n\n");
				else
					pcm_start_loc[i] = num_cmp[i]+1;
			}
			else{
				if(memcmp(sampledata+pcm_start_loc[i], pcm_data[i].buff, num_rd)!=0)
					printk("\n\ndata wrong\n\n");
				else{
					pcm_start_loc[i]+=num_rd;
					if(pcm_start_loc[i]>=256)
						pcm_start_loc[i]=pcm_start_loc[i]-256;
				}
			}
			
			rdindex +=num_rd;
			if(rdindex>=pcm_data[i].len)
				rdindex=rdindex-pcm_data[i].len;
#ifdef	PCM_ACCESS_DEBUG
			meiDebugRead_16(pcm_start_addr+i*16+8, &temp, 1);
#else
			meiDMARead_16(pcm_start_addr+i*16+8, &temp, 1);
#endif
			temp= (temp & 0xffff0000) + rdindex;
#ifdef	PCM_ACCESS_DEBUG
			meiDebugWrite_16(pcm_start_addr+i*16+8, &temp, 1); // update rdindex 
#else
			meiDMAWrite_16(pcm_start_addr+i*16+8, &temp, 1); // update rdindex
#endif
			
			bytes_to_wr = num_rd;
			
	//		if(bytes_to_wr>0){
		//		printk(" %d", num_rd);
		//		printk(" %d", rdindex);
//				printk("\n\nrdindex = %d", rdindex);	
			//}
		}
		else{	//tx channel
	//		if((bytes_to_wr + num_rd) < pcm_data[i].len){
				for(j=0;j<bytes_to_wr/4;j++){
					if(pcm_data[i].finish!=1){
						if((wrindex+j*4)>=pcm_data[i].len)
							temp16=(wrindex+j*4) - pcm_data[i].len;
						else
							temp16=wrindex + j*4;
/*							
#ifdef	PCM_ACCESS_DEBUG
						meiDebugWrite_8((((u32)(pcm_data[i].LSW))+(((u32)(pcm_data[i].MSW))<<16))+temp16,(u32*)(pcm_data[i+1].buff+j*4), 1);
#else
						meiDMAWrite_8((((u32)(pcm_data[i].LSW))+(((u32)(pcm_data[i].MSW))<<16))+temp16,(u32*)(pcm_data[i+1].buff+j*4), 1);
#endif*/
							
#ifdef	PCM_ACCESS_DEBUG
						meiDebugWrite_8((((u32)(pcm_data[i].LSW))+(((u32)(pcm_data[i].MSW))<<16))+temp16,(u32*)(pcm_data[i].buff+pcm_data[i].point), 1);
				//		meiDebugWrite_8((((u32)(pcm_data[i].LSW))+(((u32)(pcm_data[i].MSW))<<16))+temp16,(u32*)(pcm_data[i].buff), 1);
#else
						meiDMAWrite_8((((u32)(pcm_data[i].LSW))+(((u32)(pcm_data[i].MSW))<<16))+temp16,(u32*)(pcm_data[i].buff+pcm_data[i].point), 1);
#endif		
						pcm_data[i].point+=4;
						if(pcm_data[i].point==PCM_BUFF_SIZE){
					//		pcm_data[i].finish=1; 
					//		printk("\nchannel[%d] finished", i);
							pcm_data[i].point=0;
						}	
					}
				}
				wrindex+=bytes_to_wr;
				if(wrindex>=pcm_data[i].len)
					wrindex=wrindex-pcm_data[i].len;
#ifdef	PCM_ACCESS_DEBUG
				meiDebugRead_16(pcm_start_addr+i*16+12, &temp, 1);
#else
				meiDMARead_16(pcm_start_addr+i*16+12, &temp, 1);
#endif
				temp=(temp&0xffff) + (wrindex<<16);
#ifdef	PCM_ACCESS_DEBUG
				meiDebugWrite_16(pcm_start_addr+i*16+12, &temp, 1); // update wrindex
#else
				meiDMAWrite_16(pcm_start_addr+i*16+12, &temp, 1); // update wrindex
#endif
			
				//if(bytes_to_wr>0){
			//		printk(" %d", bytes_to_wr);
			//		printk(" %d", wrindex);
//					printk("\n\nwrindex = %d", wrindex);	
				//}
		//	}
		}
	}
	return;
}
//000002:fchang Start
static int meiResetArc(void)
{
	u32 auxreg0;
	u32 auxreg5;
	int flshcnt=0;
	int flshcnt1=0;
	int flshcnt2=0;
	
	meiLongwordWrite(MEI_CONTROL, 1);
	meiLongwordWrite(MEI_DEBUG_DEC, 3);	
	meiLongwordWrite(MEI_DEBUG_WAD, 0x3c);
	meiLongwordWrite(MEI_DEBUG_DATA, 0x10);
	meiPollForDbgDone();
	meiLongwordWrite(MEI_DEBUG_DEC, 0x0);
	meiLongwordWrite(MEI_DEBUG_WAD, 0x2);
	meiLongwordWrite(MEI_DEBUG_DATA, 0x0);
	meiPollForDbgDone();
	meiLongwordWrite(MEI_DEBUG_WAD, 0x3);
	meiLongwordWrite(MEI_DEBUG_DATA, 0x0);
	meiPollForDbgDone();
	meiLongwordWrite(MEI_DEBUG_DEC, 0x0);
	meiLongwordWrite(MEI_DEBUG_RAD, 0x0);
	meiPollForDbgDone();
	meiLongwordRead(MEI_DEBUG_DATA, &auxreg0);
	auxreg0 = auxreg0 & 0x03ffffff;
	meiLongwordWrite(MEI_DEBUG_WAD, 0x0);
	meiLongwordWrite(MEI_DEBUG_DATA, auxreg0);
	meiPollForDbgDone();
	meiLongwordWrite(MEI_DEBUG_WAD, 0x10a);
	meiLongwordWrite(MEI_DEBUG_DATA, 0x0);
	meiPollForDbgDone();
	meiLongwordWrite(MEI_DEBUG_DEC, 0x2);
	meiLongwordWrite(MEI_DEBUG_WAD, 0xfffc);
	meiLongwordWrite(MEI_DEBUG_DATA, 0x1fffffff);
	meiPollForDbgDone();
	while(flshcnt<3){
		meiLongwordWrite(MEI_DEBUG_DEC, 0x0);
		meiLongwordWrite(MEI_DEBUG_RAD, 0x0);
		meiPollForDbgDone();
		meiLongwordRead(MEI_DEBUG_DATA, &auxreg0);
		auxreg0 = auxreg0 & 0xff000000;
		auxreg0 = auxreg0 | 0x3fff;
		meiLongwordWrite(MEI_DEBUG_WAD, 0x0);
		meiLongwordWrite(MEI_DEBUG_DATA, auxreg0);
		meiPollForDbgDone();
		
		meiLongwordWrite(MEI_DEBUG_DEC, 0x0);
		meiLongwordWrite(MEI_DEBUG_RAD, 0x5);
		meiPollForDbgDone();
		meiLongwordRead(MEI_DEBUG_DATA, &auxreg5);
		auxreg5 = auxreg5 | 0x801;
		meiLongwordWrite(MEI_DEBUG_WAD, 0x5);
		meiLongwordWrite(MEI_DEBUG_DATA, auxreg5);
		meiPollForDbgDone();
		meiLongwordWrite(MEI_DEBUG_RAD, 0x0);
		meiPollForDbgDone();
		meiLongwordRead(MEI_DEBUG_DATA, &auxreg0);
		auxreg0 = auxreg0 & 0x00ffffff;
		if(auxreg0 == 0x4000)
			flshcnt = flshcnt+1;
		else{
			if(flshcnt == 0)
				flshcnt1 = flshcnt1 +1;
			else
				flshcnt2 = flshcnt2 +1;
		}	
	}
	
	return 1;
}

static int meiResetCore(void)
{
	meiLongwordWrite(MEI_CONTROL, 0x1);
	meiLongwordWrite(MEI_DEBUG_DEC, 0x2);
	meiLongwordWrite(MEI_DEBUG_WAD, 0x31f10);
	meiLongwordWrite(MEI_DEBUG_DATA, 0xf);
	meiPollForDbgDone();
	meiLongwordWrite(MEI_DEBUG_WAD, 0x31f10);
	meiLongwordWrite(MEI_DEBUG_DATA, 0x0);
	meiPollForDbgDone();
	meiLongwordWrite(MEI_DEBUG_WAD, 0x31f00);
	meiLongwordWrite(MEI_DEBUG_DATA, 0x55);
	meiPollForDbgDone();
	return 1;
}

static int meiEnalbeMailboxInt(void)
{
	u32 arc2meiintmsk;
	meiLongwordRead(ARC_TO_MEI_INT_MASK, &arc2meiintmsk);
	arc2meiintmsk = arc2meiintmsk | 0x1;
	meiLongwordWrite(ARC_TO_MEI_INT_MASK, arc2meiintmsk);
	meiLongwordWrite(MEI_CONTROL, 0x0);
	return 1;
}



//000002:fchang End

static int mei_ioctl(struct inode * ino, struct file * fil, unsigned int command, unsigned long lon)
{
        int i,k;
	u32 boot_loop;
	u32 page_size;
	u32 dest_addr;
	u32 j;
	u32 temp;
	u32 temp2;
	u16 trapsflag=0;
	amazon_clreoc_pkt * current_clreoc;
	struct timeval time_now;
	struct timeval time_fini;
	struct list_head * ptr;
	amazon_mei_mib * mib_ptr;
//	u16 buff[MSG_LENGTH]__attribute__ ((aligned(4)));
	structpts pts;
        int meierr=MEI_SUCCESS;
	u16 data[12];  //used in makeCMV, to pass in payload when CMV set, ignored when CMV read.
	meireg regrdwr;
	meidebug debugrdwr;
	amazon_mei_mib * temp_intvl;
	struct sk_buff * eoc_skb;
// 603221:tc.chen start
	u16 hdlc_cmd[2];
	u16 hdlc_rx_buffer[32];
	int hdlc_rx_len=0;
// 603221:tc.chen end
	
	int from_kernel = 0;//joelin
	if (ino == (struct inode *)0) from_kernel = 1;//joelin
	
//	printk("\n switch.command = %i\n", command);
        switch(command){
		case GET_ADSL_LINE_CODE:
			pts.adslLineTableEntry_pt = (adslLineTableEntry *)kmalloc(sizeof(adslLineTableEntry), GFP_KERNEL);
			copy_from_user((char *)pts.adslLineTableEntry_pt, (char *)lon, sizeof(adslLineTableEntry));
			if(IS_FLAG_SET((&(pts.adslLineTableEntry_pt->flags)), LINE_CODE_FLAG)){
				pts.adslLineTableEntry_pt->adslLineCode = 2;
			}
			copy_to_user((char *)lon, (char *)pts.adslLineTableEntry_pt, sizeof(adslLineTableEntry));
			kfree(pts.adslLineTableEntry_pt);
			break;
#ifdef AMAZON_MEI_MIB_RFC3440
		case GET_ADSL_ATUC_LINE_EXT:
			if(down_interruptible(&mei_sema))
                		return -ERESTARTSYS;
			pts.adslLineExtTableEntry_pt = (adslLineExtTableEntry *)kmalloc(sizeof(adslLineExtTableEntry), GFP_KERNEL);
			copy_from_user((char *)pts.adslLineExtTableEntry_pt, (char *)lon, sizeof(adslLineExtTableEntry));
			if(IS_FLAG_SET((&(pts.adslLineExtTableEntry_pt->flags)), ATUC_LINE_TRANS_CAP_FLAG)){
				ATUC_LINE_TRANS_CAP_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 3 Address 67 Index 0");
#endif
					CLR_FLAG((&(pts.adslLineExtTableEntry_pt->flags)), ATUC_LINE_TRANS_CAP_FLAG);	
				}	
				else{
					memcpy((&(pts.adslLineExtTableEntry_pt->adslLineTransAtucCap)), RxMessage+4, ((RxMessage[0]&0xf)*2));
				}
			}
			if(IS_FLAG_SET((&(pts.adslLineExtTableEntry_pt->flags)), ATUC_LINE_TRANS_CONFIG_FLAG)){
				ATUC_LINE_TRANS_CONFIG_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 3 Address 67 Index 0");
#endif
					CLR_FLAG((&(pts.adslLineExtTableEntry_pt->flags)), ATUC_LINE_TRANS_CONFIG_FLAG);	
				}	
				else{
					memcpy((&(pts.adslLineExtTableEntry_pt->adslLineTransAtucConfig)), RxMessage+4, ((RxMessage[0]&0xf)*2));
				}
			}
			if(IS_FLAG_SET((&(pts.adslLineExtTableEntry_pt->flags)), ATUC_LINE_TRANS_ACTUAL_FLAG)){
				ATUC_LINE_TRANS_ACTUAL_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 2 Address 1 Index 0");
#endif
					CLR_FLAG((&(pts.adslLineExtTableEntry_pt->flags)), ATUC_LINE_TRANS_ACTUAL_FLAG);	
				}	
				else{
					memcpy((&(pts.adslLineExtTableEntry_pt->adslLineTransAtucActual)), RxMessage+4, ((RxMessage[0]&0xf)*2));
				}
			}
			if(IS_FLAG_SET((&(pts.adslLineExtTableEntry_pt->flags)), LINE_GLITE_POWER_STATE_FLAG)){    // not supported currently
/*
				LINE_GLITE_POWER_STATE_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 2 Address 0 Index 0");
#endif
					CLR_FLAG((&(pts.adslLineExtTableEntry_pt->flags)), LINE_GLITE_POWER_STATE_FLAG);	
				}	
				else{
					memcpy((&(pts.adslLineExtTableEntry_pt->adslLineGlitePowerState)), RxMessage+4, ((RxMessage[0]&0xf)*2));
				}
*/
				CLR_FLAG((&(pts.adslLineExtTableEntry_pt->flags)), LINE_GLITE_POWER_STATE_FLAG);
			}
			copy_to_user((char *)lon, (char *)pts.adslLineExtTableEntry_pt, sizeof(adslLineExtTableEntry));
			kfree(pts.adslLineTableEntry_pt);
			up(&mei_sema);
			break;
#endif

#ifdef AMAZON_MEI_MIB_RFC3440
		case SET_ADSL_ATUC_LINE_EXT:
			if(down_interruptible(&mei_sema))
                		return -ERESTARTSYS;
			pts.adslLineExtTableEntry_pt = (adslLineExtTableEntry *)kmalloc(sizeof(adslLineExtTableEntry), GFP_KERNEL);
			copy_from_user((char *)pts.adslLineExtTableEntry_pt, (char *)lon, sizeof(adslLineExtTableEntry));
			
			//only adslLineTransAtucConfig can be set.
			CLR_FLAG((&(pts.adslLineExtTableEntry_pt->flags)), ATUC_LINE_TRANS_CAP_FLAG);
			if(IS_FLAG_SET((&(pts.adslLineExtTableEntry_pt->flags)), ATUC_LINE_TRANS_CONFIG_FLAG)){
				memcpy(data,(&(pts.adslLineExtTableEntry_pt->adslLineTransAtucConfig)), 2); 
				ATUC_LINE_TRANS_CONFIG_FLAG_MAKECMV_WR;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 3 Address 67 Index 0");
#endif
					CLR_FLAG((&(pts.adslLineExtTableEntry_pt->flags)), ATUC_LINE_TRANS_CONFIG_FLAG);	
				}	
			}
			CLR_FLAG((&(pts.adslLineExtTableEntry_pt->flags)), ATUC_LINE_TRANS_ACTUAL_FLAG);
			CLR_FLAG((&(pts.adslLineExtTableEntry_pt->flags)), LINE_GLITE_POWER_STATE_FLAG);
	
			copy_to_user((char *)lon, (char *)pts.adslLineExtTableEntry_pt, sizeof(adslLineExtTableEntry));
			kfree(pts.adslLineTableEntry_pt);
			up(&mei_sema);
			break;
#endif

		case GET_ADSL_ATUC_PHY:
			if(down_interruptible(&mei_sema))
                		return -ERESTARTSYS;
			
			pts.adslAtucPhysEntry_pt = (adslAtucPhysEntry *)kmalloc(sizeof(adslAtucPhysEntry), GFP_KERNEL);
			copy_from_user((char *)pts.adslAtucPhysEntry_pt, (char *)lon, sizeof(adslAtucPhysEntry));
			if(IS_FLAG_SET((&(pts.adslAtucPhysEntry_pt->flags)), ATUC_PHY_SER_NUM_FLAG)){
				ATUC_PHY_SER_NUM_FLAG_MAKECMV1;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 3 Address 57 Index 0");
#endif
					CLR_FLAG((&(pts.adslAtucPhysEntry_pt->flags)), ATUC_PHY_SER_NUM_FLAG);	
				}
				else{
					memcpy(pts.adslAtucPhysEntry_pt->serial_no, RxMessage+4, ((RxMessage[0]&0xf)*2));
				}
				ATUC_PHY_SER_NUM_FLAG_MAKECMV2;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 3 Address 57 Index 12");
#endif
					CLR_FLAG((&(pts.adslAtucPhysEntry_pt->flags)), ATUC_PHY_SER_NUM_FLAG);	
				}
				else{
					memcpy((pts.adslAtucPhysEntry_pt->serial_no+24), RxMessage+4, ((RxMessage[0]&0xf)*2));
				} 
			}
			if(IS_FLAG_SET((&(pts.adslAtucPhysEntry_pt->flags)), ATUC_PHY_VENDOR_ID_FLAG)){
				ATUC_PHY_VENDOR_ID_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 3 Address 64 Index 0");
#endif
					CLR_FLAG((&(pts.adslAtucPhysEntry_pt->flags)), ATUC_PHY_VENDOR_ID_FLAG);	
				}	
				else{
					memcpy(pts.adslAtucPhysEntry_pt->vendor_id.vendor_id, RxMessage+4, ((RxMessage[0]&0xf)*2));
				}
			}
			if(IS_FLAG_SET((&(pts.adslAtucPhysEntry_pt->flags)), ATUC_PHY_VER_NUM_FLAG)){
				ATUC_PHY_VER_NUM_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 3 Address 58 Index 0");
#endif
					CLR_FLAG((&(pts.adslAtucPhysEntry_pt->flags)), ATUC_PHY_VER_NUM_FLAG);	
				}	
				else{
					memcpy(pts.adslAtucPhysEntry_pt->version_no, RxMessage+4, ((RxMessage[0]&0xf)*2));
				}
			}
			if(IS_FLAG_SET((&(pts.adslAtucPhysEntry_pt->flags)), ATUC_CURR_STAT_FLAG)){
				pts.adslAtucPhysEntry_pt->status = CurrStatus.adslAtucCurrStatus;
			}		
			if(IS_FLAG_SET((&(pts.adslAtucPhysEntry_pt->flags)), ATUC_CURR_OUT_PWR_FLAG)){
				ATUC_CURR_OUT_PWR_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 3 Address 68 Index 5");
#endif
					CLR_FLAG((&(pts.adslAtucPhysEntry_pt->flags)), ATUC_CURR_OUT_PWR_FLAG);	
				}	
				else{
					memcpy((&(pts.adslAtucPhysEntry_pt->outputPwr)), RxMessage+4, ((RxMessage[0]&0xf)*2));
				}
			}
			if(IS_FLAG_SET((&(pts.adslAtucPhysEntry_pt->flags)), ATUC_CURR_ATTR_FLAG)){
				ATUC_CURR_ATTR_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 3 Address 69 Index 0");
#endif
					CLR_FLAG((&(pts.adslAtucPhysEntry_pt->flags)), ATUC_CURR_ATTR_FLAG);	
				}	
				else{
					memcpy((&(pts.adslAtucPhysEntry_pt->attainableRate)), RxMessage+4, ((RxMessage[0]&0xf)*2));
				}
			}
			copy_to_user((char *)lon, (char *)pts.adslAtucPhysEntry_pt, sizeof(adslAtucPhysEntry));
			kfree(pts.adslAtucPhysEntry_pt);
			
			up(&mei_sema);
			break;
		case GET_ADSL_ATUR_PHY:
			if(down_interruptible(&mei_sema))
                		return -ERESTARTSYS;
			
			pts.adslAturPhysEntry_pt = (adslAturPhysEntry *)kmalloc(sizeof(adslAturPhysEntry), GFP_KERNEL);
			copy_from_user((char *)pts.adslAturPhysEntry_pt, (char *)lon, sizeof(adslAturPhysEntry));
			if(IS_FLAG_SET((&(pts.adslAturPhysEntry_pt->flags)), ATUR_PHY_SER_NUM_FLAG)){
				ATUR_PHY_SER_NUM_FLAG_MAKECMV1;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 3 Address 62 Index 0");
#endif
					CLR_FLAG((&(pts.adslAturPhysEntry_pt->flags)), ATUR_PHY_SER_NUM_FLAG);	
				}
				else{
					memcpy(pts.adslAturPhysEntry_pt->serial_no, RxMessage+4, ((RxMessage[0]&0xf)*2));
				}
				ATUR_PHY_SER_NUM_FLAG_MAKECMV2;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 3 Address 62 Index 12");
#endif
					CLR_FLAG((&(pts.adslAturPhysEntry_pt->flags)), ATUR_PHY_SER_NUM_FLAG);	
				}
				else{
					memcpy((pts.adslAturPhysEntry_pt->serial_no+24), RxMessage+4, ((RxMessage[0]&0xf)*2));
				} 
			}
			if(IS_FLAG_SET((&(pts.adslAturPhysEntry_pt->flags)), ATUR_PHY_VENDOR_ID_FLAG)){
				ATUR_PHY_VENDOR_ID_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 3 Address 65 Index 0");
#endif
					CLR_FLAG((&(pts.adslAturPhysEntry_pt->flags)), ATUR_PHY_VENDOR_ID_FLAG);	
				}	
				else{
					memcpy(pts.adslAturPhysEntry_pt->vendor_id.vendor_id, RxMessage+4, ((RxMessage[0]&0xf)*2));
				}
			}
			if(IS_FLAG_SET((&(pts.adslAturPhysEntry_pt->flags)), ATUR_PHY_VER_NUM_FLAG)){
				ATUR_PHY_VER_NUM_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 3 Address 61 Index 0");
#endif
					CLR_FLAG((&(pts.adslAturPhysEntry_pt->flags)), ATUR_PHY_VER_NUM_FLAG);	
				}	
				else{
					memcpy(pts.adslAturPhysEntry_pt->version_no, RxMessage+4, ((RxMessage[0]&0xf)*2));
				}
			}
			if(IS_FLAG_SET((&(pts.adslAturPhysEntry_pt->flags)), ATUR_SNRMGN_FLAG)){
				ATUR_SNRMGN_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 3 Address 68 Index 4");
#endif
					CLR_FLAG((&(pts.adslAturPhysEntry_pt->flags)), ATUR_SNRMGN_FLAG);	
				}	
				else{
					memcpy((&(pts.adslAturPhysEntry_pt->SnrMgn)), RxMessage+4, ((RxMessage[0]&0xf)*2));
				}
			}
			if(IS_FLAG_SET((&(pts.adslAturPhysEntry_pt->flags)), ATUR_ATTN_FLAG)){
				ATUR_ATTN_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 3 Address 68 Index 2");
#endif
					CLR_FLAG((&(pts.adslAturPhysEntry_pt->flags)), ATUR_ATTN_FLAG);	
				}	
				else{
					memcpy((&(pts.adslAturPhysEntry_pt->Attn)), RxMessage+4, ((RxMessage[0]&0xf)*2));
				}
			}
			if(IS_FLAG_SET((&(pts.adslAturPhysEntry_pt->flags)), ATUR_CURR_STAT_FLAG)){
				pts.adslAturPhysEntry_pt->status = CurrStatus.adslAturCurrStatus;
			}
			if(IS_FLAG_SET((&(pts.adslAturPhysEntry_pt->flags)), ATUR_CURR_OUT_PWR_FLAG)){
				ATUR_CURR_OUT_PWR_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 3 Address 69 Index 5");
#endif
					CLR_FLAG((&(pts.adslAturPhysEntry_pt->flags)), ATUR_CURR_OUT_PWR_FLAG);	
				}	
				else{
					memcpy((&(pts.adslAturPhysEntry_pt->outputPwr)), RxMessage+4, ((RxMessage[0]&0xf)*2));
				}
			}
			if(IS_FLAG_SET((&(pts.adslAturPhysEntry_pt->flags)), ATUR_CURR_ATTR_FLAG)){
				ATUR_CURR_ATTR_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 3 Address 68 Index 0");
#endif
					CLR_FLAG((&(pts.adslAturPhysEntry_pt->flags)), ATUR_CURR_ATTR_FLAG);	
				}	
				else{
					memcpy((&(pts.adslAturPhysEntry_pt->attainableRate)), RxMessage+4, ((RxMessage[0]&0xf)*2));
				}
			}
			copy_to_user((char *)lon, (char *)pts.adslAturPhysEntry_pt, sizeof(adslAturPhysEntry));
			kfree(pts.adslAturPhysEntry_pt);
			
			up(&mei_sema);
			break;
		case GET_ADSL_ATUC_CHAN_INFO:
			if(down_interruptible(&mei_sema))
                		return -ERESTARTSYS;
			
			pts.adslAtucChanInfo_pt = (adslAtucChanInfo *)kmalloc(sizeof(adslAtucChanInfo), GFP_KERNEL);
			copy_from_user((char *)pts.adslAtucChanInfo_pt, (char *)lon, sizeof(adslAtucChanInfo));
			if(IS_FLAG_SET((&(pts.adslAtucChanInfo_pt->flags)), ATUC_CHAN_INTLV_DELAY_FLAG)){
				if((chantype.interleave!=1) || (chantype.fast==1)){
					CLR_FLAG((&(pts.adslAtucChanInfo_pt->flags)), ATUC_CHAN_INTLV_DELAY_FLAG);
				}
				else{
					ATUC_CHAN_INTLV_DELAY_FLAG_MAKECMV;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group 6 Address 3 Index 1");
#endif
						CLR_FLAG((&(pts.adslAtucChanInfo_pt->flags)), ATUC_CHAN_INTLV_DELAY_FLAG);	
					}	
					else{
						memcpy((&(pts.adslAtucChanInfo_pt->interleaveDelay)), RxMessage+4, ((RxMessage[0]&0xf)*2));
					}
				}
			}
			if(IS_FLAG_SET((&(pts.adslAtucChanInfo_pt->flags)), ATUC_CHAN_CURR_TX_RATE_FLAG)){
				ATUC_CHAN_CURR_TX_RATE_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 6 Address 1 Index 0");
#endif
					CLR_FLAG((&(pts.adslAtucChanInfo_pt->flags)), ATUC_CHAN_CURR_TX_RATE_FLAG);	
				}	
				else{
					pts.adslAtucChanInfo_pt->currTxRate = (u32)(RxMessage[4]) + (((u32)(RxMessage[5]))<<16);
				}
			}
			if(IS_FLAG_SET((&(pts.adslAtucChanInfo_pt->flags)), ATUC_CHAN_PREV_TX_RATE_FLAG)){
				pts.adslAtucChanInfo_pt->prevTxRate = PrevTxRate.adslAtucChanPrevTxRate;
			}
			copy_to_user((char *)lon, (char *)pts.adslAtucChanInfo_pt, sizeof(adslAtucChanInfo));
			kfree(pts.adslAtucChanInfo_pt);
			
			up(&mei_sema);
			break;
		case GET_ADSL_ATUR_CHAN_INFO:
			if(down_interruptible(&mei_sema))
                		return -ERESTARTSYS;
			
			pts.adslAturChanInfo_pt = (adslAturChanInfo *)kmalloc(sizeof(adslAturChanInfo), GFP_KERNEL);
			copy_from_user((char *)pts.adslAturChanInfo_pt, (char *)lon, sizeof(adslAturChanInfo));
			if(IS_FLAG_SET((&(pts.adslAturChanInfo_pt->flags)), ATUR_CHAN_INTLV_DELAY_FLAG)){
				if((chantype.interleave!=1) || (chantype.fast==1)){
					CLR_FLAG((&(pts.adslAturChanInfo_pt->flags)), ATUR_CHAN_INTLV_DELAY_FLAG);
				}
				else{
					ATUR_CHAN_INTLV_DELAY_FLAG_MAKECMV;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group 6 Address 2 Index 1");
#endif
						CLR_FLAG((&(pts.adslAturChanInfo_pt->flags)), ATUR_CHAN_INTLV_DELAY_FLAG);	
					}	
					else{
						memcpy((&(pts.adslAturChanInfo_pt->interleaveDelay)), RxMessage+4, ((RxMessage[0]&0xf)*2));
					}
				}
			}
			if(IS_FLAG_SET((&(pts.adslAturChanInfo_pt->flags)), ATUR_CHAN_CURR_TX_RATE_FLAG)){
				ATUR_CHAN_CURR_TX_RATE_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 6 Address 0 Index 0");
#endif
					CLR_FLAG((&(pts.adslAturChanInfo_pt->flags)), ATUR_CHAN_CURR_TX_RATE_FLAG);	
				}	
				else{
					pts.adslAturChanInfo_pt->currTxRate = (u32)(RxMessage[4]) + (((u32)(RxMessage[5]))<<16);
				}
			}
			if(IS_FLAG_SET((&(pts.adslAturChanInfo_pt->flags)), ATUR_CHAN_PREV_TX_RATE_FLAG)){
				pts.adslAturChanInfo_pt->prevTxRate = PrevTxRate.adslAturChanPrevTxRate;
			}
			if(IS_FLAG_SET((&(pts.adslAturChanInfo_pt->flags)), ATUR_CHAN_CRC_BLK_LEN_FLAG)){
				// ? no CMV to update this 
				CLR_FLAG((&(pts.adslAturChanInfo_pt->flags)), ATUR_CHAN_CRC_BLK_LEN_FLAG);
			}
			copy_to_user((char *)lon, (char *)pts.adslAturChanInfo_pt, sizeof(adslAturChanInfo));
			kfree(pts.adslAturChanInfo_pt);
			
			up(&mei_sema);
			break;
		case GET_ADSL_ATUC_PERF_DATA:
			pts.atucPerfDataEntry_pt = (atucPerfDataEntry *)kmalloc(sizeof(atucPerfDataEntry), GFP_KERNEL);
			copy_from_user((char *)pts.atucPerfDataEntry_pt, (char *)lon, sizeof(atucPerfDataEntry));
			if(IS_FLAG_SET((&(pts.atucPerfDataEntry_pt->flags)), ATUC_PERF_LOFS_FLAG)){
				pts.atucPerfDataEntry_pt->adslAtucPerfLofs=ATUC_PERF_LOFS;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataEntry_pt->flags)), ATUC_PERF_LOSS_FLAG)){
				pts.atucPerfDataEntry_pt->adslAtucPerfLoss=ATUC_PERF_LOSS;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataEntry_pt->flags)), ATUC_PERF_ESS_FLAG)){
				pts.atucPerfDataEntry_pt->adslAtucPerfESs=ATUC_PERF_ESS;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataEntry_pt->flags)), ATUC_PERF_INITS_FLAG)){
				pts.atucPerfDataEntry_pt->adslAtucPerfInits=ATUC_PERF_INITS;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataEntry_pt->flags)), ATUC_PERF_VALID_INTVLS_FLAG)){
				i=0;
				for(ptr=(current_intvl->list).prev; ptr!=&interval_list; ptr=ptr->prev){
					i++;
					if(i==96)
						break;
				}
				pts.atucPerfDataEntry_pt->adslAtucPerfValidIntervals=i;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataEntry_pt->flags)), ATUC_PERF_INVALID_INTVLS_FLAG)){
				pts.atucPerfDataEntry_pt->adslAtucPerfInvalidIntervals=0;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataEntry_pt->flags)), ATUC_PERF_CURR_15MIN_TIME_ELAPSED_FLAG)){
				do_gettimeofday(&time_now);
				pts.atucPerfDataEntry_pt->adslAtucPerfCurr15MinTimeElapsed=time_now.tv_sec - (current_intvl->start_time).tv_sec;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataEntry_pt->flags)), ATUC_PERF_CURR_15MIN_LOFS_FLAG)){
				pts.atucPerfDataEntry_pt->adslAtucPerfCurr15MinLofs=current_intvl->AtucPerfLof;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataEntry_pt->flags)), ATUC_PERF_CURR_15MIN_LOSS_FLAG)){
				pts.atucPerfDataEntry_pt->adslAtucPerfCurr15MinLoss=current_intvl->AtucPerfLos;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataEntry_pt->flags)), ATUC_PERF_CURR_15MIN_ESS_FLAG)){
				pts.atucPerfDataEntry_pt->adslAtucPerfCurr15MinESs=current_intvl->AtucPerfEs;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataEntry_pt->flags)), ATUC_PERF_CURR_15MIN_INIT_FLAG)){
				pts.atucPerfDataEntry_pt->adslAtucPerfCurr15MinInits=current_intvl->AtucPerfInit;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataEntry_pt->flags)), ATUC_PERF_CURR_1DAY_TIME_ELAPSED_FLAG)){
				i=0;		
				for(ptr=(current_intvl->list).prev; ptr!=&interval_list; ptr=ptr->prev){
					i+=900;	
				}
				do_gettimeofday(&time_now);
				i+=time_now.tv_sec - (current_intvl->start_time).tv_sec;
				if(i>=86400)
					pts.atucPerfDataEntry_pt->adslAtucPerfCurr1DayTimeElapsed=i-86400;
				else
					pts.atucPerfDataEntry_pt->adslAtucPerfCurr1DayTimeElapsed=i;		
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataEntry_pt->flags)), ATUC_PERF_CURR_1DAY_LOFS_FLAG)){		
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AtucPerfLof;
					i++;	
					if(i==96)
						j=0;
				}
				j+=current_intvl->AtucPerfLof;
				pts.atucPerfDataEntry_pt->adslAtucPerfCurr1DayLofs=j;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataEntry_pt->flags)), ATUC_PERF_CURR_1DAY_LOSS_FLAG)){		
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AtucPerfLos;
					i++;	
					if(i==96)
						j=0;
				}
				j+=current_intvl->AtucPerfLos;
				pts.atucPerfDataEntry_pt->adslAtucPerfCurr1DayLoss=j;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataEntry_pt->flags)), ATUC_PERF_CURR_1DAY_ESS_FLAG)){		
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AtucPerfEs;
					i++;	
					if(i==96)
						j=0;
				}
				j+=current_intvl->AtucPerfEs;
				pts.atucPerfDataEntry_pt->adslAtucPerfCurr1DayESs=j;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataEntry_pt->flags)), ATUC_PERF_CURR_1DAY_INIT_FLAG)){		
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AtucPerfInit;
					i++;	
					if(i==96)
						j=0;
				}
				j+=current_intvl->AtucPerfInit;
				pts.atucPerfDataEntry_pt->adslAtucPerfCurr1DayInits=j;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataEntry_pt->flags)), ATUC_PERF_PREV_1DAY_MON_SEC_FLAG)){		
				i=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					i++;		
				}
				if(i>=96)
					pts.atucPerfDataEntry_pt->adslAtucPerfPrev1DayMoniSecs=86400;
				else
					pts.atucPerfDataEntry_pt->adslAtucPerfPrev1DayMoniSecs=0;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataEntry_pt->flags)), ATUC_PERF_PREV_1DAY_LOFS_FLAG)){
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AtucPerfLof;
					i++;
					if(i==96)
						break;
				}	
				if(i==96)
					pts.atucPerfDataEntry_pt->adslAtucPerfPrev1DayLofs=j;
				else
					pts.atucPerfDataEntry_pt->adslAtucPerfPrev1DayLofs=0;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataEntry_pt->flags)), ATUC_PERF_PREV_1DAY_LOSS_FLAG)){
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AtucPerfLos;
					i++;
					if(i==96)
						break;
				}	
				if(i==96)
					pts.atucPerfDataEntry_pt->adslAtucPerfPrev1DayLoss=j;
				else
					pts.atucPerfDataEntry_pt->adslAtucPerfPrev1DayLoss=0;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataEntry_pt->flags)), ATUC_PERF_PREV_1DAY_ESS_FLAG)){
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AtucPerfEs;
					i++;
					if(i==96)
						break;
				}	
				if(i==96)
					pts.atucPerfDataEntry_pt->adslAtucPerfPrev1DayESs=j;
				else
					pts.atucPerfDataEntry_pt->adslAtucPerfPrev1DayESs=0;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataEntry_pt->flags)), ATUC_PERF_PREV_1DAY_INITS_FLAG)){
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AtucPerfInit;
					i++;
					if(i==96)
						break;
				}	
				if(i==96)
					pts.atucPerfDataEntry_pt->adslAtucPerfPrev1DayInits=j;
				else
					pts.atucPerfDataEntry_pt->adslAtucPerfPrev1DayInits=0;
			}
			
			copy_to_user((char *)lon, (char *)pts.atucPerfDataEntry_pt, sizeof(atucPerfDataEntry));
			kfree(pts.atucPerfDataEntry_pt);
			break;
#ifdef AMAZON_MEI_MIB_RFC3440
		case GET_ADSL_ATUC_PERF_DATA_EXT:	//??? CMV mapping not available
			pts.atucPerfDataExtEntry_pt = (atucPerfDataExtEntry *)kmalloc(sizeof(atucPerfDataExtEntry), GFP_KERNEL);
			copy_from_user((char *)pts.atucPerfDataExtEntry_pt, (char *)lon, sizeof(atucPerfDataExtEntry));
			if(IS_FLAG_SET((&(pts.atucPerfDataExtEntry_pt->flags)), ATUC_PERF_STAT_FASTR_FLAG)){
				pts.atucPerfDataExtEntry_pt->adslAtucPerfStatFastR=ATUC_PERF_STAT_FASTR;
			}	
			if(IS_FLAG_SET((&(pts.atucPerfDataExtEntry_pt->flags)), ATUC_PERF_STAT_FAILED_FASTR_FLAG)){
				pts.atucPerfDataExtEntry_pt->adslAtucPerfStatFailedFastR=ATUC_PERF_STAT_FAILED_FASTR;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataExtEntry_pt->flags)), ATUC_PERF_STAT_SESL_FLAG)){
				pts.atucPerfDataExtEntry_pt->adslAtucPerfStatSesL=ATUC_PERF_STAT_SESL;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataExtEntry_pt->flags)), ATUC_PERF_STAT_UASL_FLAG)){
				pts.atucPerfDataExtEntry_pt->adslAtucPerfStatUasL=ATUC_PERF_STAT_UASL;
			}	
			if(IS_FLAG_SET((&(pts.atucPerfDataExtEntry_pt->flags)), ATUC_PERF_CURR_15MIN_FASTR_FLAG)){
				pts.atucPerfDataExtEntry_pt->adslAtucPerfCurr15MinFastR=current_intvl->AtucPerfStatFastR;
			}	
			if(IS_FLAG_SET((&(pts.atucPerfDataExtEntry_pt->flags)), ATUC_PERF_CURR_15MIN_FAILED_FASTR_FLAG)){
				pts.atucPerfDataExtEntry_pt->adslAtucPerfCurr15MinFailedFastR=current_intvl->AtucPerfStatFailedFastR;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataExtEntry_pt->flags)), ATUC_PERF_CURR_15MIN_SESL_FLAG)){
				pts.atucPerfDataExtEntry_pt->adslAtucPerfCurr15MinSesL=current_intvl->AtucPerfStatSesL;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataExtEntry_pt->flags)), ATUC_PERF_CURR_15MIN_UASL_FLAG)){
				pts.atucPerfDataExtEntry_pt->adslAtucPerfCurr15MinUasL=current_intvl->AtucPerfStatUasL;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataExtEntry_pt->flags)), ATUC_PERF_CURR_1DAY_FASTR_FLAG)){
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AtucPerfStatFastR;
					i++;	
					if(i==96)
						j=0;
				}
				j+=current_intvl->AtucPerfStatFastR;
				pts.atucPerfDataExtEntry_pt->adslAtucPerfCurr1DayFastR=j;
			}	
			if(IS_FLAG_SET((&(pts.atucPerfDataExtEntry_pt->flags)), ATUC_PERF_CURR_1DAY_FAILED_FASTR_FLAG)){
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AtucPerfStatFailedFastR;
					i++;	
					if(i==96)
						j=0;
				}
				j+=current_intvl->AtucPerfStatFailedFastR;
				pts.atucPerfDataExtEntry_pt->adslAtucPerfCurr1DayFailedFastR=j;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataExtEntry_pt->flags)), ATUC_PERF_CURR_1DAY_SESL_FLAG)){
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AtucPerfStatSesL;
					i++;	
					if(i==96)
						j=0;
				}
				j+=current_intvl->AtucPerfStatSesL;
				pts.atucPerfDataExtEntry_pt->adslAtucPerfCurr1DaySesL=j;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataExtEntry_pt->flags)), ATUC_PERF_CURR_1DAY_UASL_FLAG)){
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AtucPerfStatUasL;
					i++;	
					if(i==96)
						j=0;
				}
				j+=current_intvl->AtucPerfStatUasL;
				pts.atucPerfDataExtEntry_pt->adslAtucPerfCurr1DayUasL=j;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataExtEntry_pt->flags)), ATUC_PERF_PREV_1DAY_FASTR_FLAG)){
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AtucPerfStatFastR;
					i++;
					if(i==96)
						break;
				}	
				if(i==96)
					pts.atucPerfDataExtEntry_pt->adslAtucPerfPrev1DayFastR=j;
				else
					pts.atucPerfDataExtEntry_pt->adslAtucPerfPrev1DayFastR=0;
			}	
			if(IS_FLAG_SET((&(pts.atucPerfDataExtEntry_pt->flags)), ATUC_PERF_PREV_1DAY_FAILED_FASTR_FLAG)){
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AtucPerfStatFailedFastR;
					i++;
					if(i==96)
						break;
				}	
				if(i==96)
					pts.atucPerfDataExtEntry_pt->adslAtucPerfPrev1DayFailedFastR=j;
				else
					pts.atucPerfDataExtEntry_pt->adslAtucPerfPrev1DayFailedFastR=0;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataExtEntry_pt->flags)), ATUC_PERF_PREV_1DAY_SESL_FLAG)){
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AtucPerfStatSesL;
					i++;
					if(i==96)
						break;
				}	
				if(i==96)
					pts.atucPerfDataExtEntry_pt->adslAtucPerfPrev1DaySesL=j;
				else
					pts.atucPerfDataExtEntry_pt->adslAtucPerfPrev1DaySesL=0;
			}
			if(IS_FLAG_SET((&(pts.atucPerfDataExtEntry_pt->flags)), ATUC_PERF_PREV_1DAY_UASL_FLAG)){
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AtucPerfStatUasL;
					i++;
					if(i==96)
						break;
				}	
				if(i==96)
					pts.atucPerfDataExtEntry_pt->adslAtucPerfPrev1DayUasL=j;
				else
					pts.atucPerfDataExtEntry_pt->adslAtucPerfPrev1DayUasL=0;
			}
			copy_to_user((char *)lon, (char *)pts.atucPerfDataExtEntry_pt, sizeof(atucPerfDataExtEntry));
			kfree(pts.atucPerfDataExtEntry_pt);
			break;
#endif
		case GET_ADSL_ATUR_PERF_DATA:
			pts.aturPerfDataEntry_pt = (aturPerfDataEntry *)kmalloc(sizeof(aturPerfDataEntry), GFP_KERNEL);
			copy_from_user((char *)pts.aturPerfDataEntry_pt, (char *)lon, sizeof(aturPerfDataEntry));
			if(IS_FLAG_SET((&(pts.aturPerfDataEntry_pt->flags)), ATUR_PERF_LOFS_FLAG)){
				pts.aturPerfDataEntry_pt->adslAturPerfLofs=ATUR_PERF_LOFS;
			}
			if(IS_FLAG_SET((&(pts.aturPerfDataEntry_pt->flags)), ATUR_PERF_LOSS_FLAG)){
				pts.aturPerfDataEntry_pt->adslAturPerfLoss=ATUR_PERF_LOSS;
			}
			if(IS_FLAG_SET((&(pts.aturPerfDataEntry_pt->flags)), ATUR_PERF_LPR_FLAG)){
				pts.aturPerfDataEntry_pt->adslAturPerfLprs=ATUR_PERF_LPR;
			}
			if(IS_FLAG_SET((&(pts.aturPerfDataEntry_pt->flags)), ATUR_PERF_ESS_FLAG)){
				pts.aturPerfDataEntry_pt->adslAturPerfESs=ATUR_PERF_ESS;
			}
			if(IS_FLAG_SET((&(pts.aturPerfDataEntry_pt->flags)), ATUR_PERF_VALID_INTVLS_FLAG)){
				i=0;
				for(ptr=(current_intvl->list).prev; ptr!=&interval_list; ptr=ptr->prev){
					i++;
					if(i==96)
						break;
				}
				pts.aturPerfDataEntry_pt->adslAturPerfValidIntervals=i;
			}
			if(IS_FLAG_SET((&(pts.aturPerfDataEntry_pt->flags)), ATUR_PERF_INVALID_INTVLS_FLAG)){
				pts.aturPerfDataEntry_pt->adslAturPerfInvalidIntervals=0;
			}
			if(IS_FLAG_SET((&(pts.aturPerfDataEntry_pt->flags)), ATUR_PERF_CURR_15MIN_TIME_ELAPSED_FLAG)){
				do_gettimeofday(&time_now);
				pts.aturPerfDataEntry_pt->adslAturPerfCurr15MinTimeElapsed=time_now.tv_sec - (current_intvl->start_time).tv_sec;
			}
			if(IS_FLAG_SET((&(pts.aturPerfDataEntry_pt->flags)), ATUR_PERF_CURR_15MIN_LOFS_FLAG)){
				pts.aturPerfDataEntry_pt->adslAturPerfCurr15MinLofs=current_intvl->AturPerfLof;
			}
			if(IS_FLAG_SET((&(pts.aturPerfDataEntry_pt->flags)), ATUR_PERF_CURR_15MIN_LOSS_FLAG)){
				pts.aturPerfDataEntry_pt->adslAturPerfCurr15MinLoss=current_intvl->AturPerfLos;
			}
			if(IS_FLAG_SET((&(pts.aturPerfDataEntry_pt->flags)), ATUR_PERF_CURR_15MIN_LPR_FLAG)){
				pts.aturPerfDataEntry_pt->adslAturPerfCurr15MinLprs=current_intvl->AturPerfLpr;
			}
			if(IS_FLAG_SET((&(pts.aturPerfDataEntry_pt->flags)), ATUR_PERF_CURR_15MIN_ESS_FLAG)){
				pts.aturPerfDataEntry_pt->adslAturPerfCurr15MinESs=current_intvl->AturPerfEs;
			}
			if(IS_FLAG_SET((&(pts.aturPerfDataEntry_pt->flags)), ATUR_PERF_CURR_1DAY_TIME_ELAPSED_FLAG)){
				i=0;		
				for(ptr=(current_intvl->list).prev; ptr!=&interval_list; ptr=ptr->prev){
					i+=900;	
				}
				do_gettimeofday(&time_now);
				i+=time_now.tv_sec - (current_intvl->start_time).tv_sec;
				if(i>=86400)
					pts.aturPerfDataEntry_pt->adslAturPerfCurr1DayTimeElapsed=i-86400;
				else
					pts.aturPerfDataEntry_pt->adslAturPerfCurr1DayTimeElapsed=i;		
			}
			if(IS_FLAG_SET((&(pts.aturPerfDataEntry_pt->flags)), ATUR_PERF_CURR_1DAY_LOFS_FLAG)){		
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AturPerfLof;
					i++;	
					if(i==96)
						j=0;
				}
				j+=current_intvl->AturPerfLof;
				pts.aturPerfDataEntry_pt->adslAturPerfCurr1DayLofs=j;
			}
			if(IS_FLAG_SET((&(pts.aturPerfDataEntry_pt->flags)), ATUR_PERF_CURR_1DAY_LOSS_FLAG)){		
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AturPerfLos;
					i++;	
					if(i==96)
						j=0;
				}
				j+=current_intvl->AturPerfLos;
				pts.aturPerfDataEntry_pt->adslAturPerfCurr1DayLoss=j;
			}
			if(IS_FLAG_SET((&(pts.aturPerfDataEntry_pt->flags)), ATUR_PERF_CURR_1DAY_LPR_FLAG)){		
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AturPerfLpr;
					i++;	
					if(i==96)
						j=0;
				}
				j+=current_intvl->AturPerfLpr;
				pts.aturPerfDataEntry_pt->adslAturPerfCurr1DayLprs=j;
			}
			if(IS_FLAG_SET((&(pts.aturPerfDataEntry_pt->flags)), ATUR_PERF_CURR_1DAY_ESS_FLAG)){		
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AturPerfEs;
					i++;	
					if(i==96)
						j=0;
				}
				j+=current_intvl->AturPerfEs;
				pts.aturPerfDataEntry_pt->adslAturPerfCurr1DayESs=j;
			}
			if(IS_FLAG_SET((&(pts.aturPerfDataEntry_pt->flags)), ATUR_PERF_PREV_1DAY_MON_SEC_FLAG)){		
				i=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					i++;		
				}
				if(i>=96)
					pts.aturPerfDataEntry_pt->adslAturPerfPrev1DayMoniSecs=86400;
				else
					pts.aturPerfDataEntry_pt->adslAturPerfPrev1DayMoniSecs=0;
			}
			if(IS_FLAG_SET((&(pts.aturPerfDataEntry_pt->flags)), ATUR_PERF_PREV_1DAY_LOFS_FLAG)){
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AturPerfLof;
					i++;
					if(i==96)
						break;
				}	
				if(i==96)
					pts.aturPerfDataEntry_pt->adslAturPerfPrev1DayLofs=j;
				else
					pts.aturPerfDataEntry_pt->adslAturPerfPrev1DayLofs=0;
			}
			if(IS_FLAG_SET((&(pts.aturPerfDataEntry_pt->flags)), ATUR_PERF_PREV_1DAY_LOSS_FLAG)){
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AturPerfLos;
					i++;
					if(i==96)
						break;
				}	
				if(i==96)
					pts.aturPerfDataEntry_pt->adslAturPerfPrev1DayLoss=j;
				else
					pts.aturPerfDataEntry_pt->adslAturPerfPrev1DayLoss=0;
			}
			if(IS_FLAG_SET((&(pts.aturPerfDataEntry_pt->flags)), ATUR_PERF_PREV_1DAY_LPR_FLAG)){
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AturPerfLpr;
					i++;
					if(i==96)
						break;
				}	
				if(i==96)
					pts.aturPerfDataEntry_pt->adslAturPerfPrev1DayLprs=j;
				else
					pts.aturPerfDataEntry_pt->adslAturPerfPrev1DayLprs=0;
			}
			if(IS_FLAG_SET((&(pts.aturPerfDataEntry_pt->flags)), ATUR_PERF_PREV_1DAY_ESS_FLAG)){
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AturPerfEs;
					i++;
					if(i==96)
						break;
				}	
				if(i==96)
					pts.aturPerfDataEntry_pt->adslAturPerfPrev1DayESs=j;
				else
					pts.aturPerfDataEntry_pt->adslAturPerfPrev1DayESs=0;
			}		
			
			copy_to_user((char *)lon, (char *)pts.aturPerfDataEntry_pt, sizeof(aturPerfDataEntry));
			kfree(pts.aturPerfDataEntry_pt);
			break;
#ifdef AMAZON_MEI_MIB_RFC3440
		case GET_ADSL_ATUR_PERF_DATA_EXT:
			pts.aturPerfDataExtEntry_pt = (aturPerfDataExtEntry *)kmalloc(sizeof(aturPerfDataExtEntry), GFP_KERNEL);
			copy_from_user((char *)pts.aturPerfDataExtEntry_pt, (char *)lon, sizeof(aturPerfDataExtEntry));
			if(IS_FLAG_SET((&(pts.aturPerfDataExtEntry_pt->flags)), ATUR_PERF_STAT_SESL_FLAG)){
				pts.aturPerfDataExtEntry_pt->adslAturPerfStatSesL=ATUR_PERF_STAT_SESL;
			}	
			if(IS_FLAG_SET((&(pts.aturPerfDataExtEntry_pt->flags)), ATUR_PERF_STAT_UASL_FLAG)){
				pts.aturPerfDataExtEntry_pt->adslAturPerfStatUasL=ATUR_PERF_STAT_UASL;
			}
			if(IS_FLAG_SET((&(pts.aturPerfDataExtEntry_pt->flags)), ATUR_PERF_CURR_15MIN_SESL_FLAG)){
				pts.aturPerfDataExtEntry_pt->adslAturPerfCurr15MinSesL=current_intvl->AturPerfStatSesL;
			}	
			if(IS_FLAG_SET((&(pts.aturPerfDataExtEntry_pt->flags)), ATUR_PERF_CURR_15MIN_UASL_FLAG)){
				pts.aturPerfDataExtEntry_pt->adslAturPerfCurr15MinUasL=current_intvl->AturPerfStatUasL;
			}
			if(IS_FLAG_SET((&(pts.aturPerfDataExtEntry_pt->flags)), ATUR_PERF_CURR_1DAY_SESL_FLAG)){
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AturPerfStatSesL;
					i++;	
					if(i==96)
						j=0;
				}
				j+=current_intvl->AturPerfStatSesL;
				pts.aturPerfDataExtEntry_pt->adslAturPerfCurr1DaySesL=j;	
			}	
			if(IS_FLAG_SET((&(pts.aturPerfDataExtEntry_pt->flags)), ATUR_PERF_CURR_1DAY_UASL_FLAG)){
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AturPerfStatUasL;
					i++;	
					if(i==96)
						j=0;
				}
				j+=current_intvl->AturPerfStatUasL;
				pts.aturPerfDataExtEntry_pt->adslAturPerfCurr1DayUasL=j;
			}
			if(IS_FLAG_SET((&(pts.aturPerfDataExtEntry_pt->flags)), ATUR_PERF_PREV_1DAY_SESL_FLAG)){
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AturPerfStatSesL;
					i++;
					if(i==96)
						break;
				}	
				if(i==96)
					pts.aturPerfDataExtEntry_pt->adslAturPerfPrev1DaySesL=j;
				else
					pts.aturPerfDataExtEntry_pt->adslAturPerfPrev1DaySesL=0;	
			}	
			if(IS_FLAG_SET((&(pts.aturPerfDataExtEntry_pt->flags)), ATUR_PERF_PREV_1DAY_UASL_FLAG)){
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AturPerfStatUasL;
					i++;
					if(i==96)
						break;
				}	
				if(i==96)
					pts.aturPerfDataExtEntry_pt->adslAturPerfPrev1DayUasL=j;
				else
					pts.aturPerfDataExtEntry_pt->adslAturPerfPrev1DayUasL=0;
			}
			copy_to_user((char *)lon, (char *)pts.aturPerfDataExtEntry_pt, sizeof(aturPerfDataExtEntry));
			kfree(pts.aturPerfDataExtEntry_pt);
			break;
#endif
		case GET_ADSL_ATUC_INTVL_INFO:
			pts.adslAtucIntvlInfo_pt = (adslAtucIntvlInfo *)kmalloc(sizeof(adslAtucIntvlInfo), GFP_KERNEL);
			copy_from_user((char *)pts.adslAtucIntvlInfo_pt, (char *)lon, sizeof(adslAtucIntvlInfo));
			
			if(pts.adslAtucIntvlInfo_pt->IntervalNumber <1){
				pts.adslAtucIntvlInfo_pt->intervalLOF = ATUC_PERF_LOFS;
				pts.adslAtucIntvlInfo_pt->intervalLOS = ATUC_PERF_LOSS;
				pts.adslAtucIntvlInfo_pt->intervalES = ATUC_PERF_ESS;
				pts.adslAtucIntvlInfo_pt->intervalInits = ATUC_PERF_INITS;
				pts.adslAtucIntvlInfo_pt->intervalValidData = 1;	
			}
			else{
				i=0;
				for(ptr=(current_intvl->list).prev; ptr!=&interval_list; ptr=ptr->prev){
					i++;
					if(i==pts.adslAtucIntvlInfo_pt->IntervalNumber){
						temp_intvl = list_entry(ptr, amazon_mei_mib, list);
						pts.adslAtucIntvlInfo_pt->intervalLOF = temp_intvl->AtucPerfLof;
						pts.adslAtucIntvlInfo_pt->intervalLOS = temp_intvl->AtucPerfLos;
						pts.adslAtucIntvlInfo_pt->intervalES = temp_intvl->AtucPerfEs;
						pts.adslAtucIntvlInfo_pt->intervalInits = temp_intvl->AtucPerfInit;
						pts.adslAtucIntvlInfo_pt->intervalValidData = 1;
						break;
					}		
				}
				if(ptr==&interval_list){
					pts.adslAtucIntvlInfo_pt->intervalValidData = 0;
					pts.adslAtucIntvlInfo_pt->flags = 0;
					pts.adslAtucIntvlInfo_pt->intervalLOF = 0;
					pts.adslAtucIntvlInfo_pt->intervalLOS = 0;
					pts.adslAtucIntvlInfo_pt->intervalES = 0;
					pts.adslAtucIntvlInfo_pt->intervalInits = 0;	
				}
			}
									
			copy_to_user((char *)lon, (char *)pts.adslAtucIntvlInfo_pt, sizeof(adslAtucIntvlInfo));
			kfree(pts.adslAtucIntvlInfo_pt);
			break;
#ifdef AMAZON_MEI_MIB_RFC3440
		case GET_ADSL_ATUC_INTVL_EXT_INFO:
			pts.adslAtucInvtlExtInfo_pt = (adslAtucInvtlExtInfo *)kmalloc(sizeof(adslAtucInvtlExtInfo), GFP_KERNEL);
			copy_from_user((char *)pts.adslAtucInvtlExtInfo_pt, (char *)lon, sizeof(adslAtucInvtlExtInfo));
			if(pts.adslAtucInvtlExtInfo_pt->IntervalNumber <1){
				pts.adslAtucInvtlExtInfo_pt->adslAtucIntervalFastR = ATUC_PERF_STAT_FASTR;
				pts.adslAtucInvtlExtInfo_pt->adslAtucIntervalFailedFastR = ATUC_PERF_STAT_FAILED_FASTR;
				pts.adslAtucInvtlExtInfo_pt->adslAtucIntervalSesL = ATUC_PERF_STAT_SESL;
				pts.adslAtucInvtlExtInfo_pt->adslAtucIntervalUasL = ATUC_PERF_STAT_UASL;
//				pts.adslAtucInvtlExtInfo_pt->intervalValidData = 1;	
			}
			else{
				i=0;
				for(ptr=(current_intvl->list).prev; ptr!=&interval_list; ptr=ptr->prev){
					i++;
					if(i==pts.adslAtucInvtlExtInfo_pt->IntervalNumber){
						temp_intvl = list_entry(ptr, amazon_mei_mib, list);
						pts.adslAtucInvtlExtInfo_pt->adslAtucIntervalFastR = temp_intvl->AtucPerfStatFastR;
						pts.adslAtucInvtlExtInfo_pt->adslAtucIntervalFailedFastR = temp_intvl->AtucPerfStatFailedFastR;
						pts.adslAtucInvtlExtInfo_pt->adslAtucIntervalSesL = temp_intvl->AtucPerfStatSesL;
						pts.adslAtucInvtlExtInfo_pt->adslAtucIntervalUasL = temp_intvl->AtucPerfStatUasL;
//						pts.adslAtucInvtlExtInfo_pt->intervalValidData = 1;
						break;
					}		
				} 
				if(ptr==&interval_list){
//					pts.adslAtucInvtlExtInfo_pt->intervalValidData = 0;
					pts.adslAtucInvtlExtInfo_pt->flags = 0;	
					pts.adslAtucInvtlExtInfo_pt->adslAtucIntervalFastR = 0;
					pts.adslAtucInvtlExtInfo_pt->adslAtucIntervalFailedFastR = 0;
					pts.adslAtucInvtlExtInfo_pt->adslAtucIntervalSesL = 0;
					pts.adslAtucInvtlExtInfo_pt->adslAtucIntervalUasL = 0;
				}
			}
			copy_to_user((char *)lon, (char *)pts.adslAtucInvtlExtInfo_pt, sizeof(adslAtucInvtlExtInfo));
			kfree(pts.adslAtucInvtlExtInfo_pt);
			break;
#endif
		case GET_ADSL_ATUR_INTVL_INFO:
			pts.adslAturIntvlInfo_pt = (adslAturIntvlInfo *)kmalloc(sizeof(adslAturIntvlInfo), GFP_KERNEL);
			copy_from_user((char *)pts.adslAturIntvlInfo_pt, (char *)lon, sizeof(adslAturIntvlInfo));
			
			if(pts.adslAturIntvlInfo_pt->IntervalNumber <1){
				pts.adslAturIntvlInfo_pt->intervalLOF = ATUR_PERF_LOFS;
				pts.adslAturIntvlInfo_pt->intervalLOS = ATUR_PERF_LOSS;
				pts.adslAturIntvlInfo_pt->intervalES = ATUR_PERF_ESS;
				pts.adslAturIntvlInfo_pt->intervalLPR = ATUR_PERF_LPR;
				pts.adslAturIntvlInfo_pt->intervalValidData = 1;	
			}
			else{
				i=0;
				for(ptr=(current_intvl->list).prev; ptr!=&interval_list; ptr=ptr->prev){
					i++;
					if(i==pts.adslAturIntvlInfo_pt->IntervalNumber){
						temp_intvl = list_entry(ptr, amazon_mei_mib, list);
						pts.adslAturIntvlInfo_pt->intervalLOF = temp_intvl->AturPerfLof;
						pts.adslAturIntvlInfo_pt->intervalLOS = temp_intvl->AturPerfLos;
						pts.adslAturIntvlInfo_pt->intervalES = temp_intvl->AturPerfEs;
						pts.adslAturIntvlInfo_pt->intervalLPR = temp_intvl->AturPerfLpr;
						pts.adslAturIntvlInfo_pt->intervalValidData = 1;
						break;
					}		
				}
				if(ptr==&interval_list){
					pts.adslAturIntvlInfo_pt->intervalValidData = 0;
					pts.adslAturIntvlInfo_pt->flags = 0;
					pts.adslAturIntvlInfo_pt->intervalLOF = 0;
					pts.adslAturIntvlInfo_pt->intervalLOS = 0;
					pts.adslAturIntvlInfo_pt->intervalES = 0;
					pts.adslAturIntvlInfo_pt->intervalLPR = 0;	
				}
			}
			
			copy_to_user((char *)lon, (char *)pts.adslAturIntvlInfo_pt, sizeof(adslAturIntvlInfo));
			kfree(pts.adslAturIntvlInfo_pt);
			break;
#ifdef AMAZON_MEI_MIB_RFC3440
		case GET_ADSL_ATUR_INTVL_EXT_INFO:
			pts.adslAturInvtlExtInfo_pt = (adslAturInvtlExtInfo *)kmalloc(sizeof(adslAturInvtlExtInfo), GFP_KERNEL);
			copy_from_user((char *)pts.adslAturInvtlExtInfo_pt, (char *)lon, sizeof(adslAturInvtlExtInfo));
			
			if(pts.adslAturInvtlExtInfo_pt->IntervalNumber <1){
				pts.adslAturInvtlExtInfo_pt->adslAturIntervalSesL = ATUR_PERF_STAT_SESL;
				pts.adslAturInvtlExtInfo_pt->adslAturIntervalUasL = ATUR_PERF_STAT_UASL;
//				pts.adslAturInvtlExtInfo_pt->intervalValidData = 1;
			}
			else{
				i=0;
				for(ptr=(current_intvl->list).prev; ptr!=&interval_list; ptr=ptr->prev){
					i++;
					if(i==pts.adslAturInvtlExtInfo_pt->IntervalNumber){
						temp_intvl = list_entry(ptr, amazon_mei_mib, list);
						pts.adslAturInvtlExtInfo_pt->adslAturIntervalSesL = temp_intvl->AturPerfStatSesL;
						pts.adslAturInvtlExtInfo_pt->adslAturIntervalUasL = temp_intvl->AturPerfStatUasL;
//						pts.adslAturInvtlExtInfo_pt->intervalValidData = 1;
						break; 
					}		
				}
				if(ptr==&interval_list){
//					pts.adslAturInvtlExtInfo_pt->intervalValidData = 0;
					pts.adslAturInvtlExtInfo_pt->flags = 0;	
					pts.adslAturInvtlExtInfo_pt->adslAturIntervalSesL = 0;
					pts.adslAturInvtlExtInfo_pt->adslAturIntervalUasL = 0; 
				}
			}
			
			copy_to_user((char *)lon, (char *)pts.adslAturInvtlExtInfo_pt, sizeof(adslAturInvtlExtInfo));
			kfree(pts.adslAturInvtlExtInfo_pt);
			break;
#endif
		case GET_ADSL_ATUC_CHAN_PERF_DATA:
			pts.atucChannelPerfDataEntry_pt = (atucChannelPerfDataEntry *)kmalloc(sizeof(atucChannelPerfDataEntry), GFP_KERNEL);
			copy_from_user((char *)pts.atucChannelPerfDataEntry_pt, (char *)lon, sizeof(atucChannelPerfDataEntry));
	
			pts.atucChannelPerfDataEntry_pt->flags = 0;
			
			copy_to_user((char *)lon, (char *)pts.atucChannelPerfDataEntry_pt, sizeof(atucChannelPerfDataEntry));
			kfree(pts.atucChannelPerfDataEntry_pt);
			break;
		case GET_ADSL_ATUR_CHAN_PERF_DATA:
			pts.aturChannelPerfDataEntry_pt = (aturChannelPerfDataEntry *)kmalloc(sizeof(aturChannelPerfDataEntry), GFP_KERNEL);
			copy_from_user((char *)pts.aturChannelPerfDataEntry_pt, (char *)lon, sizeof(aturChannelPerfDataEntry));
			if(IS_FLAG_SET((&(pts.aturChannelPerfDataEntry_pt->flags)), ATUR_CHAN_RECV_BLK_FLAG)){
				pts.aturChannelPerfDataEntry_pt->adslAturChanReceivedBlks=ATUR_CHAN_RECV_BLK;
			}
			if(IS_FLAG_SET((&(pts.aturChannelPerfDataEntry_pt->flags)), ATUR_CHAN_TX_BLK_FLAG)){
				pts.aturChannelPerfDataEntry_pt->adslAturChanTransmittedBlks=ATUR_CHAN_TX_BLK;
			}
			if(IS_FLAG_SET((&(pts.aturChannelPerfDataEntry_pt->flags)), ATUR_CHAN_CORR_BLK_FLAG)){
				pts.aturChannelPerfDataEntry_pt->adslAturChanCorrectedBlks=ATUR_CHAN_CORR_BLK;
			}
			if(IS_FLAG_SET((&(pts.aturChannelPerfDataEntry_pt->flags)), ATUR_CHAN_UNCORR_BLK_FLAG)){
				pts.aturChannelPerfDataEntry_pt->adslAturChanUncorrectBlks=ATUR_CHAN_UNCORR_BLK;
			}
			if(IS_FLAG_SET((&(pts.aturChannelPerfDataEntry_pt->flags)), ATUR_CHAN_PERF_VALID_INTVL_FLAG)){
				i=0;
				for(ptr=(current_intvl->list).prev; ptr!=&interval_list; ptr=ptr->prev){
					i++;
					if(i==96)
						break;
				}
				pts.aturChannelPerfDataEntry_pt->adslAturChanPerfValidIntervals=i;
			}
			if(IS_FLAG_SET((&(pts.aturChannelPerfDataEntry_pt->flags)), ATUR_CHAN_PERF_INVALID_INTVL_FLAG)){
				pts.aturChannelPerfDataEntry_pt->adslAturChanPerfInvalidIntervals=0;
			}
 			if(IS_FLAG_SET((&(pts.aturChannelPerfDataEntry_pt->flags)), ATUR_CHAN_PERF_CURR_15MIN_TIME_ELAPSED_FLAG)){
				do_gettimeofday(&time_now);
				pts.aturChannelPerfDataEntry_pt->adslAturChanPerfCurr15MinTimeElapsed=time_now.tv_sec - (current_intvl->start_time).tv_sec;
			}
			if(IS_FLAG_SET((&(pts.aturChannelPerfDataEntry_pt->flags)), ATUR_CHAN_PERF_CURR_15MIN_RECV_BLK_FLAG)){
				pts.aturChannelPerfDataEntry_pt->adslAturChanPerfCurr15MinReceivedBlks=current_intvl->AturChanPerfRxBlk;
			}
			if(IS_FLAG_SET((&(pts.aturChannelPerfDataEntry_pt->flags)), ATUR_CHAN_PERF_CURR_15MIN_TX_BLK_FLAG)){
				pts.aturChannelPerfDataEntry_pt->adslAturChanPerfCurr15MinTransmittedBlks=current_intvl->AturChanPerfTxBlk;
			}
			if(IS_FLAG_SET((&(pts.aturChannelPerfDataEntry_pt->flags)), ATUR_CHAN_PERF_CURR_15MIN_CORR_BLK_FLAG)){
				pts.aturChannelPerfDataEntry_pt->adslAturChanPerfCurr15MinCorrectedBlks=current_intvl->AturChanPerfCorrBlk;
			}
			if(IS_FLAG_SET((&(pts.aturChannelPerfDataEntry_pt->flags)), ATUR_CHAN_PERF_CURR_15MIN_UNCORR_BLK_FLAG)){
				pts.aturChannelPerfDataEntry_pt->adslAturChanPerfCurr15MinUncorrectBlks=current_intvl->AturChanPerfUncorrBlk;
			}
			if(IS_FLAG_SET((&(pts.aturChannelPerfDataEntry_pt->flags)), ATUR_CHAN_PERF_CURR_1DAY_TIME_ELAPSED_FLAG)){
				i=0;		
				for(ptr=(current_intvl->list).prev; ptr!=&interval_list; ptr=ptr->prev){
					i+=900;	
				}
				do_gettimeofday(&time_now);
				i+=time_now.tv_sec - (current_intvl->start_time).tv_sec;
				if(i>=86400)
					pts.aturChannelPerfDataEntry_pt->adslAturChanPerfCurr1DayTimeElapsed=i-86400;
				else
					pts.aturChannelPerfDataEntry_pt->adslAturChanPerfCurr1DayTimeElapsed=i;		
			}
			if(IS_FLAG_SET((&(pts.aturChannelPerfDataEntry_pt->flags)), ATUR_CHAN_PERF_CURR_1DAY_RECV_BLK_FLAG)){		
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AturChanPerfRxBlk;
					i++;	
					if(i==96)
						j=0;
				}
				j+=current_intvl->AturChanPerfRxBlk;
				pts.aturChannelPerfDataEntry_pt->adslAturChanPerfCurr1DayReceivedBlks=j;
			}
			if(IS_FLAG_SET((&(pts.aturChannelPerfDataEntry_pt->flags)), ATUR_CHAN_PERF_CURR_1DAY_TX_BLK_FLAG)){		
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AturChanPerfTxBlk;
					i++;	
					if(i==96)
						j=0;
				}
				j+=current_intvl->AturChanPerfTxBlk;
				pts.aturChannelPerfDataEntry_pt->adslAturChanPerfCurr1DayTransmittedBlks=j;
			}
			if(IS_FLAG_SET((&(pts.aturChannelPerfDataEntry_pt->flags)), ATUR_CHAN_PERF_CURR_1DAY_CORR_BLK_FLAG)){		
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AturChanPerfCorrBlk;
					i++;	
					if(i==96)
						j=0;
				}
				j+=current_intvl->AturChanPerfCorrBlk;
				pts.aturChannelPerfDataEntry_pt->adslAturChanPerfCurr1DayCorrectedBlks=j;
			}
			if(IS_FLAG_SET((&(pts.aturChannelPerfDataEntry_pt->flags)), ATUR_CHAN_PERF_CURR_1DAY_UNCORR_BLK_FLAG)){		
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AturChanPerfUncorrBlk;
					i++;	
					if(i==96)
						j=0;
				}
				j+=current_intvl->AturChanPerfUncorrBlk;
				pts.aturChannelPerfDataEntry_pt->adslAturChanPerfCurr1DayUncorrectBlks=j;
			}
			if(IS_FLAG_SET((&(pts.aturChannelPerfDataEntry_pt->flags)), ATUR_CHAN_PERF_PREV_1DAY_MONI_SEC_FLAG)){		
				i=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					i++;		
				}
				if(i>=96)
					pts.aturChannelPerfDataEntry_pt->adslAturChanPerfPrev1DayMoniSecs=86400;
				else
					pts.aturChannelPerfDataEntry_pt->adslAturChanPerfPrev1DayMoniSecs=0;
			}
			if(IS_FLAG_SET((&(pts.aturChannelPerfDataEntry_pt->flags)), ATUR_CHAN_PERF_PREV_1DAY_RECV_BLK_FLAG)){
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AturChanPerfRxBlk;
					i++;
					if(i==96)
						break;
				}	
				if(i==96)
					pts.aturChannelPerfDataEntry_pt->adslAturChanPerfPrev1DayReceivedBlks=j;
				else
					pts.aturChannelPerfDataEntry_pt->adslAturChanPerfPrev1DayReceivedBlks=0;
			}
			if(IS_FLAG_SET((&(pts.aturChannelPerfDataEntry_pt->flags)), ATUR_CHAN_PERF_PREV_1DAY_TRANS_BLK_FLAG)){
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AturChanPerfTxBlk;
					i++;
					if(i==96)
						break;
				}	
				if(i==96)
					pts.aturChannelPerfDataEntry_pt->adslAturChanPerfPrev1DayTransmittedBlks=j;
				else
					pts.aturChannelPerfDataEntry_pt->adslAturChanPerfPrev1DayTransmittedBlks=0;
			}
			if(IS_FLAG_SET((&(pts.aturChannelPerfDataEntry_pt->flags)), ATUR_CHAN_PERF_PREV_1DAY_CORR_BLK_FLAG)){
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AturChanPerfCorrBlk;
					i++;
					if(i==96)
						break;
				}	
				if(i==96)
					pts.aturChannelPerfDataEntry_pt->adslAturChanPerfPrev1DayCorrectedBlks=j;
				else
					pts.aturChannelPerfDataEntry_pt->adslAturChanPerfPrev1DayCorrectedBlks=0;
			}
			if(IS_FLAG_SET((&(pts.aturChannelPerfDataEntry_pt->flags)), ATUR_CHAN_PERF_PREV_1DAY_UNCORR_BLK_FLAG)){
				i=0;
				j=0;
				for(ptr=interval_list.next; ptr!=&(current_intvl->list); ptr=ptr->next){
					mib_ptr = list_entry(ptr, amazon_mei_mib, list);
					j+=mib_ptr->AturChanPerfUncorrBlk;
					i++;
					if(i==96)
						break;
				}	
				if(i==96)
					pts.aturChannelPerfDataEntry_pt->adslAturChanPerfPrev1DayUncorrectBlks=j;
				else
					pts.aturChannelPerfDataEntry_pt->adslAturChanPerfPrev1DayUncorrectBlks=0;
			}
			
			copy_to_user((char *)lon, (char *)pts.aturChannelPerfDataEntry_pt, sizeof(aturChannelPerfDataEntry));
			kfree(pts.aturChannelPerfDataEntry_pt);
			break;
		case GET_ADSL_ATUC_CHAN_INTVL_INFO:
			pts.adslAtucChanIntvlInfo_pt = (adslAtucChanIntvlInfo *)kmalloc(sizeof(adslAtucChanIntvlInfo), GFP_KERNEL);
			copy_from_user((char *)pts.adslAtucChanIntvlInfo_pt, (char *)lon, sizeof(adslAtucChanIntvlInfo));
				
				pts.adslAtucChanIntvlInfo_pt->flags = 0;
				
			copy_to_user((char *)lon, (char *)pts.adslAtucChanIntvlInfo_pt, sizeof(adslAtucChanIntvlInfo));
			kfree(pts.adslAtucChanIntvlInfo_pt);
			break;
		case GET_ADSL_ATUR_CHAN_INTVL_INFO:
			pts.adslAturChanIntvlInfo_pt = (adslAturChanIntvlInfo *)kmalloc(sizeof(adslAturChanIntvlInfo), GFP_KERNEL);
			copy_from_user((char *)pts.adslAturChanIntvlInfo_pt, (char *)lon, sizeof(adslAturChanIntvlInfo));
			
			if(pts.adslAturChanIntvlInfo_pt->IntervalNumber <1){
				pts.adslAturChanIntvlInfo_pt->chanIntervalRecvdBlks = ATUR_CHAN_RECV_BLK;
				pts.adslAturChanIntvlInfo_pt->chanIntervalXmitBlks = ATUR_CHAN_TX_BLK;
				pts.adslAturChanIntvlInfo_pt->chanIntervalCorrectedBlks = ATUR_CHAN_CORR_BLK;
				pts.adslAturChanIntvlInfo_pt->chanIntervalUncorrectBlks = ATUR_CHAN_UNCORR_BLK;
				pts.adslAturChanIntvlInfo_pt->intervalValidData = 1;
			}
			else{
				i=0;
				for(ptr=(current_intvl->list).prev; ptr!=&interval_list; ptr=ptr->prev){
					i++;
					if(i==pts.adslAturChanIntvlInfo_pt->IntervalNumber){
						temp_intvl = list_entry(ptr, amazon_mei_mib, list);
						pts.adslAturChanIntvlInfo_pt->chanIntervalRecvdBlks = temp_intvl->AturChanPerfRxBlk;
						pts.adslAturChanIntvlInfo_pt->chanIntervalXmitBlks = temp_intvl->AturChanPerfTxBlk;
						pts.adslAturChanIntvlInfo_pt->chanIntervalCorrectedBlks = temp_intvl->AturChanPerfCorrBlk;
						pts.adslAturChanIntvlInfo_pt->chanIntervalUncorrectBlks = temp_intvl->AturChanPerfUncorrBlk;
						pts.adslAturChanIntvlInfo_pt->intervalValidData = 1;
						break;
					}		
				}
				if(ptr==&interval_list){
					pts.adslAturChanIntvlInfo_pt->intervalValidData = 0;
					pts.adslAturChanIntvlInfo_pt->flags = 0;	
				}
			}
			
			copy_to_user((char *)lon, (char *)pts.adslAturChanIntvlInfo_pt, sizeof(adslAturChanIntvlInfo));
			kfree(pts.adslAturChanIntvlInfo_pt);
			break;
		case GET_ADSL_ALRM_CONF_PROF:
			pts.adslLineAlarmConfProfileEntry_pt = (adslLineAlarmConfProfileEntry *)kmalloc(sizeof(adslLineAlarmConfProfileEntry), GFP_KERNEL);
			copy_from_user((char *)pts.adslLineAlarmConfProfileEntry_pt, (char *)lon, sizeof(adslLineAlarmConfProfileEntry));
			
			strncpy(pts.adslLineAlarmConfProfileEntry_pt->adslLineAlarmConfProfileName, AlarmConfProfile.adslLineAlarmConfProfileName, 32); 
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUC_THRESH_15MIN_LOFS_FLAG)){
				pts.adslLineAlarmConfProfileEntry_pt->adslAtucThresh15MinLofs=AlarmConfProfile.adslAtucThresh15MinLofs;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUC_THRESH_15MIN_LOSS_FLAG)){
				pts.adslLineAlarmConfProfileEntry_pt->adslAtucThresh15MinLoss=AlarmConfProfile.adslAtucThresh15MinLoss;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUC_THRESH_15MIN_ESS_FLAG)){
				pts.adslLineAlarmConfProfileEntry_pt->adslAtucThresh15MinESs=AlarmConfProfile.adslAtucThresh15MinESs;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUC_THRESH_FAST_RATEUP_FLAG)){
				pts.adslLineAlarmConfProfileEntry_pt->adslAtucThreshFastRateUp=AlarmConfProfile.adslAtucThreshFastRateUp;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUC_THRESH_INTERLEAVE_RATEUP_FLAG)){
				pts.adslLineAlarmConfProfileEntry_pt->adslAtucThreshInterleaveRateUp=AlarmConfProfile.adslAtucThreshInterleaveRateUp;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUC_THRESH_FAST_RATEDOWN_FLAG)){
				pts.adslLineAlarmConfProfileEntry_pt->adslAtucThreshFastRateDown=AlarmConfProfile.adslAtucThreshFastRateDown;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUC_THRESH_INTERLEAVE_RATEDOWN_FLAG)){
				pts.adslLineAlarmConfProfileEntry_pt->adslAtucThreshInterleaveRateDown=AlarmConfProfile.adslAtucThreshInterleaveRateDown;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUC_INIT_FAILURE_TRAP_ENABLE_FLAG)){
				pts.adslLineAlarmConfProfileEntry_pt->adslAtucInitFailureTrapEnable=AlarmConfProfile.adslAtucInitFailureTrapEnable;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUR_THRESH_15MIN_LOFS_FLAG)){
				pts.adslLineAlarmConfProfileEntry_pt->adslAturThresh15MinLofs=AlarmConfProfile.adslAturThresh15MinLofs;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUR_THRESH_15MIN_LOSS_FLAG)){
				pts.adslLineAlarmConfProfileEntry_pt->adslAturThresh15MinLoss=AlarmConfProfile.adslAturThresh15MinLoss;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUR_THRESH_15MIN_LPRS_FLAG)){
				pts.adslLineAlarmConfProfileEntry_pt->adslAturThresh15MinLprs=AlarmConfProfile.adslAturThresh15MinLprs;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUR_THRESH_15MIN_ESS_FLAG)){
				pts.adslLineAlarmConfProfileEntry_pt->adslAturThresh15MinESs=AlarmConfProfile.adslAturThresh15MinESs;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUR_THRESH_FAST_RATEUP_FLAG)){
				pts.adslLineAlarmConfProfileEntry_pt->adslAturThreshFastRateUp=AlarmConfProfile.adslAturThreshFastRateUp;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUR_THRESH_INTERLEAVE_RATEUP_FLAG)){
				pts.adslLineAlarmConfProfileEntry_pt->adslAturThreshInterleaveRateUp=AlarmConfProfile.adslAturThreshInterleaveRateUp;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUR_THRESH_FAST_RATEDOWN_FLAG)){
				pts.adslLineAlarmConfProfileEntry_pt->adslAturThreshFastRateDown=AlarmConfProfile.adslAturThreshFastRateDown;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUR_THRESH_INTERLEAVE_RATEDOWN_FLAG)){
				pts.adslLineAlarmConfProfileEntry_pt->adslAturThreshInterleaveRateDown=AlarmConfProfile.adslAturThreshInterleaveRateDown;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), LINE_ALARM_CONF_PROFILE_ROWSTATUS_FLAG)){
				pts.adslLineAlarmConfProfileEntry_pt->adslLineAlarmConfProfileRowStatus=AlarmConfProfile.adslLineAlarmConfProfileRowStatus;
			}
			copy_to_user((char *)lon, (char *)pts.adslLineAlarmConfProfileEntry_pt, sizeof(adslLineAlarmConfProfileEntry));
			kfree(pts.adslLineAlarmConfProfileEntry_pt);
			break;
#ifdef AMAZON_MEI_MIB_RFC3440
		case GET_ADSL_ALRM_CONF_PROF_EXT:
			pts.adslLineAlarmConfProfileExtEntry_pt = (adslLineAlarmConfProfileExtEntry *)kmalloc(sizeof(adslLineAlarmConfProfileExtEntry), GFP_KERNEL);
			copy_from_user((char *)pts.adslLineAlarmConfProfileExtEntry_pt, (char *)lon, sizeof(adslLineAlarmConfProfileExtEntry));
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileExtEntry_pt->flags)), ATUC_THRESH_15MIN_FAILED_FASTR_FLAG)){
				pts.adslLineAlarmConfProfileExtEntry_pt->adslAtucThreshold15MinFailedFastR=AlarmConfProfileExt.adslAtucThreshold15MinFailedFastR;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileExtEntry_pt->flags)), ATUC_THRESH_15MIN_SESL_FLAG)){
				pts.adslLineAlarmConfProfileExtEntry_pt->adslAtucThreshold15MinSesL=AlarmConfProfileExt.adslAtucThreshold15MinSesL;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileExtEntry_pt->flags)), ATUC_THRESH_15MIN_UASL_FLAG)){
				pts.adslLineAlarmConfProfileExtEntry_pt->adslAtucThreshold15MinUasL=AlarmConfProfileExt.adslAtucThreshold15MinUasL;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileExtEntry_pt->flags)), ATUR_THRESH_15MIN_SESL_FLAG)){
				pts.adslLineAlarmConfProfileExtEntry_pt->adslAturThreshold15MinSesL=AlarmConfProfileExt.adslAturThreshold15MinSesL;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileExtEntry_pt->flags)), ATUR_THRESH_15MIN_UASL_FLAG)){
				pts.adslLineAlarmConfProfileExtEntry_pt->adslAturThreshold15MinUasL=AlarmConfProfileExt.adslAturThreshold15MinUasL;
			}
			copy_to_user((char *)lon, (char *)pts.adslLineAlarmConfProfileExtEntry_pt, sizeof(adslLineAlarmConfProfileExtEntry));
			kfree(pts.adslLineAlarmConfProfileExtEntry_pt);
			break;
#endif
		case SET_ADSL_ALRM_CONF_PROF:
			pts.adslLineAlarmConfProfileEntry_pt = (adslLineAlarmConfProfileEntry *)kmalloc(sizeof(adslLineAlarmConfProfileEntry), GFP_KERNEL);
			copy_from_user((char *)pts.adslLineAlarmConfProfileEntry_pt, (char *)lon, sizeof(adslLineAlarmConfProfileEntry));

			strncpy(AlarmConfProfile.adslLineAlarmConfProfileName, pts.adslLineAlarmConfProfileEntry_pt->adslLineAlarmConfProfileName, 32);
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUC_THRESH_15MIN_LOFS_FLAG)){
				AlarmConfProfile.adslAtucThresh15MinLofs=pts.adslLineAlarmConfProfileEntry_pt->adslAtucThresh15MinLofs;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUC_THRESH_15MIN_LOSS_FLAG)){
				AlarmConfProfile.adslAtucThresh15MinLoss=pts.adslLineAlarmConfProfileEntry_pt->adslAtucThresh15MinLoss;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUC_THRESH_15MIN_ESS_FLAG)){
				AlarmConfProfile.adslAtucThresh15MinESs=pts.adslLineAlarmConfProfileEntry_pt->adslAtucThresh15MinESs;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUC_THRESH_FAST_RATEUP_FLAG)){
				AlarmConfProfile.adslAtucThreshFastRateUp=pts.adslLineAlarmConfProfileEntry_pt->adslAtucThreshFastRateUp;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUC_THRESH_INTERLEAVE_RATEUP_FLAG)){
				AlarmConfProfile.adslAtucThreshInterleaveRateUp=pts.adslLineAlarmConfProfileEntry_pt->adslAtucThreshInterleaveRateUp;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUC_THRESH_FAST_RATEDOWN_FLAG)){
				AlarmConfProfile.adslAtucThreshFastRateDown=pts.adslLineAlarmConfProfileEntry_pt->adslAtucThreshFastRateDown;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUC_THRESH_INTERLEAVE_RATEDOWN_FLAG)){
				AlarmConfProfile.adslAtucThreshInterleaveRateDown=pts.adslLineAlarmConfProfileEntry_pt->adslAtucThreshInterleaveRateDown;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUC_INIT_FAILURE_TRAP_ENABLE_FLAG)){
				AlarmConfProfile.adslAtucInitFailureTrapEnable=pts.adslLineAlarmConfProfileEntry_pt->adslAtucInitFailureTrapEnable;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUR_THRESH_15MIN_LOFS_FLAG)){
				AlarmConfProfile.adslAturThresh15MinLofs=pts.adslLineAlarmConfProfileEntry_pt->adslAturThresh15MinLofs;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUR_THRESH_15MIN_LOSS_FLAG)){
				AlarmConfProfile.adslAturThresh15MinLoss=pts.adslLineAlarmConfProfileEntry_pt->adslAturThresh15MinLoss;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUR_THRESH_15MIN_LPRS_FLAG)){
				AlarmConfProfile.adslAturThresh15MinLprs=pts.adslLineAlarmConfProfileEntry_pt->adslAturThresh15MinLprs;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUR_THRESH_15MIN_ESS_FLAG)){
				AlarmConfProfile.adslAturThresh15MinESs=pts.adslLineAlarmConfProfileEntry_pt->adslAturThresh15MinESs;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUR_THRESH_FAST_RATEUP_FLAG)){
				AlarmConfProfile.adslAturThreshFastRateUp=pts.adslLineAlarmConfProfileEntry_pt->adslAturThreshFastRateUp;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUR_THRESH_INTERLEAVE_RATEUP_FLAG)){
				AlarmConfProfile.adslAturThreshInterleaveRateUp=pts.adslLineAlarmConfProfileEntry_pt->adslAturThreshInterleaveRateUp;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUR_THRESH_FAST_RATEDOWN_FLAG)){
				AlarmConfProfile.adslAturThreshFastRateDown=pts.adslLineAlarmConfProfileEntry_pt->adslAturThreshFastRateDown;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), ATUR_THRESH_INTERLEAVE_RATEDOWN_FLAG)){
				AlarmConfProfile.adslAturThreshInterleaveRateDown=pts.adslLineAlarmConfProfileEntry_pt->adslAturThreshInterleaveRateDown;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileEntry_pt->flags)), LINE_ALARM_CONF_PROFILE_ROWSTATUS_FLAG)){
				AlarmConfProfile.adslLineAlarmConfProfileRowStatus=pts.adslLineAlarmConfProfileEntry_pt->adslLineAlarmConfProfileRowStatus;
			}
			copy_to_user((char *)lon, (char *)pts.adslLineAlarmConfProfileEntry_pt, sizeof(adslLineAlarmConfProfileEntry));
			kfree(pts.adslLineAlarmConfProfileEntry_pt);
			break;
			
#ifdef AMAZON_MEI_MIB_RFC3440
		case SET_ADSL_ALRM_CONF_PROF_EXT:
			pts.adslLineAlarmConfProfileExtEntry_pt = (adslLineAlarmConfProfileExtEntry *)kmalloc(sizeof(adslLineAlarmConfProfileExtEntry), GFP_KERNEL);
			copy_from_user((char *)pts.adslLineAlarmConfProfileExtEntry_pt, (char *)lon, sizeof(adslLineAlarmConfProfileExtEntry));
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileExtEntry_pt->flags)), ATUC_THRESH_15MIN_FAILED_FASTR_FLAG)){
				AlarmConfProfileExt.adslAtucThreshold15MinFailedFastR=pts.adslLineAlarmConfProfileExtEntry_pt->adslAtucThreshold15MinFailedFastR;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileExtEntry_pt->flags)), ATUC_THRESH_15MIN_SESL_FLAG)){
				AlarmConfProfileExt.adslAtucThreshold15MinSesL=pts.adslLineAlarmConfProfileExtEntry_pt->adslAtucThreshold15MinSesL;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileExtEntry_pt->flags)), ATUC_THRESH_15MIN_UASL_FLAG)){
				AlarmConfProfileExt.adslAtucThreshold15MinUasL=pts.adslLineAlarmConfProfileExtEntry_pt->adslAtucThreshold15MinUasL;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileExtEntry_pt->flags)), ATUR_THRESH_15MIN_SESL_FLAG)){
				AlarmConfProfileExt.adslAturThreshold15MinSesL=pts.adslLineAlarmConfProfileExtEntry_pt->adslAturThreshold15MinSesL;
			}
			if(IS_FLAG_SET((&(pts.adslLineAlarmConfProfileExtEntry_pt->flags)), ATUR_THRESH_15MIN_UASL_FLAG)){
				AlarmConfProfileExt.adslAturThreshold15MinUasL=pts.adslLineAlarmConfProfileExtEntry_pt->adslAturThreshold15MinUasL;
			}
			copy_to_user((char *)lon, (char *)pts.adslLineAlarmConfProfileExtEntry_pt, sizeof(adslLineAlarmConfProfileExtEntry));
			kfree(pts.adslLineAlarmConfProfileExtEntry_pt);
			break;
#endif

		case ADSL_ATUR_TRAPS:
			if(down_interruptible(&mei_sema))
                		return -ERESTARTSYS;
			
			trapsflag=0;
			if(AlarmConfProfile.adslAtucThresh15MinLofs!=0 && current_intvl->AtucPerfLof>=AlarmConfProfile.adslAtucThresh15MinLofs)
				trapsflag|=ATUC_PERF_LOFS_THRESH_FLAG;
			if(AlarmConfProfile.adslAtucThresh15MinLoss!=0 && current_intvl->AtucPerfLos>=AlarmConfProfile.adslAtucThresh15MinLoss)
				trapsflag|=ATUC_PERF_LOSS_THRESH_FLAG;
			if(AlarmConfProfile.adslAtucThresh15MinESs!=0 && current_intvl->AtucPerfEs>=AlarmConfProfile.adslAtucThresh15MinESs)
				trapsflag|=ATUC_PERF_ESS_THRESH_FLAG;
			if(chantype.fast==1){
				if(AlarmConfProfile.adslAtucThreshFastRateUp!=0 || AlarmConfProfile.adslAtucThreshFastRateDown!=0){
					ATUC_CHAN_CURR_TX_RATE_FLAG_MAKECMV;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group 6 Address 1 Index 0");
#endif			
					}
					else{
						temp = (u32)(RxMessage[4]) + (((u32)(RxMessage[5]))<<16); 
						if((AlarmConfProfile.adslAtucThreshFastRateUp!=0) && (temp>=PrevTxRate.adslAtucChanPrevTxRate+AlarmConfProfile.adslAtucThreshFastRateUp)){
							trapsflag|=ATUC_RATE_CHANGE_FLAG;
							PrevTxRate.adslAtucChanPrevTxRate = temp;						
						}
						if((AlarmConfProfile.adslAtucThreshFastRateDown!=0) && (temp<=PrevTxRate.adslAtucChanPrevTxRate-AlarmConfProfile.adslAtucThreshFastRateDown)){
							trapsflag|=ATUC_RATE_CHANGE_FLAG;
							PrevTxRate.adslAtucChanPrevTxRate = temp;						
						}
					}
				}
			}	
			if(chantype.interleave==1){
				if(AlarmConfProfile.adslAtucThreshInterleaveRateUp!=0 || AlarmConfProfile.adslAtucThreshInterleaveRateDown!=0){
					ATUC_CHAN_CURR_TX_RATE_FLAG_MAKECMV;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group 6 Address 1 Index 0");
#endif			
					}
					else{
						temp = (u32)(RxMessage[4]) + (((u32)(RxMessage[5]))<<16); 
						if((AlarmConfProfile.adslAtucThreshInterleaveRateUp!=0) && (temp>=PrevTxRate.adslAtucChanPrevTxRate+AlarmConfProfile.adslAtucThreshInterleaveRateUp)){
							trapsflag|=ATUC_RATE_CHANGE_FLAG;
							PrevTxRate.adslAtucChanPrevTxRate = temp;						
						}
						if((AlarmConfProfile.adslAtucThreshInterleaveRateDown!=0) && (temp<=PrevTxRate.adslAtucChanPrevTxRate-AlarmConfProfile.adslAtucThreshInterleaveRateDown)){
							trapsflag|=ATUC_RATE_CHANGE_FLAG;
							PrevTxRate.adslAtucChanPrevTxRate = temp;						
						}
					}
				}
			}	
			if(AlarmConfProfile.adslAturThresh15MinLofs!=0 && current_intvl->AturPerfLof>=AlarmConfProfile.adslAturThresh15MinLofs)
				trapsflag|=ATUR_PERF_LOFS_THRESH_FLAG;
			if(AlarmConfProfile.adslAturThresh15MinLoss!=0 && current_intvl->AturPerfLos>=AlarmConfProfile.adslAturThresh15MinLoss)
				trapsflag|=ATUR_PERF_LOSS_THRESH_FLAG;
			if(AlarmConfProfile.adslAturThresh15MinLprs!=0 && current_intvl->AturPerfLpr>=AlarmConfProfile.adslAturThresh15MinLprs)
				trapsflag|=ATUR_PERF_LPRS_THRESH_FLAG;
			if(AlarmConfProfile.adslAturThresh15MinESs!=0 && current_intvl->AturPerfEs>=AlarmConfProfile.adslAturThresh15MinESs)
				trapsflag|=ATUR_PERF_ESS_THRESH_FLAG;	
			if(chantype.fast==1){
				if(AlarmConfProfile.adslAturThreshFastRateUp!=0 || AlarmConfProfile.adslAturThreshFastRateDown!=0){
					ATUR_CHAN_CURR_TX_RATE_FLAG_MAKECMV;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group 6 Address 0 Index 0");
#endif			
					}
					else{
						temp = (u32)(RxMessage[4]) + (((u32)(RxMessage[5]))<<16); 
						if((AlarmConfProfile.adslAturThreshFastRateUp!=0) && (temp>=PrevTxRate.adslAturChanPrevTxRate+AlarmConfProfile.adslAturThreshFastRateUp)){
							trapsflag|=ATUR_RATE_CHANGE_FLAG;
							PrevTxRate.adslAturChanPrevTxRate = temp;						
						}
						if((AlarmConfProfile.adslAturThreshFastRateDown!=0) && (temp<=PrevTxRate.adslAturChanPrevTxRate-AlarmConfProfile.adslAturThreshFastRateDown)){
							trapsflag|=ATUR_RATE_CHANGE_FLAG;
							PrevTxRate.adslAturChanPrevTxRate = temp;						
						}
					}
				}
			}	
			if(chantype.interleave==1){
				if(AlarmConfProfile.adslAturThreshInterleaveRateUp!=0 || AlarmConfProfile.adslAturThreshInterleaveRateDown!=0){
					ATUR_CHAN_CURR_TX_RATE_FLAG_MAKECMV;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group 6 Address 0 Index 0");
#endif			
					}
					else{
						temp = (u32)(RxMessage[4]) + (((u32)(RxMessage[5]))<<16);
						if((AlarmConfProfile.adslAturThreshInterleaveRateUp!=0) && (temp>=PrevTxRate.adslAturChanPrevTxRate+AlarmConfProfile.adslAturThreshInterleaveRateUp)){
							trapsflag|=ATUR_RATE_CHANGE_FLAG;
							PrevTxRate.adslAturChanPrevTxRate = temp;						
						}
						if((AlarmConfProfile.adslAturThreshInterleaveRateDown!=0) && (temp<=PrevTxRate.adslAturChanPrevTxRate-AlarmConfProfile.adslAturThreshInterleaveRateDown)){
							trapsflag|=ATUR_RATE_CHANGE_FLAG;
							PrevTxRate.adslAturChanPrevTxRate = temp;						
						}
					}
				}
			}	
			copy_to_user((char *)lon, (char *)(&trapsflag), 2);
			
			up(&mei_sema);	
			break;
			
#ifdef AMAZON_MEI_MIB_RFC3440
		case ADSL_ATUR_EXT_TRAPS:
			trapsflag=0;
			if(AlarmConfProfileExt.adslAtucThreshold15MinFailedFastR!=0 && current_intvl->AtucPerfStatFailedFastR>=AlarmConfProfileExt.adslAtucThreshold15MinFailedFastR)
				trapsflag|=ATUC_15MIN_FAILED_FASTR_TRAP_FLAG;
			if(AlarmConfProfileExt.adslAtucThreshold15MinSesL!=0 && current_intvl->AtucPerfStatSesL>=AlarmConfProfileExt.adslAtucThreshold15MinSesL)
				trapsflag|=ATUC_15MIN_SESL_TRAP_FLAG;
			if(AlarmConfProfileExt.adslAtucThreshold15MinUasL!=0 && current_intvl->AtucPerfStatUasL>=AlarmConfProfileExt.adslAtucThreshold15MinUasL)
				trapsflag|=ATUC_15MIN_UASL_TRAP_FLAG;
			if(AlarmConfProfileExt.adslAturThreshold15MinSesL!=0 && current_intvl->AturPerfStatSesL>=AlarmConfProfileExt.adslAturThreshold15MinSesL)
				trapsflag|=ATUR_15MIN_SESL_TRAP_FLAG;
			if(AlarmConfProfileExt.adslAturThreshold15MinUasL!=0 && current_intvl->AturPerfStatUasL>=AlarmConfProfileExt.adslAturThreshold15MinUasL)
				trapsflag|=ATUR_15MIN_UASL_TRAP_FLAG;
			copy_to_user((char *)lon, (char *)(&trapsflag), 2);
			break;
#endif

// 603221:tc.chen start
		case GET_ADSL_LINE_STATUS:
			if(down_interruptible(&mei_sema))
                		return -ERESTARTSYS;
			
			pts.adslLineStatusInfo_pt = (adslLineStatusInfo *)kmalloc(sizeof(adslLineStatusInfo), GFP_KERNEL);
			copy_from_user((char *)pts.adslLineStatusInfo_pt, (char *)lon, sizeof(adslLineStatusInfo));
			
			if(IS_FLAG_SET((&(pts.adslLineStatusInfo_pt->flags)), LINE_STAT_MODEM_STATUS_FLAG)){
				LINE_STAT_MODEM_STATUS_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group STAT Address 0 Index 0");
#endif
					pts.adslLineStatusInfo_pt->adslModemStatus = 0;	
				}
				else{
					pts.adslLineStatusInfo_pt->adslModemStatus = RxMessage[4];
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslLineStatusInfo_pt->flags)), LINE_STAT_MODE_SEL_FLAG)){
				LINE_STAT_MODE_SEL_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group STAT Address 1 Index 0");
#endif
					pts.adslLineStatusInfo_pt->adslModeSelected = 0;	
				}
				else{
					pts.adslLineStatusInfo_pt->adslModeSelected = RxMessage[4];
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslLineStatusInfo_pt->flags)), LINE_STAT_TRELLCOD_ENABLE_FLAG)){
				LINE_STAT_TRELLCOD_ENABLE_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group OPTN Address 2 Index 0");
#endif
					pts.adslLineStatusInfo_pt->adslTrellisCodeEnable = 0;	
				}
				else{
					
					pts.adslLineStatusInfo_pt->adslTrellisCodeEnable = (RxMessage[4]>>13)&0x1==0x1?0:1;
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslLineStatusInfo_pt->flags)), LINE_STAT_LATENCY_FLAG)){
				LINE_STAT_LATENCY_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group STAT Address 12 Index 0");
#endif
					pts.adslLineStatusInfo_pt->adslLatency = 0;	
				}
				else{
					pts.adslLineStatusInfo_pt->adslLatency = RxMessage[4];
				}
			}
			
			copy_to_user((char *)lon, (char *)pts.adslLineStatusInfo_pt, sizeof(adslLineStatusInfo));
			kfree(pts.adslLineStatusInfo_pt);
			
			up(&mei_sema);
			break;


		case GET_ADSL_LINE_RATE:
			if (showtime!=1)
				return -ERESTARTSYS;
			if(down_interruptible(&mei_sema))
                		return -ERESTARTSYS;
			
			pts.adslLineRateInfo_pt = (adslLineRateInfo *)kmalloc(sizeof(adslLineRateInfo), GFP_KERNEL);
			copy_from_user((char *)pts.adslLineRateInfo_pt, (char *)lon, sizeof(adslLineRateInfo));
			
			if(IS_FLAG_SET((&(pts.adslLineRateInfo_pt->flags)), LINE_RATE_DATA_RATEDS_FLAG)){
				if (adsl_mode <=8 && adsl_mode_extend==0) // adsl mode
				{
					if (chantype.interleave)
						LINE_RATE_DATA_RATEDS_FLAG_ADSL1_LP0_MAKECMV;
					else
						LINE_RATE_DATA_RATEDS_FLAG_ADSL1_LP1_MAKECMV;
						
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group RATE Address 1 Index 0");
#endif
						pts.adslLineRateInfo_pt->adslDataRateds = 0;	
					}
					else{
						pts.adslLineRateInfo_pt->adslDataRateds = (u32)(RxMessage[4]) + (((u32)(RxMessage[5]))<<16);
					}		
				}else  // adsl 2/2+
				{
					unsigned long Mp,Lp,Tp,Rp,Kp,Bpn,DataRate,DataRate_remain;
					Mp=Lp=Tp=Rp=Kp=Bpn=DataRate=DataRate_remain=0;
					//// up stream data rate
					  
					    if (chantype.interleave)
					    {
					    	    LINE_RATE_DATA_RATEUS_FLAG_ADSL2_LP_LP0_MAKECMV;
					    	    if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n\nCMV fail, Group CNFG Address 25 Index 0");
#endif
							Lp = 0;	
						    }else
				                    	Lp=RxMessage[4];
				   
				                    LINE_RATE_DATA_RATEUS_FLAG_ADSL2_RP_LP0_MAKECMV;
					    	    if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n\nCMV fail, Group CNFG Address 23 Index 0");
#endif
							Rp = 0;	
						    }else
				                    	Rp=RxMessage[4];
				
				                    LINE_RATE_DATA_RATEUS_FLAG_ADSL2_MP_LP0_MAKECMV;
					    	    if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n\nCMV fail, Group CNFG Address 24 Index 0");
#endif
							Mp = 0;	
						    }else
				                    	Mp=RxMessage[4];
				
				                    LINE_RATE_DATA_RATEUS_FLAG_ADSL2_TP_LP0_MAKECMV;
					    	    if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n\nCMV fail, Group CNFG Address 26 Index 0");
#endif
							Tp = 0;	
						    }else
				                    	Tp=RxMessage[4];
				
				                    LINE_RATE_DATA_RATEUS_FLAG_ADSL2_KP_LP0_MAKECMV;
					    	    if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n\nCMV fail, Group CNFG Address 28 Index 0");
#endif
							Kp = 0;	
						    }else
						    {
				                    	Kp=RxMessage[4]+ RxMessage[5]+1;
				                    	Bpn=RxMessage[4]+ RxMessage[5];
				                    }
			                    }else
			                    {
			                    	    LINE_RATE_DATA_RATEUS_FLAG_ADSL2_LP_LP1_MAKECMV;
					    	    if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n\nCMV fail, Group CNFG Address 25 Index 1");
#endif
							Lp = 0;	
						    }else
				                    	Lp=RxMessage[4];
				   
				                    LINE_RATE_DATA_RATEUS_FLAG_ADSL2_RP_LP1_MAKECMV;
					    	    if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n\nCMV fail, Group CNFG Address 23 Index 1");
#endif
							Rp = 0;	
						    }else
				                    	Rp=RxMessage[4];
				
				                    LINE_RATE_DATA_RATEUS_FLAG_ADSL2_MP_LP1_MAKECMV;
					    	    if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n\nCMV fail, Group CNFG Address 24 Index 1");
#endif
							Mp = 0;	
						    }else
				                    	Mp=RxMessage[4];
				
				                    LINE_RATE_DATA_RATEUS_FLAG_ADSL2_TP_LP1_MAKECMV;
					    	    if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n\nCMV fail, Group CNFG Address 26 Index 1");
#endif
							Tp = 0;	
						    }else
				                    	Tp=RxMessage[4];
				
				                    LINE_RATE_DATA_RATEUS_FLAG_ADSL2_KP_LP1_MAKECMV;
					    	    if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n\nCMV fail, Group CNFG Address 28 Index 2");
#endif
							Kp = 0;	
						    }else
						    {
				                    	Kp=RxMessage[4]+ RxMessage[5]+1;
				                    	Bpn=RxMessage[4]+ RxMessage[5];
				                    }
			                    }
  					    DataRate=((Tp*(Bpn+1)-1)*Mp*Lp*4)/(Tp*(Kp*Mp+Rp));
			                    //DataRate_remain=((((Tp*(Bpn+1)-1)*Mp*Lp*4)%(Tp*(Kp*Mp+Rp)))*1000)/(Tp*(Kp*Mp+Rp));
			                    //pts.adslLineRateInfo_pt->adslDataRateds = DataRate * 1000 + DataRate_remain;
			                    pts.adslLineRateInfo_pt->adslDataRateds = DataRate;
				}		
			}
			
			if(IS_FLAG_SET((&(pts.adslLineRateInfo_pt->flags)), LINE_RATE_DATA_RATEUS_FLAG)){
				if (adsl_mode <=8 && adsl_mode_extend==0) // adsl mode
				{
					if (chantype.interleave)
						LINE_RATE_DATA_RATEUS_FLAG_ADSL1_LP0_MAKECMV;
					else
						LINE_RATE_DATA_RATEUS_FLAG_ADSL1_LP1_MAKECMV;
						
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
	#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group RATE Address 0 Index 0");
	#endif
						pts.adslLineRateInfo_pt->adslDataRateus = 0;	
					}
					else{
						pts.adslLineRateInfo_pt->adslDataRateus = (u32)(RxMessage[4]) + (((u32)(RxMessage[5]))<<16);
					}		
				}else  // adsl 2/2+
				{
					unsigned long Mp,Lp,Tp,Rp,Kp,Bpn,DataRate,DataRate_remain;
					Mp=Lp=Tp=Rp=Kp=Bpn=DataRate=DataRate_remain=0;
					//// down stream data rate
					   
			 		    if (chantype.interleave)
					    {
					    	    LINE_RATE_DATA_RATEDS_FLAG_ADSL2_LP_LP0_MAKECMV;
					    	    if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n\nCMV fail, Group CNFG Address 14 Index 0");
#endif
							Lp = 0;	
						    }else
				                    	Lp=RxMessage[4];
				   
				                    LINE_RATE_DATA_RATEDS_FLAG_ADSL2_RP_LP0_MAKECMV;
					    	    if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n\nCMV fail, Group CNFG Address 12 Index 0");
#endif
							Rp = 0;	
						    }else
				                    	Rp=RxMessage[4];
				
				                    LINE_RATE_DATA_RATEDS_FLAG_ADSL2_MP_LP0_MAKECMV;
					    	    if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n\nCMV fail, Group CNFG Address 13 Index 0");
#endif
							Mp = 0;	
						    }else
				                    	Mp=RxMessage[4];
				
				                    LINE_RATE_DATA_RATEDS_FLAG_ADSL2_TP_LP0_MAKECMV;
					    	    if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n\nCMV fail, Group CNFG Address 15 Index 0");
#endif
							Tp = 0;	
						    }else
				                    	Tp=RxMessage[4];
				
				                    LINE_RATE_DATA_RATEDS_FLAG_ADSL2_KP_LP0_MAKECMV;
					    	    if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n\nCMV fail, Group CNFG Address 17 Index 0");
#endif
							Kp = 0;	
						    }else
						    {
				                    	Kp=RxMessage[4]+ RxMessage[5]+1;
				                    	Bpn=RxMessage[4]+ RxMessage[5];
				                    }
			                    }else
			                    {
			                    	    LINE_RATE_DATA_RATEDS_FLAG_ADSL2_LP_LP1_MAKECMV;
					    	    if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n\nCMV fail, Group CNFG Address 14 Index 1");
#endif
							Lp = 0;	
						    }else
				                    	Lp=RxMessage[4];
				   
				                    LINE_RATE_DATA_RATEDS_FLAG_ADSL2_RP_LP1_MAKECMV;
					    	    if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n\nCMV fail, Group CNFG Address 12 Index 1");
#endif
							Rp = 0;	
						    }else
				                    	Rp=RxMessage[4];
				
				                    LINE_RATE_DATA_RATEDS_FLAG_ADSL2_MP_LP1_MAKECMV;
					    	    if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n\nCMV fail, Group CNFG Address 13 Index 1");
#endif
							Mp = 0;	
						    }else
				                    	Mp=RxMessage[4];
				
				                    LINE_RATE_DATA_RATEDS_FLAG_ADSL2_TP_LP1_MAKECMV;
					    	    if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n\nCMV fail, Group CNFG Address 15 Index 1");
#endif
							Tp = 0;	
						    }else
				                    	Tp=RxMessage[4];
				
				                    LINE_RATE_DATA_RATEDS_FLAG_ADSL2_KP_LP1_MAKECMV;
					    	    if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n\nCMV fail, Group CNFG Address 17 Index 2");
#endif
							Kp = 0;	
						    }else
						    {
				                    	Kp=RxMessage[4]+ RxMessage[5]+1;
				                    	Bpn=RxMessage[4]+ RxMessage[5];
				                    }
			                    }
			                    DataRate=((Tp*(Bpn+1)-1)*Mp*Lp*4)/(Tp*(Kp*Mp+Rp));
			                    //DataRate_remain=((((Tp*(Bpn+1)-1)*Mp*Lp*4)%(Tp*(Kp*Mp+Rp)))*1000)/(Tp*(Kp*Mp+Rp));
			                    //pts.adslLineRateInfo_pt->adslDataRateus = DataRate * 1000 + DataRate_remain;
			                    pts.adslLineRateInfo_pt->adslDataRateus = DataRate;
				}		
			}
			
			if(IS_FLAG_SET((&(pts.adslLineRateInfo_pt->flags)), LINE_RATE_ATTNDRDS_FLAG)){
				LINE_RATE_ATTNDRDS_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group INFO Address 68 Index 4");
#endif
					pts.adslLineRateInfo_pt->adslATTNDRds = 0;	
				}
				else{
					pts.adslLineRateInfo_pt->adslATTNDRds = (u32)(RxMessage[4]) + (((u32)(RxMessage[5]))<<16);
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslLineRateInfo_pt->flags)), LINE_RATE_ATTNDRUS_FLAG)){
				if (adsl_mode <=8 && adsl_mode_extend==0) // adsl mode
				{
					LINE_RATE_ATTNDRUS_FLAG_MAKECMV;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
	#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group INFO Address 69 Index 4");
	#endif
						pts.adslLineRateInfo_pt->adslATTNDRus = 0;	
					}
					else{
						pts.adslLineRateInfo_pt->adslATTNDRus = (u32)(RxMessage[4]) + (((u32)(RxMessage[5]))<<16);
					}
				}else
				{
					hdlc_cmd[0]=0x0181;
					hdlc_cmd[1]=0x24;
					up(&mei_sema);
					if (ifx_me_hdlc_send((unsigned char *)&hdlc_cmd[0],4)!= -EBUSY)
					{
						set_current_state(TASK_INTERRUPTIBLE);
						schedule_timeout(1);		
						hdlc_rx_len=0;			
						hdlc_rx_len = ifx_mei_hdlc_read(&hdlc_rx_buffer,32*2);
						if (hdlc_rx_len <=0)
						{
	                				meierr = -ERESTARTSYS;
	                				goto GET_ADSL_LINE_RATE_END;
	                			}
						pts.adslLineRateInfo_pt->adslATTNDRus = (u32)le16_to_cpu(hdlc_rx_buffer[1])<<16 | (u32)le16_to_cpu(hdlc_rx_buffer[2]);
					}
					if(down_interruptible(&mei_sema))
					{
                				meierr = -ERESTARTSYS;
                				goto GET_ADSL_LINE_RATE_END;
                			}
				}
			}	
			copy_to_user((char *)lon, (char *)pts.adslLineRateInfo_pt, sizeof(adslLineRateInfo));
			up(&mei_sema);
			
GET_ADSL_LINE_RATE_END:						
			kfree(pts.adslLineRateInfo_pt);
			break;

		case GET_ADSL_LINE_INFO:
			if (showtime!=1)
				return -ERESTARTSYS;
			if(down_interruptible(&mei_sema))
                		return -ERESTARTSYS;
			
			pts.adslLineInfo_pt = (adslLineInfo *)kmalloc(sizeof(adslLineInfo), GFP_KERNEL);
			copy_from_user((char *)pts.adslLineInfo_pt, (char *)lon, sizeof(adslLineInfo));
			
			if(IS_FLAG_SET((&(pts.adslLineInfo_pt->flags)), LINE_INFO_INTLV_DEPTHDS_FLAG)){
				if (chantype.interleave)
					LINE_INFO_INTLV_DEPTHDS_FLAG_LP0_MAKECMV;
				else
					LINE_INFO_INTLV_DEPTHDS_FLAG_LP1_MAKECMV;
					
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group CNFG Address 27 Index 0");
#endif
					pts.adslLineInfo_pt->adslInterleaveDepthds = 0;	
				}
				else{
					pts.adslLineInfo_pt->adslInterleaveDepthds = RxMessage[4];
				}				
			}
			
			if(IS_FLAG_SET((&(pts.adslLineInfo_pt->flags)), LINE_INFO_INTLV_DEPTHUS_FLAG)){
				if (chantype.interleave)
					LINE_INFO_INTLV_DEPTHUS_FLAG_LP0_MAKECMV;
				else
					LINE_INFO_INTLV_DEPTHUS_FLAG_LP1_MAKECMV;
					
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group CNFG Address 16 Index 0");
#endif
					pts.adslLineInfo_pt->adslInterleaveDepthus = 0;	
				}
				else{
					pts.adslLineInfo_pt->adslInterleaveDepthus = RxMessage[4];
				}				
			}
			
			if(IS_FLAG_SET((&(pts.adslLineInfo_pt->flags)), LINE_INFO_LATNDS_FLAG)){
				LINE_INFO_LATNDS_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group INFO Address 68 Index 1");
#endif
					pts.adslLineInfo_pt->adslLATNds = 0;	
				}
				else{
					pts.adslLineInfo_pt->adslLATNds = RxMessage[4];
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslLineInfo_pt->flags)), LINE_INFO_LATNUS_FLAG)){
				if (adsl_mode <=8 && adsl_mode_extend==0) // adsl mode
				{
					LINE_INFO_LATNUS_FLAG_MAKECMV;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group INFO Address 69 Index 1");
#endif
						pts.adslLineInfo_pt->adslLATNus = 0;	
					}
					else{
						pts.adslLineInfo_pt->adslLATNus = RxMessage[4];
					}
				}else
				{
					hdlc_cmd[0]=0x0181;
					hdlc_cmd[1]=0x21;
					up(&mei_sema);
					if (ifx_me_hdlc_send((unsigned char *)&hdlc_cmd[0],4)!= -EBUSY)
					{
						set_current_state(TASK_INTERRUPTIBLE);
						schedule_timeout(1);		
						hdlc_rx_len=0;			
						hdlc_rx_len = ifx_mei_hdlc_read(&hdlc_rx_buffer,32*2);
						if (hdlc_rx_len <=0)
						{
	                				meierr = -ERESTARTSYS;
	                				goto GET_ADSL_LINE_INFO_END;
	                			}
						pts.adslLineInfo_pt->adslLATNus = le16_to_cpu(hdlc_rx_buffer[1]);
					}
					if(down_interruptible(&mei_sema))
					{
                				meierr = -ERESTARTSYS;
                				goto GET_ADSL_LINE_INFO_END;
                			}
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslLineInfo_pt->flags)), LINE_INFO_SATNDS_FLAG)){
				LINE_INFO_SATNDS_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group INFO Address 68 Index 2");
#endif
					pts.adslLineInfo_pt->adslSATNds = 0;	
				}
				else{
					pts.adslLineInfo_pt->adslSATNds = RxMessage[4];
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslLineInfo_pt->flags)), LINE_INFO_SATNUS_FLAG)){
				if (adsl_mode <=8 && adsl_mode_extend==0) // adsl mode
				{
					LINE_INFO_SATNUS_FLAG_MAKECMV;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group INFO Address 69 Index 2");
#endif
						pts.adslLineInfo_pt->adslSATNus = 0;	
					}
					else{
						pts.adslLineInfo_pt->adslSATNus = RxMessage[4];
					}
				}else
				{
					hdlc_cmd[0]=0x0181;
					hdlc_cmd[1]=0x22;
					up(&mei_sema);
					if (ifx_me_hdlc_send((unsigned char *)&hdlc_cmd[0],4)!= -EBUSY)
					{
						set_current_state(TASK_INTERRUPTIBLE);
						schedule_timeout(1);		
						hdlc_rx_len=0;			
						hdlc_rx_len = ifx_mei_hdlc_read(&hdlc_rx_buffer,32*2);
						if (hdlc_rx_len <=0)
						{
	                				meierr = -ERESTARTSYS;
	                				goto GET_ADSL_LINE_INFO_END;
	                			}
						pts.adslLineInfo_pt->adslSATNus = le16_to_cpu(hdlc_rx_buffer[1]);
					}
					if(down_interruptible(&mei_sema))
                			{
                				meierr = -ERESTARTSYS;
                				goto GET_ADSL_LINE_INFO_END;
                			}
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslLineInfo_pt->flags)), LINE_INFO_SNRMNDS_FLAG)){
				if (adsl_mode <=8 && adsl_mode_extend==0) // adsl mode
				{
					LINE_INFO_SNRMNDS_FLAG_ADSL1_MAKECMV;
				}
				else if ((adsl_mode == 0x4000) || (adsl_mode == 0x8000) || adsl_mode_extend > 0)
				{
					LINE_INFO_SNRMNDS_FLAG_ADSL2PLUS_MAKECMV;
				}
				else
				{
					LINE_INFO_SNRMNDS_FLAG_ADSL2_MAKECMV;
				}
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group INFO Address 68 Index 3");
#endif
					pts.adslLineInfo_pt->adslSNRMds = 0;	
				}
				else{
					if (adsl_mode>8 || adsl_mode_extend>0)
					{
						int SNRMds,SNRMds_remain;
						SNRMds=RxMessage[4]; 	
						SNRMds_remain=((SNRMds&0xff)*1000)/256;
						SNRMds=(SNRMds>>8)&0xff;
						if ((SNRMds_remain%100)>=50) SNRMds_remain=(SNRMds_remain/100)+1;
						else  SNRMds_remain=(SNRMds_remain/100);
						pts.adslLineInfo_pt->adslSNRMds = SNRMds*10 + SNRMds_remain;
					}else
					{
						pts.adslLineInfo_pt->adslSNRMds = RxMessage[4];
					}
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslLineInfo_pt->flags)), LINE_INFO_SNRMNUS_FLAG)){
				if (adsl_mode <=8 && adsl_mode_extend == 0)
				{
					LINE_INFO_SNRMNUS_FLAG_MAKECMV;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group INFO Address 69 Index 3");
#endif
						pts.adslLineInfo_pt->adslSNRMus = 0;	
					}
					else{
						pts.adslLineInfo_pt->adslSNRMus = RxMessage[4];
					}
				}else
				{
					hdlc_cmd[0]=0x0181;
					hdlc_cmd[1]=0x23;
					up(&mei_sema);
					if (ifx_me_hdlc_send((unsigned char *)&hdlc_cmd[0],4)!= -EBUSY)
					{
						set_current_state(TASK_INTERRUPTIBLE);
						schedule_timeout(1);		
						hdlc_rx_len=0;			
						hdlc_rx_len = ifx_mei_hdlc_read(&hdlc_rx_buffer,32*2);
						if (hdlc_rx_len <=0)
						{
	                				meierr = -ERESTARTSYS;
	                				goto GET_ADSL_LINE_INFO_END;
	                			}
						pts.adslLineInfo_pt->adslSNRMus = le16_to_cpu(hdlc_rx_buffer[1]);
					}
					if(down_interruptible(&mei_sema))
                			{
                				meierr = -ERESTARTSYS;
                				goto GET_ADSL_LINE_INFO_END;
                			}
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslLineInfo_pt->flags)), LINE_INFO_ACATPDS_FLAG)){
				if (adsl_mode <=8 && adsl_mode_extend == 0)
				{
					LINE_INFO_ACATPDS_FLAG_MAKECMV;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
	#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group INFO Address 68 Index 6");
	#endif
						pts.adslLineInfo_pt->adslACATPds = 0;	
					}
					else{
						pts.adslLineInfo_pt->adslACATPds = RxMessage[4];
					}
				}else
				{
					hdlc_cmd[0]=0x0181;
					hdlc_cmd[1]=0x25;
					up(&mei_sema);
					if (ifx_me_hdlc_send((unsigned char *)&hdlc_cmd[0],4)!= -EBUSY)
					{
						set_current_state(TASK_INTERRUPTIBLE);
						schedule_timeout(1);		
						hdlc_rx_len=0;			
						hdlc_rx_len = ifx_mei_hdlc_read(&hdlc_rx_buffer,32*2);
						if (hdlc_rx_len <=0)
						{
	                				meierr = -ERESTARTSYS;
	                				goto GET_ADSL_LINE_INFO_END;
	                			}
						pts.adslLineInfo_pt->adslACATPds = le16_to_cpu(hdlc_rx_buffer[1]);
					}
					if(down_interruptible(&mei_sema))
					{
                				meierr = -ERESTARTSYS;
                				goto GET_ADSL_LINE_INFO_END;
                			}
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslLineInfo_pt->flags)), LINE_INFO_ACATPUS_FLAG)){
				if (adsl_mode <=8 && adsl_mode_extend == 0)
				{
					LINE_INFO_ACATPUS_FLAG_MAKECMV;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group INFO Address 69 Index 6");
#endif
						pts.adslLineInfo_pt->adslACATPus = 0;	
					}
					else{
						pts.adslLineInfo_pt->adslACATPus = RxMessage[4];
					}
				}else
				{
					hdlc_cmd[0]=0x0181;
					hdlc_cmd[1]=0x26;
					up(&mei_sema);
					if (ifx_me_hdlc_send((unsigned char *)&hdlc_cmd[0],4)!= -EBUSY)
					{
						set_current_state(TASK_INTERRUPTIBLE);
						schedule_timeout(1);		
						hdlc_rx_len=0;			
						hdlc_rx_len = ifx_mei_hdlc_read(&hdlc_rx_buffer,32*2);
						if (hdlc_rx_len <=0)
						{
	                				meierr = -ERESTARTSYS;
	                				goto GET_ADSL_LINE_INFO_END;
	                			}
						pts.adslLineInfo_pt->adslACATPus = le16_to_cpu(hdlc_rx_buffer[1]);
					}
					if(down_interruptible(&mei_sema))
					{
                				meierr = -ERESTARTSYS;
                				goto GET_ADSL_LINE_INFO_END;
                			}
				}
			}
			
			copy_to_user((char *)lon, (char *)pts.adslLineInfo_pt, sizeof(adslLineInfo));
			up(&mei_sema);

GET_ADSL_LINE_INFO_END:			
			kfree(pts.adslLineInfo_pt);
			break;

		case GET_ADSL_NEAREND_STATS:
			if (showtime!=1)
				return -ERESTARTSYS;
			if(down_interruptible(&mei_sema))
                		return -ERESTARTSYS;
			
			pts.adslNearEndPerfStats_pt = (adslNearEndPerfStats *)kmalloc(sizeof(adslNearEndPerfStats), GFP_KERNEL);
			copy_from_user((char *)pts.adslNearEndPerfStats_pt, (char *)lon, sizeof(adslNearEndPerfStats));
			
			if(IS_FLAG_SET((&(pts.adslNearEndPerfStats_pt->flags)), NEAREND_PERF_SUPERFRAME_FLAG)){
				NEAREND_PERF_SUPERFRAME_FLAG_LSW_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group PLAM Address 20 Index 0");
#endif
					pts.adslNearEndPerfStats_pt->adslSuperFrames = 0;	
				}
				else{
					pts.adslNearEndPerfStats_pt->adslSuperFrames = (u32)(RxMessage[4]);
				}
				NEAREND_PERF_SUPERFRAME_FLAG_MSW_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group PLAM Address 21 Index 0");
#endif
					pts.adslNearEndPerfStats_pt->adslSuperFrames = 0;	
				}
				else{
					pts.adslNearEndPerfStats_pt->adslSuperFrames += (((u32)(RxMessage[4]))<<16);
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslNearEndPerfStats_pt->flags)), NEAREND_PERF_LOS_FLAG) || 
			   IS_FLAG_SET((&(pts.adslNearEndPerfStats_pt->flags)), NEAREND_PERF_LOF_FLAG) ||
			   IS_FLAG_SET((&(pts.adslNearEndPerfStats_pt->flags)), NEAREND_PERF_LPR_FLAG) ||
			   IS_FLAG_SET((&(pts.adslNearEndPerfStats_pt->flags)), NEAREND_PERF_NCD_FLAG) ||
			   IS_FLAG_SET((&(pts.adslNearEndPerfStats_pt->flags)), NEAREND_PERF_LCD_FLAG) ){
				NEAREND_PERF_LOS_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group PLAM Address 0 Index 0");
#endif					
					RxMessage[4] = 0;
				}
				if(IS_FLAG_SET((&(pts.adslNearEndPerfStats_pt->flags)), NEAREND_PERF_LOS_FLAG)){
					if( (RxMessage[4]&0x1) == 0x1)
						pts.adslNearEndPerfStats_pt->adslneLOS = 1;
					else
						pts.adslNearEndPerfStats_pt->adslneLOS = 0;
				}
				
				if(IS_FLAG_SET((&(pts.adslNearEndPerfStats_pt->flags)), NEAREND_PERF_LOF_FLAG)){
					if( (RxMessage[4]&0x2) == 0x2)
						pts.adslNearEndPerfStats_pt->adslneLOF = 1;
					else
						pts.adslNearEndPerfStats_pt->adslneLOF = 0;
				}
				
				if(IS_FLAG_SET((&(pts.adslNearEndPerfStats_pt->flags)), NEAREND_PERF_LPR_FLAG)){
					if( (RxMessage[4]&0x4) == 0x4)
						pts.adslNearEndPerfStats_pt->adslneLPR = 1;
					else
						pts.adslNearEndPerfStats_pt->adslneLPR = 0;
				}
				
				if(IS_FLAG_SET((&(pts.adslNearEndPerfStats_pt->flags)), NEAREND_PERF_NCD_FLAG)){
					pts.adslNearEndPerfStats_pt->adslneNCD = (RxMessage[4]>>4)&0x3;
				}
				
				if(IS_FLAG_SET((&(pts.adslNearEndPerfStats_pt->flags)), NEAREND_PERF_LCD_FLAG)){
					pts.adslNearEndPerfStats_pt->adslneLCD = (RxMessage[4]>>6)&0x3;
				}
			}
						
			if(IS_FLAG_SET((&(pts.adslNearEndPerfStats_pt->flags)), NEAREND_PERF_CRC_FLAG)){
				if (chantype.interleave)
					NEAREND_PERF_CRC_FLAG_LP0_MAKECMV;
				else
					NEAREND_PERF_CRC_FLAG_LP1_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group PLAM Address 2 Index 0");
#endif
					pts.adslNearEndPerfStats_pt->adslneCRC = 0;	
				}
				else{
					pts.adslNearEndPerfStats_pt->adslneCRC = (u32)(RxMessage[4]) + (((u32)(RxMessage[5]))<<16);
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslNearEndPerfStats_pt->flags)), NEAREND_PERF_RSCORR_FLAG)){
				if (chantype.interleave)
					NEAREND_PERF_RSCORR_FLAG_LP0_MAKECMV;
				else
					NEAREND_PERF_RSCORR_FLAG_LP1_MAKECMV;
					
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group PLAM Address 3 Index 0");
#endif
					pts.adslNearEndPerfStats_pt->adslneRSCorr = 0;	
				}
				else{
					pts.adslNearEndPerfStats_pt->adslneRSCorr = RxMessage[4];
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslNearEndPerfStats_pt->flags)), NEAREND_PERF_FECS_FLAG)){
				NEAREND_PERF_FECS_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group PLAM Address 6 Index 0");
#endif
					pts.adslNearEndPerfStats_pt->adslneFECS = 0;	
				}
				else{
					pts.adslNearEndPerfStats_pt->adslneFECS = RxMessage[4];
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslNearEndPerfStats_pt->flags)), NEAREND_PERF_ES_FLAG)){
				NEAREND_PERF_ES_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group PLAM Address 7 Index 0");
#endif
					pts.adslNearEndPerfStats_pt->adslneES = 0;	
				}
				else{
					pts.adslNearEndPerfStats_pt->adslneES = RxMessage[4];
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslNearEndPerfStats_pt->flags)), NEAREND_PERF_SES_FLAG)){
				NEAREND_PERF_SES_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group PLAM Address 8 Index 0");
#endif
					pts.adslNearEndPerfStats_pt->adslneSES = 0;	
				}
				else{
					pts.adslNearEndPerfStats_pt->adslneSES = RxMessage[4];
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslNearEndPerfStats_pt->flags)), NEAREND_PERF_LOSS_FLAG)){
				NEAREND_PERF_LOSS_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group PLAM Address 9 Index 0");
#endif
					pts.adslNearEndPerfStats_pt->adslneLOSS = 0;	
				}
				else{
					pts.adslNearEndPerfStats_pt->adslneLOSS = RxMessage[4];
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslNearEndPerfStats_pt->flags)), NEAREND_PERF_UAS_FLAG)){
				NEAREND_PERF_UAS_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group PLAM Address 10 Index 0");
#endif
					pts.adslNearEndPerfStats_pt->adslneUAS = 0;	
				}
				else{
					pts.adslNearEndPerfStats_pt->adslneUAS = RxMessage[4];
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslNearEndPerfStats_pt->flags)), NEAREND_PERF_HECERR_FLAG)){
				if (chantype.bearchannel0)
				{
					NEAREND_PERF_HECERR_FLAG_BC0_MAKECMV;
				}else if (chantype.bearchannel1)
				{
					NEAREND_PERF_HECERR_FLAG_BC1_MAKECMV;
				}
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group PLAM Address 11 Index 0");
#endif
					pts.adslNearEndPerfStats_pt->adslneHECErrors = 0;	
				}
				else{
					pts.adslNearEndPerfStats_pt->adslneHECErrors = (u32)(RxMessage[4]) + (((u32)(RxMessage[5]))<<16);
				}
			}
			
			copy_to_user((char *)lon, (char *)pts.adslNearEndPerfStats_pt, sizeof(adslNearEndPerfStats));
			kfree(pts.adslNearEndPerfStats_pt);
			
			up(&mei_sema);
			break;

		case GET_ADSL_FAREND_STATS:

                	if (showtime!=1)
                		return -ERESTARTSYS;
			
			if (adsl_mode>8 || adsl_mode_extend > 0)
			{
				do_gettimeofday(&time_now);
				if( FarendData_acquire_time.tv_sec==0 || time_now.tv_sec - FarendData_acquire_time.tv_sec>=1)
				{								
					hdlc_cmd[0]=0x105;
					
					if (ifx_me_hdlc_send((unsigned char *)&hdlc_cmd[0],2)!= -EBUSY)
					{
						set_current_state(TASK_INTERRUPTIBLE);
						schedule_timeout(1);		
						hdlc_rx_len=0;			
						hdlc_rx_len = ifx_mei_hdlc_read(&hdlc_rx_buffer,32*2);
						if (hdlc_rx_len <=0)
						{
							return -ERESTARTSYS;
						}
						FarendStatsData.adslfeRSCorr = ((u32)le16_to_cpu(hdlc_rx_buffer[1]) << 16) + (u32)le16_to_cpu(hdlc_rx_buffer[2]);
						FarendStatsData.adslfeCRC = ((u32)le16_to_cpu(hdlc_rx_buffer[3]) << 16) + (u32)le16_to_cpu(hdlc_rx_buffer[4]);
						FarendStatsData.adslfeFECS = ((u32)le16_to_cpu(hdlc_rx_buffer[5]) << 16) + (u32)le16_to_cpu(hdlc_rx_buffer[6]);
						FarendStatsData.adslfeES = ((u32)le16_to_cpu(hdlc_rx_buffer[7]) << 16) + (u32)le16_to_cpu(hdlc_rx_buffer[8]);
						FarendStatsData.adslfeSES = ((u32)le16_to_cpu(hdlc_rx_buffer[9]) << 16) + (u32)le16_to_cpu(hdlc_rx_buffer[10]);
						FarendStatsData.adslfeLOSS = ((u32)le16_to_cpu(hdlc_rx_buffer[11]) << 16) + (u32)le16_to_cpu(hdlc_rx_buffer[12]);
						FarendStatsData.adslfeUAS = ((u32)le16_to_cpu(hdlc_rx_buffer[13]) << 16) + (u32)le16_to_cpu(hdlc_rx_buffer[14]);
						do_gettimeofday(&FarendData_acquire_time);
					}
					
				}
			}
			
			if(down_interruptible(&mei_sema))
                		return -ERESTARTSYS;
                	pts.adslFarEndPerfStats_pt = (adslFarEndPerfStats *)kmalloc(sizeof(adslFarEndPerfStats), GFP_KERNEL);
			copy_from_user((char *)pts.adslFarEndPerfStats_pt, (char *)lon, sizeof(adslFarEndPerfStats));
			if(IS_FLAG_SET((&(pts.adslFarEndPerfStats_pt->flags)), FAREND_PERF_LOS_FLAG) || 
			   IS_FLAG_SET((&(pts.adslFarEndPerfStats_pt->flags)), FAREND_PERF_LOF_FLAG) ||
			   IS_FLAG_SET((&(pts.adslFarEndPerfStats_pt->flags)), FAREND_PERF_LPR_FLAG) ||
			   IS_FLAG_SET((&(pts.adslFarEndPerfStats_pt->flags)), FAREND_PERF_NCD_FLAG) ||
			   IS_FLAG_SET((&(pts.adslFarEndPerfStats_pt->flags)), FAREND_PERF_LCD_FLAG) ){
				FAREND_PERF_LOS_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group PLAM Address 1 Index 0");
#endif					
					RxMessage[4] = 0;
				}
				if(IS_FLAG_SET((&(pts.adslFarEndPerfStats_pt->flags)), FAREND_PERF_LOS_FLAG)){
					if((RxMessage[4]&0x1) == 0x1)
						pts.adslFarEndPerfStats_pt->adslfeLOS = 1;
					else
						pts.adslFarEndPerfStats_pt->adslfeLOS = 0;
				}
				
				if(IS_FLAG_SET((&(pts.adslFarEndPerfStats_pt->flags)), FAREND_PERF_LOF_FLAG)){
					if((RxMessage[4]&0x2) == 0x2)
						pts.adslFarEndPerfStats_pt->adslfeLOF = 1;
					else
						pts.adslFarEndPerfStats_pt->adslfeLOF = 0;
				}
				
				if(IS_FLAG_SET((&(pts.adslFarEndPerfStats_pt->flags)), FAREND_PERF_LPR_FLAG)){
					if((RxMessage[4]&0x4) == 0x4)
						pts.adslFarEndPerfStats_pt->adslfeLPR = 1;
					else
						pts.adslFarEndPerfStats_pt->adslfeLPR = 0;
				}
				
				if(IS_FLAG_SET((&(pts.adslFarEndPerfStats_pt->flags)), FAREND_PERF_NCD_FLAG)){
					pts.adslFarEndPerfStats_pt->adslfeNCD = (RxMessage[4]>>4)&0x3;
				}
				
				if(IS_FLAG_SET((&(pts.adslFarEndPerfStats_pt->flags)), FAREND_PERF_LCD_FLAG)){
					pts.adslFarEndPerfStats_pt->adslfeLCD = (RxMessage[4]>>6)&0x3;
				}
			}
						
			if(IS_FLAG_SET((&(pts.adslFarEndPerfStats_pt->flags)), FAREND_PERF_CRC_FLAG)){
				if (adsl_mode<=8 && adsl_mode_extend == 0)
				{
					if (chantype.interleave)
					{
						FAREND_PERF_CRC_FLAG_LP0_MAKECMV;
					}
					else
					{
						FAREND_PERF_CRC_FLAG_LP1_MAKECMV;
					}
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group PLAM Address 24 Index 0");
#endif
						pts.adslFarEndPerfStats_pt->adslfeCRC = 0;	
					}
					else{
						pts.adslFarEndPerfStats_pt->adslfeCRC = RxMessage[4];					
					}
				}else
				{
					pts.adslFarEndPerfStats_pt->adslfeCRC = FarendStatsData.adslfeCRC;
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslFarEndPerfStats_pt->flags)), FAREND_PERF_RSCORR_FLAG)){
				if (adsl_mode<=8 && adsl_mode_extend == 0)
				{
					if (chantype.interleave)
						FAREND_PERF_RSCORR_FLAG_LP0_MAKECMV;
					else
						FAREND_PERF_RSCORR_FLAG_LP1_MAKECMV;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group PLAM Address 28 Index 0");
#endif
						pts.adslFarEndPerfStats_pt->adslfeRSCorr = 0;	
					}
					else{
						pts.adslFarEndPerfStats_pt->adslfeRSCorr = RxMessage[4];
						
					}
				}
				else
				{
					pts.adslFarEndPerfStats_pt->adslfeRSCorr = FarendStatsData.adslfeRSCorr;
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslFarEndPerfStats_pt->flags)), FAREND_PERF_FECS_FLAG)){
				if (adsl_mode<=8 && adsl_mode_extend == 0)
				{
					FAREND_PERF_FECS_FLAG_MAKECMV;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group PLAM Address 32 Index 0");
#endif
						pts.adslFarEndPerfStats_pt->adslfeFECS = 0;	
					}
					else{
						pts.adslFarEndPerfStats_pt->adslfeFECS = RxMessage[4];						
					}
				}else {
					pts.adslFarEndPerfStats_pt->adslfeFECS = FarendStatsData.adslfeFECS;
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslFarEndPerfStats_pt->flags)), FAREND_PERF_ES_FLAG)){
				if (adsl_mode<=8 && adsl_mode_extend == 0)
				{
					FAREND_PERF_ES_FLAG_MAKECMV;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group PLAM Address 33 Index 0");
#endif
						pts.adslFarEndPerfStats_pt->adslfeES = 0;	
					}
					else{
						pts.adslFarEndPerfStats_pt->adslfeES = RxMessage[4];						
					}
				}else
				{
					pts.adslFarEndPerfStats_pt->adslfeES = FarendStatsData.adslfeES;
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslFarEndPerfStats_pt->flags)), FAREND_PERF_SES_FLAG)){
				if (adsl_mode<=8 && adsl_mode_extend == 0)
				{
					FAREND_PERF_SES_FLAG_MAKECMV;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group PLAM Address 34 Index 0");
#endif
						pts.adslFarEndPerfStats_pt->adslfeSES = 0;	
					}
					else{
						pts.adslFarEndPerfStats_pt->adslfeSES = RxMessage[4];
						
					}
				}else
				{
					pts.adslFarEndPerfStats_pt->adslfeSES = FarendStatsData.adslfeSES;
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslFarEndPerfStats_pt->flags)), FAREND_PERF_LOSS_FLAG)){
				if (adsl_mode<=8 && adsl_mode_extend == 0)
				{
					FAREND_PERF_LOSS_FLAG_MAKECMV;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group PLAM Address 35 Index 0");
#endif
						pts.adslFarEndPerfStats_pt->adslfeLOSS = 0;	
					}
					else{
						pts.adslFarEndPerfStats_pt->adslfeLOSS = RxMessage[4];
						
					}
				}else
				{
					pts.adslFarEndPerfStats_pt->adslfeLOSS = FarendStatsData.adslfeLOSS;
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslFarEndPerfStats_pt->flags)), FAREND_PERF_UAS_FLAG)){
				if (adsl_mode<=8 && adsl_mode_extend == 0)
				{
					FAREND_PERF_UAS_FLAG_MAKECMV;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group PLAM Address 36 Index 0");
#endif
						pts.adslFarEndPerfStats_pt->adslfeUAS = 0;	
					}
					else{
						pts.adslFarEndPerfStats_pt->adslfeUAS = RxMessage[4];
						
					}
				}else
				{
					pts.adslFarEndPerfStats_pt->adslfeUAS = FarendStatsData.adslfeUAS;
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslFarEndPerfStats_pt->flags)), FAREND_PERF_HECERR_FLAG)){
				if (chantype.bearchannel0)
				{
					FAREND_PERF_HECERR_FLAG_BC0_MAKECMV;
				}else if (chantype.bearchannel1)
				{
					FAREND_PERF_HECERR_FLAG_BC1_MAKECMV;
				}
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group PLAM Address 37 Index 0");
#endif
					pts.adslFarEndPerfStats_pt->adslfeHECErrors = 0;	
				}
				else{
					pts.adslFarEndPerfStats_pt->adslfeHECErrors = (u32)(RxMessage[4]) + (((u32)(RxMessage[5]))<<16);
				}
			}
			
			copy_to_user((char *)lon, (char *)pts.adslFarEndPerfStats_pt, sizeof(adslFarEndPerfStats));		
			kfree(pts.adslFarEndPerfStats_pt);
			
			up(&mei_sema);
			
			break;
// 603221:tc.chen end
		case GET_ADSL_LOOP_DIAGNOSTICS_MODE:
			//lon = loop_diagnostics_mode;
			copy_to_user((char *)lon, (char *)&loop_diagnostics_mode, sizeof(int));	
			break;
//>> SHC
		case IS_ADSL_LOOP_DIAGNOSTICS_MODE_COMPLETE:
			copy_to_user((char *)lon, (char *)&loop_diagnostics_completed, sizeof(int));	
			break;
			
//<< end SHC
		case LOOP_DIAGNOSTIC_MODE_COMPLETE:
			loop_diagnostics_completed = 1;
			// read adsl mode
			makeCMV(H2D_CMV_READ, STAT, 1, 0, 1, data); 
			if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
				printk("\n\nCMV fail, Group STAT Address 1 Index 0");
#endif
			}
			adsl_mode = RxMessage[4];

			makeCMV(H2D_CMV_READ, STAT, 17, 0, 1, data); 
			if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
				printk("\n\nCMV fail, Group STAT Address 1 Index 0");
#endif
			}
			adsl_mode_extend = RxMessage[4];
			wake_up_interruptible(&wait_queue_loop_diagnostic);	
			break;
		case SET_ADSL_LOOP_DIAGNOSTICS_MODE:
			if (lon != loop_diagnostics_mode)
			{
				loop_diagnostics_completed = 0;
				loop_diagnostics_mode = lon;

				mei_ioctl((struct inode *)0,NULL, AMAZON_MEI_REBOOT, NULL);
				
			}
			break;
		case GET_ADSL_ATUR_SUBCARRIER_STATS:
			if (loop_diagnostics_completed == 0)
			{
				interruptible_sleep_on_timeout(&wait_queue_loop_diagnostic,300*HZ);
				if (loop_diagnostics_completed==0)
				{
					return -ERESTARTSYS;
				}
			}
			if(down_interruptible(&mei_sema))
                		return -ERESTARTSYS;
                		
                	pts.adslATURSubcarrierInfo_pt = (adslATURSubcarrierInfo *)kmalloc(sizeof(adslATURSubcarrierInfo), GFP_KERNEL);
			copy_from_user((char *)pts.adslATURSubcarrierInfo_pt, (char *)lon, sizeof(adslATURSubcarrierInfo));
			
			if(IS_FLAG_SET((&(pts.adslATURSubcarrierInfo_pt->flags)),  FAREND_HLINSC)){
				FAREND_HLINSC_MAKECMV(H2D_CMV_READ);
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group PLAM Address 35 Index 0");
#endif
					pts.adslATURSubcarrierInfo_pt->HLINSCds = 0;	
				}
				else{
					pts.adslATURSubcarrierInfo_pt->HLINSCds = RxMessage[4];
					
				}
			}
			if(IS_FLAG_SET((&(pts.adslATURSubcarrierInfo_pt->flags)),  FAREND_HLINPS)){
				int index=0,size=12;
				//printk("FAREND_HLINPS\n");
				for (index=0;index<1024;index+=size)
				{
					if (index+size>=1024)
						size = 1024-index;
					FAREND_HLINPS_MAKECMV(H2D_CMV_READ,index,size);
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group PLAM Address 35 Index 0");
#endif
					}
					else{
						memcpy(&pts.adslATURSubcarrierInfo_pt->HLINpsds[index],&RxMessage[4],size*2);
#if 0
						int msg_idx;
						for(msg_idx=0;msg_idx<size;msg_idx++)
							printk("index:%d ,cmv_result: %04X\n",index+msg_idx,RxMessage[4+msg_idx]);
#endif
					}
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslATURSubcarrierInfo_pt->flags)),  FAREND_HLOGMT)){
				FAREND_HLOGMT_MAKECMV(H2D_CMV_READ);
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group PLAM Address 35 Index 0");
#endif
					pts.adslATURSubcarrierInfo_pt->HLOGMTds = 0;	
				}
				else{
					pts.adslATURSubcarrierInfo_pt->HLOGMTds = RxMessage[4];
					
				}
			}
			
			/////////////////////////////////////////////////////////////////////////
			if(IS_FLAG_SET((&(pts.adslATURSubcarrierInfo_pt->flags)),  FAREND_HLOGPS)){
				//printk("FAREND_HLOGPS\n");
				int index=0,size=12;
				for (index=0;index<256;index+=size)
				{
					if (index+size>=256)
						size = 256-index;

					FAREND_HLOGPS_MAKECMV(H2D_CMV_READ,index,size);
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group PLAM Address 35 Index 0");
#endif
					}
					else{
						if (adsl_mode < 0x4000 && adsl_mode_extend==0)//adsl2 mode
						{
							memcpy(&pts.adslATURSubcarrierInfo_pt->HLOGpsds[index],&RxMessage[4],size*2);
						}else
						{
							int msg_idx=0;
							for (msg_idx=0;msg_idx<size;msg_idx++)
							{
								pts.adslATURSubcarrierInfo_pt->HLOGpsds[(index+msg_idx)*2+1] = RxMessage[4+msg_idx];
								//printk("index:%d ,cmv_result: %04X\n",index+msg_idx,RxMessage[4+msg_idx]);
							}
						}	
					}
				}
				if (adsl_mode >= 0x4000 || adsl_mode_extend >0)//adsl2+ mode
				{
					pts.adslATURSubcarrierInfo_pt->HLOGpsds[0] = pts.adslATURSubcarrierInfo_pt->HLOGpsds[1];
					for (index=1;index<256;index++)
					{
						pts.adslATURSubcarrierInfo_pt->HLOGpsds[index*2]   = (pts.adslATURSubcarrierInfo_pt->HLOGpsds[(index)*2-1] +  pts.adslATURSubcarrierInfo_pt->HLOGpsds[(index)*2+1] +1) >>1;
					}
				}
			}
			if(IS_FLAG_SET((&(pts.adslATURSubcarrierInfo_pt->flags)),  FAREND_QLNMT)){
				FAREND_QLNMT_MAKECMV(H2D_CMV_READ);
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group PLAM Address 35 Index 0");
#endif
					pts.adslATURSubcarrierInfo_pt->QLNMTds = 0;	
				}
				else{
					pts.adslATURSubcarrierInfo_pt->QLNMTds = RxMessage[4];					
				}
			}
			
			/////////////////////////////////////////////////////////////////////////
			if(IS_FLAG_SET((&(pts.adslATURSubcarrierInfo_pt->flags)),  FAREND_QLNPS)){
				int index=0,size=12;
				//printk("FAREND_QLNPS\n");
				for (index=0;index<128;index+=size)
				{
					if (index+size>=128)
						size = 128-index;
					FAREND_QLNPS_MAKECMV(H2D_CMV_READ,index,size);
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group PLAM Address 35 Index 0");
#endif
					}
					else{
						int msg_idx=0;
						for (msg_idx=0;msg_idx<size;msg_idx++)
						{
							//memcpy(&pts.adslATURSubcarrierInfo_pt->QLNpsds[index],&RxMessage[4],size*2);
							if (adsl_mode < 0x4000 && adsl_mode_extend==0)//adsl2 mode
							{
								pts.adslATURSubcarrierInfo_pt->QLNpsds[(index+msg_idx)*2] = (u16)(RxMessage[4+msg_idx]&0xFF);
								pts.adslATURSubcarrierInfo_pt->QLNpsds[(index+msg_idx)*2+1] = (u16)((RxMessage[4+msg_idx]>>8)&0xFF);
							}else
							{
								pts.adslATURSubcarrierInfo_pt->QLNpsds[(index+msg_idx)*4+1] = (u16)(RxMessage[4+msg_idx]&0xFF);
								pts.adslATURSubcarrierInfo_pt->QLNpsds[(index+msg_idx)*4+3] = (u16)((RxMessage[4+msg_idx]>>8)&0xFF);
								//printk("index:%d ,cmv_result: %04X\n",index+msg_idx,RxMessage[4+msg_idx]);
							}
						}

						
					}
				}
				if (adsl_mode >= 0x4000 || adsl_mode_extend >0)//adsl2+ mode
				{
					pts.adslATURSubcarrierInfo_pt->QLNpsds[0] = pts.adslATURSubcarrierInfo_pt->QLNpsds[1];
					for (index=1;index<256;index++)
					{
						pts.adslATURSubcarrierInfo_pt->QLNpsds[index*2]   = (pts.adslATURSubcarrierInfo_pt->QLNpsds[(index)*2-1] +  pts.adslATURSubcarrierInfo_pt->QLNpsds[(index)*2+1]) >>1;
					}
				}
			}
			if(IS_FLAG_SET((&(pts.adslATURSubcarrierInfo_pt->flags)),  FAREND_SNRMT)){
				FAREND_SNRMT_MAKECMV(H2D_CMV_READ);
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group PLAM Address 35 Index 0");
#endif
					pts.adslATURSubcarrierInfo_pt->SNRMTds = 0;	
				}
				else{
					pts.adslATURSubcarrierInfo_pt->SNRMTds = RxMessage[4];					
				}
			}
			if(IS_FLAG_SET((&(pts.adslATURSubcarrierInfo_pt->flags)),  FAREND_SNRPS)){
				int index=0,size=12;
				//printk("FAREND_SNRPS\n");
				for (index=0;index<512;index+=size)
				{
					if (index+size>=512)
						size = 512-index;
					FAREND_SNRPS_MAKECMV(H2D_CMV_READ,index,size);
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group PLAM Address 35 Index 0");
#endif
					}
					else{
						//memcpy(&pts.adslATURSubcarrierInfo_pt->SNRpsds[index],&RxMessage[4],size*2);
						int msg_idx=0;
						for (msg_idx=0;msg_idx<size;msg_idx++)
						{
							pts.adslATURSubcarrierInfo_pt->SNRpsds[index+msg_idx] = (u16)(RxMessage[4+msg_idx]&0xFF);
							//printk("index:%d ,cmv_result: %04X\n",index+msg_idx,RxMessage[4+msg_idx]);
						}
						
					}
				}
			}
			if(IS_FLAG_SET((&(pts.adslATURSubcarrierInfo_pt->flags)),  FAREND_BITPS)){
				int index=0,size=12;
				//printk("FAREND_BITPS\n");
				for (index=0;index<256;index+=size)
				{
					if (index+size>=256)
						size = 256-index;
					FAREND_BITPS_MAKECMV(H2D_CMV_READ,index,size);
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group PLAM Address 35 Index 0");
#endif
					}
					else{
						int msg_idx=0;
						for (msg_idx=0;msg_idx<size;msg_idx++)
						{
							pts.adslATURSubcarrierInfo_pt->BITpsds[(index+msg_idx)*2] = (u16)(RxMessage[4+msg_idx]&0xFF);
							pts.adslATURSubcarrierInfo_pt->BITpsds[(index+msg_idx)*2+1] = (u16)((RxMessage[4+msg_idx]>>8)&0xFF);
							//printk("index:%d ,cmv_result: %04X, %d\n",index+msg_idx,RxMessage[4+msg_idx],RxMessage[4+msg_idx]);
							
						}
						
					}
				}
			}
			if(IS_FLAG_SET((&(pts.adslATURSubcarrierInfo_pt->flags)),  FAREND_GAINPS)){
				int index=0,size=12;
				//printk("FAREND_GAINPS\n");
				for (index=0;index<512;index+=size)
				{
					FAREND_GAINPS_MAKECMV(H2D_CMV_READ,index,size);
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group PLAM Address 35 Index 0");
#endif
					}
					else{
					/*
						int msg_idx=0;
						for (msg_idx=0;msg_idx<size;msg_idx++)
						{
							pts.adslATURSubcarrierInfo_pt->GAINpsds[(index+msg_idx)*2] = RxMessage[4+msg_idx]&0xFF;
							pts.adslATURSubcarrierInfo_pt->GAINpsds[(index+msg_idx)*2+1] = (RxMessage[4+msg_idx]>>8)&0xFF;
							
						}
						*/
						memcpy(&pts.adslATURSubcarrierInfo_pt->GAINpsds[index],&RxMessage[4],size*2);	
#if 0
						int msg_idx=0;
						for (msg_idx=0;msg_idx<size;msg_idx++)
						{
							printk("index:%d ,cmv_result: %04X\n",index+msg_idx,RxMessage[4+msg_idx]);
							
						}
#endif
					}
				}
			}				
			copy_to_user((char *)lon, (char *)pts.adslATURSubcarrierInfo_pt, sizeof(adslATURSubcarrierInfo));		
			kfree(pts.adslATURSubcarrierInfo_pt);
			
			up(&mei_sema);
			break;
		case GET_ADSL_ATUC_SUBCARRIER_STATS:
			if (loop_diagnostics_completed == 0)
			{
				interruptible_sleep_on_timeout(&wait_queue_loop_diagnostic,300*HZ);
				if (loop_diagnostics_completed==0)
				{
					return -ERESTARTSYS;
				}
			}
			if(down_interruptible(&mei_sema))
                		return -ERESTARTSYS;
                	pts.adslATUCSubcarrierInfo_pt = (adslATUCSubcarrierInfo *)kmalloc(sizeof(adslATUCSubcarrierInfo), GFP_KERNEL);
			copy_from_user((char *)pts.adslATUCSubcarrierInfo_pt, (char *)lon, sizeof(adslATUCSubcarrierInfo));
			
			
			if(IS_FLAG_SET((&(pts.adslATUCSubcarrierInfo_pt->flags)),  NEAREND_HLINSC)){
				NEAREND_HLINSC_MAKECMV(H2D_CMV_READ);
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group INFO Address 71 Index 2");
#endif
					pts.adslATUCSubcarrierInfo_pt->HLINSCus = 0;	
				}
				else{
					pts.adslATUCSubcarrierInfo_pt->HLINSCus = RxMessage[4];
					
				}
			}
			if(IS_FLAG_SET((&(pts.adslATUCSubcarrierInfo_pt->flags)),  NEAREND_HLINPS)){
				int index=0,size=12;
				//printk("NEAREND_HLINPS\n");
				for (index=0;index<128;index+=size)
				{
					if (index+size>=128)
						size = 128-index;
					NEAREND_HLINPS_MAKECMV(H2D_CMV_READ,index,size);
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group INFO Address 73 Index 0");
#endif
					}
					else{
						memcpy(&pts.adslATUCSubcarrierInfo_pt->HLINpsus[index],&RxMessage[4],size*2);
#if 0
						int msg_idx;
						for (msg_idx=0;msg_idx<size;msg_idx++)
						{
							printk("index:%d ,cmv_result: %04X\n",index+msg_idx,RxMessage[4+msg_idx]);
						}
#endif
					}
				}
			}
			
			if(IS_FLAG_SET((&(pts.adslATUCSubcarrierInfo_pt->flags)),  NEAREND_HLOGMT)){
				NEAREND_HLOGMT_MAKECMV(H2D_CMV_READ);
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group INFO Address 80 Index 0");
#endif
					pts.adslATUCSubcarrierInfo_pt->HLOGMTus = 0;	
				}
				else{
					pts.adslATUCSubcarrierInfo_pt->HLOGMTus = RxMessage[4];
					
				}
			}
			
			/////////////////////////////////////////////////////////////////////////
			if(IS_FLAG_SET((&(pts.adslATUCSubcarrierInfo_pt->flags)),  NEAREND_HLOGPS)){
				int index=0,size=12;
				//printk("NEAREND_HLOGPS\n");
				for (index=0;index<64;index+=size)
				{
					if (index+size>=64)
						size = 64-index;
					NEAREND_HLOGPS_MAKECMV(H2D_CMV_READ,index,size);
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group INFO Address 75 Index 0");
#endif
					}
					else{						
#if 0
						if (adsl_mode <0x4000)//adsl /adsl2 mode
						{
#endif
						memcpy(&pts.adslATUCSubcarrierInfo_pt->HLOGpsus[index],&RxMessage[4],size*2);
#if 0
						}else
						{
							int msg_idx=0;
							for (msg_idx=0;msg_idx<size;msg_idx++)
							{
								//pts.adslATUCSubcarrierInfo_pt->HLOGpsus[(index+msg_idx)*2+1] = RxMessage[4+msg_idx];
								pts.adslATUCSubcarrierInfo_pt->HLOGpsus[(index+msg_idx)] = RxMessage[4+msg_idx];
							}
						}						
#endif
					}
				}
#if 0
				if (adsl_mode >= 0x4000)//adsl2 mode
				{
					pts.adslATUCSubcarrierInfo_pt->HLOGpsus[0] = pts.adslATUCSubcarrierInfo_pt->HLOGpsus[1];
					for (index=1;index<64;index++)
					{
						pts.adslATUCSubcarrierInfo_pt->HLOGpsus[index*2]   = (pts.adslATUCSubcarrierInfo_pt->HLOGpsus[(index)*2-1] +  pts.adslATUCSubcarrierInfo_pt->HLOGpsus[(index)*2+1]) >>1;
					}
				}
#endif
			}
			if(IS_FLAG_SET((&(pts.adslATUCSubcarrierInfo_pt->flags)),  NEAREND_QLNMT)){
				NEAREND_QLNMT_MAKECMV(H2D_CMV_READ);
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group INFO Address 80 Index 1");
#endif
					pts.adslATUCSubcarrierInfo_pt->QLNMTus = 0;	
				}
				else{
					pts.adslATUCSubcarrierInfo_pt->QLNMTus = RxMessage[4];					
				}
			}
			
			/////////////////////////////////////////////////////////////////////////
			if(IS_FLAG_SET((&(pts.adslATUCSubcarrierInfo_pt->flags)),  NEAREND_QLNPS)){
				int index=0,size=12;
				//printk("NEAREND_QLNPS\n");
				for (index=0;index<32;index+=size)
				{
					if (index+size>=32)
						size = 32-index;
					NEAREND_QLNPS_MAKECMV(H2D_CMV_READ,index,size);
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group INFO Address 77 Index 0");
#endif
					}
					else{
						int msg_idx=0;
						for (msg_idx=0;msg_idx<size;msg_idx++)
						{

#if 0
							//memcpy(&pts.adslATUCSubcarrierInfo_pt->QLNpsds[index],&RxMessage[4],size*2);
							if (adsl_mode == 0x200 || adsl_mode == 0x800 || adsl_mode ==0x2000  || adsl_mode ==0x4000 || (adsl_mode == 0 && (adsl_mode_extend == 0x4 || adsl_mode_extend == 0x2))//ADSL 2 Annex B(0x200)/J(0x800)/M(0x2000) //ADSL 2+ B,J,M
							if (adsl_mode < 0x4000 && adsl_mode_extend==0)//adsl2 mode
							{
								pts.adslATUCSubcarrierInfo_pt->QLNpsus[(index+msg_idx)*4+1] = (u16)(RxMessage[4+msg_idx]&0xFF);
								pts.adslATUCSubcarrierInfo_pt->QLNpsus[(index+msg_idx)*4+3] = (u16)((RxMessage[4+msg_idx]>>8)&0xFF);
							}else
#endif
							{
								pts.adslATUCSubcarrierInfo_pt->QLNpsus[(index+msg_idx)*2] = (u16)(RxMessage[4+msg_idx]&0xFF);
								pts.adslATUCSubcarrierInfo_pt->QLNpsus[(index+msg_idx)*2+1] = (u16)((RxMessage[4+msg_idx]>>8)&0xFF);
							//printk("index:%d ,cmv_result: %04X\n",index+msg_idx,RxMessage[4+msg_idx]);
							}
						}

						
					}
				}
#if 0
				//if (adsl_mode <0x4000)//Annex I/J/L/M
				if (adsl_mode == 0x200 || adsl_mode == 0x800 || adsl_mode ==0x2000  || adsl_mode ==0x4000 || (adsl_mode == 0 && (adsl_mode_extend == 0x4 || adsl_mode_extend == 0x2))//ADSL 2 Annex B(0x200)/J(0x800)/M(0x2000) //ADSL 2+ B,J,M
				{
					pts.adslATUCSubcarrierInfo_pt->QLNpsus[0] = pts.adslATUCSubcarrierInfo_pt->QLNpsus[1];
					for (index=1;index<64;index++)
					{
						pts.adslATUCSubcarrierInfo_pt->QLNpsus[index*2]   = (pts.adslATUCSubcarrierInfo_pt->QLNpsus[(index)*2-1] +  pts.adslATUCSubcarrierInfo_pt->QLNpsus[(index)*2+1]) >>1;
					}
				}
#endif
			}
			if(IS_FLAG_SET((&(pts.adslATUCSubcarrierInfo_pt->flags)),  NEAREND_SNRMT)){
				NEAREND_SNRMT_MAKECMV(H2D_CMV_READ);
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group INFO Address 80 Index 2");
#endif
					pts.adslATUCSubcarrierInfo_pt->SNRMTus = 0;	
				}
				else{
					pts.adslATUCSubcarrierInfo_pt->SNRMTus = RxMessage[4];					
				}
			}
			if(IS_FLAG_SET((&(pts.adslATUCSubcarrierInfo_pt->flags)),  NEAREND_SNRPS)){
				int index=0,size=12;
				//printk("NEAREND_SNRPS\n");
				for (index=0;index<64;index+=size)
				{
					if (index+size>=64)
						size = 64-index;
					NEAREND_SNRPS_MAKECMV(H2D_CMV_READ,index,size);
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group INFO Address 78 Index 0");
#endif
					}
					else{
						//memcpy(&pts.adslATUCSubcarrierInfo_pt->SNRpsus[index],&RxMessage[4],size*2);
						int msg_idx=0;
						for (msg_idx=0;msg_idx<size;msg_idx++)
						{
							pts.adslATUCSubcarrierInfo_pt->SNRpsus[index+msg_idx] = (u16)(RxMessage[4+msg_idx]&0xFF);
							//printk("index:%d ,cmv_result: %04X\n",index+msg_idx,RxMessage[4+msg_idx]);
						}
						
					}
				}
			}
			if(IS_FLAG_SET((&(pts.adslATUCSubcarrierInfo_pt->flags)),  NEAREND_BITPS)){
				int index=0,size=12;
				//printk("NEAREND_BITPS\n");
				for (index=0;index<32;index+=size)
				{
					if (index+size>=32)
						size = 32-index;
					NEAREND_BITPS_MAKECMV(H2D_CMV_READ,index,size);
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group INFO Address 22 Index 0");
#endif
					}
					else{
						int msg_idx=0;
						for (msg_idx=0;msg_idx<size;msg_idx++)
						{
							pts.adslATUCSubcarrierInfo_pt->BITpsus[(index+msg_idx)*2] = (u16)(RxMessage[4+msg_idx]&0xFF);
							pts.adslATUCSubcarrierInfo_pt->BITpsus[(index+msg_idx)*2+1] = (u16)((RxMessage[4+msg_idx]>>8)&0xFF);
							//printk("index:%d ,cmv_result: %04X\n",index+msg_idx,RxMessage[4+msg_idx]);
						}
						
					}
				}
			}
			if(IS_FLAG_SET((&(pts.adslATUCSubcarrierInfo_pt->flags)),  NEAREND_GAINPS)){
				int index=0,size=12;
				//printk("NEAREND_GAINPS\n");
				for (index=0;index<64;index+=size)
				{
					if (index+size>=64)
						size = 64-index;
					NEAREND_GAINPS_MAKECMV(H2D_CMV_READ,index,size);
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group INFO Address 24 Index 0");
#endif
					}
					else{
					/*
						int msg_idx=0;
						for (msg_idx=0;msg_idx<size;msg_idx++)
						{
							pts.adslATUCSubcarrierInfo_pt->GAINpsds[(index+msg_idx)*2] = RxMessage[4+msg_idx]&0xFF;
							pts.adslATUCSubcarrierInfo_pt->GAINpsds[(index+msg_idx)*2+1] = (RxMessage[4+msg_idx]>>8)&0xFF;
							
						}
						*/
						memcpy(&pts.adslATUCSubcarrierInfo_pt->GAINpsus[index],&RxMessage[4],size*2);	
#if 0
						int msg_idx;
						for (msg_idx=0;msg_idx<size;msg_idx++)
						{
							printk("index:%d ,cmv_result: %04X\n",index+msg_idx,RxMessage[4+msg_idx]);
						}
#endif
					}
				}
			}				
			copy_to_user((char *)lon, (char *)pts.adslATUCSubcarrierInfo_pt, sizeof(adslATUCSubcarrierInfo));		
			kfree(pts.adslATUCSubcarrierInfo_pt);			
			up(&mei_sema);
			break;
		case GET_ADSL_LINE_INIT_STATS:			
			copy_to_user((char *)lon, (char *)&AdslInitStatsData, sizeof(AdslInitStatsData));		
			break;
		
		case GET_ADSL_POWER_SPECTRAL_DENSITY:		
			if(down_interruptible(&mei_sema))
                		return -ERESTARTSYS;
			i=0;
			pts.adslPowerSpectralDensity_pt = (adslPowerSpectralDensity *)kmalloc(sizeof(adslPowerSpectralDensity), GFP_KERNEL);
			memset((char *)pts.adslPowerSpectralDensity_pt, 0, sizeof(adslPowerSpectralDensity));	

			//US
			NOMPSD_US_MAKECMV;
			if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
				printk("\n\nCMV fail, Group PLAM Address 35 Index 0");
#endif
				i=-1;
			}
			else{
				j=RxMessage[4];
			}
			PCB_US_MAKECMV;
			if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
				printk("\n\nCMV fail, Group PLAM Address 35 Index 0");
#endif
				i=-1;
			}
			else{
				temp=RxMessage[4];
			}
			RMSGI_US_MAKECMV;
			if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
				printk("\n\nCMV fail, Group PLAM Address 35 Index 0");
#endif
				i=-1;
			}
			else{
				k=(int16_t)RxMessage[4];
			}
			if (i==0)
			{
				pts.adslPowerSpectralDensity_pt->ACTPSDus = ((int )(j*256 - temp*10*256 + k*10)) /256;
			}
			// DS
			i=0;
			j=temp=temp2=0;
			NOMPSD_DS_MAKECMV;
			if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
				printk("\n\nCMV fail, Group PLAM Address 35 Index 0");
#endif
				i=-1;
			}
			else{
				j=RxMessage[4];
			}
			PCB_DS_MAKECMV;
			if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
				printk("\n\nCMV fail, Group PLAM Address 35 Index 0");
#endif
				i=-1;
			}
			else{
				temp=RxMessage[4];
			}
			RMSGI_DS_MAKECMV;
			if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
				printk("\n\nCMV fail, Group PLAM Address 35 Index 0");
#endif
				i=-1;
			}
			else{
				//temp2=RxMessage[4];
				k=(int16_t)RxMessage[4];
			}
			if (i==0)
			{
				pts.adslPowerSpectralDensity_pt->ACTPSDds = ((int )(j*256 - temp*10*256 + k*10)) /256;
			}
			copy_to_user((char *)lon, (char *)pts.adslPowerSpectralDensity_pt, sizeof(adslPowerSpectralDensity));	
			kfree(pts.adslPowerSpectralDensity_pt);		
			up(&mei_sema);
			break;
                case AMAZON_MEI_START:
			showtime=0;
			loop_diagnostics_completed = 0;
#ifdef	ARC_READY_ACK
#ifdef LOCK_RETRY
			i=0;
lock_retry:
			if(down_trylock(&mei_sema)!=0)
			{
				reboot_lock = 1;
				printk("lock fail\n");
				i++;
				if (i <=5)
				{
	        			set_current_state(TASK_INTERRUPTIBLE);
	                		schedule_timeout(10);
					goto lock_retry;
				}else
				{
					printk("Force to Reboot ADSL!\n");
					up(&mei_sema);
	        			set_current_state(TASK_INTERRUPTIBLE);
	                		schedule_timeout(1000);
	        			sema_init(&mei_sema, 1);  // semaphore initialization, mutex
				}
			}else
			{
				reboot_lock = 1;
			}
#else
			if(down_interruptible(&mei_sema))	//disable CMV access until ARC ready
			{
	                	return -ERESTARTSYS;
			}
#endif
#endif
                	//CLEAR_BIT((*((volatile u32 *)0xB0100B40)), 0x40); //Warning LED GPIO ON
			if(chantype.interleave==1){
				kfree(interleave_mei_net.priv);
				unregister_netdev(&interleave_mei_net);
			}
			else if(chantype.fast==1){
				kfree(fast_mei_net.priv);
				unregister_netdev(&fast_mei_net);
			}		
			chantype.interleave=0;
			chantype.fast=0;
			meiMailboxInterruptsDisable(); //disable all MEI interrupts
			if(mei_arc_swap_buff == NULL){
			mei_arc_swap_buff = (u32 *)kmalloc(MAXSWAPSIZE*4, GFP_KERNEL);
			if(mei_arc_swap_buff==NULL){
#ifdef AMAZON_MEI_DEBUG_ON
				printk("\n\n malloc fail for codeswap buff");
#endif
				meierr=MEI_FAILURE;
			}
			}
                        if(meiForceRebootAdslModem() != MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
                                printk("\n\n meiForceRebootAdslModem()  error...");
#endif
                                meierr=MEI_FAILURE;
                        }     
			interruptible_sleep_on(&wait_queue_codeswap);
			// reset is called
                        break;
		case AMAZON_MEI_MIB_DAEMON:
#ifdef IFX_SMALL_FOOTPRINT /* [ */
			return -1;
#else /* ][ !IFX_SMALL_FOOTPRINT */
			i=0;
			while(1){
				if(i<MIB_INTERVAL)
					interruptible_sleep_on_timeout(&wait_queue_mibdaemon, ((MIB_INTERVAL-i)/(1000/HZ)));
				i=0;
				if(showtime==1){
//					printk("\n\n update mib");
					
					do_gettimeofday(&time_now);
					if(time_now.tv_sec - current_intvl->start_time.tv_sec>=900){
						if(current_intvl->list.next!=&interval_list){
							current_intvl = list_entry(current_intvl->list.next, amazon_mei_mib, list);
							do_gettimeofday(&(current_intvl->start_time));
						}
						else{
							mib_ptr = list_entry(interval_list.next, amazon_mei_mib, list);
							list_del(interval_list.next);
							memset(mib_ptr, 0, sizeof(amazon_mei_mib));
							list_add_tail(&(mib_ptr->list), &interval_list);
							if(current_intvl->list.next==&interval_list)
#ifdef AMAZON_MEI_DEBUG_ON
								printk("\n\nlink list error");
#endif
							current_intvl = list_entry(current_intvl->list.next, amazon_mei_mib, list);
							do_gettimeofday(&(current_intvl->start_time));
						}	
					}
					
					if(down_interruptible(&mei_sema))
                				return -ERESTARTSYS;
/*						
					ATUC_PERF_LO_FLAG_MAKECMV;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group 7 Address 0 Index 0");
#endif
					}
					else{
						if(RxMessage[4]&PLAM_LOS_FailureBit){
							current_intvl->AtucPerfLos++;
							ATUC_PERF_LOSS++;
							CurrStatus.adslAtucCurrStatus = 2;
						}
						if(RxMessage[4]&PLAM_LOF_FailureBit){
							current_intvl->AtucPerfLof++;
							ATUC_PERF_LOFS++;
							CurrStatus.adslAtucCurrStatus = 1;
						}
						if(!(RxMessage[4]&(PLAM_LOS_FailureBit|PLAM_LOF_FailureBit)))
							CurrStatus.adslAtucCurrStatus = 0;
					}
*/
					ATUC_PERF_ESS_FLAG_MAKECMV;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group 7 Address 7 Index 0");
#endif
					}
					else{
						temp = RxMessage[4]-mib_pread.ATUC_PERF_ESS;
						if(temp>=0){
							current_intvl->AtucPerfEs+=temp;
							ATUC_PERF_ESS+=temp;
							mib_pread.ATUC_PERF_ESS = RxMessage[4];
						}
						else{
							current_intvl->AtucPerfEs+=0xffff-mib_pread.ATUC_PERF_ESS+RxMessage[4];
							ATUC_PERF_ESS+=0xffff-mib_pread.ATUC_PERF_ESS+RxMessage[4];
							mib_pread.ATUC_PERF_ESS = RxMessage[4];		
						}
					}
/*		
					ATUR_PERF_LO_FLAG_MAKECMV;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group 7 Address 1 Index 0");
#endif
					}
					else{
						if(RxMessage[4]&PLAM_LOS_FailureBit){
							current_intvl->AturPerfLos++;
							ATUR_PERF_LOSS++;
							CurrStatus.adslAturCurrStatus = 2;
						}
						if(RxMessage[4]&PLAM_LOF_FailureBit){
							current_intvl->AturPerfLof++;
							ATUR_PERF_LOFS++;
							CurrStatus.adslAturCurrStatus = 1;
						}
						if(RxMessage[4]&PLAM_LPR_FailureBit){
							current_intvl->AturPerfLpr++;
							ATUR_PERF_LPR++;
							CurrStatus.adslAturCurrStatus = 3;
						}
						if(!(RxMessage[4]&(PLAM_LOS_FailureBit|PLAM_LOF_FailureBit|PLAM_LPR_FailureBit)))
							CurrStatus.adslAturCurrStatus = 0;	
					}
*/
					ATUR_PERF_ESS_FLAG_MAKECMV;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group 7 Address 33 Index 0");
#endif
					}
					else{
						temp = RxMessage[4]-mib_pread.ATUR_PERF_ESS;
						if(temp>=0){
							current_intvl->AturPerfEs+=temp;
							ATUR_PERF_ESS+=temp;
							mib_pread.ATUR_PERF_ESS = RxMessage[4];
						}
						else{
							current_intvl->AturPerfEs+=0xffff-mib_pread.ATUR_PERF_ESS+RxMessage[4];
							ATUR_PERF_ESS+=	0xffff-mib_pread.ATUR_PERF_ESS+RxMessage[4];
							mib_pread.ATUR_PERF_ESS=RxMessage[4];
						}
					}
					// to update rx/tx blocks
					ATUR_CHAN_RECV_BLK_FLAG_MAKECMV_LSW;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group 7 Address 20 Index 0");
#endif
					}
					else{
						temp = RxMessage[4];	
					}
					ATUR_CHAN_RECV_BLK_FLAG_MAKECMV_MSW;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group 7 Address 21 Index 0");
#endif
					}
					else{
						temp2 = RxMessage[4];	
					}
					if((temp + (temp2<<16) - mib_pread.ATUR_CHAN_RECV_BLK)>=0){
						current_intvl->AturChanPerfRxBlk+=temp + (temp2<<16) - mib_pread.ATUR_CHAN_RECV_BLK;
						ATUR_CHAN_RECV_BLK+=temp + (temp2<<16) - mib_pread.ATUR_CHAN_RECV_BLK;
						mib_pread.ATUR_CHAN_RECV_BLK = temp + (temp2<<16);
						}	
					else{
						current_intvl->AturChanPerfRxBlk+=0xffffffff - mib_pread.ATUR_CHAN_RECV_BLK +(temp + (temp2<<16));
						ATUR_CHAN_RECV_BLK+=0xffffffff - mib_pread.ATUR_CHAN_RECV_BLK +(temp + (temp2<<16));
						mib_pread.ATUR_CHAN_RECV_BLK = temp + (temp2<<16);
					}
					current_intvl->AturChanPerfTxBlk = current_intvl->AturChanPerfRxBlk;
					ATUR_CHAN_TX_BLK = ATUR_CHAN_RECV_BLK;
/*					
					ATUR_CHAN_TX_BLK_FLAG_MAKECMV_LSW;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS)
						printk("\n\nCMV fail, Group 7 Address 20 Index 0");
					else{
						if(RxMessage[4]){
							current_intvl->AturChanPerfTxBlk+=RxMessage[4];
							ATUR_CHAN_TX_BLK+=RxMessage[4];
						}	
					}
					ATUR_CHAN_TX_BLK_FLAG_MAKECMV_MSW;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS)
						printk("\n\nCMV fail, Group 7 Address 21 Index 0");
					else{
						if(RxMessage[4]){
							current_intvl->AturChanPerfTxBlk+=(int)((RxMessage[4])<<16);
							ATUR_CHAN_TX_BLK+=(int)((RxMessage[4])<<16);
						}	
					}
*/					
					if(chantype.interleave == 1){
						ATUR_CHAN_CORR_BLK_FLAG_MAKECMV_INTL;
						if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n\nCMV fail, Group 7 Address 3 Index 0");
#endif
						}
						else{
							temp = RxMessage[4] - mib_pread.ATUR_CHAN_CORR_BLK_INTL;
							if(temp>=0){
								current_intvl->AturChanPerfCorrBlk+=temp;
								ATUR_CHAN_CORR_BLK+=temp;
								mib_pread.ATUR_CHAN_CORR_BLK_INTL = RxMessage[4];
							}	
							else{
								current_intvl->AturChanPerfCorrBlk+=0xffff - mib_pread.ATUR_CHAN_CORR_BLK_INTL +RxMessage[4];
								ATUR_CHAN_CORR_BLK+=0xffff - mib_pread.ATUR_CHAN_CORR_BLK_INTL +RxMessage[4];
								mib_pread.ATUR_CHAN_CORR_BLK_INTL = RxMessage[4];	
							}	
						}	
					}
					else if(chantype.fast == 1){
						ATUR_CHAN_CORR_BLK_FLAG_MAKECMV_FAST;
						if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n\nCMV fail, Group 7 Address 3 Index 1");
#endif
						}
						else{
							temp = RxMessage[4] - mib_pread.ATUR_CHAN_CORR_BLK_FAST;
							if(temp>=0){
								current_intvl->AturChanPerfCorrBlk+=temp;
								ATUR_CHAN_CORR_BLK+=temp;
								mib_pread.ATUR_CHAN_CORR_BLK_FAST = RxMessage[4];
							}	
							else{
								current_intvl->AturChanPerfCorrBlk+=0xffff - mib_pread.ATUR_CHAN_CORR_BLK_FAST + RxMessage[4];
								ATUR_CHAN_CORR_BLK+=0xffff - mib_pread.ATUR_CHAN_CORR_BLK_FAST + RxMessage[4];
								mib_pread.ATUR_CHAN_CORR_BLK_FAST = RxMessage[4];
							}	
						}		
					}
					
					if(chantype.interleave == 1){
						ATUR_CHAN_UNCORR_BLK_FLAG_MAKECMV_INTL;
						if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n\nCMV fail, Group 7 Address 2 Index 0");
#endif
						}
						else{
							temp = RxMessage[4] - mib_pread.ATUR_CHAN_UNCORR_BLK_INTL;
							if(temp>=0){
								current_intvl->AturChanPerfUncorrBlk+=temp;
								ATUR_CHAN_UNCORR_BLK+=temp;
								mib_pread.ATUR_CHAN_UNCORR_BLK_INTL = RxMessage[4];
							}
							else{
								current_intvl->AturChanPerfUncorrBlk+=0xffff - mib_pread.ATUR_CHAN_UNCORR_BLK_INTL + RxMessage[4];
								ATUR_CHAN_UNCORR_BLK+=0xffff - mib_pread.ATUR_CHAN_UNCORR_BLK_INTL + RxMessage[4];
								mib_pread.ATUR_CHAN_UNCORR_BLK_INTL = RxMessage[4];
							}
						}		
					}
					else if(chantype.fast == 1){
						ATUR_CHAN_UNCORR_BLK_FLAG_MAKECMV_FAST;
						if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n\nCMV fail, Group 7 Address 2 Index 1");
#endif
						}
						else{
							temp = RxMessage[4] - mib_pread.ATUR_CHAN_UNCORR_BLK_FAST;
							if(temp>=0){
								current_intvl->AturChanPerfUncorrBlk+=temp;
								ATUR_CHAN_UNCORR_BLK+=temp;
								mib_pread.ATUR_CHAN_UNCORR_BLK_FAST = RxMessage[4];
							}
							else{
								current_intvl->AturChanPerfUncorrBlk+=0xffff - mib_pread.ATUR_CHAN_UNCORR_BLK_FAST + RxMessage[4];
								ATUR_CHAN_UNCORR_BLK+=0xffff - mib_pread.ATUR_CHAN_UNCORR_BLK_FAST + RxMessage[4];
								mib_pread.ATUR_CHAN_UNCORR_BLK_FAST = RxMessage[4];
							}
						}		
					}
					
					//RFC-3440

#ifdef AMAZON_MEI_MIB_RFC3440
					ATUC_PERF_STAT_FASTR_FLAG_MAKECMV; //???
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group 0 Address 0 Index 0");
#endif
					}
					else{
						temp = RxMessage[4] - mib_pread.ATUC_PERF_STAT_FASTR;
						if(temp>=0){
							current_intvl->AtucPerfStatFastR+=temp;
							ATUC_PERF_STAT_FASTR+=temp;
							mib_pread.ATUC_PERF_STAT_FASTR = RxMessage[4];
						}
						else{
							current_intvl->AtucPerfStatFastR+=0xffff - mib_pread.ATUC_PERF_STAT_FASTR + RxMessage[4];
							ATUC_PERF_STAT_FASTR+=0xffff - mib_pread.ATUC_PERF_STAT_FASTR + RxMessage[4];
							mib_pread.ATUC_PERF_STAT_FASTR = RxMessage[4];
						}
					}
					ATUC_PERF_STAT_FAILED_FASTR_FLAG_MAKECMV; //???
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group 0 Address 0 Index 0");
#endif
					}
					else{
						temp = RxMessage[4] - mib_pread.ATUC_PERF_STAT_FAILED_FASTR;
						if(temp>=0){
							current_intvl->AtucPerfStatFailedFastR+=temp;
							ATUC_PERF_STAT_FAILED_FASTR+=temp;
							mib_pread.ATUC_PERF_STAT_FAILED_FASTR = RxMessage[4];
						}
						else{
							current_intvl->AtucPerfStatFailedFastR+=0xffff - mib_pread.ATUC_PERF_STAT_FAILED_FASTR + RxMessage[4];
							ATUC_PERF_STAT_FAILED_FASTR+=0xffff - mib_pread.ATUC_PERF_STAT_FAILED_FASTR + RxMessage[4];
							mib_pread.ATUC_PERF_STAT_FAILED_FASTR = RxMessage[4];
						}
					}
					ATUC_PERF_STAT_SESL_FLAG_MAKECMV;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group 7 Address 8 Index 0");
#endif
					}
					else{
						temp = RxMessage[4] - mib_pread.ATUC_PERF_STAT_SESL;
						if(temp>=0){
							current_intvl->AtucPerfStatSesL+=temp;
							ATUC_PERF_STAT_SESL+=temp;
							mib_pread.ATUC_PERF_STAT_SESL = RxMessage[4];
						}
						else{
							current_intvl->AtucPerfStatSesL+=0xffff - mib_pread.ATUC_PERF_STAT_SESL + RxMessage[4];
							ATUC_PERF_STAT_SESL+=0xffff - mib_pread.ATUC_PERF_STAT_SESL + RxMessage[4];
							mib_pread.ATUC_PERF_STAT_SESL = RxMessage[4];
						}
					}
					ATUC_PERF_STAT_UASL_FLAG_MAKECMV;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group 7 Address 10 Index 0");
#endif
					}
					else{
						temp = RxMessage[4] - mib_pread.ATUC_PERF_STAT_UASL;
						if(temp>=0){
							current_intvl->AtucPerfStatUasL+=temp;
							ATUC_PERF_STAT_UASL+=temp;
							mib_pread.ATUC_PERF_STAT_UASL = RxMessage[4];
						}
						else{
							current_intvl->AtucPerfStatUasL+=0xffff - mib_pread.ATUC_PERF_STAT_UASL + RxMessage[4];
							ATUC_PERF_STAT_UASL+=0xffff - mib_pread.ATUC_PERF_STAT_UASL + RxMessage[4];
							mib_pread.ATUC_PERF_STAT_UASL = RxMessage[4];
						}
					}
					ATUR_PERF_STAT_SESL_FLAG_MAKECMV;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group 7 Address 34 Index 0");
#endif
					}
					else{
						temp = RxMessage[4] - mib_pread.ATUR_PERF_STAT_SESL;
						if(temp>=0){
							current_intvl->AtucPerfStatUasL+=temp;
							ATUC_PERF_STAT_UASL+=temp;
							mib_pread.ATUR_PERF_STAT_SESL = RxMessage[4];
						}
						else{
							current_intvl->AtucPerfStatUasL+=0xffff - mib_pread.ATUR_PERF_STAT_SESL + RxMessage[4];
							ATUC_PERF_STAT_UASL+=0xffff - mib_pread.ATUR_PERF_STAT_SESL + RxMessage[4];
							mib_pread.ATUR_PERF_STAT_SESL = RxMessage[4];
						}
					}
					
#endif
					up(&mei_sema);
					
					do_gettimeofday(&time_fini);
					i = ((int)((time_fini.tv_sec-time_now.tv_sec)*1000)) + ((int)((time_fini.tv_usec-time_now.tv_usec)/1000))  ; //msec 
				}//showtime==1
			}	 
			break;
#endif /* ] !IFX_SMALL_FOOTPRINT */
		case AMAZON_MEI_RESET:			
		case AMAZON_MEI_REBOOT:
		case AMAZON_MEI_SHOWTIME:
/*			if(mei_arc_swap_buff !=NULL){
				kfree(mei_arc_swap_buff);
				mei_arc_swap_buff=NULL;
			}	
			if(image_buffer !=NULL){
//				kfree(image_buffer);
				vfree(image_buffer);
				image_buffer =NULL;
			}
*/
			if(clreoc_command_pkt !=NULL){
				kfree(clreoc_command_pkt);
				clreoc_command_pkt =NULL;
			}
			for(i=0;i<CLREOC_BUFF_SIZE;i++)
				clreoc_pkt[i].len=0;	//flush all remaining clreoc commands in buffer
/*
			memset(mei_mib, 0, (sizeof(amazon_mei_mib)*INTERVAL_NUM));
			INIT_LIST_HEAD(&interval_list);
			for(i=0;i<INTERVAL_NUM;i++)
				list_add_tail(&(mei_mib[i].list), &interval_list); 
			current_intvl = list_entry(interval_list.next, amazon_mei_mib, list);
			do_gettimeofday(&(current_intvl->start_time));
			ATUC_PERF_LOFS=0;
			ATUC_PERF_LOSS=0;
			ATUC_PERF_ESS=0;
	 		ATUC_PERF_INITS=0;
 			ATUR_PERF_LOFS=0;
	 		ATUR_PERF_LOSS=0;
	 		ATUR_PERF_LPR=0;
	 		ATUR_PERF_ESS=0;
	 		ATUR_CHAN_RECV_BLK=0;
	 		ATUR_CHAN_TX_BLK=0;
	 		ATUR_CHAN_CORR_BLK=0;
	 		ATUR_CHAN_UNCORR_BLK=0;
			memset((((u8 *)&AlarmConfProfile)+32), 0, 16*4);
			AlarmConfProfile.adslLineAlarmConfProfileRowStatus=1;
*/
			PrevTxRate.adslAtucChanPrevTxRate=0;
			PrevTxRate.adslAturChanPrevTxRate=0;
			CurrStatus.adslAtucCurrStatus=0;
			CurrStatus.adslAturCurrStatus=0;
			
			if((command==AMAZON_MEI_RESET) || (command==AMAZON_MEI_REBOOT)){
#ifdef	AMAZON_CHECK_LINK
				if (adsl_link_notify){
					(*adsl_link_notify)(0);
				}
#endif
				showtime=0;
				//CLEAR_BIT((*((volatile u32 *)0xB0100B40)), 0x40); //Warning LED GPIO ON
				// disconnect net_dev
				if(chantype.interleave==1){
					kfree(interleave_mei_net.priv);
					unregister_netdev(&interleave_mei_net);
//        				if(unregister_netdev(&interleave_mei_net)!=0)
//						printk("\n unregister interleave fail");
				}
				else if(chantype.fast==1){
					kfree(fast_mei_net.priv);
					unregister_netdev(&fast_mei_net);
//       				if(unregister_netdev(&fast_mei_net)!=0)
//						printk("\n unregister fast fail");
				}
				chantype.interleave=0;
				chantype.fast=0;
// 603221:tc.chen start
				chantype.bearchannel0 = 0;
				chantype.bearchannel1 = 0;
				adsl_mode = 0;
// 603221:tc.chen end
				
				while(1){
					
					makeCMV(H2D_CMV_READ, STAT, 0, 0, 1, NULL); //maximum allowed tx message length, in bytes
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
						//printk("AdslInitStatsData.FullInitializationCount++\n");
						AdslInitStatsData.FullInitializationCount++;
						//printk("AdslInitStatsData.FailedFullInitializationCount++\n");
						AdslInitStatsData.FailedFullInitializationCount++;
						//printk("AdslInitStatsData.LINIT_Errors++\n");
						AdslInitStatsData.LINIT_Errors++;
					}else
					{
						//printk("RxMessage=%X\n",RxMessage[4]);
						if ( RxMessage[4]!=0x1)
						{
							//printk("AdslInitStatsData.FullInitializationCount++\n");
							AdslInitStatsData.FullInitializationCount++;	
							if ( RxMessage[4] != 0x7)
							{
								//printk("AdslInitStatsData.LINIT_Errors++\n");
								AdslInitStatsData.LINIT_Errors++;
								//printk("AdslInitStatsData.FailedFullInitializationCount++\n");
								AdslInitStatsData.FailedFullInitializationCount++;
								
							}			
						}
					}

					reboot_flag=0;
					wake_up_interruptible(&wait_queue_codeswap); //wake up codeswap daemon				

					interruptible_sleep_on_timeout(&wait_queue_reboot, 1*HZ);  // sleep until arc ready
#ifdef	ARC_READY_ACK
					if(reboot_flag!=0)
						break;
					else
					{
						up(&mei_sema);
						printk("\n reboot retry");
					}
#else
					break;
#endif
				}
			}
			else{ //AMAZON_MEI_SHOWTIME 
				if(down_interruptible(&mei_sema))
                			return -ERESTARTSYS;
				
				// clreoc stuff
				makeCMV(H2D_CMV_READ, INFO, 83, 0, 1, data); //maximum allowed tx message length, in bytes
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 3 Address 83 Index 0");
#endif
				}
				else{
					clreoc_max_tx_len = (int)RxMessage[4];
					clreoc_command_pkt = kmalloc((clreoc_max_tx_len*CLREOC_BUFF_SIZE), GFP_KERNEL);
					if(clreoc_command_pkt == NULL){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("kmalloc error for clreoc_command_pkt\n\n");
#endif
						up(&mei_sema);
						return -1;
					}
					for(i=0;i<CLREOC_BUFF_SIZE;i++){
						clreoc_pkt[i].command = (u8 *)(((u8 *)clreoc_command_pkt) + (clreoc_max_tx_len*i));
						clreoc_pkt[i].len=0;
					}	
				}
				
				// decide what channel, then register	
				makeCMV(H2D_CMV_READ, STAT, 12, 0, 1, data); 
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 2 Address 12 Index 0");
#endif
				}
				else{
					if((RxMessage[4]&0x1)==1){	
						if(register_netdev(&interleave_mei_net)!=0){
#ifdef AMAZON_MEI_DEBUG_ON
                					printk("\n\n Register interleave Device Failed.");
#endif
        					}
						else{
							chantype.interleave = 1;
							chantype.fast= 0;
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n channel is interleave");
#endif
						}	
					}
					else if((RxMessage[4]&0x2)==2){	
						if(register_netdev(&fast_mei_net)!=0){
#ifdef AMAZON_MEI_DEBUG_ON
                					printk("\n\n Register fast Device Failed.");
#endif
        					}
						else{
							chantype.fast = 1;
							chantype.interleave = 0;
#ifdef AMAZON_MEI_DEBUG_ON
							printk("\n channel is fast");
#endif
						}
					}
					else{
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\nunknown channel type, 0x%8x", RxMessage[4]);
#endif
					}	
// 603221:tc.chen start	
					if ( (RxMessage[4]&0x100) == 0x100)
					{
						chantype.bearchannel0 = 1;
					}else 	if ( (RxMessage[4]&0x100) == 0x200)
					{
						chantype.bearchannel1 = 1;
					}
// 603221:tc.chen end
				}
// 603221:tc.chen start			
				// read adsl mode
				makeCMV(H2D_CMV_READ, STAT, 1, 0, 1, data); 
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group STAT Address 1 Index 0");
#endif
				}
				adsl_mode = RxMessage[4];
// 603221:tc.chen end
				makeCMV(H2D_CMV_READ, STAT, 17, 0, 1, data); 
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group STAT Address 1 Index 0");
#endif
				}
				adsl_mode_extend = RxMessage[4];
				
				// update previous channel tx rate
				ATUC_CHAN_CURR_TX_RATE_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 6 Address 1 Index 0");
#endif
					PrevTxRate.adslAtucChanPrevTxRate = 0;	
				}
				else{
					PrevTxRate.adslAtucChanPrevTxRate = (u32)(RxMessage[4]) + (((u32)(RxMessage[5]))<<16);
				}
				ATUR_CHAN_CURR_TX_RATE_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 6 Address 0 Index 0");
#endif
					PrevTxRate.adslAturChanPrevTxRate = 0;	
				}
				else{
					PrevTxRate.adslAturChanPrevTxRate = (u32)(RxMessage[4]) + (((u32)(RxMessage[5]))<<16);
				}
				
//				up(&mei_sema);
				
//				showtime=1;
				//SET_BIT((*((volatile u32 *)0xB0100B40)), 0x40); //Warning LED GPIO OFF
//dying gasp -start	
#ifdef IFX_DYING_GASP			
				lop_debugwr.buffer[0]=0xffffffff;		//dying gasp
				lop_debugwr.iCount=1;				//dying gasp
				makeCMV(H2D_CMV_READ, INFO, 66, 4, 1, NULL);
				//mei_ioctl((struct inode *)0,NULL, AMAZON_MEI_CMV_WINHOST, &RxMessage);	
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 3 Address 66 Index 4");
				}
#endif
				lop_debugwr.iAddress=(u32)RxMessage[4];
				makeCMV(H2D_CMV_READ, INFO, 66, 5, 1, NULL);
				//mei_ioctl((struct inode *)0,NULL, AMAZON_MEI_CMV_WINHOST, &RxMessage);			
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 3 Address 66 Index 5");
				}
#endif
				lop_debugwr.iAddress+=((u32)RxMessage[4])<<16;
				
//dying gasp -end				
#endif	// IFX_DYING_GASP			

//joelin 04/16/2005-start
				makeCMV(H2D_CMV_WRITE, PLAM, 10, 0, 1, &unavailable_seconds);
				//mei_ioctl((struct inode *)0,NULL, AMAZON_MEI_CMV_WINHOST, &RxMessage);			
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 7 Address 10 Index 0");
				
#endif
				}
				

//joelin 04/16/2005-end		
				showtime=1;
				up(&mei_sema);
#ifdef	AMAZON_CHECK_LINK
				if (adsl_link_notify){
					(*adsl_link_notify)(1);
				}
#endif
				
				
			}
			break;
/*
                case AMAZON_MEI_REPLY:
                        copy_from_user((char *)buff, (char *)lon, MSG_LENGTH * 2);
                        if(meiCMV(buff, YES_REPLY) != MEI_SUCCESS){
                                printk("\n\n meiCMV no reply back");
                                meierr=MEI_FAILURE;
                        }
                        else
                                copy_to_user((char *)lon, (char *)RxMessage, MSG_LENGTH * 2);
                        break;
                case AMAZON_MEI_NOREPLY:
                        copy_from_user((char *)buff, (char *)lon, MSG_LENGTH * 2);
                        if(meiCMV(buff, NO_REPLY) != MEI_SUCCESS){
                                printk("\n\n meiCMV Host to DSP failed");
                                meierr=MEI_FAILURE;
                        }  
                        break;
*/
		
		case AMAZON_MEI_HALT:
			meiHaltArc();
			break;
		case AMAZON_MEI_CMV_WINHOST:
			if(down_interruptible(&mei_sema))
                		return -ERESTARTSYS;

			if (!from_kernel )	
				copy_from_user((char *)TxMessage, (char *)lon, MSG_LENGTH*2);//joelin
			else
				memcpy(TxMessage,(char *)lon,MSG_LENGTH*2);
				
#if 0			
//			if((TxMessage[0]&0xff0)==0x0a0){
				for(i=0;i<16;i++){
					printk("\nTxMessage[%i]=%8x", i, TxMessage[i]);
				}
//			}			
#endif			
			
			if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
//				printk("\n\n WINHOST CMV fail ");
                                printk("\n\nWINHOST CMV fail :TxMessage:%X %X %X %X, RxMessage:%X %X %X %X %X\n",TxMessage[0],TxMessage[1],TxMessage[2],TxMessage[3],RxMessage[0],RxMessage[1],RxMessage[2],RxMessage[3],RxMessage[4]);
#endif
				meierr = MEI_FAILURE;	
			}
			else 
			{
				if (!from_kernel )	//joelin
				copy_to_user((char *)lon, (char *)RxMessage, MSG_LENGTH*2);
				else
					memcpy((char *)lon,(char *)RxMessage,MSG_LENGTH*2);
			}
				
			up(&mei_sema);	
			break;
#ifdef AMAZON_MEI_CMV_EXTRA
		case AMAZON_MEI_CMV_READ:
			copy_from_user((char *)(&regrdwr), (char *)lon, sizeof(meireg));
			meiLongwordRead(regrdwr.iAddress, &(regrdwr.iData));
			{
//				printk("\n\n iAddress = %8x",regrdwr.iAddress);
			}
			copy_to_user((char *)lon, (char *)(&regrdwr), sizeof(meireg));
			{
//				printk("\n\n iData readback = %8x", regrdwr.iData);
			}
			break;
#endif

#ifdef AMAZON_MEI_CMV_EXTRA
		case AMAZON_MEI_CMV_WRITE:
			copy_from_user((char *)(&regrdwr), (char *)lon, sizeof(meireg));
			{
//				printk("\n\n iAddress = %8x",regrdwr.iAddress);
//				printk("\n\n iData = %8x",regrdwr.iData);
			}
			meiLongwordWrite(regrdwr.iAddress, regrdwr.iData);
			break;
#endif

#ifdef AMAZON_MEI_CMV_EXTRA
		case AMAZON_MEI_REMOTE:
			copy_from_user((char *)(&i), (char *)lon, sizeof(int));
			if(i==0){
				meiMailboxInterruptsEnable();
					
				up(&mei_sema);
			}
			else if(i==1){
				meiMailboxInterruptsDisable();
				if(down_interruptible(&mei_sema))
                			return -ERESTARTSYS;
			}
			else{
#ifdef AMAZON_MEI_DEBUG_ON
				printk("\n\n AMAZON_MEI_REMOTE argument error");
#endif
				meierr=MEI_FAILURE;
			}		
			break;
#endif

#ifdef AMAZON_MEI_CMV_EXTRA
		case AMAZON_MEI_READDEBUG:
		case AMAZON_MEI_WRITEDEBUG:
			if(down_interruptible(&mei_sema))
                		return -ERESTARTSYS;
#ifdef IFX_DYING_GASP			
	if (!from_kernel) copy_from_user((char *)(&debugrdwr), (char *)lon, sizeof(debugrdwr));//dying gasp
				else memcpy((char *)(&debugrdwr), (char *)lon,  sizeof(debugrdwr));
#else //IFX_DYING_GASP	
			copy_from_user((char *)(&debugrdwr), (char *)lon, sizeof(debugrdwr));

#endif //IFX_DYING_GASP							
#if 0
			printk("\nIN iAddress: %8x, iCount:%8x\n", debugrdwr.iAddress, debugrdwr.iCount);
#endif
			
			if(command==AMAZON_MEI_READDEBUG)
				meiDebugRead(debugrdwr.iAddress, debugrdwr.buffer, debugrdwr.iCount);
			else
				meiDebugWrite(debugrdwr.iAddress, debugrdwr.buffer, debugrdwr.iCount);	
				
#ifdef IFX_DYING_GASP				
			if (!from_kernel) copy_to_user((char *)lon, (char*)(&debugrdwr), sizeof(debugrdwr));//dying gasp
#else //IFX_DYING_GASP	
			copy_to_user((char *)lon, (char*)(&debugrdwr), sizeof(debugrdwr));
#endif //IFX_DYING_GASP	
			up(&mei_sema);
			
#if 0
			printk("\nOUT iAddress: %8x, iCount:%8x\n", debugrdwr.iAddress, debugrdwr.iCount);
			for(i=0;i<debugrdwr.iCount;i++)
				printk("\n %8x",debugrdwr.buffer[i]);
#endif
			break;
#endif

		case AMAZON_MEI_LOP:	
		//GPIO31 :dying gasp event indication
		//	(1) logic high: dying gasp event is false (default)
		//	(2) logic low: dying gasp event is true
#ifdef IFX_DYING_GASP
			break;
#else					
			CLEAR_BIT((*((volatile u32 *)0xB0100B48)), 0x8000);
			CLEAR_BIT((*((volatile u32 *)0xB0100B4C)), 0x8000);
			CLEAR_BIT((*((volatile u32 *)0xB0100B50)), 0x8000);
			SET_BIT((*((volatile u32 *)0xB0100B54)), 0x8000);
			asm("SYNC");
			if(((*((volatile u32 *)0xB0100B44))&0x8000)==0x0)
				meierr=MEI_FAILURE; //return - to indicate loss of power
			break;
#endif //#ifdef IFX_DYING_GASP			
			
					//for PCM
		case AMAZON_MEI_PCM_SETUP:
			//gpio
			
			*AMAZON_GPIO_P0_DIR |=1<<15;
    			*AMAZON_GPIO_P0_ALTSEL0 |=1<<15;
    			*AMAZON_GPIO_P0_ALTSEL1 &=~(1<<15);
    			*AMAZON_GPIO_P0_OD |=1<<15;
                                                                                
    			/*GPIO 16 TDM_DI*/
    			*AMAZON_GPIO_P1_DIR &=~1;
    			*AMAZON_GPIO_P1_ALTSEL0 |=1;
    			*AMAZON_GPIO_P1_ALTSEL1 &=~1;
                                                                                
    			/*GPIO 17 TDM_DCL */
    			*AMAZON_GPIO_P1_DIR|=0x02;
    			*AMAZON_GPIO_P1_ALTSEL0|=0x02;
    			*AMAZON_GPIO_P1_ALTSEL1 &=(u32)~0x02;
    			*AMAZON_GPIO_P1_OD|=0x02;
                                                                                
    			/*GPIO 18 TDM FSC*/
    			*AMAZON_GPIO_P1_DIR|=0x04;
    			*AMAZON_GPIO_P1_ALTSEL0|=0x04;
    			*AMAZON_GPIO_P1_ALTSEL1 &=(u32)~0x04;
    			*AMAZON_GPIO_P1_OD|=0x04;
			
			for(i=0;i<2;i++){
				for(j=0;j<256;j++)
					sampledata[i*256+j]=j;
			}
			
			pcm_start_addr = lon;
			
			printk("\n\n pcm_start_addr is %8x", lon);
			
			for(i=0;i<PCM_CHANNEL_NUM;i++){
#ifdef	PCM_ACCESS_DEBUG
				meiDebugRead_16((pcm_start_addr+i*16), (u32*)(pcm_data+i), 4);
#else
				meiDMARead_16((pcm_start_addr+i*16), (u32*)(pcm_data+i), 4);
#endif
				if((pcm_data[i].S!=8)||(pcm_data[i].len<1)||(pcm_data[i].rdindex!=0)||(pcm_data[i].wrindex!=0)||(pcm_data[i].flow!=0))
					printk("\n\n pcm_data fill in wrongly\n\n");
				printk("\npcm_data %d",i);
				printk("\n S = %d", pcm_data[i].S);
				printk("\n LSW = %4x", pcm_data[i].LSW);
				printk("\n MSW = %4x", pcm_data[i].MSW);
				printk("\n len = %d", pcm_data[i].len);
				printk("\n rdindex = %d", pcm_data[i].rdindex);
				printk("\n wrindex = %d", pcm_data[i].wrindex);	
				printk("\n flow = %d", pcm_data[i].flow);				
				pcm_data[i].finish=0;	
				if(i%2==0){//tx channel
					for(j=0;j<PCM_BUFF_SIZE/256;j++){
						for(k=0;k<256;k++){
							pcm_data[i].buff[j*256+k]=k;
						/*	if(k%2==0)
								pcm_data[i].buff[j*256+k]=0xaa;
							else
								pcm_data[i].buff[j*256+k]=0x55;*/
						}
					}
#ifdef	PCM_ACCESS_DEBUG
					meiDebugWrite_8((((u32)(pcm_data[i].LSW))+(((u32)(pcm_data[i].MSW))<<16)), (u32*)(pcm_data[i].buff),((pcm_data[i].len/4)/2));//fill half first
//					meiDebugWrite_8((((u32)(pcm_data[i].LSW))+(((u32)(pcm_data[i].MSW))<<16)), (u32*)(pcm_data[i].buff),2);//fill half first
#else
					meiDMAWrite_8((((u32)(pcm_data[i].LSW))+(((u32)(pcm_data[i].MSW))<<16)), (u32*)(pcm_data[i].buff),((pcm_data[i].len/4)/2));//fill half first
//					meiDMAWrite_8((((u32)(pcm_data[i].LSW))+(((u32)(pcm_data[i].MSW))<<16)), (u32*)(pcm_data[i].buff),2);//fill half first
#endif
					pcm_data[i].point=(pcm_data[i].len)/2;
//					pcm_data[i].point=8;
#ifdef	PCM_ACCESS_DEBUG
					meiDebugRead_16(pcm_start_addr+i*16+12, &temp, 1);
#else
					meiDMARead_16(pcm_start_addr+i*16+12, &temp, 1);
#endif
					temp = (temp &0xffff) + (((u32)(pcm_data[i].point))<<16);
#ifdef	PCM_ACCESS_DEBUG
					meiDebugWrite_16(pcm_start_addr+i*16+12,&temp, 1);//update wrindex
#else
					meiDMAWrite_16(pcm_start_addr+i*16+12,&temp, 1);//update wrindex
#endif
				}
				else{// rx channel
					pcm_data[i].point=0;
				}
			}
			break;
		case AMAZON_MEI_PCM_START_TIMER:
				/* GPTU timer 6 */
			/* enable the timer in the PMU */
        		*(AMAZON_PMU_PWDCR) = (*(AMAZON_PMU_PWDCR))| AMAZON_PMU_PWDCR_GPT|AMAZON_PMU_PWDCR_FPI;
			/* setup the GPTU for timer tick  f_fpi == f_gptu*/
			*(AMAZON_GPTU_CLC) = 0x100;
			//reload value = fpi/(HZ * P), timer mode, Prescaler = 4 ( T6I = 000, T6BPS2 = 0)
			*(AMAZON_GPTU_CAPREL) = (int)(117500000/(lon*4));	
			*(AMAZON_GPTU_T6CON) = 0x80C0;
			
			if (request_irq(AMAZON_TIMER6_INT, amazon_timer6_interrupt_MEI,0, "hrt", NULL)!=0){
#ifdef AMAZON_MEI_DEBUG_ON
                		printk("\n\n unable to register irq for hrt!!!");
#endif
                		return -1;
        		}
			enable_irq(AMAZON_TIMER6_INT);
			break;
		case AMAZON_MEI_PCM_STOP_TIMER:
			disable_irq(AMAZON_TIMER6_INT);
			free_irq(AMAZON_TIMER6_INT, NULL);
			break;
		case AMAZON_MEI_PCM_CHECK:
			for(i=0;i<PCM_CHANNEL_NUM;i++){
				if(pcm_data[i].finish!=1)
					return 0;	
			}
			for(i=0;i<PCM_CHANNEL_NUM/2;i++){
				j=0;
				while(1){
					if((pcm_data[i*2+1].buff[j]==0x0) && (pcm_data[i*2+1].buff[j+1]==0x1)&& (pcm_data[i*2+1].buff[j+2]==0x2))
						break;
					else
						j++;
				}
				printk("\n j=%d", j);
				temp=0;
				for(k=0;k<((PCM_BUFF_SIZE-j)/4);k++){
					if(memcmp(pcm_data[i*2].buff+k*4, pcm_data[i*2+1].buff+j+k*4, 4)!=0){
						temp++;
						printk("\n\n%2x %2x %2x %2x %2x %2x %2x %2x\n\n", *((u8*)(pcm_data[i*2].buff+k*4)), *((u8*)(pcm_data[i*2].buff+k*4+1)),*((u8*)(pcm_data[i*2].buff+k*4+2)),*((u8*)(pcm_data[i*2].buff+k*4+3)),*((u8*)(pcm_data[i*2+1].buff+j+k*4)),*((u8*)(pcm_data[i*2+1].buff+j+k*4+1)),*((u8*)(pcm_data[i*2+1].buff+j+k*4+2)),*((u8*)(pcm_data[i*2+1].buff+j+k*4+3)));
						break;
					}
				}
				if(temp!=0)
					printk("\n\n Channel pair %d not match: err32 %d\n\n", i, temp);
				else
					printk("\n\n Channel pair %d match\n\n", i);
			}
			for(i=0;i<PCM_CHANNEL_NUM;i++){
				if(i%2==1){//rx channel
#ifdef	PCM_ACCESS_DEBUG
					meiDebugRead_16(pcm_start_addr+i*16+12, &temp, 1);
#else
					meiDMARead_16(pcm_start_addr+i*16+12, &temp, 1);
#endif
					printk("\n\nRx channel %d: Overflow Bytes %d", i, (temp&0xffff));	
				}
				else{//tx channel
#ifdef	PCM_ACCESS_DEBUG
					meiDebugRead_16(pcm_start_addr+i*16, &temp, 1);
#else
					meiDMARead_16(pcm_start_addr+i*16, &temp, 1);
#endif
					printk("\n\nElectra Err: %d",(temp&0xffff));		
				}
			}
			//check electra overflow
			
			meierr=1;
			break;
		case AMAZON_MEI_PCM_GETDATA:
			copy_to_user(lon, pcm_data[1].buff, PCM_BUFF_SIZE);
			break;
		case AMAZON_MEI_PCM_GPIO:
			//gpio
			
			*AMAZON_GPIO_P0_DIR |=1<<15;
    			*AMAZON_GPIO_P0_ALTSEL0 |=1<<15;
    			*AMAZON_GPIO_P0_ALTSEL1 &=~(1<<15);
    			*AMAZON_GPIO_P0_OD |=1<<15;
                                                                                
    			/*GPIO 16 TDM_DI*/
    			*AMAZON_GPIO_P1_DIR &=~1;
    			*AMAZON_GPIO_P1_ALTSEL0 |=1;
    			*AMAZON_GPIO_P1_ALTSEL1 &=~1;
                                                                                
    			/*GPIO 17 TDM_DCL */
    			*AMAZON_GPIO_P1_DIR|=0x02;
    			*AMAZON_GPIO_P1_ALTSEL0|=0x02;
    			*AMAZON_GPIO_P1_ALTSEL1 &=(u32)~0x02;
    			*AMAZON_GPIO_P1_OD|=0x02;
                                                                                
    			/*GPIO 18 TDM FSC*/
    			*AMAZON_GPIO_P1_DIR|=0x04;
    			*AMAZON_GPIO_P1_ALTSEL0|=0x04;
    			*AMAZON_GPIO_P1_ALTSEL1 &=(u32)~0x04;
    			*AMAZON_GPIO_P1_OD|=0x04;	
			break;
			
			
			
				//for clearEoC
#ifdef AMAZON_CLEAR_EOC
		case AMAZON_MEI_GET_EOC_LEN:
			while(1){
				current_clreoc = list_entry(clreoc_list.next, amazon_clreoc_pkt, list);
				if((current_clreoc->len)>0){
					copy_to_user((char *)lon, (char*)(&(current_clreoc->len)), 4);
					break;	
				}
				else//wait for eoc data from higher layer
					interruptible_sleep_on(&wait_queue_clreoc);	
			}
			break;
		case AMAZON_MEI_GET_EOC_DATA:
			current_clreoc = list_entry(clreoc_list.next, amazon_clreoc_pkt, list);
			if((current_clreoc->len)>0){
				copy_to_user((char*)lon, (char*)(current_clreoc->command), current_clreoc->len);
				meierr=1;
				list_del(clreoc_list.next);	//remove and add to end of list
				current_clreoc->len = 0;
				list_add_tail(&(current_clreoc->list), &clreoc_list);
			}
			else
				meierr=-1;
			break;
		case AMAZON_MEI_EOC_SEND:
			copy_from_user((char *)(&debugrdwr), (char *)lon, sizeof(debugrdwr));
			eoc_skb = dev_alloc_skb(debugrdwr.iCount*4);
			if(eoc_skb==NULL){
				printk("\n\nskb alloc fail");
				break;
			}
			
			eoc_skb->len=debugrdwr.iCount*4;
			memcpy(skb_put(eoc_skb, debugrdwr.iCount*4), (char *)debugrdwr.buffer, debugrdwr.iCount*4);
			
			ifx_push_eoc(eoc_skb);	//pass data to higher layer
			break;
#endif //#ifdef AMAZON_CLEAR_EOC
		case AMAZON_MIB_LO_ATUC:
			do_gettimeofday(&time_now);
			if(lon&0x1){
				if((time_now.tv_sec-(mib_pflagtime.ATUC_PERF_LOSS_PTIME).tv_sec)>2){
					current_intvl->AtucPerfLos++;
					ATUC_PERF_LOSS++;
					CurrStatus.adslAtucCurrStatus = 2;
				}
				(mib_pflagtime.ATUC_PERF_LOSS_PTIME).tv_sec = time_now.tv_sec;
			}
			if(lon&0x2){
				if((time_now.tv_sec-(mib_pflagtime.ATUC_PERF_LOFS_PTIME).tv_sec)>2){
					current_intvl->AtucPerfLof++;
					ATUC_PERF_LOFS++;
					CurrStatus.adslAtucCurrStatus = 1;
				}
				(mib_pflagtime.ATUC_PERF_LOFS_PTIME).tv_sec = time_now.tv_sec;
			}
			if(!(lon&0x3))
				CurrStatus.adslAtucCurrStatus = 0;
			break;
		case AMAZON_MIB_LO_ATUR:
			do_gettimeofday(&time_now);
			if(lon&0x1){
				if((time_now.tv_sec-(mib_pflagtime.ATUR_PERF_LOSS_PTIME).tv_sec)>2){
					current_intvl->AturPerfLos++;
					ATUR_PERF_LOSS++;
					CurrStatus.adslAturCurrStatus = 2;
			}
				(mib_pflagtime.ATUR_PERF_LOSS_PTIME).tv_sec = time_now.tv_sec;
			}
			if(lon&0x2){
				if((time_now.tv_sec-(mib_pflagtime.ATUR_PERF_LOFS_PTIME).tv_sec)>2){
					current_intvl->AturPerfLof++;
					ATUR_PERF_LOFS++;
					CurrStatus.adslAturCurrStatus = 1;
				}
				(mib_pflagtime.ATUR_PERF_LOFS_PTIME).tv_sec = time_now.tv_sec;
			}
			if(lon&0x4){
				if((time_now.tv_sec-(mib_pflagtime.ATUR_PERF_LPR_PTIME).tv_sec)>2){
					current_intvl->AturPerfLpr++;
					ATUR_PERF_LPR++;
					CurrStatus.adslAturCurrStatus = 3;
				}
				(mib_pflagtime.ATUR_PERF_LPR_PTIME).tv_sec = time_now.tv_sec;
			}
			if(!(lon&0x7))
				CurrStatus.adslAturCurrStatus = 0;
			break;
		case AMAZON_MEI_DOWNLOAD:
			// DMA the boot code page(s)
#ifdef AMAZON_MEI_DEBUG_ON
			printk("\n\n start download pages");
#endif
			for( boot_loop = 0; boot_loop < img_hdr->count; boot_loop++){
				if( img_hdr->page[boot_loop].p_size & BOOT_FLAG){
					page_size = meiGetPage( boot_loop, GET_PROG, MAXSWAPSIZE, mei_arc_swap_buff, &dest_addr);
					if( page_size > 0){
						meiDMAWrite(dest_addr, mei_arc_swap_buff, page_size);
					}
				}
				if( img_hdr->page[boot_loop].d_size & BOOT_FLAG){
					page_size = meiGetPage( boot_loop, GET_DATA, MAXSWAPSIZE, mei_arc_swap_buff, &dest_addr);
					if( page_size > 0){
						meiDMAWrite( dest_addr, mei_arc_swap_buff, page_size);
					}
				}
			}
#ifdef AMAZON_MEI_DEBUG_ON
			printk("\n\n pages downloaded");
#endif
			break;
		//509221:tc.chen start
                case AMAZON_MEI_DEBUG_MODE:
                        mei_debug_mode = lon;
			break;
		//509221:tc.chen end
        }
        return meierr;
}


//////////////////////          Interrupt handler               /////////////////////////////////////////////////////
static void mei_interrupt_arcmsgav(int,void *,struct pt_regs *);
static void mei_interrupt_arcmsgav(int int1, void * void0, struct pt_regs * regs)
{
        u32 scratch;
        u32 fetchpage;
        u32 size;
        u32 dest_addr;
	u32 temp;
	int i;
        
        meiDebugRead(ARC_MEI_MAILBOXR, &scratch, 1);
        if(scratch & OMB_CODESWAP_MESSAGE_MSG_TYPE_MASK)
        {
		if(showtime==1){
#ifdef AMAZON_MEI_DEBUG_ON
			printk("\n\n Code Swap Request After ShowTime !!!");
#endif
		}
		else{
#ifdef AMAZON_MEI_DEBUG_ON
//			printk("\n\n Code Swap Request");
#endif
			fetchpage = scratch & ~OMB_CODESWAP_MESSAGE_MSG_TYPE_MASK;
			size = meiGetPage( fetchpage, GET_PROG, MAXSWAPSIZE, mei_arc_swap_buff, &dest_addr);
			if( size > 0)
			{
#ifdef AMAZON_MEI_DEBUG_ON
//				printk("  : prom page num %d",fetchpage);
#endif
				meiDMAWrite( dest_addr, mei_arc_swap_buff, size);
			}

                	size = meiGetPage( fetchpage, GET_DATA, MAXSWAPSIZE, mei_arc_swap_buff, &dest_addr);
			if( size > 0)
			{
#ifdef AMAZON_MEI_DEBUG_ON
//				printk("  : data page num %d",fetchpage);
#endif
				meiDMAWrite( dest_addr, mei_arc_swap_buff, size);
			}
		}
                        //	Notify arc that mailbox read complete
 		meiLongwordWrite(ARC_TO_MEI_INT, ARC_TO_MEI_MSGAV);

		//	Tell ARC Codeswap is done
		meiLongwordWrite(MEI_TO_ARC_INT, MEI_TO_ARC_CS_DONE);
		asm("SYNC");
		i=0;
		while(i<WHILE_DELAY){
			meiLongwordRead(MEI_TO_ARC_INT, &temp);
			if((temp & MEI_TO_ARC_CS_DONE) ==0){
#ifdef AMAZON_MEI_DEBUG_ON
//				printk("\n\n Code Swap Done");
#endif
				break;
			}
			i++;
			if(i==WHILE_DELAY){
#ifdef AMAZON_MEI_DEBUG_ON
				printk("\n\n MEI_TO_ARC_CS_DONE not cleared by ARC");
				//509221:tc.chen start
				if (!mei_debug_mode)
				{
					//printk("Reset Arc!\n");
					//printk("AdslInitStatsData.FullInitializationCount++\n");
					AdslInitStatsData.FullInitializationCount++;	
					if (!showtime)
					{
						//printk("AdslInitStatsData.FailedFullInitializationCount++\n");
						AdslInitStatsData.FailedFullInitializationCount++;
						//printk("AdslInitStatsData.LINIT_Errors++\n");
						AdslInitStatsData.LINIT_Errors++;
					}

					wake_up_interruptible(&wait_queue_codeswap);	// wait up ioctl reboot
				}
				//509221:tc.chen end
#endif
			}
		}
        }
        else{    // normal message
//			printk("\n\n interrupt");	
                        meiMailboxRead(RxMessage, MSG_LENGTH);
                        if(cmv_waiting==1){            
                                arcmsgav=1;
                                cmv_waiting=0;
                                wake_up_interruptible(&wait_queue_arcmsgav);                  
                        }
                        else{
                                indicator_count++;
                                memcpy((char *)Recent_indicator, (char *)RxMessage,  MSG_LENGTH *2);
#ifdef	ARC_READY_ACK
				if(((RxMessage[0]&0xff0)>>4)==D2H_AUTONOMOUS_MODEM_READY_MSG){	//check ARC ready message
					
#ifdef LOCK_RETRY
					if (reboot_lock)
					{
					    reboot_lock = 0;
					    up(&mei_sema);	// allow cmv access
					}
#else
				        up(&mei_sema);	// allow cmv access
#endif
					reboot_flag=1;
//#ifdef ADSL_LED_SUPPORT					
#if 0
					led_support_check=1;//adsl led for 1.1.2.7.1.1
					adsl_led_flash();//adsl led for 1.1.2.7.1.1
#endif
					wake_up_interruptible(&wait_queue_reboot);	// wait up ioctl reboot
				}
#endif
      			}                  
        }
//	meiLongwordWrite(ARC_TO_MEI_INT, ARC_TO_MEI_MSGAV);
	mask_and_ack_amazon_irq(AMAZON_MEI_INT);
        return;
}

// 603221:tc.chen start
////////////////////////hdlc ////////////////

// get hdlc status
static unsigned int ifx_me_hdlc_status(void)
{
	u16 CMVMSG[MSG_LENGTH]; 
	int ret;

	if (showtime!=1)
		return -ENETRESET;
	
	makeCMV_local(H2D_CMV_READ, STAT, 14, 0, 1, NULL,CMVMSG);	//Get HDLC status 
	ret = mei_ioctl((struct inode *)0,NULL, AMAZON_MEI_CMV_WINHOST, CMVMSG);
	if (ret != 0)
	{
		return -EIO;
	}
	return CMVMSG[4]&0x0F;
}

int ifx_me_is_resloved(int status)
{
	u16 CMVMSG[MSG_LENGTH];
	int ret;
	
	if (status == ME_HDLC_MSG_QUEUED || status == ME_HDLC_MSG_SENT)
		return ME_HDLC_UNRESOLVED;
	if (status == ME_HDLC_IDLE)
	{
		makeCMV_local(H2D_CMV_READ, CNTL, 2, 0, 1, NULL,CMVMSG);	//Get ME-HDLC Control
		ret = mei_ioctl((struct inode *)0,NULL, AMAZON_MEI_CMV_WINHOST, CMVMSG);
		if (ret != 0)
		{
			return IFX_POP_EOC_FAIL;
		}
		if (CMVMSG[4]&(1<<0))
		{
			return ME_HDLC_UNRESOLVED;
		}
		
	}
	return ME_HDLC_RESOLVED;
}

int _ifx_me_hdlc_send(unsigned char *hdlc_pkt,int len,int max_length)
{
	int ret;
	u16 CMVMSG[MSG_LENGTH];
	u16 data=0;
	u16 pkt_len=len;
	if (pkt_len > max_length)
	{
		printk("Exceed maximum eoc message length\n");
		return -ENOBUFS;
	}
	//while(pkt_len > 0)
	{		
		makeCMV_local(H2D_CMV_WRITE, INFO, 81, 0, (pkt_len+1)/2,(u16 *)hdlc_pkt,CMVMSG);	//Write clear eoc message to ARC
		ret = mei_ioctl((struct inode *)0,NULL, AMAZON_MEI_CMV_WINHOST, CMVMSG);
		if (ret != 0)
		{
			return -EIO;
		}
		
		makeCMV_local(H2D_CMV_WRITE, INFO, 83, 2, 1,&pkt_len,CMVMSG);	//Update tx message length
		ret = mei_ioctl((struct inode *)0,NULL, AMAZON_MEI_CMV_WINHOST, CMVMSG);
		if (ret != 0)
		{
			return -EIO;
		}
		
		data = (1<<0);
		makeCMV_local(H2D_CMV_WRITE, CNTL, 2, 0, 1,&data,CMVMSG);	//Start to send
		ret = mei_ioctl((struct inode *)0,NULL, AMAZON_MEI_CMV_WINHOST, CMVMSG);
		if (ret != 0)
		{
			return -EIO;
		}
		return 0;
	}
}

static int ifx_me_hdlc_send(unsigned char *hdlc_pkt,int hdlc_pkt_len)
{
	int hdlc_status=0;
	u16 CMVMSG[MSG_LENGTH];
	int max_hdlc_tx_length=0,ret=0,retry=0;
	
	while(retry<10)
	{
		hdlc_status = ifx_me_hdlc_status();
		if (ifx_me_is_resloved(hdlc_status)==ME_HDLC_RESOLVED) // arc ready to send HDLC message
		{
			makeCMV_local(H2D_CMV_READ, INFO, 83, 0, 1, NULL,CMVMSG);	//Get Maximum Allowed HDLC Tx Message Length
			ret = mei_ioctl((struct inode *)0,NULL, AMAZON_MEI_CMV_WINHOST, CMVMSG);
			if (ret != 0)
			{
				return -EIO;
			}
			max_hdlc_tx_length = CMVMSG[4];
			ret = _ifx_me_hdlc_send(hdlc_pkt,hdlc_pkt_len,max_hdlc_tx_length);
			return ret;
		}
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(10);
	}
	return -EBUSY;
}

int ifx_mei_hdlc_read(char *hdlc_pkt,int max_hdlc_pkt_len)
{
	u16 CMVMSG[MSG_LENGTH]; 
	int msg_read_len,ret=0,pkt_len=0,retry = 0;
		
	while(retry<10)
	{
		ret = ifx_me_hdlc_status();
		if (ret == ME_HDLC_RESP_RCVD)
		{
			int current_size=0;
			makeCMV_local(H2D_CMV_READ, INFO, 83, 3, 1, NULL,CMVMSG);	//Get EoC packet length
			ret = mei_ioctl((struct inode *)0,NULL, AMAZON_MEI_CMV_WINHOST, CMVMSG);
			if (ret != 0)
			{
				return -EIO;
			}
	
			pkt_len = CMVMSG[4];
			if (pkt_len > max_hdlc_pkt_len)
			{
				ret = -ENOMEM;
				goto error;
			}
			while( current_size < pkt_len)
			{
				if (pkt_len - current_size >(MSG_LENGTH*2-8))
					msg_read_len = (MSG_LENGTH*2-8);
				else
					msg_read_len = pkt_len - (current_size);
				makeCMV_local(H2D_CMV_READ, INFO, 82, 0 + (current_size/2), (msg_read_len+1)/2, NULL,CMVMSG);	//Get hdlc packet
				ret = mei_ioctl((struct inode *)0,NULL, AMAZON_MEI_CMV_WINHOST, CMVMSG);
				if (ret != 0)
				{
					goto error;
				}
				memcpy(hdlc_pkt+current_size,&CMVMSG[4],msg_read_len);
				current_size +=msg_read_len;
			}
			ret = current_size;
			break;
		}else
		{
			ret = -ENODATA;
		}
		
		retry++;
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(10);
		
	}
	return ret;
error:
	
	return ret;
}

////////////////////////hdlc ////////////////
// 603221:tc.chen end

/////////////////////// clearEoC, int ifx_pop_eoc(sk_buff * pkt)  //////////
int ifx_pop_eoc(struct sk_buff * pkt);
int ifx_pop_eoc(struct sk_buff * pkt)
{
	amazon_clreoc_pkt * current;
	if(showtime!=1){
		dev_kfree_skb(pkt);
		return IFX_POP_EOC_FAIL;
	}
	if((pkt->len)>clreoc_max_tx_len){
		dev_kfree_skb(pkt);
		return IFX_POP_EOC_FAIL;
	}
	current = list_entry(clreoc_list.next, amazon_clreoc_pkt, list);
	while(1){
		if(current->len==0){
			memcpy(current->command, pkt->data, pkt->len);
			current->len=pkt->len;
			break;
		}
		else{
			if((current->list).next==&clreoc_list){
				dev_kfree_skb(pkt);
				return IFX_POP_EOC_FAIL;	//buffer full
			}
			current = list_entry((current->list).next,amazon_clreoc_pkt, list);
		}
	}	
	wake_up_interruptible(&wait_queue_clreoc);
	
	dev_kfree_skb(pkt);
	return IFX_POP_EOC_DONE;
}	
/*  this is used in circular fifo mode */
/*
int ifx_pop_eoc(sk_buff * pkt);
int ifx_pop_eoc(sk_buff * pkt)
{
	int buff_space,i;
	if(showtime!=1)
		return IFX_POP_EOC_FAIL;
	
	if(clreoc_wr>=clreoc_rd)
		buff_space = (MEI_CLREOC_BUFF_SIZE-1)-(clreoc_wr - clreoc_rd);
	else
		buff_space = clreoc_rd - clreoc_wr - 1;
	if((pkt->len)>buff_space)
		return IFX_POP_EOC_FAIL;
					
	if((clreoc_wr+pkt->len)>MEI_CLREOC_BUFF_SIZE){
		memcpy((clreoc+clreoc_wr), pkt->data, ((clreoc_wr+pkt->len)-MEI_CLREOC_BUFF_SIZE+1));
		memcpy(clreoc, (pkt->data)+((clreoc_wr+pkt->len)-MEI_CLREOC_BUFF_SIZE+1), (pkt->len)-((clreoc_wr+pkt->len)-MEI_CLREOC_BUFF_SIZE+1)); 
		clreoc_wr=(clreoc_wr+pkt->len)-MEI_CLREOC_BUFF_SIZE;
	}	
	else{
		memcpy((clreoc+clreoc_wr), pkt->data, pkt->len);
		if((clreoc_wr+pkt->len)=MEI_CLREOC_BUFF_SIZE)
			clreoc_wr=0;
		else
			clreoc_wr+=pkt->len;
	}
	wake_up_interruptible(&wait_queue_clreoc);
	return IFX_POP_EOC_DONE; 			
}
*/


////////////////////////////////////////////////////////////////////////////
//int amazon_mei_init_module (void);
//void amazon_mei_cleanup_module (void);
//int __init init_module (void);
//void __exit cleanup_module (void);

int __init amazon_mei_init_module(void)
//int __init init_module(void)
{
        struct proc_dir_entry *entry;
        int i;
	
//dying gasp-start	
#ifdef IFX_DYING_GASP

//000003:fchang Start
#ifdef CONFIG_CPU_AMAZON_E
		//GPIO31 :dying gasp event indication
		//	(1) logic high: dying gasp event is false (default)
		//	(2) logic low: dying gasp event is true
			CLEAR_BIT((*((volatile u32 *)0xB0100B18)), 0x4);
			CLEAR_BIT((*((volatile u32 *)0xB0100B1c)), 0x4);
			CLEAR_BIT((*((volatile u32 *)0xB0100B20)), 0x4);
			SET_BIT((*((volatile u32 *)0xB0100B24)), 0x4);
			asm("SYNC");			
#else //000003:fchang End

		//GPIO31 :dying gasp event indication
		//	(1) logic high: dying gasp event is false (default)
		//	(2) logic low: dying gasp event is true
			CLEAR_BIT((*((volatile u32 *)0xB0100B48)), 0x8000);
			CLEAR_BIT((*((volatile u32 *)0xB0100B4C)), 0x8000);
			CLEAR_BIT((*((volatile u32 *)0xB0100B50)), 0x8000);
			SET_BIT((*((volatile u32 *)0xB0100B54)), 0x8000);
#if 0			
//warning-led-start	
//GPIO 22		
			SET_BIT ((*((volatile u32 *)0xB0100B48)), 0x40);
			CLEAR_BIT((*((volatile u32 *)0xB0100B4C)), 0x40);
			CLEAR_BIT((*((volatile u32 *)0xB0100B50)), 0x40);
			SET_BIT((*((volatile u32 *)0xB0100B54)), 0x40);			
			CLEAR_BIT((*((volatile u32 *)0xB0100B40)), 0x40); //GPIO ON
			printk("LED ON ON ON ON ON ON.....");
//warning-led-end			
#endif
			asm("SYNC");	
#endif //000003:fchang

#endif //IFX_DYING_GASP	
//dying gasp -end        
        
	
        reg_entry_t regs_temp[PROC_ITEMS] =                                     // Items being debugged
        {
        /*	{       flag,          name,          description } */
	{ &arcmsgav, "arcmsgav", "arc to mei message ", 0 },
            { &cmv_reply, "cmv_reply", "cmv needs reply", 0},
            { &cmv_waiting, "cmv_waiting", "waiting for cmv reply from arc", 0},
            { &indicator_count, "indicator_count", "ARC to MEI indicator count", 0},
            { &cmv_count, "cmv_count", "MEI to ARC CMVs", 0},
            { &reply_count, "reply_count", "ARC to MEI Reply", 0},
            { (int *)Recent_indicator, "Recent_indicator", "most recent indicator", 0},
	    { (int *)8, "version", "version of firmware", 0},
        };
        memcpy((char *)regs, (char *)regs_temp, sizeof(regs_temp));


        //sema_init(&mei_sema, 0);  // semaphore initialization, mutex
        sema_init(&mei_sema, 1);  // semaphore initialization, mutex
	
        init_waitqueue_head(&wait_queue_arcmsgav);         	// for ARCMSGAV
	init_waitqueue_head(&wait_queue_codeswap);		// for codeswap daemon
	init_waitqueue_head(&wait_queue_mibdaemon);		// for mib daemon
	init_waitqueue_head(&wait_queue_reboot);		// for ioctl reboot 
	init_waitqueue_head(&wait_queue_clreoc);		// for clreoc_wr function
	init_waitqueue_head(&wait_queue_loop_diagnostic);		// for loop diagnostic function
#ifdef ADSL_LED_SUPPORT	
	init_waitqueue_head(&wait_queue_led);			// adsl led for led function
	init_waitqueue_head(&wait_queue_led_polling);		// adsl led for led function
	led_task.routine = adsl_led_flash_task;			// adsl led for led function
	led_poll_init();					// adsl led for led function
#endif	//ADSL_LED_SUPPORT
#ifdef IFX_DYING_GASP	
	init_waitqueue_head(&wait_queue_dying_gasp);		// IFX_DYING_GASP
	lop_poll_init();					// IFX_DYING_GASP 
#endif	//IFX_DYING_GASP
 
	init_waitqueue_head(&wait_queue_uas_poll);//joelin 04/16/2005
	unavailable_seconds_poll_init();//joelin 04/16/2005
	memset(&mib_pflagtime, 0, (sizeof(mib_flags_pretime)));
	
	// initialize link list for intervals
	mei_mib = (amazon_mei_mib *)kmalloc((sizeof(amazon_mei_mib)*INTERVAL_NUM), GFP_KERNEL);
	if(mei_mib == NULL){
#ifdef AMAZON_MEI_DEBUG_ON
		printk("kmalloc error for amazon_mei_mib\n\n");
#endif
		return -1;
	}
	memset(mei_mib, 0, (sizeof(amazon_mei_mib)*INTERVAL_NUM));
	INIT_LIST_HEAD(&interval_list);
	for(i=0;i<INTERVAL_NUM;i++)
		list_add_tail(&(mei_mib[i].list), &interval_list); 
	current_intvl = list_entry(interval_list.next, amazon_mei_mib, list);
	do_gettimeofday(&(current_intvl->start_time)); 
	// initialize clreoc list
	clreoc_pkt = (amazon_clreoc_pkt *)kmalloc((sizeof(amazon_clreoc_pkt)*CLREOC_BUFF_SIZE), GFP_KERNEL);
	if(clreoc_pkt == NULL){
#ifdef AMAZON_MEI_DEBUG_ON
		printk("kmalloc error for clreoc_pkt\n\n");
#endif
		return -1;
	}
	memset(clreoc_pkt, 0, (sizeof(amazon_clreoc_pkt)*CLREOC_BUFF_SIZE));
	INIT_LIST_HEAD(&clreoc_list);
	for(i=0;i<CLREOC_BUFF_SIZE;i++)
		list_add_tail(&(clreoc_pkt[i].list), &clreoc_list);
		
        memset(&AdslInitStatsData, 0, sizeof(AdslInitStatsData));
        if (register_chrdev(major, "amazon_mei", &mei_operations)!=0) {
#ifdef AMAZON_MEI_DEBUG_ON
                printk("\n\n unable to register major for amazon_mei!!!");
#endif
                return -1;
        }
        if (request_irq(AMAZON_MEI_INT, mei_interrupt_arcmsgav,0, "amazon_mei_arcmsgav", NULL)!=0){
#ifdef AMAZON_MEI_DEBUG_ON
                printk("\n\n unable to register irq for amazon_mei!!!");
#endif
                return -1;
        }
//        disable_irq(AMAZON_MEI_INT);
	enable_irq(AMAZON_MEI_INT);
        // procfs
        meidir=proc_mkdir(MEI_DIRNAME, &proc_root);
        if ( meidir == NULL) {
#ifdef AMAZON_MEI_DEBUG_ON
		printk(KERN_ERR ": can't create /proc/" MEI_DIRNAME "\n\n");
#endif
		return(-ENOMEM);
        }

        for(i=0;i<NUM_OF_REG_ENTRY;i++) {
		entry = create_proc_entry(regs[i].name,
				S_IWUSR |S_IRUSR | S_IRGRP | S_IROTH,
				meidir);
		if(entry) {
			regs[i].low_ino = entry->low_ino;
			entry->proc_fops = &proc_operations;
		} else {
#ifdef AMAZON_MEI_DEBUG_ON
			printk( KERN_ERR 
				": can't create /proc/" MEI_DIRNAME
				"/%s\n\n", regs[i].name);
#endif
			return(-ENOMEM);
		}
        }
        ///////////////////////////////// register net device ////////////////////////////
        if(register_netdev(&phy_mei_net)!=0){
#ifdef AMAZON_MEI_DEBUG_ON
                printk("\n\n Register phy Device Failed.");
#endif
                return -1;
        }
/*
	if(register_netdev(&interleave_mei_net)!=0){
                printk("\n\n Register interleave Device Failed.");
                return -1;
        }
	if(register_netdev(&fast_mei_net)!=0){
                printk("\n\n Register fast Device Failed.");
                return -1;
        }
*/
#ifdef DFE_LOOPBACK
	mei_arc_swap_buff = (u32 *)kmalloc(MAXSWAPSIZE*4, GFP_KERNEL);
	if (mei_arc_swap_buff){
#ifdef	ARC_READY_ACK
		if(down_interruptible(&mei_sema))	//disable CMV access until ARC ready
		{
                	return -ERESTARTSYS;
		}
#ifdef LOCK_RETRY
		reboot_lock = 1;
#endif
#endif
		meiForceRebootAdslModem();
		kfree(mei_arc_swap_buff);
	}else{
#ifdef AMAZON_MEI_DEBUG_ON
		printk("cannot load image: no memory\n\n");
#endif
	}
#endif
#ifdef IFX_SMALL_FOOTPRINT
	mib_poll_init();
#endif
        return 0;
}

void __exit amazon_mei_cleanup_module(void)
//void __exit cleanup_module(void)
{
        int i;
#ifdef ADSL_LED_SUPPORT        
        stop_led_module=1;			//wake up and clean led module 
        led_support_check=0;//joelin , clear task
        showtime=0;//joelin,clear task
        //CLEAR_BIT((*((volatile u32 *)0xB0100B40)), 0x40); //Warning LED GPIO ON
        firmware_support_led=0;//joelin ,clear task
        wake_up_interruptible(&wait_queue_led); //wake up and clean led module 
        wake_up_interruptible(&wait_queue_led_polling); //wake up and clean led module         
#endif        
        for(i=0;i<NUM_OF_REG_ENTRY;i++)
		remove_proc_entry(regs[i].name, meidir);
        remove_proc_entry(MEI_DIRNAME, &proc_root);
	
        disable_irq(AMAZON_MEI_INT);
        free_irq(AMAZON_MEI_INT, NULL);
        unregister_chrdev(major, "amazon_mei");

        kfree(mei_mib);
	kfree(clreoc_pkt);
	
	kfree(phy_mei_net.priv);
        unregister_netdev(&phy_mei_net);
	
        return;
}
#ifdef IFX_SMALL_FOOTPRINT


int adsl_mib_poll(void *unused)
{
	struct task_struct *tsk = current;
	int i=0;
	struct timeval time_now;
	struct timeval time_fini;
	u32 temp,temp2;

	amazon_mei_mib * mib_ptr;
//	u16 buff[MSG_LENGTH]__attribute__ ((aligned(4)));
	u16 * data=NULL;  //used in makeCMV, to pass in payload when CMV set, ignored when CMV read.

	daemonize();
	strcpy(tsk->comm, "kmibpoll");
	sigfillset(&tsk->blocked);

	printk("Inside mib poll loop ...\n");
	i=0;
	while(1){
		if(i<MIB_INTERVAL)
			interruptible_sleep_on_timeout(&wait_queue_mibdaemon, ((MIB_INTERVAL-i)/(1000/HZ)));
		i=0;
		if(showtime==1){
//			printk("\n\n update mib");
					
			do_gettimeofday(&time_now);
			if(time_now.tv_sec - current_intvl->start_time.tv_sec>=900){
				if(current_intvl->list.next!=&interval_list){
					current_intvl = list_entry(current_intvl->list.next, amazon_mei_mib, list);
					do_gettimeofday(&(current_intvl->start_time));
				}
				else{
					mib_ptr = list_entry(interval_list.next, amazon_mei_mib, list);
					list_del(interval_list.next);
					memset(mib_ptr, 0, sizeof(amazon_mei_mib));
					list_add_tail(&(mib_ptr->list), &interval_list);
					if(current_intvl->list.next==&interval_list)
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nlink list error");
#endif
					current_intvl = list_entry(current_intvl->list.next, amazon_mei_mib, list);
					do_gettimeofday(&(current_intvl->start_time));
				}	
			}
					
			if(down_interruptible(&mei_sema))
      				return -ERESTARTSYS;
/*						
			ATUC_PERF_LO_FLAG_MAKECMV;
			if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
				printk("\n\nCMV fail, Group 7 Address 0 Index 0");
#endif
			}
			else{
				if(RxMessage[4]&PLAM_LOS_FailureBit){
					current_intvl->AtucPerfLos++;
					ATUC_PERF_LOSS++;
					CurrStatus.adslAtucCurrStatus = 2;
				}
				if(RxMessage[4]&PLAM_LOF_FailureBit){
					current_intvl->AtucPerfLof++;
					ATUC_PERF_LOFS++;
					CurrStatus.adslAtucCurrStatus = 1;
				}
				if(!(RxMessage[4]&(PLAM_LOS_FailureBit|PLAM_LOF_FailureBit)))
					CurrStatus.adslAtucCurrStatus = 0;
				}
*/
				
				if(showtime!=1)
					goto mib_poll_end;
				ATUC_PERF_ESS_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 7 Address 7 Index 0");
#endif
				}
				else{
						temp = RxMessage[4]-mib_pread.ATUC_PERF_ESS;
						if(temp>=0){
							current_intvl->AtucPerfEs+=temp;
							ATUC_PERF_ESS+=temp;
							mib_pread.ATUC_PERF_ESS = RxMessage[4];
						}
						else{
							current_intvl->AtucPerfEs+=0xffff-mib_pread.ATUC_PERF_ESS+RxMessage[4];
							ATUC_PERF_ESS+=0xffff-mib_pread.ATUC_PERF_ESS+RxMessage[4];
							mib_pread.ATUC_PERF_ESS = RxMessage[4];		
					}
				}
/*		
				ATUR_PERF_LO_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 7 Address 1 Index 0");
#endif
				}
				else{
					if(RxMessage[4]&PLAM_LOS_FailureBit){
						current_intvl->AturPerfLos++;
						ATUR_PERF_LOSS++;
						CurrStatus.adslAturCurrStatus = 2;
					}
					if(RxMessage[4]&PLAM_LOF_FailureBit){
						current_intvl->AturPerfLof++;
						ATUR_PERF_LOFS++;
						CurrStatus.adslAturCurrStatus = 1;
					}
					if(RxMessage[4]&PLAM_LPR_FailureBit){
						current_intvl->AturPerfLpr++;
						ATUR_PERF_LPR++;
						CurrStatus.adslAturCurrStatus = 3;
					}
					if(!(RxMessage[4]&(PLAM_LOS_FailureBit|PLAM_LOF_FailureBit|PLAM_LPR_FailureBit)))
						CurrStatus.adslAturCurrStatus = 0;	
				}
*/
				if(showtime!=1)
					goto mib_poll_end;
				ATUR_PERF_ESS_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 7 Address 33 Index 0");
#endif
				}
				else{
						temp = RxMessage[4]-mib_pread.ATUR_PERF_ESS;
						if(temp>=0){
							current_intvl->AturPerfEs+=temp;
							ATUR_PERF_ESS+=temp;
							mib_pread.ATUR_PERF_ESS = RxMessage[4];
						}
						else{
							current_intvl->AturPerfEs+=0xffff-mib_pread.ATUR_PERF_ESS+RxMessage[4];
							ATUR_PERF_ESS+=	0xffff-mib_pread.ATUR_PERF_ESS+RxMessage[4];
							mib_pread.ATUR_PERF_ESS=RxMessage[4];
					}
				}
				if(showtime!=1)
					goto mib_poll_end;
				// to update rx/tx blocks
				ATUR_CHAN_RECV_BLK_FLAG_MAKECMV_LSW;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 7 Address 20 Index 0");
#endif
				}
				else{
						temp = RxMessage[4];	
				}
				if(showtime!=1)
					goto mib_poll_end;
				ATUR_CHAN_RECV_BLK_FLAG_MAKECMV_MSW;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 7 Address 21 Index 0");
#endif
				}
				else{
						temp2 = RxMessage[4];	
					}	
					if((temp + (temp2<<16) - mib_pread.ATUR_CHAN_RECV_BLK)>=0){
						current_intvl->AturChanPerfRxBlk+=temp + (temp2<<16) - mib_pread.ATUR_CHAN_RECV_BLK;
						ATUR_CHAN_RECV_BLK+=temp + (temp2<<16) - mib_pread.ATUR_CHAN_RECV_BLK;
						mib_pread.ATUR_CHAN_RECV_BLK = temp + (temp2<<16);
					}
					else{
						current_intvl->AturChanPerfRxBlk+=0xffffffff - mib_pread.ATUR_CHAN_RECV_BLK +(temp + (temp2<<16));
						ATUR_CHAN_RECV_BLK+=0xffffffff - mib_pread.ATUR_CHAN_RECV_BLK +(temp + (temp2<<16));
						mib_pread.ATUR_CHAN_RECV_BLK = temp + (temp2<<16);
				}
				current_intvl->AturChanPerfTxBlk = current_intvl->AturChanPerfRxBlk;
				ATUR_CHAN_TX_BLK = ATUR_CHAN_RECV_BLK;
/*					
				ATUR_CHAN_TX_BLK_FLAG_MAKECMV_LSW;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS)
					printk("\n\nCMV fail, Group 7 Address 20 Index 0");
				else{
					if(RxMessage[4]){
						current_intvl->AturChanPerfTxBlk+=RxMessage[4];
						ATUR_CHAN_TX_BLK+=RxMessage[4];
					}	
				}
				ATUR_CHAN_TX_BLK_FLAG_MAKECMV_MSW;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS)
					printk("\n\nCMV fail, Group 7 Address 21 Index 0");
				else{
					if(RxMessage[4]){
						current_intvl->AturChanPerfTxBlk+=(int)((RxMessage[4])<<16);
						ATUR_CHAN_TX_BLK+=(int)((RxMessage[4])<<16);
					}	
				}
*/					
				if(chantype.interleave == 1){
				if(showtime!=1)
					goto mib_poll_end;
					ATUR_CHAN_CORR_BLK_FLAG_MAKECMV_INTL;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group 7 Address 3 Index 0");
#endif
					}
					else{
							temp = RxMessage[4] - mib_pread.ATUR_CHAN_CORR_BLK_INTL;
							if(temp>=0){
								current_intvl->AturChanPerfCorrBlk+=temp;
								ATUR_CHAN_CORR_BLK+=temp;
								mib_pread.ATUR_CHAN_CORR_BLK_INTL = RxMessage[4];
							}	
							else{
								current_intvl->AturChanPerfCorrBlk+=0xffff - mib_pread.ATUR_CHAN_CORR_BLK_INTL +RxMessage[4];
								ATUR_CHAN_CORR_BLK+=0xffff - mib_pread.ATUR_CHAN_CORR_BLK_INTL +RxMessage[4];
								mib_pread.ATUR_CHAN_CORR_BLK_INTL = RxMessage[4];	
						}	
					}	
				}
				else if(chantype.fast == 1){
				if(showtime!=1)
					goto mib_poll_end;
					ATUR_CHAN_CORR_BLK_FLAG_MAKECMV_FAST;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group 7 Address 3 Index 1");
#endif
					}
					else{
							temp = RxMessage[4] - mib_pread.ATUR_CHAN_CORR_BLK_FAST;
							if(temp>=0){
								current_intvl->AturChanPerfCorrBlk+=temp;
								ATUR_CHAN_CORR_BLK+=temp;
								mib_pread.ATUR_CHAN_CORR_BLK_FAST = RxMessage[4];
							}	
							else{
								current_intvl->AturChanPerfCorrBlk+=0xffff - mib_pread.ATUR_CHAN_CORR_BLK_FAST + RxMessage[4];
								ATUR_CHAN_CORR_BLK+=0xffff - mib_pread.ATUR_CHAN_CORR_BLK_FAST + RxMessage[4];
								mib_pread.ATUR_CHAN_CORR_BLK_FAST = RxMessage[4];
						}	
					}		
				}
				
				if(chantype.interleave == 1){
				if(showtime!=1)
					goto mib_poll_end;
					ATUR_CHAN_UNCORR_BLK_FLAG_MAKECMV_INTL;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group 7 Address 2 Index 0");
#endif
					}
					else{
							temp = RxMessage[4] - mib_pread.ATUR_CHAN_UNCORR_BLK_INTL;
							if(temp>=0){
								current_intvl->AturChanPerfUncorrBlk+=temp;
								ATUR_CHAN_UNCORR_BLK+=temp;
								mib_pread.ATUR_CHAN_UNCORR_BLK_INTL = RxMessage[4];
							}
							else{
								current_intvl->AturChanPerfUncorrBlk+=0xffff - mib_pread.ATUR_CHAN_UNCORR_BLK_INTL + RxMessage[4];
								ATUR_CHAN_UNCORR_BLK+=0xffff - mib_pread.ATUR_CHAN_UNCORR_BLK_INTL + RxMessage[4];
								mib_pread.ATUR_CHAN_UNCORR_BLK_INTL = RxMessage[4];
						}
					}		
				}
				else if(chantype.fast == 1){
				if(showtime!=1)
					goto mib_poll_end;
					ATUR_CHAN_UNCORR_BLK_FLAG_MAKECMV_FAST;
					if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
						printk("\n\nCMV fail, Group 7 Address 2 Index 1");
#endif
					}
					else{
							temp = RxMessage[4] - mib_pread.ATUR_CHAN_UNCORR_BLK_FAST;
							if(temp>=0){
								current_intvl->AturChanPerfUncorrBlk+=temp;
								ATUR_CHAN_UNCORR_BLK+=temp;
								mib_pread.ATUR_CHAN_UNCORR_BLK_FAST = RxMessage[4];
							}
							else{
								current_intvl->AturChanPerfUncorrBlk+=0xffff - mib_pread.ATUR_CHAN_UNCORR_BLK_FAST + RxMessage[4];
								ATUR_CHAN_UNCORR_BLK+=0xffff - mib_pread.ATUR_CHAN_UNCORR_BLK_FAST + RxMessage[4];
								mib_pread.ATUR_CHAN_UNCORR_BLK_FAST = RxMessage[4];
						}
					}		
				}
					
				//RFC-3440

#ifdef AMAZON_MEI_MIB_RFC3440
				if(showtime!=1)
					goto mib_poll_end;
				ATUC_PERF_STAT_FASTR_FLAG_MAKECMV; //???
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 0 Address 0 Index 0");
#endif
				}
				else{
						temp = RxMessage[4] - mib_pread.ATUC_PERF_STAT_FASTR;
						if(temp>=0){
							current_intvl->AtucPerfStatFastR+=temp;
							ATUC_PERF_STAT_FASTR+=temp;
							mib_pread.ATUC_PERF_STAT_FASTR = RxMessage[4];
						}
						else{
							current_intvl->AtucPerfStatFastR+=0xffff - mib_pread.ATUC_PERF_STAT_FASTR + RxMessage[4];
							ATUC_PERF_STAT_FASTR+=0xffff - mib_pread.ATUC_PERF_STAT_FASTR + RxMessage[4];
							mib_pread.ATUC_PERF_STAT_FASTR = RxMessage[4];
					}
				}
				if(showtime!=1)
					goto mib_poll_end;
				ATUC_PERF_STAT_FAILED_FASTR_FLAG_MAKECMV; //???
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 0 Address 0 Index 0");
#endif
				}
				else{
						temp = RxMessage[4] - mib_pread.ATUC_PERF_STAT_FAILED_FASTR;
						if(temp>=0){
							current_intvl->AtucPerfStatFailedFastR+=temp;
							ATUC_PERF_STAT_FAILED_FASTR+=temp;
							mib_pread.ATUC_PERF_STAT_FAILED_FASTR = RxMessage[4];
						}
						else{
							current_intvl->AtucPerfStatFailedFastR+=0xffff - mib_pread.ATUC_PERF_STAT_FAILED_FASTR + RxMessage[4];
							ATUC_PERF_STAT_FAILED_FASTR+=0xffff - mib_pread.ATUC_PERF_STAT_FAILED_FASTR + RxMessage[4];
							mib_pread.ATUC_PERF_STAT_FAILED_FASTR = RxMessage[4];
					}
				}
				if(showtime!=1)
					goto mib_poll_end;
				ATUC_PERF_STAT_SESL_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 7 Address 8 Index 0");
#endif
				}
				else{
						temp = RxMessage[4] - mib_pread.ATUC_PERF_STAT_SESL;
						if(temp>=0){
							current_intvl->AtucPerfStatSesL+=temp;
							ATUC_PERF_STAT_SESL+=temp;
							mib_pread.ATUC_PERF_STAT_SESL = RxMessage[4];
						}
						else{
							current_intvl->AtucPerfStatSesL+=0xffff - mib_pread.ATUC_PERF_STAT_SESL + RxMessage[4];
							ATUC_PERF_STAT_SESL+=0xffff - mib_pread.ATUC_PERF_STAT_SESL + RxMessage[4];
							mib_pread.ATUC_PERF_STAT_SESL = RxMessage[4];
					}
				}
				if(showtime!=1)
					goto mib_poll_end;
				ATUC_PERF_STAT_UASL_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 7 Address 10 Index 0");
#endif
				}
				else{
						temp = RxMessage[4] - mib_pread.ATUC_PERF_STAT_UASL;
						if(temp>=0){
							current_intvl->AtucPerfStatUasL+=temp;
							ATUC_PERF_STAT_UASL+=temp;
							mib_pread.ATUC_PERF_STAT_UASL = RxMessage[4];
						}
						else{
							current_intvl->AtucPerfStatUasL+=0xffff - mib_pread.ATUC_PERF_STAT_UASL + RxMessage[4];
							ATUC_PERF_STAT_UASL+=0xffff - mib_pread.ATUC_PERF_STAT_UASL + RxMessage[4];
							mib_pread.ATUC_PERF_STAT_UASL = RxMessage[4];
					}
				}
				if(showtime!=1)
					goto mib_poll_end;
				ATUR_PERF_STAT_SESL_FLAG_MAKECMV;
				if(meiCMV(TxMessage, YES_REPLY)!=MEI_SUCCESS){
#ifdef AMAZON_MEI_DEBUG_ON
					printk("\n\nCMV fail, Group 7 Address 34 Index 0");
#endif
				}
				else{
						temp = RxMessage[4] - mib_pread.ATUR_PERF_STAT_SESL;
						if(temp>=0){
							current_intvl->AtucPerfStatUasL+=temp;
							ATUC_PERF_STAT_UASL+=temp;
							mib_pread.ATUR_PERF_STAT_SESL = RxMessage[4];
						}
						else{
							current_intvl->AtucPerfStatUasL+=0xffff - mib_pread.ATUR_PERF_STAT_SESL + RxMessage[4];
							ATUC_PERF_STAT_UASL+=0xffff - mib_pread.ATUR_PERF_STAT_SESL + RxMessage[4];
							mib_pread.ATUR_PERF_STAT_SESL = RxMessage[4];
					}
				}
					
#endif
mib_poll_end:
				up(&mei_sema);
				
				do_gettimeofday(&time_fini);
				i = ((int)((time_fini.tv_sec-time_now.tv_sec)*1000)) + ((int)((time_fini.tv_usec-time_now.tv_usec)/1000))  ; //msec 
			}//showtime==1
		}	 
	
}
int mib_poll_init(void)
{
	printk("Starting mib_poll...\n");

	kernel_thread(adsl_mib_poll, NULL, CLONE_FS | CLONE_FILES | CLONE_SIGNAL);
	return 0;
}
#endif //IFX_SMALL_FOOTPRINT
//EXPORT_NO_SYMBOLS;

#ifdef ADSL_LED_SUPPORT
// adsl led -start
int led_status_on=0,led_need_to_flash=0;
int led_current_flashing=0;
unsigned long led_delay=0;
static int led_poll(void *unused)
{
	stop_led_module=0;	//begin polling ...
	while(!stop_led_module){
		if ((!led_status_on)&&(!led_need_to_flash)) interruptible_sleep_on_timeout (&wait_queue_led_polling,1000); //10 seconds timeout for waiting wakeup
//			else printk("direct running task, no waiting");
		run_task_queue(&tq_ifx_led);//joelin task
//	printk("led and LOP polling...\n");
		}
	return 0;	
}	
static int led_poll_init(void)
{
//	printk("Starting adsl led polling...\n");

//warning-led-start
//	CLEAR_BIT((*((volatile u32 *)0xB0100B40)), 0x40); //Warning LED GPIO ON
//warning-led-end

	kernel_thread(led_poll, NULL, CLONE_FS | CLONE_FILES | CLONE_SIGNAL);
	return 0;
}

int adsl_led_flash(void)
{	
	int i;
	if (!firmware_support_led)	return 0;	//joelin version check 

	if (led_status_on == 0 && led_need_to_flash == 0)
	{
		queue_task(&led_task, &tq_ifx_led);//joelin task
		wake_up_interruptible(&wait_queue_led_polling); //wake up and clean led module 
//		printk("queue Task 1...\n");	//joelin  test	
	}
	led_need_to_flash=1;//asking to flash led

	return 0;
}

int adsl_led_flash_task(void *ptr)
{

	u16	one=1;
	u16	zero=0;
	u16	data=0x0600;
	int kernel_use=1;
	u16 CMVMSG[MSG_LENGTH];                                                                           
//adsl-led-start for >v1.1.2.7.1.1
//	printk("Task Running...\n");	//joelin  test
	if ((firmware_support_led==2)&&(led_support_check))
	{
	led_support_check=0;
	data=0x0600;
	makeCMV_local(H2D_CMV_WRITE, INFO, 91, 0, 1, &data,CMVMSG);	//configure GPIO9 GPIO10 as outputs
	mei_ioctl((struct inode *)0,NULL, AMAZON_MEI_CMV_WINHOST, CMVMSG);

	makeCMV_local(H2D_CMV_WRITE, INFO, 91, 2, 1, &data,CMVMSG);	//enable writing to bit 9 and bit10
	mei_ioctl((struct inode *)0,NULL, AMAZON_MEI_CMV_WINHOST, CMVMSG);
	
	data=0x0a01;
	makeCMV_local(H2D_CMV_WRITE, INFO, 91, 4, 1, &data,CMVMSG);	//use GPIO10 for TR68 .Enable and don't invert.
	mei_ioctl((struct inode *)0,NULL, AMAZON_MEI_CMV_WINHOST, CMVMSG);	
	
#ifdef DATA_LED_ON_MODE	
	data=0x0903;//tecom //use GPIO9 for TR68 data led .turn on.
#else
	data=0x0900;
#endif	
	makeCMV_local(H2D_CMV_WRITE, INFO, 91, 5, 1, &data,CMVMSG);	//use GPIO9 for TR68 data led .turn off.
	mei_ioctl((struct inode *)0,NULL, AMAZON_MEI_CMV_WINHOST, CMVMSG);		
	
	}
	if (!showtime) {led_need_to_flash=0; return 0;} 
//adsl-led-end for >v1.1.2.7.1.1

	if (led_status_on == 0 || led_need_to_flash == 1)
	{

		if (led_current_flashing==0)
		{
			if (firmware_support_led==1){//>1.1.2.3.1.1
			makeCMV_local(H2D_CMV_WRITE, INFO, 91, 0, 1, &one,CMVMSG);	//flash
			mei_ioctl((struct inode *)0,NULL, AMAZON_MEI_CMV_WINHOST, &CMVMSG);	
		}
			else if (firmware_support_led==2){//>1.1.2.7.1.1
				data=0x0901;//flash
				makeCMV_local(H2D_CMV_WRITE, INFO, 91, 5, 1, &data,CMVMSG);	//use GPIO9 for TR68 data led .flash.
				mei_ioctl((struct inode *)0,NULL, AMAZON_MEI_CMV_WINHOST, &CMVMSG);			
				
			}//(firmware_support_led==2)
			led_current_flashing = 1;//turn on led
		}
		led_status_on=1;

		do{//do nothing , waiting untill no data traffic 
			led_need_to_flash=0;
			interruptible_sleep_on_timeout(&wait_queue_led, 25); //the time for LED Off , if no data traffic			 
		}while(led_need_to_flash==1);
		
	}else if (led_status_on == 1 && led_need_to_flash==0)
	{
		if (led_current_flashing==1)
		{//turn off led
			if (firmware_support_led==1){//>1.1.2.3.1.1
			makeCMV_local(H2D_CMV_WRITE, INFO, 91, 0, 1, &zero,CMVMSG);//off	
			mei_ioctl((struct inode *)0,NULL, AMAZON_MEI_CMV_WINHOST, &CMVMSG);
			}	//>1.1.2.3.1.1		
			else if (firmware_support_led==2){//>1.1.2.7.1.1
#ifdef DATA_LED_ON_MODE					
				data=0x0903;//tecom //use GPIO9 for TR68 data led .turn on.
#else
				data=0x0900;//off
#endif				
				makeCMV_local(H2D_CMV_WRITE, INFO, 91, 5, 1, &data,CMVMSG);	//use GPIO9 for TR68 data led .off.
			mei_ioctl((struct inode *)0,NULL, AMAZON_MEI_CMV_WINHOST, &CMVMSG);
				
			}//(firmware_support_led==2)
			led_status_on=0;
			led_current_flashing = 0;
		}
		}
	
	if (led_status_on == 1 || led_need_to_flash)
	{//led flash job on going or led need to flash 
		queue_task(&led_task, &tq_ifx_led);	//joelin task	
//		printk("queue Task 2...\n");	//joelin  test	
	}
	return 0;
}
//joelin adsl led-end
#else 
int adsl_led_flash(void)
{
	return 0;
}
#endif //ADSL_LED_SUPPORT
#ifdef IFX_DYING_GASP
static int lop_poll(void *unused)
{
	
	while(1)
	{
	interruptible_sleep_on_timeout(&wait_queue_dying_gasp, 1); 
#ifdef CONFIG_CPU_AMAZON_E //000003:fchang
        if(showtime&&((*((volatile u32 *)0xB0100B14))&0x4)==0x0)	{//000003:fchang
#else //000003:fchang
	if(showtime&&((*((volatile u32 *)0xB0100B44))&0x8000)==0x0)	{
#endif //CONFIG_CPU_AMAZON_E
		mei_ioctl((struct inode *)0,NULL, AMAZON_MEI_WRITEDEBUG, &lop_debugwr);
		printk("send dying gasp..\n");}
		
	}
	return 0;	
	}
static int lop_poll_init(void)
{
//	printk("Starting LOP polling...\n");
	kernel_thread(lop_poll, NULL, CLONE_FS | CLONE_FILES | CLONE_SIGNAL);
	return 0;
}

#endif //IFX_DYING_GASP

//joelin 04/16/2005-satrt
static int unavailable_seconds_poll(void *unused)
{
	while(1){
		interruptible_sleep_on_timeout (&wait_queue_uas_poll,100); //1 second timeout for waiting wakeup
   		if (!showtime) unavailable_seconds++;
	}
	return 0;	
}	
static int unavailable_seconds_poll_init(void)
{
  
	kernel_thread(unavailable_seconds_poll, NULL, CLONE_FS | CLONE_FILES | CLONE_SIGNAL);
	return 0;
}


//joelin 04/16/2005-end
EXPORT_SYMBOL(meiDebugWrite);
EXPORT_SYMBOL(ifx_pop_eoc);

MODULE_LICENSE("GPL");

module_init(amazon_mei_init_module);
module_exit(amazon_mei_cleanup_module);

