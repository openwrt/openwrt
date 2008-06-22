/******************************************************************************
**
** FILE NAME    : ifxmips_mei_core.c
** PROJECT      : Danube
** MODULES      : MEI
**
** DATE         : 1 Jan 2006
** AUTHOR       : TC Chen
** DESCRIPTION  : MEI Driver
** COPYRIGHT    :       Copyright (c) 2006
**                      Infineon Technologies AG
**                      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Version $Date      $Author     $Comment
   1.00.01             TC Chen     Fixed cell rate calculation issue
   				   Fixed pvovider_id of adsl mib swapping issue
   1.00.02             TC Chen     Added L3 Low Poewr Mode support.   
   1.00.03             TC Chen     Fixed Clear Eoc transmit issue.
   1.00.04  31/08/2006 TC Chen     Add ADSL Link/Data Led 
				   Add Dual Latency Path
                                   Add AUTOBOOT_ENABLE_SET ioctl for autoboot 
				        mode enable/disable  
                                   Fix fast path cell rate calculation
   1.00.05  25/09/2006 TC Chen     Fix ATM QoS fail on interface 0(fast path).
   1.00.06  02/10/2006 TC Chen     Change ifxmips_ppe_set_cell_rate to 
					ifx_atm_set_cell_rate
   				   Add ATM Led callback function
   1.00.07  13/11/2006 TC Chen	   Invert ADSL Link LED Signal
   1.00.08  08/12/2006 TC Chen	   Fix loop diagnostic warning message issue
   1.00.09  20/12/2006 TC Chen	   Workaround for USB OC interrupt which is trigegred once DSL reset
******************************************************************************/

/*
 * ===========================================================================
 *                           INCLUDE FILES
 * ===========================================================================
 */

#include <asm/ifxmips/ifxmips_mei_linux.h>

char IFXMIPS_MEI_VERSION[] = "1.00.09";

#define IFXMIPS_MEI_CMV_EXTRA	//WINHOST debug
#define IFX_ADSL_L3_MODE_SUPPORT	//L3 Low Power Mode Support
#define IFX_ADSL_DUAL_LATENCY_SUPPORT
#undef IFXMIPS_CLEAR_EOC		//clear eoc support

// for ARC memory access
#define WHILE_DELAY 20000
#if defined(IFXMIPS_PORT_RTEMS)
#undef IFXMIPS_DMA_DEBUG_MUTEX
#else
#define IFXMIPS_DMA_DEBUG_MUTEX
#endif

#define IMAGE_SWAP
#define BOOT_SWAP
#define HEADER_SWAP

//TODO
#undef DFE_LOOPBACK		// testing code //undefined by Henry , start to real link test.
		    //165203:henryhsu 

#ifdef DFE_LOOPBACK
//#define DFE_MEM_TEST
//#define DFE_PING_TEST
#define DFE_ATM_LOOPBACK
#endif

#undef DATA_LED_ON_MODE
#define DATA_LED_SUPPORT	// support adsl data led
//#define DATA_LED_ADSL_FW_HANDLE // adsl data led handle by firmware
#define CONFIG_IFXMIPS_MEI_LED	// adsl led support

//  Block size per BAR
#define SDRAM_SEGMENT_SIZE	(64*1024)
// Number of Bar registers
#define MAX_BAR_REGISTERS	(17)

#define XDATA_REGISTER		(15)

#define IFXMIPS_MEI_DEVNAME "mei"

#ifdef DFE_LOOPBACK
#ifndef UINT32
#define UINT32 unsigned long
#endif
#ifdef DFE_PING_TEST
#include "dsp_xmem_arb_rand_em.h"
#endif
#ifdef DFE_MEM_TEST
#include "aai_mem_test.h"
#endif
#ifdef DFE_ATM_LOOPBACK
#include "aai_lpbk_dyn_rate.h"
#endif
#endif

/************************************************************************
 *  Function declaration
 ************************************************************************/
static MEI_ERROR meiDMAWrite (u32 destaddr, u32 * databuff, u32 databuffsize);
static MEI_ERROR meiDMARead (u32 srcaddr, u32 * databuff, u32 databuffsize);
static void meiControlModeSwitch (int mode);
static void meiPollForDbgDone (void);
static MEI_ERROR _meiDebugLongWordRead (u32 DEC_mode, u32 address,
					u32 * data);
static MEI_ERROR _meiDebugLongWordWrite (u32 DEC_mode, u32 address, u32 data);
MEI_ERROR meiDebugWrite (u32 destaddr, u32 * databuff, u32 databuffsize);
static MEI_ERROR meiDebugRead (u32 srcaddr, u32 * databuff, u32 databuffsize);
static MEI_ERROR meiMailboxWrite (u16 * msgsrcbuffer, u16 msgsize);
static MEI_ERROR meiDownloadBootCode (void);
static MEI_ERROR meiHaltArc (void);
static MEI_ERROR meiRunArc (void);
static MEI_ERROR meiRunAdslModem (void);
static int meiGetPage (u32 Page, u32 data, u32 MaxSize, u32 * Buffer,
		       u32 * Dest);
void makeCMV (u8 opcode, u8 group, u16 address, u16 index, int size,
	      u16 * data, u16 * CMVMSG);
MEI_ERROR meiCMV (u16 * request, int reply, u16 * response);
static void meiMailboxInterruptsDisable (void);
static void meiMailboxInterruptsEnable (void);
static int update_bar_register (int nTotalBar);
static int free_image_buffer (int type);
static int alloc_processor_memory (unsigned long size,
				   smmu_mem_info_t * adsl_mem_info);
ssize_t mei_write (MEI_file_t * filp, char *buf, size_t size, loff_t * loff);
int mei_ioctl (MEI_inode_t * ino, MEI_file_t * fil, unsigned int command,
	       unsigned long lon);

#ifdef CONFIG_PROC_FS
static int proc_read (struct file *file, char *buf, size_t nbytes,
		      loff_t * ppos);
static ssize_t proc_write (struct file *file, const char *buffer,
			   size_t count, loff_t * ppos);
#endif

#ifdef CONFIG_IFXMIPS_MEI_MIB
int mei_mib_ioctl (MEI_inode_t * ino, MEI_file_t * fil, unsigned int command,
		   unsigned long lon);
int mei_mib_adsl_link_up (void);
int mei_mib_adsl_link_down (void);
int ifxmips_mei_mib_init (void);
int ifxmips_mei_mib_cleanup (void);
#endif
#if defined(CONFIG_IFXMIPS_MEI_LED) && defined(DATA_LED_SUPPORT)
static int ifxmips_mei_led_init (void);
static int ifxmips_mei_led_cleanup (void);
static int adsl_led_flash_task (void);
#endif
// for clearEoC 
#ifdef IFXMIPS_CLEAR_EOC
extern void ifx_push_eoc (struct sk_buff *pkt);
#endif

/************************************************************************
 *  variable declaration
 ************************************************************************/
static smmu_mem_info_t adsl_mem_info[MAX_BAR_REGISTERS];
static unsigned long image_size = 0;
static struct timeval time_disconnect, time_showtime;
static u16 unavailable_seconds = 0;
#ifdef IFXMIPS_CLEAR_EOC
static wait_queue_head_t wait_queue_hdlc_poll;	///clear eoc
#endif

static int showtime_lock_flag = 0;
static int quiet_mode_flag = 0;

int showtime = 0;
static int major = IFXMIPS_MEI_MAJOR;
MEI_mutex_t mei_sema;

// Mei to ARC CMV count, reply count, ARC Indicator count
static int indicator_count = 0;
static int cmv_count = 0;
static int reply_count = 0;
static u16 Recent_indicator[MSG_LENGTH];
static int reset_arc_flag = 0;

// Used in interrupt handler as flags
static int arcmsgav = 0;
static int cmv_reply = 0;
static int cmv_waiting = 0;
static int modem_ready = 0;
//  to wait for arc cmv reply, sleep on wait_queue_arcmsgav;
static wait_queue_head_t wait_queue_arcmsgav;

// CMV mailbox messages
// ARC to MEI message
static u16 CMV_RxMsg[MSG_LENGTH] __attribute__ ((aligned (4)));
// MEI to ARC message
static u16 CMV_TxMsg[MSG_LENGTH] __attribute__ ((aligned (4)));

static u32 *mei_arc_swap_buff = NULL;	//  holding swap pages
static ARC_IMG_HDR *img_hdr;
static int arc_halt_flag = 0;
static int nBar = 0;		// total bars to be used.

static u32 loop_diagnostics_mode = 0;
wait_queue_head_t wait_queue_loop_diagnostic;
int loop_diagnostics_completed = 0;
u32 adsl_mode, adsl_mode_extend;	// adsl mode : adsl/ 2/ 2+
static int autoboot_enable_flag = 0;

#ifdef IFX_ADSL_DUAL_LATENCY_SUPPORT
static u8 bDualLatency = 0;
#endif

#ifdef IFXMIPS_CLEAR_EOC
static u16 ceoc_read_idx = 0;
#endif

#ifdef IFX_ADSL_L3_MODE_SUPPORT
static wait_queue_head_t wait_queue_l3;	// l3 power mode 
static int l3_shutdown = 0;
int get_l3_power_status (void);
#endif

#if defined(CONFIG_IFXMIPS_MEI_LED) && defined(DATA_LED_SUPPORT)
int led_status_on = 0, led_need_to_flash = 0;
static int stop_led_module = 0;	//wakeup and clean led module
static wait_queue_head_t wait_queue_led_polling;	// adsl led
#endif

static struct file_operations mei_operations = {
      write : mei_write,
      ioctl : mei_ioctl,
};

#ifdef CONFIG_PROC_FS
static struct proc_dir_entry *meidir;
static struct file_operations proc_operations = {
      read:proc_read,
      write:proc_write,
};
static reg_entry_t regs[PROC_ITEMS];	//total items to be monitored by /proc/mei
#define NUM_OF_REG_ENTRY	(sizeof(regs)/sizeof(reg_entry_t))
#endif //#ifdef CONFIG_PROC_FS

#ifdef DFE_LOOPBACK
unsigned char got_int = 0;
#endif

/////////////////               mei access Rd/Wr methods       ///////////////
/**
 * Write a value to register 
 * This function writes a value to ifxmips register
 * 
 * \param  	ul_address	The address to write
 * \param  	ul_data		The value to write
 * \ingroup	Internal
 */
static void
meiLongwordWrite (u32* ul_address, u32 ul_data)
{
	ifxmips_w32(ul_data, ul_address);
	wmb();
	return;
}				//    end of "meiLongwordWrite(..."

/**
 * Read the ifxmips register 
 * This function read the value from ifxmips register
 * 
 * \param  	ul_address	The address to write
 * \param  	pul_data	Pointer to the data
 * \ingroup	Internal
 */
static void
meiLongwordRead (u32* ul_address, u32 * pul_data)
{
	//*pul_data = *((volatile u32 *)ul_address);
	*pul_data = ifxmips_r32(ul_address);
	wmb();
	return;
}				//    end of "meiLongwordRead(..."

/**
 * Write several DWORD datas to ARC memory via ARC DMA interface
 * This function writes several DWORD datas to ARC memory via DMA interface.
 * 
 * \param  	destaddr	The address to write
 * \param  	databuff	Pointer to the data buffer
 * \param  	databuffsize	Number of DWORDs to write
 * \return	MEI_SUCCESS or MEI_FAILURE
 * \ingroup	Internal
 */
static MEI_ERROR
meiDMAWrite (u32 destaddr, u32 * databuff, u32 databuffsize)
{
	u32 *p = databuff;
	u32 temp;
	MEI_intstat_t flags;

	if (destaddr & 3)
		return MEI_FAILURE;

#ifdef IFXMIPS_DMA_DEBUG_MUTEX
	MEI_LOCKINT (flags);
#endif

	//printk("destaddr=%X,size=%d\n",destaddr,databuffsize);
	//      Set the write transfer address
	meiLongwordWrite (MEI_XFR_ADDR, destaddr);

	//      Write the data pushed across DMA
	while (databuffsize--) {
		temp = *p;
		if (databuff == (u32 *) CMV_TxMsg)
			MEI_HALF_WORD_SWAP (temp);
		meiLongwordWrite (MEI_DATA_XFR, temp);
		p++;
	}			//    end of "while(..."

#ifdef IFXMIPS_DMA_DEBUG_MUTEX
	MEI_UNLOCKINT (flags);
#endif

	return MEI_SUCCESS;

}				//    end of "meiDMAWrite(..."

/**
 * Read several DWORD datas from ARC memory via ARC DMA interface
 * This function reads several DWORD datas from ARC memory via DMA interface.
 * 
 * \param  	srcaddr		The address to read
 * \param  	databuff	Pointer to the data buffer
 * \param  	databuffsize	Number of DWORDs to read
 * \return	MEI_SUCCESS or MEI_FAILURE
 * \ingroup	Internal
 */
static MEI_ERROR
meiDMARead (u32 srcaddr, u32 * databuff, u32 databuffsize)
{
	u32 *p = databuff;
	u32 temp;
#ifdef IFXMIPS_DMA_DEBUG_MUTEX
	MEI_intstat_t flags;
#endif
	//printk("destaddr=%X,size=%X\n",srcaddr,databuffsize);
	if (srcaddr & 3)
		return MEI_FAILURE;

#ifdef IFXMIPS_DMA_DEBUG_MUTEX
	MEI_LOCKINT (flags);
#endif

	//      Set the read transfer address
	meiLongwordWrite (MEI_XFR_ADDR, srcaddr);

	//      Read the data popped across DMA
	while (databuffsize--) {
		meiLongwordRead (MEI_DATA_XFR, &temp);
		if (databuff == (u32 *) CMV_RxMsg)	// swap half word
			MEI_HALF_WORD_SWAP (temp);
		*p = temp;
		p++;
	}			//    end of "while(..."

#ifdef IFXMIPS_DMA_DEBUG_MUTEX
	MEI_UNLOCKINT (flags);
#endif

	return MEI_SUCCESS;

}				//    end of "meiDMARead(..."

/**
 * Switch the ARC control mode
 * This function switchs the ARC control mode to JTAG mode or MEI mode
 * 
 * \param  	mode		The mode want to switch: JTAG_MASTER_MODE or MEI_MASTER_MODE.
 * \ingroup	Internal
 */
static void
meiControlModeSwitch (int mode)
{
	u32 temp = 0x0;
	meiLongwordRead ( MEI_DBG_MASTER, &temp);
	switch (mode) {
	case JTAG_MASTER_MODE:
		temp &= ~(HOST_MSTR);
		break;
	case MEI_MASTER_MODE:
		temp |= (HOST_MSTR);
		break;
	default:
		printk ("meiControlModeSwitch: unkonwn mode [%d]\n",
				 mode);
		return;
	}
	meiLongwordWrite (MEI_DBG_MASTER, temp);
}

/**
 * Poll for transaction complete signal
 * This function polls and waits for transaction complete signal.
 * 
 * \ingroup	Internal
 */
static void
meiPollForDbgDone (void)
{
	u32 query = 0;
	int i = 0;
	while (i < WHILE_DELAY) {
		meiLongwordRead (ARC_TO_MEI_INT, &query);
		query &= (ARC_TO_MEI_DBG_DONE);
		if (query)
			break;
		i++;
		if (i == WHILE_DELAY) {
			printk ("\n\n PollforDbg fail");
		}
	}
	meiLongwordWrite ( ARC_TO_MEI_INT, ARC_TO_MEI_DBG_DONE);	// to clear this interrupt
}				//    end of "meiPollForDbgDone(..."

/**
 * ARC Debug Memory Access for a single DWORD reading.
 * This function used for direct, address-based access to ARC memory.
 * 
 * \param  	DEC_mode	ARC memory space to used
 * \param  	address	  	Address to read
 * \param  	data	  	Pointer to data
 * \return	MEI_SUCCESS or MEI_FAILURE
 * \ingroup	Internal
 */
static MEI_ERROR
_meiDebugLongWordRead (u32 DEC_mode, u32 address, u32 * data)
{
	meiLongwordWrite ( MEI_DEBUG_DEC, DEC_mode);
	meiLongwordWrite ( MEI_DEBUG_RAD, address);
	meiPollForDbgDone ();
	meiLongwordRead (MEI_DEBUG_DATA, data);
	return MEI_SUCCESS;
}

/**
 * ARC Debug Memory Access for a single DWORD writing.
 * This function used for direct, address-based access to ARC memory.
 * 
 * \param  	DEC_mode	ARC memory space to used
 * \param  	address	  	The address to write
 * \param  	data	  	The data to write
 * \return	MEI_SUCCESS or MEI_FAILURE
 * \ingroup	Internal
 */
static MEI_ERROR
_meiDebugLongWordWrite (u32 DEC_mode, u32 address, u32 data)
{
	meiLongwordWrite (MEI_DEBUG_DEC, DEC_mode);
	meiLongwordWrite (MEI_DEBUG_WAD, address);
	meiLongwordWrite (MEI_DEBUG_DATA, data);
	meiPollForDbgDone ();
	return MEI_SUCCESS;
}

/**
 * ARC Debug Memory Access for writing.
 * This function used for direct, address-based access to ARC memory.
 * 
 * \param  	destaddr	The address to ead
 * \param  	databuffer  	Pointer to data
 * \param	databuffsize	The number of DWORDs to read
 * \return	MEI_SUCCESS or MEI_FAILURE
 * \ingroup	Internal
 */

MEI_ERROR
meiDebugWrite (u32 destaddr, u32 * databuff, u32 databuffsize)
{
	u32 i;
	u32 temp = 0x0;
	u32 address = 0x0;
	u32 *buffer = 0x0;
#ifdef IFXMIPS_DMA_DEBUG_MUTEX
	MEI_intstat_t flags;
#endif

#ifdef IFXMIPS_DMA_DEBUG_MUTEX
	MEI_LOCKINT (flags);
#endif

	//      Open the debug port before DMP memory write
	meiControlModeSwitch (MEI_MASTER_MODE);

	meiLongwordWrite (MEI_DEBUG_DEC, MEI_DEBUG_DEC_DMP1_MASK);

	//      For the requested length, write the address and write the data
	address = destaddr;
	buffer = databuff;
	for (i = 0; i < databuffsize; i++) {
		temp = *buffer;
		_meiDebugLongWordWrite (MEI_DEBUG_DEC_DMP1_MASK, address,
					temp);
		address += 4;
		buffer++;
	}			//    end of "for(..."

	//      Close the debug port after DMP memory write
	meiControlModeSwitch (JTAG_MASTER_MODE);

#ifdef IFXMIPS_DMA_DEBUG_MUTEX
	MEI_UNLOCKINT (flags);
#endif

	//      Return
	return MEI_SUCCESS;

}				//    end of "meiDebugWrite(..."

/**
 * ARC Debug Memory Access for reading.
 * This function used for direct, address-based access to ARC memory.
 * 
 * \param  	srcaddr	  	The address to read
 * \param  	databuffer  	Pointer to data
 * \param	databuffsize	The number of DWORDs to read
 * \return	MEI_SUCCESS or MEI_FAILURE
 * \ingroup	Internal
 */
static MEI_ERROR
meiDebugRead (u32 srcaddr, u32 * databuff, u32 databuffsize)
{
	u32 i;
	u32 temp = 0x0;
	u32 address = 0x0;
	u32 *buffer = 0x0;
#ifdef IFXMIPS_DMA_DEBUG_MUTEX
	MEI_intstat_t flags;
#endif

#ifdef IFXMIPS_DMA_DEBUG_MUTEX
	MEI_LOCKINT (flags);
#endif

	//      Open the debug port before DMP memory read
	meiControlModeSwitch (MEI_MASTER_MODE);

	meiLongwordWrite (MEI_DEBUG_DEC, MEI_DEBUG_DEC_DMP1_MASK);

	//      For the requested length, write the address and read the data
	address = srcaddr;
	buffer = databuff;
	for (i = 0; i < databuffsize; i++) {
		_meiDebugLongWordRead (MEI_DEBUG_DEC_DMP1_MASK, address,
				       &temp);
		*buffer = temp;
		address += 4;
		buffer++;
	}			//    end of "for(..."

	//      Close the debug port after DMP memory read
	meiControlModeSwitch (JTAG_MASTER_MODE);

#ifdef IFXMIPS_DMA_DEBUG_MUTEX
	MEI_UNLOCKINT (flags);
#endif

	//      Return
	return MEI_SUCCESS;

}				//    end of "meiDebugRead(..."

/**
 * Send a message to ARC MailBox.
 * This function sends a message to ARC Mailbox via ARC DMA interface.
 * 
 * \param  	msgsrcbuffer  	Pointer to message.
 * \param	msgsize		The number of words to write.
 * \return	MEI_SUCCESS or MEI_FAILURE
 * \ingroup	Internal
 */
static MEI_ERROR
meiMailboxWrite (u16 * msgsrcbuffer, u16 msgsize)
{
	int i;
	u32 arc_mailbox_status = 0x0;
	u32 temp = 0;
	MEI_ERROR meiMailboxError = MEI_SUCCESS;

	//      Write to mailbox
	meiMailboxError =
		meiDMAWrite (MEI_TO_ARC_MAILBOX, (u32 *) msgsrcbuffer,
			     msgsize / 2);
	meiMailboxError =
		meiDMAWrite (MEI_TO_ARC_MAILBOXR, (u32 *) (&temp), 1);

	//      Notify arc that mailbox write completed
	cmv_waiting = 1;
	meiLongwordWrite (MEI_TO_ARC_INT, MEI_TO_ARC_MSGAV);

	i = 0;
	while (i < WHILE_DELAY) {	// wait for ARC to clear the bit
		meiLongwordRead ( MEI_TO_ARC_INT, &arc_mailbox_status);
		if ((arc_mailbox_status & MEI_TO_ARC_MSGAV) !=
		    MEI_TO_ARC_MSGAV)
			break;
		i++;
		if (i == WHILE_DELAY) {
			printk
				("\n\n MEI_TO_ARC_MSGAV not cleared by ARC");
			meiMailboxError = MEI_FAILURE;
		}
	}

	//      Return
	return meiMailboxError;

}				//    end of "meiMailboxWrite(..."

/**
 * Read a message from ARC MailBox.
 * This function reads a message from ARC Mailbox via ARC DMA interface.
 * 
 * \param  	msgsrcbuffer  	Pointer to message.
 * \param	msgsize		The number of words to read
 * \return	MEI_SUCCESS or MEI_FAILURE
 * \ingroup	Internal
 */
static MEI_ERROR
meiMailboxRead (u16 * msgdestbuffer, u16 msgsize)
{
	MEI_ERROR meiMailboxError = MEI_SUCCESS;
	//      Read from mailbox
	meiMailboxError =
		meiDMARead (ARC_TO_MEI_MAILBOX, (u32 *) msgdestbuffer,
			    msgsize / 2);

	//      Notify arc that mailbox read completed
	meiLongwordWrite (ARC_TO_MEI_INT, ARC_TO_MEI_MSGAV);

	//      Return
	return meiMailboxError;

}				//    end of "meiMailboxRead(..."

/**
 * Download boot pages to ARC.
 * This function downloads boot pages to ARC.
 * 
 * \return	MEI_SUCCESS or MEI_FAILURE
 * \ingroup	Internal
 */
static MEI_ERROR
meiDownloadBootPages (void)
{
	int boot_loop;
	int page_size;
	u32 dest_addr;

	/*
	 **     DMA the boot code page(s)
	 */
#ifndef HEADER_SWAP
	for (boot_loop = 1; boot_loop < le32_to_cpu (img_hdr->count);
	     boot_loop++)
#else
	for (boot_loop = 1; boot_loop < (img_hdr->count); boot_loop++)
#endif
	{
#ifndef HEADER_SWAP
		if (le32_to_cpu (img_hdr->page[boot_loop].p_size) & BOOT_FLAG)
#else
		if ((img_hdr->page[boot_loop].p_size) & BOOT_FLAG)
#endif
		{
			page_size =
				meiGetPage (boot_loop, GET_PROG, MAXSWAPSIZE,
					    mei_arc_swap_buff, &dest_addr);
			if (page_size > 0) {
				meiDMAWrite (dest_addr, mei_arc_swap_buff,
					     page_size);
			}
		}
#ifndef HEADER_SWAP
		if (le32_to_cpu (img_hdr->page[boot_loop].d_size) & BOOT_FLAG)
#else
		if ((img_hdr->page[boot_loop].d_size) & BOOT_FLAG)
#endif
		{
			page_size =
				meiGetPage (boot_loop, GET_DATA, MAXSWAPSIZE,
					    mei_arc_swap_buff, &dest_addr);
			if (page_size > 0) {
				meiDMAWrite (dest_addr, mei_arc_swap_buff,
					     page_size);
			}
		}
	}
	return MEI_SUCCESS;
}

/**
 * Initial efuse rar.
 **/
static void
mei_fuse_rar_init (void)
{
	u32 data = 0;
	meiDMAWrite (IRAM0_BASE, &data, 1);
	meiDMAWrite (IRAM0_BASE + 4, &data, 1);
	meiDMAWrite (IRAM1_BASE, &data, 1);
	meiDMAWrite (IRAM1_BASE + 4, &data, 1);
	meiDMAWrite (BRAM_BASE, &data, 1);
	meiDMAWrite (BRAM_BASE + 4, &data, 1);
	meiDMAWrite (ADSL_DILV_BASE, &data, 1);
	meiDMAWrite (ADSL_DILV_BASE + 4, &data, 1);
}

/**
 * efuse rar program
 **/
static void
mei_fuse_prg (void)
{
	u32 reg_data, fuse_value;
	int i = 0;
	meiLongwordRead ( IFXMIPS_RCU_RST, &reg_data);
	while ((reg_data & 0x10000000) == 0) {
		meiLongwordRead ( IFXMIPS_RCU_RST, &reg_data);
		//add a watchdog
		i++;
		/* 0x4000 translate to  about 16 ms@111M, so should be enough */
		if (i == 0x4000)
			return;
	}
	// STEP a: Prepare memory for external accesses
	// Write fuse_en bit24
	meiLongwordRead (IFXMIPS_RCU_RST, &reg_data);
	meiLongwordWrite (IFXMIPS_RCU_RST, reg_data | (1 << 24));

	mei_fuse_rar_init ();
	for (i = 0; i < 4; i++) {
		meiLongwordRead((u32*)(IFXMIPS_FUSE_BASE_ADDR + (i * 4)), &fuse_value);
		switch (fuse_value & 0xF0000) {
		case 0x80000:
			reg_data =
				((fuse_value & RX_DILV_ADDR_BIT_MASK) |
				 (RX_DILV_ADDR_BIT_MASK + 0x1));
			meiDMAWrite (ADSL_DILV_BASE, &reg_data, 1);
			break;
		case 0x90000:
			reg_data =
				((fuse_value & RX_DILV_ADDR_BIT_MASK) |
				 (RX_DILV_ADDR_BIT_MASK + 0x1));
			meiDMAWrite (ADSL_DILV_BASE + 4, &reg_data, 1);
			break;
		case 0xA0000:
			reg_data =
				((fuse_value & IRAM0_ADDR_BIT_MASK) |
				 (IRAM0_ADDR_BIT_MASK + 0x1));
			meiDMAWrite (IRAM0_BASE, &reg_data, 1);
			break;
		case 0xB0000:
			reg_data =
				((fuse_value & IRAM0_ADDR_BIT_MASK) |
				 (IRAM0_ADDR_BIT_MASK + 0x1));
			meiDMAWrite (IRAM0_BASE + 4, &reg_data, 1);
			break;
		case 0xC0000:
			reg_data =
				((fuse_value & IRAM1_ADDR_BIT_MASK) |
				 (IRAM1_ADDR_BIT_MASK + 0x1));
			meiDMAWrite (IRAM1_BASE, &reg_data, 1);
			break;
		case 0xD0000:
			reg_data =
				((fuse_value & IRAM1_ADDR_BIT_MASK) |
				 (IRAM1_ADDR_BIT_MASK + 0x1));
			meiDMAWrite (IRAM1_BASE + 4, &reg_data, 1);
			break;
		case 0xE0000:
			reg_data =
				((fuse_value & BRAM_ADDR_BIT_MASK) |
				 (BRAM_ADDR_BIT_MASK + 0x1));
			meiDMAWrite (BRAM_BASE, &reg_data, 1);
			break;
		case 0xF0000:
			reg_data =
				((fuse_value & BRAM_ADDR_BIT_MASK) |
				 (BRAM_ADDR_BIT_MASK + 0x1));
			meiDMAWrite (BRAM_BASE + 4, &reg_data, 1);
			break;
		default:	// PPE efuse
			break;
		}
	}
	meiLongwordRead (IFXMIPS_RCU_RST, &reg_data);
	meiLongwordWrite (IFXMIPS_RCU_RST, reg_data & 0xF7FFFFFF);
}

/**
 * Download boot code to ARC.
 * This function downloads boot code to ARC.
 * 
 * \return	MEI_SUCCESS or MEI_FAILURE
 * \ingroup	Internal
 */
static MEI_ERROR
meiDownloadBootCode (void)
{
	u32 arc_debug_data = ACL_CLK_MODE_ENABLE;	//0x10

	meiMailboxInterruptsDisable ();

	//      Switch arc control from JTAG mode to MEI mode
	meiControlModeSwitch (MEI_MASTER_MODE);
	//enable ac_clk signal  
	_meiDebugLongWordRead (MEI_DEBUG_DEC_DMP1_MASK, CRI_CCR0,
			       &arc_debug_data);
	arc_debug_data |= ACL_CLK_MODE_ENABLE;
	_meiDebugLongWordWrite (MEI_DEBUG_DEC_DMP1_MASK, CRI_CCR0,
				arc_debug_data);
	//Switch arc control from MEI mode to JTAG mode
	meiControlModeSwitch (JTAG_MASTER_MODE);

	mei_fuse_prg ();	//program fuse rar

	meiDownloadBootPages ();

	return MEI_SUCCESS;

}				//    end of "meiDownloadBootCode(..."

//#endif

/**
 * Halt the ARC.
 * This function halts the ARC.
 * 
 * \return	MEI_SUCCESS or MEI_FAILURE
 * \ingroup	Internal
 */
static MEI_ERROR
meiHaltArc (void)
{
	u32 arc_debug_data = 0x0;

	//      Switch arc control from JTAG mode to MEI mode
	meiControlModeSwitch (MEI_MASTER_MODE);
	_meiDebugLongWordRead (MEI_DEBUG_DEC_AUX_MASK, ARC_DEBUG,
			       &arc_debug_data);
	arc_debug_data |= (BIT1);
	_meiDebugLongWordWrite (MEI_DEBUG_DEC_AUX_MASK, ARC_DEBUG,
				arc_debug_data);
	//      Switch arc control from MEI mode to JTAG mode
	meiControlModeSwitch (JTAG_MASTER_MODE);
	arc_halt_flag = 1;

	MEI_WAIT (10);
	//      Return
	return MEI_SUCCESS;

}				//    end of "meiHalt(..."

/**
 * Run the ARC.
 * This function runs the ARC.
 * 
 * \return	MEI_SUCCESS or MEI_FAILURE
 * \ingroup	Internal
 */
static MEI_ERROR
meiRunArc (void)
{
	u32 arc_debug_data = 0x0;

	//      Switch arc control from JTAG mode to MEI mode- write '1' to bit0
	meiControlModeSwitch (MEI_MASTER_MODE);
	_meiDebugLongWordRead (MEI_DEBUG_DEC_AUX_MASK, AUX_STATUS,
			       &arc_debug_data);

	//      Write debug data reg with content ANDd with 0xFDFFFFFF (halt bit cleared)
	arc_debug_data &= ~(BIT25);
	_meiDebugLongWordWrite (MEI_DEBUG_DEC_AUX_MASK, AUX_STATUS,
				arc_debug_data);

	//      Switch arc control from MEI mode to JTAG mode- write '0' to bit0
	meiControlModeSwitch (JTAG_MASTER_MODE);
	//      Enable mask for arc codeswap interrupts
	meiMailboxInterruptsEnable ();
	arc_halt_flag = 0;

	//      Return
	return MEI_SUCCESS;

}				//    end of "meiActivate(..."

/**
 * Reset the ARC.
 * This function resets the ARC.
 * 
 * \return	MEI_SUCCESS or MEI_FAILURE
 * \ingroup	Internal
 */
static MEI_ERROR
meiResetARC (void)
{

	u32 arc_debug_data = 0;
	showtime = 0;

	meiHaltArc ();

	meiLongwordRead (IFXMIPS_RCU_RST, &arc_debug_data);
	meiLongwordWrite (IFXMIPS_RCU_RST,
			  arc_debug_data | IFXMIPS_RCU_RST_REQ_DFE |
			  IFXMIPS_RCU_RST_REQ_AFE);
	meiLongwordWrite (IFXMIPS_RCU_RST, arc_debug_data);
	// reset ARC
	meiLongwordWrite(MEI_RST_CONTROL, MEI_SOFT_RESET);
	meiLongwordWrite(MEI_RST_CONTROL, 0);

	meiMailboxInterruptsDisable ();
	MEI_MUTEX_INIT (mei_sema, 1);
	reset_arc_flag = 1;
	modem_ready = 0;
	return MEI_SUCCESS;
}

/**
 * Reset the ARC, download boot codes, and run the ARC.
 * This function resets the ARC, downloads boot codes to ARC, and runs the ARC.
 * 
 * \return	MEI_SUCCESS or MEI_FAILURE
 * \ingroup	Internal
 */
static MEI_ERROR
meiRunAdslModem (void)
{
	int nSize = 0, idx = 0;

	img_hdr = (ARC_IMG_HDR *) adsl_mem_info[0].address;
#if	defined(HEADER_SWAP)
	if ((img_hdr->count) * sizeof (ARC_SWP_PAGE_HDR) > SDRAM_SEGMENT_SIZE)
#else //define(HEADER_SWAP)
	if (le32_to_cpu (img_hdr->count) * sizeof (ARC_SWP_PAGE_HDR) >
	    SDRAM_SEGMENT_SIZE)
#endif //define(HEADER_SWAP)
	{
		printk
			("segment_size is smaller than firmware header size\n");
		return -1;
	}
	// check image size 
	for (idx = 0; idx < MAX_BAR_REGISTERS; idx++) {
		nSize += adsl_mem_info[idx].nCopy;
	}
	if (nSize != image_size) {
		printk
			("Firmware download is not completed. \nPlease download firmware again!\n");
		return -1;
	}
	// TODO: check crc
	///
	if (reset_arc_flag == 0) {
		u32 arc_debug_data;

		meiResetARC ();
		meiControlModeSwitch (MEI_MASTER_MODE);
		//enable ac_clk signal  
		_meiDebugLongWordRead (MEI_DEBUG_DEC_DMP1_MASK, CRI_CCR0,
				       &arc_debug_data);
		arc_debug_data |= ACL_CLK_MODE_ENABLE;
		_meiDebugLongWordWrite (MEI_DEBUG_DEC_DMP1_MASK, CRI_CCR0,
					arc_debug_data);
		meiControlModeSwitch (JTAG_MASTER_MODE);
		meiHaltArc ();
		update_bar_register (nBar);
	}
	reset_arc_flag = 0;
	if (arc_halt_flag == 0) {
		meiHaltArc ();
	}
	printk ("Starting to meiDownloadBootCode\n");

	meiDownloadBootCode();
 
	// 1.00.09  20/12/2006 TC Chen
	// disable USB OC interrupt, reset DSL chip will triger OC interrupt
	disable_irq(IFXMIPS_USB_OC_INT);

	meiRunArc ();

	MEI_WAIT (100);		//wait 100ms 

	//1.00.09  20/12/2006 TC Chen
	// restore USB OC interrupt
	MEI_MASK_AND_ACK_IRQ(IFXMIPS_USB_OC_INT);
	enable_irq(IFXMIPS_USB_OC_INT);

	if (modem_ready != 1) {
		printk ("Running ADSL modem firmware fail!\n");
		return MEI_FAILURE;
	}


	return MEI_SUCCESS;
}

/**
 * Get the page's data pointer
 * This function caculats the data address from the firmware header.
 * 
 * \param	Page		The page number.
 * \param	data		Data page or program page.
 * \param	MaxSize		The maximum size to read.
 * \param	Buffer		Pointer to data.
 * \param	Dest		Pointer to the destination address.
 * \return	The number of bytes to read.
 * \ingroup	Internal
 */
static int
meiGetPage (u32 Page, u32 data, u32 MaxSize, u32 * Buffer, u32 * Dest)
{
	u32 size;
	u32 i;
	u32 *p;
	u32 idx, offset, nBar = 0;

	if (Page > img_hdr->count)
		return -2;
	/*
	 **     Get program or data size, depending on "data" flag
	 */
#ifndef HEADER_SWAP
	size = (data ==
		GET_DATA) ? le32_to_cpu (img_hdr->page[Page].
					 d_size) : le32_to_cpu (img_hdr->
								page[Page].
								p_size);
#else
	size = (data ==
		GET_DATA) ? (img_hdr->page[Page].d_size) : (img_hdr->
							    page[Page].
							    p_size);
#endif
	size &= BOOT_FLAG_MASK;	//      Clear boot bit!
	if (size > MaxSize)
		return -1;

	if (size == 0)
		return 0;
	/*
	 **     Get program or data offset, depending on "data" flag
	 */
#ifndef HEADER_SWAP
	i = data ? le32_to_cpu (img_hdr->page[Page].
				d_offset) : le32_to_cpu (img_hdr->page[Page].
							 p_offset);
#else
	i = data ? (img_hdr->page[Page].d_offset) : (img_hdr->page[Page].
						     p_offset);
#endif

	/*
	 **     Copy data/program to buffer
	 */

	idx = i / SDRAM_SEGMENT_SIZE;
	offset = i % SDRAM_SEGMENT_SIZE;
	p = (u32 *) ((u8 *) adsl_mem_info[idx].address + offset);

	for (i = 0; i < size; i++) {
		if (offset + i * 4 - (nBar * SDRAM_SEGMENT_SIZE) >=
		    SDRAM_SEGMENT_SIZE) {
			idx++;
			nBar++;
			p = (u32 *) ((u8 *)
				     KSEG1ADDR ((u32) adsl_mem_info[idx].
						address));
		}
		Buffer[i] = *p++;
#ifdef BOOT_SWAP
#ifndef IMAGE_SWAP
		Buffer[i] = le32_to_cpu (Buffer[i]);
#endif
#endif
	}

	/*
	 **     Pass back data/program destination address
	 */
#ifndef HEADER_SWAP
	*Dest = data ? le32_to_cpu (img_hdr->page[Page].
				    d_dest) : le32_to_cpu (img_hdr->
							   page[Page].p_dest);
#else
	*Dest = data ? (img_hdr->page[Page].d_dest) : (img_hdr->page[Page].
						       p_dest);
#endif

	return size;
}

////////////////makeCMV(Opcode, Group, Address, Index, Size, Data), CMV in u16 TxMessage[MSG_LENGTH]///////////////////////////

/**
 * Compose a message.
 * This function compose a message from opcode, group, address, index, size, and data
 * 
 * \param	opcode		The message opcode
 * \param	group		The message group number
 * \param	address		The message address.
 * \param	index		The message index.
 * \param	size		The number of words to read/write.
 * \param	data		The pointer to data.
 * \param	CMVMSG		The pointer to message buffer.
 * \ingroup	Internal
 */
void
makeCMV (u8 opcode, u8 group, u16 address, u16 index, int size, u16 * data,
	 u16 * CMVMSG)
{
	memset (CMVMSG, 0, MSG_LENGTH * 2);
	CMVMSG[0] = (opcode << 4) + (size & 0xf);
	CMVMSG[1] = (((index == 0) ? 0 : 1) << 7) + (group & 0x7f);
	CMVMSG[2] = address;
	CMVMSG[3] = index;
	if (opcode == H2D_CMV_WRITE)
		memcpy (CMVMSG + 4, data, size * 2);
	return;
}

/**
 * Send a message to ARC and read the response
 * This function sends a message to arc, waits the response, and reads the responses.
 * 
 * \param	request		Pointer to the request
 * \param	reply		Wait reply or not.
 * \param	response	Pointer to the response
 * \return	MEI_SUCCESS or MEI_FAILURE
 * \ingroup	Internal
 */
MEI_ERROR
meiCMV (u16 * request, int reply, u16 * response)	// write cmv to arc, if reply needed, wait for reply
{
	MEI_ERROR meierror;
#if defined(IFXMIPS_PORT_RTEMS)
	int delay_counter = 0;
#endif

	cmv_reply = reply;
	memcpy (CMV_TxMsg, request, MSG_LENGTH * 2);
	arcmsgav = 0;

	meierror = meiMailboxWrite (CMV_TxMsg, MSG_LENGTH);

	if (meierror != MEI_SUCCESS) {
		cmv_waiting = 0;
		arcmsgav = 0;
		printk ("\n\n MailboxWrite Fail.");
		return meierror;
	}
	else {
		cmv_count++;
	}

	if (cmv_reply == NO_REPLY)
		return MEI_SUCCESS;

#if !defined(IFXMIPS_PORT_RTEMS)
	if (arcmsgav == 0)
		MEI_WAIT_EVENT_TIMEOUT (wait_queue_arcmsgav, CMV_TIMEOUT);
#else
	while (arcmsgav == 0 && delay_counter < CMV_TIMEOUT / 5) {
		MEI_WAIT (5);
		delay_counter++;
	}
#endif

	cmv_waiting = 0;
	if (arcmsgav == 0) {	//CMV_timeout
		arcmsgav = 0;
		printk ("\nmeiCMV: MEI_MAILBOX_TIMEOUT\n");
		return MEI_MAILBOX_TIMEOUT;
	}
	else {
		arcmsgav = 0;
		reply_count++;
		memcpy (response, CMV_RxMsg, MSG_LENGTH * 2);
		return MEI_SUCCESS;
	}
	return MEI_SUCCESS;
}

/////////////////////          Interrupt handler     /////////////////////////
/**
 * Disable ARC to MEI interrupt
 * 
 * \ingroup	Internal
 */
static void
meiMailboxInterruptsDisable (void)
{
	meiLongwordWrite (ARC_TO_MEI_INT_MASK, 0x0);
}				//    end of "meiMailboxInterruptsDisable(..."

/**
 * Eable ARC to MEI interrupt
 * 
 * \ingroup	Internal
 */
static void
meiMailboxInterruptsEnable (void)
{
	meiLongwordWrite (ARC_TO_MEI_INT_MASK, MSGAV_EN);
}				//    end of "meiMailboxInterruptsEnable(..."

/**
 * MEI interrupt handler
 * 
 * \param int1	
 * \param void0
 * \param regs	Pointer to the structure of ifxmips mips registers
 * \ingroup	Internal
 */
irqreturn_t
mei_interrupt_arcmsgav (int int1, void *void0)
{
	u32 scratch;

#if defined(DFE_LOOPBACK) && defined(DFE_PING_TEST)
	dfe_loopback_irq_handler ();
	goto out;
#endif //DFE_LOOPBACK

	meiDebugRead (ARC_MEI_MAILBOXR, &scratch, 1);
	if (scratch & OMB_CODESWAP_MESSAGE_MSG_TYPE_MASK) {
		printk("\n\n Receive Code Swap Request interrupt!!!");
		goto out;
	}
	else if (scratch & OMB_CLEAREOC_INTERRUPT_CODE)	// clear eoc message interrupt
	{
		meiLongwordWrite (ARC_TO_MEI_INT, ARC_TO_MEI_MSGAV);
#if defined (IFXMIPS_CLEAR_EOC)
		MEI_WAKEUP_EVENT (wait_queue_hdlc_poll);
#endif
		MEI_MASK_AND_ACK_IRQ (IFXMIPS_MEI_INT);
		goto out;
	}
	else {			// normal message
		meiMailboxRead (CMV_RxMsg, MSG_LENGTH);
#if 0
		{
			int msg_idx = 0;
			printk ("got interrupt\n");
			for (msg_idx = 0; msg_idx < MSG_LENGTH; msg_idx++) {
				printk ("%04X ", CMV_RxMsg[msg_idx]);
				if (msg_idx % 8 == 7)
					printk ("\n");
			}
			printk ("\n");
		}
#endif
		if (cmv_waiting == 1) {
			arcmsgav = 1;
			cmv_waiting = 0;
#if !defined(IFXMIPS_PORT_RTEMS)
			MEI_WAKEUP_EVENT (wait_queue_arcmsgav);
#endif
		}
		else {
			indicator_count++;
			memcpy ((char *) Recent_indicator, (char *) CMV_RxMsg,
				MSG_LENGTH * 2);
			if (((CMV_RxMsg[0] & 0xff0) >> 4) == D2H_AUTONOMOUS_MODEM_READY_MSG)	// arc ready
			{	//check ARC ready message
				printk ("Got MODEM_READY_MSG\n");
				modem_ready = 1;
				MEI_MUTEX_UNLOCK (mei_sema);	// allow cmv access
			}
		}
	}

	MEI_MASK_AND_ACK_IRQ (IFXMIPS_MEI_INT);
out:
	return IRQ_HANDLED;;
}

////////////////////////hdlc ////////////////

/**
 * Get the hdlc status
 * 
 * \return	HDLC status
 * \ingroup	Internal
 */
static unsigned int
ifx_me_hdlc_status (void)
{
	u16 CMVMSG[MSG_LENGTH];
	int ret;

	if (showtime != 1)
		return -ENETRESET;

	makeCMV (H2D_CMV_READ, STAT, 14, 0, 1, NULL, CMVMSG);	//Get HDLC status 
	ret = mei_ioctl ((struct inode *) 0, NULL, IFXMIPS_MEI_CMV_WINHOST,
			 (unsigned long) CMVMSG);
	if (ret != 0) {
		return -EIO;
	}
	return CMVMSG[4] & 0x0F;
}

/**
 * Check if the me is reslved.
 * 
 * \param	status		the me status
 * \return	ME_HDLC_UNRESOLVED or ME_HDLC_RESOLVED
 * \ingroup	Internal
 */
int
ifx_me_is_resloved (int status)
{
	u16 CMVMSG[MSG_LENGTH];
	int ret;
	if (adsl_mode <= 8 && adsl_mode_extend == 0)	// adsl mode
	{
		makeCMV (H2D_CMV_READ, CNTL, 2, 0, 1, NULL, CMVMSG);	//Get ME-HDLC Control
		ret = mei_ioctl ((struct inode *) 0, NULL,
				 IFXMIPS_MEI_CMV_WINHOST,
				 (unsigned long) CMVMSG);
		if (ret != 0) {
			return ME_HDLC_UNRESOLVED;
		}
		if (CMVMSG[4] & (1 << 0)) {
			return ME_HDLC_UNRESOLVED;
		}
	}
	else {
		if (status == ME_HDLC_MSG_QUEUED
		    || status == ME_HDLC_MSG_SENT)
			return ME_HDLC_UNRESOLVED;
		if (status == ME_HDLC_IDLE) {
			makeCMV (H2D_CMV_READ, CNTL, 2, 0, 1, NULL, CMVMSG);	//Get ME-HDLC Control
			ret = mei_ioctl ((struct inode *) 0, NULL,
					 IFXMIPS_MEI_CMV_WINHOST,
					 (unsigned long) CMVMSG);
			if (ret != 0) {
				return IFX_POP_EOC_FAIL;
			}
			if (CMVMSG[4] & (1 << 0)) {
				return ME_HDLC_UNRESOLVED;
			}
		}
	}
	return ME_HDLC_RESOLVED;
}

int
_ifx_me_hdlc_send (unsigned char *hdlc_pkt, int pkt_len, int max_length)
{
	int ret;
	u16 CMVMSG[MSG_LENGTH];
	u16 data = 0;
	u16 len = 0;
	int rx_length = 0;
	int write_size = 0;

	if (pkt_len > max_length) {
		makeCMV (H2D_CMV_READ, INFO, 85, 2, 1, NULL, CMVMSG);	//Get ME-HDLC Control
		ret = mei_ioctl ((struct inode *) 0, NULL,
				 IFXMIPS_MEI_CMV_WINHOST,
				 (unsigned long) CMVMSG);
		if (ret != 0) {
			return -EIO;
		}
		rx_length = CMVMSG[4];
		if (rx_length + max_length < pkt_len) {
			printk ("Exceed maximum eoc rx(%d)+tx(%d) message length\n", rx_length, max_length);
			return -EMSGSIZE;
		}
		data = 1;
		makeCMV (H2D_CMV_WRITE, INFO, 85, 6, 1, &data, CMVMSG);	//disable RX Eoc
		ret = mei_ioctl ((struct inode *) 0, NULL,
				 IFXMIPS_MEI_CMV_WINHOST,
				 (unsigned long) CMVMSG);
		if (ret != 0) {
			return -EIO;
		}
	}
	while (len < pkt_len) {
		write_size = pkt_len - len;
		if (write_size > 24)
			write_size = 24;
		//printk("len=%d,write_size=%d,pkt_len=%d\n",len,write_size,pkt_len);
		memset (CMVMSG, 0, sizeof (CMVMSG));
		makeCMV (H2D_CMV_WRITE, INFO, 81, len / 2, (write_size + 1) / 2, (u16 *) (hdlc_pkt + len), CMVMSG);	//Write clear eoc message to ARC
		ret = mei_ioctl ((struct inode *) 0, NULL,
				 IFXMIPS_MEI_CMV_WINHOST,
				 (unsigned long) CMVMSG);
		if (ret != 0) {
			return -EIO;
		}
		len += write_size;
	}
	makeCMV (H2D_CMV_WRITE, INFO, 83, 2, 1, &len, CMVMSG);	//Update tx message length
	ret = mei_ioctl ((struct inode *) 0, NULL, IFXMIPS_MEI_CMV_WINHOST,
			 (unsigned long) CMVMSG);
	if (ret != 0) {
		return -EIO;
	}

	data = (1 << 0);
	makeCMV (H2D_CMV_WRITE, CNTL, 2, 0, 1, &data, CMVMSG);	//Start to send
	ret = mei_ioctl ((struct inode *) 0, NULL, IFXMIPS_MEI_CMV_WINHOST,
			 (unsigned long) CMVMSG);
	if (ret != 0) {
		return -EIO;
	}
	return 0;
}

/**
 * Send hdlc packets
 * 
 * \param	hdlc_pkt	Pointer to hdlc packet
 * \param	hdlc_pkt_len	The number of bytes to send
 * \return	success or failure.
 * \ingroup	Internal
 */
int
ifx_me_hdlc_send (unsigned char *hdlc_pkt, int hdlc_pkt_len)
{
	int hdlc_status = 0;
	u16 CMVMSG[MSG_LENGTH];
	int max_hdlc_tx_length = 0, ret = 0, retry = 0;
	int power_mode = 0;
	int send_busy_counter = 0;
	int send_retry = 0;

      HDLC_SEND:
	// retry 1000 times (10 seconds)
	while (retry < 1000) {
		/* In L2 power mode, do not read the OHC related parameters, 
		   instead give the indication to the calling IOCTL, 
		   that the readout fails (just return -EBUSY).  */
		power_mode = get_l3_power_status();
		if (power_mode == L2_POWER_MODE) {
			return -EBUSY;
		}

		hdlc_status = ifx_me_hdlc_status ();
		if (ifx_me_is_resloved (hdlc_status) == ME_HDLC_RESOLVED)	// arc ready to send HDLC message
		{
			makeCMV (H2D_CMV_READ, INFO, 83, 0, 1, NULL, CMVMSG);	//Get Maximum Allowed HDLC Tx Message Length
			ret = mei_ioctl ((struct inode *) 0, NULL,
					 IFXMIPS_MEI_CMV_WINHOST,
					 (unsigned long) CMVMSG);
			if (ret != 0) {
				printk
					("ifx_me_hdlc_send failed. Return -EIO");
				return -EIO;
			}
			max_hdlc_tx_length = CMVMSG[4];
			ret = _ifx_me_hdlc_send (hdlc_pkt, hdlc_pkt_len,
						 max_hdlc_tx_length);
			return ret;
		}
		else {
			if (hdlc_status == ME_HDLC_MSG_SENT)
				send_busy_counter++;
		}
		retry++;
		MEI_WAIT (1);
	}
	// wait 10 seconds and FW still report busy -> reset FW HDLC status
	if (send_busy_counter > 950 && send_retry == 0) {
		u16 data = 0;
		send_retry = 1;
		retry = 0;
		printk ("Reset FW HDLC status!!\n");
		send_busy_counter = 0;
		data = (1 << 1);
		makeCMV (H2D_CMV_WRITE, CNTL, 2, 0, 1, NULL, CMVMSG);	//Force reset to idle
		ret = mei_ioctl ((struct inode *) 0, NULL,
				 IFXMIPS_MEI_CMV_WINHOST,
				 (unsigned long) CMVMSG);
		if (ret != 0) {
			return -EIO;
		}
		goto HDLC_SEND;
	}
	printk ("ifx_me_hdlc_send failed. Return -EBUSY");
	return -EBUSY;
}

/**
 * Read the hdlc packets
 * 
 * \param	hdlc_pkt	Pointer to hdlc packet
 * \param	hdlc_pkt_len	The maximum number of bytes to read
 * \return	The number of bytes which reads.
 * \ingroup	Internal
 */
int
ifx_mei_hdlc_read (char *hdlc_pkt, int max_hdlc_pkt_len)
{
	u16 CMVMSG[MSG_LENGTH];
	int msg_read_len, ret = 0, pkt_len = 0, retry = 0;

	while (retry < 10) {
		ret = ifx_me_hdlc_status ();
		if (ret == ME_HDLC_RESP_RCVD) {
			int current_size = 0;
			makeCMV (H2D_CMV_READ, INFO, 83, 3, 1, NULL, CMVMSG);	//Get EoC packet length
			ret = mei_ioctl ((MEI_inode_t *) 0, NULL,
					 IFXMIPS_MEI_CMV_WINHOST,
					 (unsigned long) CMVMSG);
			if (ret != 0) {
				return -EIO;
			}

			pkt_len = CMVMSG[4];
			if (pkt_len > max_hdlc_pkt_len) {
				ret = -ENOMEM;
				goto error;
			}
			while (current_size < pkt_len) {
				if (pkt_len - current_size >
				    (MSG_LENGTH * 2 - 8))
					msg_read_len = (MSG_LENGTH * 2 - 8);
				else
					msg_read_len =
						pkt_len - (current_size);
				makeCMV (H2D_CMV_READ, INFO, 82, 0 + (current_size / 2), (msg_read_len + 1) / 2, NULL, CMVMSG);	//Get hdlc packet
				ret = mei_ioctl ((MEI_inode_t *) 0, NULL,
						 IFXMIPS_MEI_CMV_WINHOST,
						 (unsigned long) CMVMSG);
				if (ret != 0) {
					goto error;
				}
				memcpy (hdlc_pkt + current_size, &CMVMSG[4],
					msg_read_len);
				current_size += msg_read_len;
			}
			ret = current_size;
			break;
		}
		else {
			ret = -ENODATA;
		}

		retry++;

		MEI_WAIT (10);
	}
      error:
	return ret;
}

#if defined(IFXMIPS_CLEAR_EOC)
int
ifx_me_ceoc_send (struct sk_buff *eoc_pkt)
{
	int ret, pkt_len = 0;
	unsigned char *pkt_data_ptr;
	int offset = 0;
	int swap_idx = 0;

	if (adsl_mode <= 8 && adsl_mode_extend == 0)	// adsl mode
	{
		pkt_len = eoc_pkt->len;

		pkt_data_ptr = kmalloc (pkt_len + 3, GFP_KERNEL);

		offset = 2;
		pkt_data_ptr[0] = 0x4c;
		pkt_data_ptr[1] = 0x81;
		pkt_len += 2;
	} else {
		pkt_len = eoc_pkt->len + 4;
		pkt_data_ptr = kmalloc (pkt_len + 1 + 2, GFP_KERNEL);
		memset (pkt_data_ptr, 0, pkt_len + 1 + 2);
		//fill clear eoc header
		pkt_data_ptr[0] = 0x1;
		pkt_data_ptr[1] = 0x8;
		pkt_data_ptr[2] = 0x4c;
		pkt_data_ptr[3] = 0x81;
		offset = 4;
	}
	for (swap_idx = 0; swap_idx < (eoc_pkt->len / 2) * 2; swap_idx += 2)
	{
		//printk("%02X %02X ",eoc_pkt->data[swap_idx],eoc_pkt->data[swap_idx+1]);
		pkt_data_ptr[swap_idx + offset] = eoc_pkt->data[swap_idx + 1];
		pkt_data_ptr[swap_idx + 1 + offset] = eoc_pkt->data[swap_idx];
	}
	if (eoc_pkt->len % 2)
	{
		//printk("%02X ",eoc_pkt->data[eoc_pkt->len-1]);
		pkt_data_ptr[eoc_pkt->len - 1 + offset] =
			eoc_pkt->data[eoc_pkt->len - 1];
		pkt_data_ptr[eoc_pkt->len + offset] =
			eoc_pkt->data[eoc_pkt->len - 1];
	}
	ret = ifx_me_hdlc_send (pkt_data_ptr, pkt_len);

	if (pkt_data_ptr != eoc_pkt->data)
	{
		kfree (pkt_data_ptr);
	}
	dev_kfree_skb (eoc_pkt);
	return ret;
}

int
get_me_ceoc_data (int pkt_len, int rx_buffer_addr, int rx_buffer_len,
		  u8 * data_ptr1)
{
	int ret;
	MEI_ERROR dma_ret;
	u16 CMVMSG[MSG_LENGTH];
	int read_size, aread_size;
	int offset = 0;
	u8 *data = NULL, *data_ptr = NULL;
	int i, j;
	int over_read = 0;

	i = j = 0;

	read_size = (pkt_len / 4) + 4;
	offset = ceoc_read_idx % 4;
	over_read = read_size * 4 - pkt_len - offset;

	ceoc_read_idx = (ceoc_read_idx & 0xFFFFFFFC);

	data = kmalloc (read_size * 4, GFP_KERNEL);
	if (data == NULL)
		goto error;
	data_ptr = kmalloc (read_size * 4, GFP_KERNEL);
	if (data_ptr == NULL)
		goto error;
	if (ceoc_read_idx + read_size * 4 >= rx_buffer_len) {
		aread_size = (rx_buffer_len - ceoc_read_idx) / 4;
	}
	else {
		aread_size = read_size;
	}

	//printk("aread_size = %d,ceoc_read_idx=%d,read_size=%d,offset=%d\n",aread_size,ceoc_read_idx,read_size,offset);
	dma_ret =
		meiDebugRead (rx_buffer_addr + ceoc_read_idx, (u32 *) (data),
			      aread_size);
	ceoc_read_idx += aread_size * 4;
	if (aread_size != read_size) {
		dma_ret =
			meiDebugRead (rx_buffer_addr,
				      (u32 *) (data) + aread_size,
				      read_size - aread_size);
		ceoc_read_idx = (read_size - aread_size) * 4;
	}
	if (ceoc_read_idx < over_read)
		ceoc_read_idx = rx_buffer_len + ceoc_read_idx - over_read;
	else
		ceoc_read_idx -= over_read;

	if (offset == 0 || offset == 2) {
		for (i = 0; i < read_size; i++) {
			// 3412 --> 1234

			for (j = 0; j < 4; j++) {
				if (i * 4 + j - offset >= 0)
					data_ptr[i * 4 + j - offset] =
						data[i * 4 + (3 - j)];
			}
		}

	}
	else if (offset == 1) {
		for (i = 0; i < pkt_len; i = i + 4) {

			data_ptr[i + 1] = data[i + 1];
			data_ptr[i] = data[i + 2];
			data_ptr[i + 3] = data[i + 7];
			data_ptr[i + 2] = data[i];
		}
	}
	else if (offset == 3) {
		for (i = 0; i < pkt_len; i = i + 4) {
			data_ptr[i + 1] = data[i + 7];
			data_ptr[i + 0] = data[i];
			data_ptr[i + 3] = data[i + 5];
			data_ptr[i + 2] = data[i + 6];
		}
	}
	if (pkt_len % 2 == 1)
		data_ptr[pkt_len - 1] = data_ptr[pkt_len];

	kfree (data);
	memcpy (data_ptr1, data_ptr, pkt_len);
	kfree (data_ptr);

	makeCMV (H2D_CMV_WRITE, INFO, 85, 3, 1, &ceoc_read_idx, CMVMSG);
	ret = mei_ioctl ((struct inode *) 0, NULL, IFXMIPS_MEI_CMV_WINHOST,
			 (unsigned long) CMVMSG);
	if (ret != 0) {
		goto error;
	}

	return dma_ret;
      error:
	kfree (data);
	kfree (data_ptr);
	return -1;
}

int
ifx_me_ceoc_receive (int ceoc_write_idx, int rx_buffer_len,
		     struct sk_buff **eoc_pkt)
{
	u16 CMVMSG[MSG_LENGTH];
	int pkt_len, ret;
	u16 lsw_addr, msw_addr;
	u32 rx_buffer_addr = 0;
	MEI_ERROR dma_ret;

	//printk("rx_buffer_len=%d,ceoc_read_idx=%d,ceoc_write_idx=%d\n",rx_buffer_len,ceoc_read_idx,ceoc_write_idx);
	if (ceoc_write_idx > ceoc_read_idx) {
		pkt_len = ceoc_write_idx - ceoc_read_idx;
	}
	else {
		pkt_len = rx_buffer_len - ceoc_read_idx + ceoc_write_idx;
	}
	*eoc_pkt = dev_alloc_skb (pkt_len);
	if (*eoc_pkt == NULL) {
		printk ("Out of memory!\n");
		ret = -ENOMEM;
		goto error;
	}

	makeCMV (H2D_CMV_READ, INFO, 85, 0, 1, NULL, CMVMSG);	//Get HDLC packet 
	ret = mei_ioctl ((struct inode *) 0, NULL, IFXMIPS_MEI_CMV_WINHOST,
			 (unsigned long) CMVMSG);
	if (ret != 0) {
		goto error;
	}
	lsw_addr = CMVMSG[4];

	makeCMV (H2D_CMV_READ, INFO, 85, 1, 1, NULL, CMVMSG);	//Get HDLC packet 
	ret = mei_ioctl ((struct inode *) 0, NULL, IFXMIPS_MEI_CMV_WINHOST,
			 (unsigned long) CMVMSG);
	if (ret != 0) {
		goto error;
	}
	msw_addr = CMVMSG[4];
	rx_buffer_addr = msw_addr << 16 | lsw_addr;
	dma_ret =
		get_me_ceoc_data (pkt_len, rx_buffer_addr, rx_buffer_len,
				  (u16 *) skb_put (*eoc_pkt, pkt_len));
	if (dma_ret != MEI_SUCCESS) {
		ret = -EIO;
		goto error;
	}

	return 0;
      error:
	if (*eoc_pkt != NULL)
		dev_kfree_skb (*eoc_pkt);
	return ret;
}

int
ifx_mei_ceoc_rx (void)
{
	u16 CMVMSG[MSG_LENGTH];
	int rx_buffer_len, ret, pkt_len = 0;
	struct sk_buff *eoc_pkt;
	u16 ceoc_write_idx = 0;

	makeCMV (H2D_CMV_READ, INFO, 85, 2, 1, NULL, CMVMSG);	//Get EoC packet length
	ret = mei_ioctl ((struct inode *) 0, NULL, IFXMIPS_MEI_CMV_WINHOST,
			 (unsigned long) CMVMSG);
	if (ret != 0) {
		printk ("ioctl fail!!\n");
	}
	rx_buffer_len = CMVMSG[4];

	makeCMV (H2D_CMV_READ, INFO, 85, 4, 1, NULL, CMVMSG);	//Get write index
	ret = mei_ioctl ((struct inode *) 0, NULL, IFXMIPS_MEI_CMV_WINHOST,
			 (unsigned long) CMVMSG);
	if (ret != 0) {
		return -EIO;
	}

	ceoc_write_idx = CMVMSG[4];
	ret = ifx_me_ceoc_receive (ceoc_write_idx, rx_buffer_len, &eoc_pkt);
#if defined (CONFIG_ATM_IFXMIPS)
	if (ret == 0) {
		skb_pull (eoc_pkt, 2);	// skip 4c 81 header
		ifx_push_ceoc (eoc_pkt);	//pass data to higher layer
	}

	return ret;
#endif
}

static int
adsl_clear_eoc_poll (void *unused)
{
	struct task_struct *tsk = current;

	daemonize("mei_eoc_poll");
	strcpy(tsk->comm, "mei_ceoc_poll");
	sigfillset(&tsk->blocked);

	while (1)
	{
		MEI_WAIT_EVENT (wait_queue_hdlc_poll);
		if (showtime)
			ifx_mei_ceoc_rx();
		}
	return 0;
}
#endif //#if defined(IFXMIPS_CLEAR_EOC)

#ifdef IFXMIPS_CLEAR_EOC
static int
ifxmips_mei_ceoc_init (void)
{
	kernel_thread (adsl_clear_eoc_poll, NULL,
		       CLONE_FS | CLONE_FILES | CLONE_SIGNAL);
	return 0;
}
#endif

//////////////////////  Driver Structure        ///////////////////////

/**
 * Free the memory for ARC firmware
 * 
 * \param	type	Free all memory or free the unused memory after showtime
 * \ingroup	Internal
 */
static int
free_image_buffer (int type)
{
	int idx = 0;
	for (idx = 0; idx < MAX_BAR_REGISTERS; idx++) {
		printk ("meminfo[%d].type=%d,size=%ld,addr=%X\n", idx,
				 adsl_mem_info[idx].type, adsl_mem_info[idx].size,
				 (unsigned int)adsl_mem_info[idx].address);
		if (type == FREE_ALL || adsl_mem_info[idx].type == type) {
			if (adsl_mem_info[idx].size > 0) {
				kfree (adsl_mem_info[idx].org_address);
				adsl_mem_info[idx].address = 0;
				adsl_mem_info[idx].size = 0;
				adsl_mem_info[idx].type = 0;
				adsl_mem_info[idx].nCopy = 0;
			}
		}
	}
	return 0;
}

/**
 * Allocate memory for ARC firmware
 * 
 * \param	size		The number of bytes to allocate.
 * \param	adsl_mem_info	Pointer to firmware information.
 * \ingroup	Internal
 */
static int
alloc_processor_memory (unsigned long size, smmu_mem_info_t * adsl_mem_info)
{
	char *mem_ptr = NULL;
	char *org_mem_ptr = NULL;
	int idx = 0;
	long total_size = 0;
	long img_size = size;
	int err = 0;

	// Alloc Swap Pages
	while (img_size > 0 && idx < MAX_BAR_REGISTERS) {
		// skip bar15 for XDATA usage.
#ifndef DFE_LOOPBACK
		if (idx == XDATA_REGISTER)
			idx++;
#endif
		if (idx == MAX_BAR_REGISTERS - 1)
		{
			//allocate 1MB memory for bar16
			org_mem_ptr = kmalloc (1024 * 1024, GFP_ATOMIC);
			mem_ptr = (char*)((unsigned long) (org_mem_ptr +  1023) & 0xFFFFFC00);
			adsl_mem_info[idx].size = 1024 * 1024;
		} else {
			org_mem_ptr = kmalloc (SDRAM_SEGMENT_SIZE, GFP_ATOMIC);
			mem_ptr = (char*)((unsigned long) (org_mem_ptr + 1023) & 0xFFFFFC00);
			adsl_mem_info[idx].size = SDRAM_SEGMENT_SIZE;
		}
		if (org_mem_ptr == NULL)
		{
			printk ("kmalloc memory fail!\n");
			err = -ENOMEM;
			goto allocate_error;
		}
		adsl_mem_info[idx].address = mem_ptr;
		adsl_mem_info[idx].org_address = org_mem_ptr;

		img_size -= SDRAM_SEGMENT_SIZE;
		total_size += SDRAM_SEGMENT_SIZE;
		printk("alloc memory idx=%d,img_size=%ld,addr=%X\n",
				idx, img_size, (unsigned int)adsl_mem_info[idx].address);
		idx++;
	}
	if (img_size > 0)
	{
		printk ("Image size is too large!\n");
		err = -EFBIG;
		goto allocate_error;
	}
	err = idx;
	return err;

      allocate_error:
	free_image_buffer (FREE_ALL);
	return err;
}

/**
 * Program the BAR registers
 * 
 * \param	nTotalBar	The number of bar to program.
 * \ingroup	Internal
 */
static int
update_bar_register (int nTotalBar)
{
	int idx = 0;

	for (idx = 0; idx < nTotalBar; idx++) {
		//skip XDATA register
		if (idx == XDATA_REGISTER)
			idx++;
		meiLongwordWrite ( MEI_XMEM_BAR_BASE + idx * 4,
				  (((uint32_t) adsl_mem_info[idx].
				    address) & 0x0FFFFFFF));
		printk ("BAR%d=%08X, addr=%08X\n", idx,
				 (((uint32_t) adsl_mem_info[idx].
				   address) & 0x0FFFFFFF),
				 (((uint32_t) adsl_mem_info[idx].address)));
	}
	for (idx = nTotalBar; idx < MAX_BAR_REGISTERS; idx++) {
		if (idx == XDATA_REGISTER)
			idx++;
		meiLongwordWrite ( MEI_XMEM_BAR_BASE + idx * 4,
				  (((uint32_t) adsl_mem_info[nTotalBar - 1].
				    address) & 0x0FFFFFFF));
	}

	meiLongwordWrite (MEI_XMEM_BAR_BASE + XDATA_REGISTER * 4,
			  (((uint32_t) adsl_mem_info[XDATA_REGISTER].
			    address) & 0x0FFFFFFF));
	// update MEI_XDATA_BASE_SH
	printk ("update bar15 register with %08lX\n",
			 ((unsigned long) adsl_mem_info[XDATA_REGISTER].
			  address) & 0x0FFFFFFF);
	meiLongwordWrite (MEI_XDATA_BASE_SH,
			  ((unsigned long) adsl_mem_info[XDATA_REGISTER].
			   address) & 0x0FFFFFFF);
	return MEI_SUCCESS;
}

/**
 * Copy the firmware to BARs memory.
 * 
 * \param	filp		Pointer to the file structure.
 * \param	buf		Pointer to the data.
 * \param	size		The number of bytes to copy.
 * \param	loff		The file offset.
 * \return	The current file position.
 * \ingroup	Internal
 */
ssize_t
mei_write (MEI_file_t * filp, char *buf, size_t size, loff_t * loff)
{
	ARC_IMG_HDR img_hdr_tmp, *img_hdr;

	size_t nRead = 0, nCopy = 0;
	char *mem_ptr;
	ssize_t retval = -ENOMEM;
	int idx = 0;

	if (*loff == 0) {
		if (size < sizeof (img_hdr)) {
			printk ("Firmware size is too small!\n");
			return retval;
		}
		copy_from_user ((char *) &img_hdr_tmp, buf,
				sizeof (img_hdr_tmp));
		image_size = le32_to_cpu (img_hdr_tmp.size) + 8;	// header of image_size and crc are not included.
		if (image_size > 1024 * 1024) {
			printk ("Firmware size is too large!\n");
			return retval;
		}
		// check if arc is halt
		if (arc_halt_flag != 1) {
			meiResetARC ();
			meiHaltArc ();
		}

		// reset part of PPE 
		*(unsigned long *) (IFXMIPS_PPE32_SRST) = 0xC30;
		*(unsigned long *) (IFXMIPS_PPE32_SRST) = 0xFFF;

		free_image_buffer (FREE_ALL);	//free all

		retval = alloc_processor_memory (image_size, adsl_mem_info);
		if (retval < 0) {
			printk ("Error: No memory space left.\n");
			goto error;
		}

		for (idx = 0; idx < retval; idx++) {
			//skip XDATA register
			if (idx == XDATA_REGISTER)
				idx++;
			if (idx * SDRAM_SEGMENT_SIZE <
			    le32_to_cpu (img_hdr_tmp.page[0].p_offset)) {
				adsl_mem_info[idx].type = FREE_RELOAD;
			}
			else {
				adsl_mem_info[idx].type = FREE_SHOWTIME;
			}

		}
		nBar = retval;

		img_hdr = (ARC_IMG_HDR *) adsl_mem_info[0].address;

#if !defined(__LINUX__)
		adsl_mem_info[XDATA_REGISTER].org_address =
			kmalloc (SDRAM_SEGMENT_SIZE + 1023, GFP_ATOMIC);
#else
		adsl_mem_info[XDATA_REGISTER].org_address =
			kmalloc (SDRAM_SEGMENT_SIZE, GFP_ATOMIC);
#endif
		adsl_mem_info[XDATA_REGISTER].address =
			(char
			 *) ((unsigned long) (adsl_mem_info[XDATA_REGISTER].
					      org_address +
					      1023) & 0xFFFFFC00);
		adsl_mem_info[XDATA_REGISTER].size = SDRAM_SEGMENT_SIZE;
		if (adsl_mem_info[XDATA_REGISTER].address == NULL) {
			printk ("kmalloc memory fail!\n");
			retval = -ENOMEM;
			goto error;
		}
		adsl_mem_info[XDATA_REGISTER].type = FREE_RELOAD;
		update_bar_register (nBar);

	}
	else if (image_size == 0) {
		printk ("Error: Firmware size=0! \n");
		goto error;
	}
	else {
		if (arc_halt_flag == 0) {
			printk
				("Please download the firmware from the beginning of the firmware!\n");
			goto error;
		}
	}

	nRead = 0;
	while (nRead < size) {
		long offset = ((long) (*loff) + nRead) % SDRAM_SEGMENT_SIZE;
		idx = (((long) (*loff)) + nRead) / SDRAM_SEGMENT_SIZE;
		mem_ptr = (char *)
			KSEG1ADDR ((unsigned long) (adsl_mem_info[idx].
						    address) + offset);
		if ((size - nRead + offset) > SDRAM_SEGMENT_SIZE)
			nCopy = SDRAM_SEGMENT_SIZE - offset;
		else
			nCopy = size - nRead;
		copy_from_user (mem_ptr, buf + nRead, nCopy);
#ifdef IMAGE_SWAP
		for (offset = 0; offset < (nCopy / 4); offset++) {
			((unsigned long *) mem_ptr)[offset] =
				le32_to_cpu (((unsigned long *)
					      mem_ptr)[offset]);
		}
#endif //IMAGE_SWAP
		nRead += nCopy;
		adsl_mem_info[idx].nCopy += nCopy;
	}

#if	( defined(HEADER_SWAP) && !defined(IMAGE_SWAP)) || (defined(IMAGE_SWAP) && !defined(HEADER_SWAP))
	if (*loff == 0) {

		for (idx = 0;
		     idx <
		     (sizeof (ARC_IMG_HDR) +
		      (le32_to_cpu (img_hdr_tmp.count) -
		       1) * sizeof (ARC_SWP_PAGE_HDR)) / 4; idx++) {
			((unsigned long *) img_hdr)[idx] =
				le32_to_cpu (((unsigned long *)
					      img_hdr)[idx]);
		}
	}
#endif //( defined(HEADER_SWAP) && !defined(IMAGE_SWAP)) || (defined(IMAGE_SWAP) && !defined(HEADER_SWAP))
	printk ("size=%X,loff=%08X\n", size, (unsigned int) *loff);

	*loff += size;
	return size;
      error:
	free_image_buffer (FREE_ALL);

	return retval;
}

/********************************************************
 * L3 Power Mode                                        *
 ********************************************************/
/**
 * Send a CMV message.
 * This function sends a CMV message to ARC
 * 
 * \param	opcode		The message opcode
 * \param	group		The message group number
 * \param	address		The message address.
 * \param	index		The message index.
 * \param	size		The number of words to read/write.
 * \param	data		The pointer to data.
 * \param	CMVMSG		The pointer to message buffer.
 * \return	0: success 
 * \ingroup	Internal
 */
int
send_cmv (u8 opcode, u8 group, u16 address, u16 index, int size, u16 * data, u16 * CMVMSG)
{
	int ret;

	makeCMV(opcode, group, address, index, size, data, CMVMSG);
	ret = mei_ioctl((struct inode *) 0, NULL, IFXMIPS_MEI_CMV_WINHOST, (unsigned long)CMVMSG);
	return ret;
}

#ifdef IFX_ADSL_L3_MODE_SUPPORT

/**
 * Check the L3 request from CO
 * This function Check if CPE received the L3 request from CO
 * \return	1: got L3 request.  
 * \ingroup	Internal
 */
int
check_co_l3_shutdown_request (void)
{
	u16 CMVMSG[MSG_LENGTH];
	if (modem_ready == 1) {
		if (send_cmv (H2D_CMV_READ, STAT, 4, 0, 1, NULL, CMVMSG) != 0) {
			return -EBUSY;
		}
		if (CMVMSG[4] & BIT14) {
			return 1;
		}
	}
	return 0;
}

/**
 * Check the L3 status
 * This function get the CPE Power Management Mode status
 * \return	0: L0 Mode
 *		2: L2 Mode
 *		3: L3 Mode
 * \ingroup	Internal
 */
int
get_l3_power_status (void)
{
	u16 CMVMSG[MSG_LENGTH];
	if (modem_ready == 0) {
		return L3_POWER_MODE;
	}
	else {
		if (send_cmv (H2D_CMV_READ, STAT, 18, 0, 1, NULL, CMVMSG) !=
		    0) {
			return -EBUSY;
		}
		return ((int) CMVMSG[4]);

	}
	return 0;
}

/**
 * Send a L3 request to CO
 * This function send a L3 request to CO and check the CO response.
 * \return	0: Success. Others: Fail.
 * \ingroup	Internal
 */
int
send_l3_shutdown_cmd (void)
{
	u16 cmd = 0x1;
	int nRetry = 0;
	u16 CMVMSG[MSG_LENGTH];

	if (modem_ready == 0) {
		return -EBUSY;
	}
	// send l3 request to CO
	if (send_cmv (H2D_CMV_WRITE, CNTL, 3, 0, 1, &cmd, CMVMSG) != 0) {
		return -EBUSY;
	}
      retry:
	MEI_WAIT (10);

	// check CO response
	if (send_cmv (H2D_CMV_READ, STAT, 20, 0, 1, NULL, CMVMSG) != 0) {
		return -EBUSY;
	}
	if (CMVMSG[4] == 0) {
		nRetry++;
		if (nRetry < 10) {
			goto retry;
		}
		else {
			return -EBUSY;
		}

	}
	else if (CMVMSG[4] == 1)	// reject
	{
		return -EPERM;
	}
	else if (CMVMSG[4] == 2)	// ok
	{
		return 0;
	}
	else if (CMVMSG[4] == 3)	// failure
	{
		return -EAGAIN;
	}
	return 0;
}

/**
 * Enable L3 Power Mode
 * This function send a L3 request to CO and check the CO response. Then reboot the CPE to enter L3 Mode.
 * \return	0: Success. Others: Fail.
 * \ingroup	Internal
 */
int
set_l3_shutdown (void)
{
	int ret = 0;
	if (l3_shutdown == 0) {
		// send l3 request to CO
		ret = send_l3_shutdown_cmd ();
		if (ret == 0)	//got CO ACK
		{
			//reboot adsl and block autoboot daemon
			ret = mei_ioctl ((struct inode *) 0, NULL, IFXMIPS_MEI_REBOOT, (unsigned long)NULL);
			l3_shutdown = 1;
		}
	}
	return ret;
}

/**
 * Disable L3 Power Mode
 * This function disable L3 Mode and wake up the autoboot daemon.
 * \return	0: Success.
 * \ingroup	Internal
 */
//l3 power mode disable
int
set_l3_power_on (void)
{
	if (l3_shutdown == 1) {
		l3_shutdown = 0;
		// wakeup autoboot daemon
		MEI_WAKEUP_EVENT (wait_queue_l3);

	}
	return 0;
}

/********************************************************
 * End of L3 Power Mode                                 *
 ********************************************************/
#endif //IFX_ADSL_L3_MODE_SUPPORT

#ifdef CONFIG_IFXMIPS_MEI_LED
/*
 *  LED Initialization function
 */
int
meiADSLLedInit (void)
{
	u16 data = 0x0600;
	u16 CMVMSG[MSG_LENGTH];

	data = 0x0400;
#if defined(DATA_LED_SUPPORT) && defined (DATA_LED_ADSL_FW_HANDLE)
	data |= 0x200;
#endif
	// Setup ADSL Link/Data LED
	if (send_cmv (H2D_CMV_WRITE, INFO, 91, 0, 1, &data, CMVMSG) != 0) {
		return -EBUSY;
	}

	if (send_cmv (H2D_CMV_WRITE, INFO, 91, 2, 1, &data, CMVMSG) != 0) {
		return -EBUSY;
	}

	// Let FW to handle ADSL Link LED
	data = 0x0a03;		//invert the LED signal as per input from Stefan on 13/11/2006
	if (send_cmv (H2D_CMV_WRITE, INFO, 91, 4, 1, &data, CMVMSG) != 0) {
		return -EBUSY;
	}

#ifdef DATA_LED_SUPPORT
#ifdef DATA_LED_ADSL_FW_HANDLE

	// Turn ADSL Data LED on
	data = 0x0900;
	if (send_cmv (H2D_CMV_WRITE, INFO, 91, 5, 1, &data, CMVMSG) != 0) {
		return -EBUSY;
	}
#else
	ifxmips_led_set(0x1);
#endif
#endif
	return 0;
}
#endif

#ifdef IFX_ADSL_DUAL_LATENCY_SUPPORT
/* 
 * Dual Latency Path Initialization function
 */
int
meiDualLatencyInit (void)
{
	u16 nDual = 0;
	u16 CMVMSG[MSG_LENGTH];

	// setup up stream path 
	if (bDualLatency & DUAL_LATENCY_US_ENABLE) {
		nDual = 2;
	}
	else {
		nDual = 1;
	}

	if (send_cmv (H2D_CMV_WRITE, CNFG, 10, 0, 1, &nDual, CMVMSG) != 0) {
		return -EBUSY;
	}

	if (send_cmv (H2D_CMV_WRITE, CNFG, 11, 0, 1, &nDual, CMVMSG) != 0) {
		return -EBUSY;
	}

	// setup down stream path       
	if (bDualLatency & DUAL_LATENCY_DS_ENABLE) {
		nDual = 2;
	}
	else {
		nDual = 1;
	}

	if (send_cmv (H2D_CMV_WRITE, CNFG, 21, 0, 1, &nDual, CMVMSG) != 0) {
		return -EBUSY;
	}
	if (send_cmv (H2D_CMV_WRITE, CNFG, 22, 0, 1, &nDual, CMVMSG) != 0) {
		return -EBUSY;
	}
	return 0;
}

int
mei_is_dual_latency_enabled (void)
{
	return bDualLatency;
}
#endif

int
meiAdslStartupInit (void)
{
#ifdef CONFIG_IFXMIPS_MEI_LED
	meiADSLLedInit ();
#endif
#ifdef IFX_ADSL_DUAL_LATENCY_SUPPORT
	meiDualLatencyInit ();
#endif
	return 0;
}

/**
 * MEI IO controls for user space accessing
 * 
 * \param	ino		Pointer to the stucture of inode.
 * \param	fil		Pointer to the stucture of file.
 * \param	command		The ioctl command.
 * \param	lon		The address of data.
 * \return	Success or failure.
 * \ingroup	Internal
 */
int
mei_ioctl (MEI_inode_t * ino, MEI_file_t * fil, unsigned int command,
	   unsigned long lon)
{
	int i;

	int meierr = MEI_SUCCESS;
	meireg regrdwr;
	meidebug debugrdwr;
	u32 arc_debug_data, reg_data;
#ifdef IFXMIPS_CLEAR_EOC
	u16 data;
	struct sk_buff *eoc_skb;
#endif //IFXMIPS_CLEAR_EOC
	u16 RxMessage[MSG_LENGTH] __attribute__ ((aligned (4)));
	u16 TxMessage[MSG_LENGTH] __attribute__ ((aligned (4)));

	int from_kernel = 0;	//joelin
	if (ino == (MEI_inode_t *) 0)
		from_kernel = 1;	//joelin
	if (command < IFXMIPS_MEI_START) {
#ifdef CONFIG_IFXMIPS_MEI_MIB
		return mei_mib_ioctl (ino, fil, command, lon);
#endif //CONFIG_IFXMIPS_MEI_MIB

		if (command == IFXMIPS_MIB_LO_ATUR
		    || command == IFXMIPS_MIB_LO_ATUC)
			return MEI_SUCCESS;
		printk
			("No such ioctl command (0x%X)! MEI ADSL MIB is not supported!\n",
			 command);
		return -ENOIOCTLCMD;
	}
	else {
		switch (command) {
		case IFXMIPS_MEI_START:

			showtime = 0;
			loop_diagnostics_completed = 0;
			if (time_disconnect.tv_sec == 0)
				do_gettimeofday (&time_disconnect);

			if (MEI_MUTEX_LOCK (mei_sema))	//disable CMV access until ARC ready
			{
				printk ("-ERESTARTSYS\n");
				return -ERESTARTSYS;
			}

			meiMailboxInterruptsDisable ();	//disable all MEI interrupts
			if (mei_arc_swap_buff == NULL) {
				mei_arc_swap_buff =
					(u32 *) kmalloc (MAXSWAPSIZE * 4,
							 GFP_KERNEL);
				if (mei_arc_swap_buff == NULL) {
					printk
						("\n\n malloc fail for codeswap buff");
					meierr = MEI_FAILURE;
				}
			}
			if (meiRunAdslModem () != MEI_SUCCESS) {
				printk
					("meiRunAdslModem()  error...");
				meierr = MEI_FAILURE;
			}
#ifdef IFX_ADSL_L3_MODE_SUPPORT
			/* L3 Power Mode Start */
			if (l3_shutdown == 1) {
				// block autoboot daemon until l3 power mode disable
				MEI_WAIT_EVENT (wait_queue_l3);
			}
			/* L3 Power Mode End */
#endif //IFX_ADSL_L3_MODE_SUPPORT
			if (autoboot_enable_flag)
				meiAdslStartupInit ();
			break;

		case IFXMIPS_MEI_SHOWTIME:
			if (MEI_MUTEX_LOCK (mei_sema))
				return -ERESTARTSYS;

			do_gettimeofday (&time_showtime);
			unavailable_seconds +=
				time_showtime.tv_sec - time_disconnect.tv_sec;
			time_disconnect.tv_sec = 0;
			makeCMV (H2D_CMV_READ, RATE, 0, 0, 4, NULL, TxMessage);	//maximum allowed tx message length, in bytes
			if (meiCMV (TxMessage, YES_REPLY, RxMessage) !=
			    MEI_SUCCESS) {
				printk
					("\n\nCMV fail, Group RAGE Address 0 Index 0");
			}
			else {
				u32 rate_fast;
				u32 rate_intl;
				rate_intl = RxMessage[4] | RxMessage[5] << 16;
				rate_fast = RxMessage[6] | RxMessage[7] << 16;
				// 609251:tc.chen Fix ATM QoS issue start
				if (rate_intl && rate_fast)	// apply cell rate to each path
				{
#ifdef CONFIG_ATM_IFXMIPS
					ifx_atm_set_cell_rate (1,
							       rate_intl /
							       (53 * 8));
					ifx_atm_set_cell_rate (0,
							       rate_fast /
							       (53 * 8));
#endif
				}
				else if (rate_fast)	// apply fast path cell rate to atm interface 0
				{
#ifdef CONFIG_ATM_IFXMIPS
					ifx_atm_set_cell_rate (0,
							       rate_fast /
							       (53 * 8));
#endif
				}
				else if (rate_intl)	// apply interleave path cell rate to atm interface 0
				{
#ifdef CONFIG_ATM_IFXMIPS
					ifx_atm_set_cell_rate (0,
							       rate_intl /
							       (53 * 8));
#endif
				}
				else {
					printk ("Got rate fail.\n");
				}
				// 609251:tc.chen end 
			}

#ifdef IFXMIPS_CLEAR_EOC
			data = 1;
			makeCMV (H2D_CMV_WRITE, OPTN, 24, 0, 1, &data,
				 TxMessage);
			if (meiCMV (TxMessage, YES_REPLY, RxMessage) !=
			    MEI_SUCCESS) {
				printk ("Enable clear eoc fail!\n");
			}
#endif
			// read adsl mode
			makeCMV (H2D_CMV_READ, STAT, 1, 0, 1, NULL,
				 TxMessage);
			if (meiCMV (TxMessage, YES_REPLY, RxMessage) !=
			    MEI_SUCCESS) {
#ifdef IFXMIPS_MEI_DEBUG_ON
				printk ("\n\nCMV fail, Group STAT Address 1 Index 0");
#endif
			}
			adsl_mode = RxMessage[4];
			makeCMV (H2D_CMV_READ, STAT, 17, 0, 1, NULL,
				 TxMessage);
			if (meiCMV (TxMessage, YES_REPLY, RxMessage) !=
			    MEI_SUCCESS) {
#ifdef IFXMIPS_MEI_DEBUG_ON
				printk ("\n\nCMV fail, Group STAT Address 1 Index 0");
#endif
			}
			adsl_mode_extend = RxMessage[4];
#ifdef CONFIG_IFXMIPS_MEI_MIB
			mei_mib_adsl_link_up ();
#endif

//joelin 04/16/2005-start
			makeCMV (H2D_CMV_WRITE, PLAM, 10, 0, 1,
				 &unavailable_seconds, TxMessage);
			if (meiCMV (TxMessage, YES_REPLY, RxMessage) !=
			    MEI_SUCCESS) {
				printk
					("\n\nCMV fail, Group 7 Address 10 Index 0");
			}

//joelin 04/16/2005-end         
			showtime = 1;
			free_image_buffer (FREE_SHOWTIME);
			MEI_MUTEX_UNLOCK (mei_sema);
			break;

		case IFXMIPS_MEI_HALT:
			if (arc_halt_flag == 0) {
				meiResetARC ();
				meiHaltArc ();
			}
			break;
		case IFXMIPS_MEI_RUN:
			if (arc_halt_flag == 1) {
				meiRunArc ();
			}
			break;
		case IFXMIPS_MEI_CMV_WINHOST:
			if (MEI_MUTEX_LOCK (mei_sema))
				return -ERESTARTSYS;

			if (!from_kernel)
				copy_from_user ((char *) TxMessage, (char *) lon, MSG_LENGTH * 2);	//joelin
			else
				memcpy (TxMessage, (char *) lon,
					MSG_LENGTH * 2);

			if (meiCMV (TxMessage, YES_REPLY, RxMessage) !=
			    MEI_SUCCESS) {
				printk
					("\n\nWINHOST CMV fail :TxMessage:%X %X %X %X, RxMessage:%X %X %X %X %X\n",
					 TxMessage[0], TxMessage[1],
					 TxMessage[2], TxMessage[3],
					 RxMessage[0], RxMessage[1],
					 RxMessage[2], RxMessage[3],
					 RxMessage[4]);
				meierr = MEI_FAILURE;
			}
			else {
				if (!from_kernel)	//joelin
					copy_to_user ((char *) lon,
						      (char *) RxMessage,
						      MSG_LENGTH * 2);
				else
					memcpy ((char *) lon,
						(char *) RxMessage,
						MSG_LENGTH * 2);
			}

			MEI_MUTEX_UNLOCK (mei_sema);
			break;
#ifdef IFXMIPS_MEI_CMV_EXTRA
		case IFXMIPS_MEI_CMV_READ:
			copy_from_user ((char *) (&regrdwr), (char *) lon,
					sizeof (meireg));
			meiLongwordRead ((u32*)regrdwr.iAddress, &(regrdwr.iData));

			copy_to_user((char *) lon, (char *) (&regrdwr), sizeof (meireg));
			break;

		case IFXMIPS_MEI_CMV_WRITE:
			copy_from_user ((char *) (&regrdwr), (char *) lon, sizeof (meireg));
			meiLongwordWrite ((u32*)regrdwr.iAddress, regrdwr.iData);
			break;

		case IFXMIPS_MEI_REMOTE:
			copy_from_user ((char *) (&i), (char *) lon,
					sizeof (int));
			if (i == 0) {
				meiMailboxInterruptsEnable ();

				MEI_MUTEX_UNLOCK (mei_sema);
			}
			else if (i == 1) {
				meiMailboxInterruptsDisable ();
				if (MEI_MUTEX_LOCK (mei_sema))
					return -ERESTARTSYS;
			}
			else {
				printk
					("\n\n IFXMIPS_MEI_REMOTE argument error");
				meierr = MEI_FAILURE;
			}
			break;

		case IFXMIPS_MEI_READDEBUG:
		case IFXMIPS_MEI_WRITEDEBUG:
#if 0				//tc.chen:It is no necessary to acquire lock to read debug memory!!
			if (MEI_MUTEX_LOCK (mei_sema))
				return -ERESTARTSYS;
#endif
			if (!from_kernel)
				copy_from_user ((char *) (&debugrdwr),
						(char *) lon,
						sizeof (debugrdwr));
			else
				memcpy ((char *) (&debugrdwr), (char *) lon,
					sizeof (debugrdwr));

			if (command == IFXMIPS_MEI_READDEBUG)
				meiDebugRead (debugrdwr.iAddress,
					      debugrdwr.buffer,
					      debugrdwr.iCount);
			else
				meiDebugWrite (debugrdwr.iAddress,
					       debugrdwr.buffer,
					       debugrdwr.iCount);

			if (!from_kernel)
				copy_to_user ((char *) lon, (char *) (&debugrdwr), sizeof (debugrdwr));	//dying gasp
#if 0				//tc.chen:It is no necessary to acquire lock to read debug memory!!
			MEI_MUTEX_UNLOCK (mei_sema);
#endif
			break;
		case IFXMIPS_MEI_RESET:
		case IFXMIPS_MEI_REBOOT:

#ifdef CONFIG_IFXMIPS_MEI_MIB
			mei_mib_adsl_link_down ();
#endif

#ifdef IFX_ADSL_L3_MODE_SUPPORT
			/* L3 Power Mode start */
			if (check_co_l3_shutdown_request () == 1)	//co request
			{
				// cpe received co L3 request
				l3_shutdown = 1;
			}
			/* L3 Power Mode end */
#endif //IFX_ADSL_L3_MODE_SUPPORT

			meiResetARC ();
			meiControlModeSwitch (MEI_MASTER_MODE);
			//enable ac_clk signal  
			_meiDebugLongWordRead (MEI_DEBUG_DEC_DMP1_MASK,
					       CRI_CCR0, &arc_debug_data);
			arc_debug_data |= ACL_CLK_MODE_ENABLE;
			_meiDebugLongWordWrite (MEI_DEBUG_DEC_DMP1_MASK,
						CRI_CCR0, arc_debug_data);
			meiControlModeSwitch (JTAG_MASTER_MODE);
			meiHaltArc ();
			update_bar_register (nBar);
			break;
		case IFXMIPS_MEI_DOWNLOAD:
			// DMA the boot code page(s)
			printk ("Start download pages");
			meiDownloadBootPages ();
			break;
#endif //IFXMIPS_MEI_CMV_EXTRA
			//for clearEoC
#ifdef IFXMIPS_CLEAR_EOC
		case IFXMIPS_MEI_EOC_SEND:
			if (!showtime) {
				return -EIO;
			}
			if (!from_kernel) {
				copy_from_user ((char *) (&debugrdwr),
						(char *) lon,
						sizeof (debugrdwr));
				eoc_skb =
					dev_alloc_skb (debugrdwr.iCount * 4);
				if (eoc_skb == NULL) {
					printk
						("\n\nskb alloc fail");
					break;
				}

				eoc_skb->len = debugrdwr.iCount * 4;
				memcpy (skb_put
					(eoc_skb, debugrdwr.iCount * 4),
					(char *) debugrdwr.buffer,
					debugrdwr.iCount * 4);
			}
			else {
				eoc_skb = (struct sk_buff *) lon;
			}
			ifx_me_ceoc_send (eoc_skb);	//pass data to higher layer
			break;
#endif // IFXMIPS_CLEAR_EOC
		case IFXMIPS_MEI_JTAG_ENABLE:
			printk ("ARC JTAG Enable.\n");
			*(IFXMIPS_GPIO_P0_DIR) = (*IFXMIPS_GPIO_P0_DIR) & (~0x800);	// set gpio11 to input
			*(IFXMIPS_GPIO_P0_ALTSEL0) = ((*IFXMIPS_GPIO_P0_ALTSEL0) & (~0x800));
			*(IFXMIPS_GPIO_P0_ALTSEL1) = ((*IFXMIPS_GPIO_P0_ALTSEL1) & (~0x800));
			*IFXMIPS_GPIO_P0_OD = (*IFXMIPS_GPIO_P0_OD) | 0x800;

			//enable ARC JTAG
			meiLongwordRead(IFXMIPS_RCU_RST, &reg_data);
			meiLongwordWrite(IFXMIPS_RCU_RST, reg_data | IFXMIPS_RCU_RST_REQ_ARC_JTAG);
			break;

		case GET_ADSL_LOOP_DIAGNOSTICS_MODE:
			copy_to_user ((char *) lon, (char *) &loop_diagnostics_mode, sizeof(int));
			break;
		case LOOP_DIAGNOSTIC_MODE_COMPLETE:
			loop_diagnostics_completed = 1;
#ifdef CONFIG_IFXMIPS_MEI_MIB
			// read adsl mode
			makeCMV (H2D_CMV_READ, STAT, 1, 0, 1, NULL, TxMessage);
			if (meiCMV (TxMessage, YES_REPLY, RxMessage) != MEI_SUCCESS) {
#ifdef IFXMIPS_MEI_DEBUG_ON
				printk ("\n\nCMV fail, Group STAT Address 1 Index 0");
#endif
			}
			adsl_mode = RxMessage[4];

			makeCMV (H2D_CMV_READ, STAT, 17, 0, 1, NULL, TxMessage);
			if (meiCMV (TxMessage, YES_REPLY, RxMessage) != MEI_SUCCESS) {
#ifdef IFXMIPS_MEI_DEBUG_ON
				printk ("\n\nCMV fail, Group STAT Address 1 Index 0");
#endif
			}
			adsl_mode_extend = RxMessage[4];
#endif
			MEI_WAKEUP_EVENT (wait_queue_loop_diagnostic);
			break;
		case SET_ADSL_LOOP_DIAGNOSTICS_MODE:
			if (lon != loop_diagnostics_mode) {
				loop_diagnostics_completed = 0;
				loop_diagnostics_mode = lon;
#if 0 //08/12/2006 tc.chen : autoboot daemon should reset dsl
				mei_ioctl ((MEI_inode_t *) 0, NULL,
					   IFXMIPS_MEI_REBOOT,
					   (unsigned long) NULL);
#endif
			}
			break;
		case IS_ADSL_LOOP_DIAGNOSTICS_MODE_COMPLETE:
			copy_to_user ((char *) lon,
				      (char *) &loop_diagnostics_completed,
				      sizeof (int));
			break;
#ifdef IFX_ADSL_L3_MODE_SUPPORT
			/* L3 Power Mode Start */
		case GET_POWER_MANAGEMENT_MODE:
			i = get_l3_power_status ();
			copy_to_user ((char *) lon, (char *) &i,
				      sizeof (int));
			break;
		case SET_L3_POWER_MODE:
			i = 1;
			copy_from_user ((char *) &i, (char *) lon,
					sizeof (int));
			if (i == 0) {
				return set_l3_shutdown ();
			}
			else {
				return set_l3_power_on ();
			}
			break;
			/* L3 Power Mode End */
#endif //IFX_ADSL_L3_MODE_SUPPORT
#ifdef IFX_ADSL_DUAL_LATENCY_SUPPORT
		case GET_ADSL_DUAL_LATENCY:
			i = mei_is_dual_latency_enabled ();
			if (i < 0)
				return i;
			copy_to_user ((char *) lon, (char *) &i,
				      sizeof (int));
			break;
		case SET_ADSL_DUAL_LATENCY:
			i = 0;
			copy_from_user ((char *) &i, (char *) lon,
					sizeof (int));
			if (i > DUAL_LATENCY_US_DS_ENABLE) {
				return -EINVAL;
			}
			if (i != bDualLatency) {
				bDualLatency = i;
				i = 1;	// DualLatency update,need to reboot arc
			}
			else {
				i = 0;	// DualLatency is the same
			}
			if (modem_ready && i)	// modem is already start, reboot arc to apply Dual Latency changed
			{
				mei_ioctl ((MEI_inode_t *) 0, NULL,
					   IFXMIPS_MEI_REBOOT,
					   (unsigned long) NULL);
			}
			break;

#endif
		case QUIET_MODE_GET:
			copy_to_user ((char *) lon, (char *) &quiet_mode_flag,
				      sizeof (int));
			break;
		case QUIET_MODE_SET:
			copy_from_user ((char *) &i, (char *) lon,
					sizeof (int));
			if (i > 1 || i < 0)
				return -EINVAL;
			if (i == 1) {
				u16 CMVMSG[MSG_LENGTH];
				u16 data = 0;
				makeCMV (H2D_CMV_WRITE, INFO, 94, 0, 1, &data, CMVMSG);	// set tx power to 0
				meierr = mei_ioctl ((struct inode *) 0, NULL,
						    IFXMIPS_MEI_CMV_WINHOST,
						    (unsigned long) CMVMSG);
			}
			quiet_mode_flag = i;
			break;
		case SHOWTIME_LOCK_GET:
			copy_to_user ((char *) lon,
				      (char *) &showtime_lock_flag,
				      sizeof (int));
			break;
		case SHOWTIME_LOCK_SET:
			copy_from_user ((char *) &i, (char *) lon,
					sizeof (int));
			if (i > 1 || i < 0)
				return -EINVAL;
			showtime_lock_flag = i;
			break;
		case AUTOBOOT_ENABLE_SET:
			copy_from_user ((char *) &i, (char *) lon,
					sizeof (int));
			if (i > 1 || i < 0)
				return -EINVAL;
			autoboot_enable_flag = i;
			break;
		default:
			printk
				("The ioctl command(0x%X is not supported!\n",
				 command);
			meierr = -ENOIOCTLCMD;
		}
	}
	return meierr;
}				//mei_ioctl

////////////////////     procfs debug    ///////////////////////////

#ifdef CONFIG_PROC_FS
static int
proc_read (struct file *file, char *buf, size_t nbytes, loff_t * ppos)
{
	int i_ino = (file->f_dentry->d_inode)->i_ino;
	char outputbuf[64];
	int count = 0;
	int i;
	u32 version = 0;
	reg_entry_t *current_reg = NULL;
	u16 RxMessage[MSG_LENGTH] __attribute__ ((aligned (4)));
	u16 TxMessage[MSG_LENGTH] __attribute__ ((aligned (4)));

	for (i = 0; i < NUM_OF_REG_ENTRY; i++) {
		if (regs[i].low_ino == i_ino) {
			current_reg = &regs[i];
			break;
		}
	}
	if (current_reg == NULL)
		return -EINVAL;

	if (current_reg->flag == (int *) 8) {
		///proc/mei/version
		//format:
		//Firmware version: major.minor.sub_version.int_version.rel_state.spl_appl
		///Firmware Date Time Code: date/month min:hour
		if (*ppos > 0)	/* Assume reading completed in previous read */
			return 0;	// indicates end of file
		if (MEI_MUTEX_LOCK (mei_sema))
			return -ERESTARTSYS;

		if (indicator_count < 1) {
			MEI_MUTEX_UNLOCK (mei_sema);
			return -EAGAIN;
		}
		//major:bits 0-7 
		//minor:bits 8-15
		makeCMV (H2D_CMV_READ, INFO, 54, 0, 1, NULL, TxMessage);
		if (meiCMV (TxMessage, YES_REPLY, RxMessage) != MEI_SUCCESS) {
			MEI_MUTEX_UNLOCK (mei_sema);
			return -EIO;
		}
		version = RxMessage[4];
		count = sprintf (outputbuf, "%d.%d.", (version) & 0xff,
				 (version >> 8) & 0xff);

		//sub_version:bits 4-7
		//int_version:bits 0-3
		//spl_appl:bits 8-13
		//rel_state:bits 14-15
		makeCMV (H2D_CMV_READ, INFO, 54, 1, 1, NULL, TxMessage);
		if (meiCMV (TxMessage, YES_REPLY, RxMessage) != MEI_SUCCESS) {
			MEI_MUTEX_UNLOCK (mei_sema);
			return -EFAULT;
		}
		version = RxMessage[4];
		count += sprintf (outputbuf + count, "%d.%d.%d.%d",
				  (version >> 4) & 0xf,
				  version & 0xf,
				  (version >> 14) & 0x3,
				  (version >> 8) & 0x3f);
		//Date:bits 0-7
		//Month:bits 8-15
		makeCMV (H2D_CMV_READ, INFO, 55, 0, 1, NULL, TxMessage);
		if (meiCMV (TxMessage, YES_REPLY, RxMessage) != MEI_SUCCESS) {
			MEI_MUTEX_UNLOCK (mei_sema);
			return -EIO;
		}
		version = RxMessage[4];

		//Hour:bits 0-7
		//Minute:bits 8-15
		makeCMV (H2D_CMV_READ, INFO, 55, 1, 1, NULL, TxMessage);
		if (meiCMV (TxMessage, YES_REPLY, RxMessage) != MEI_SUCCESS) {
			MEI_MUTEX_UNLOCK (mei_sema);
			return -EFAULT;
		}
		version += (RxMessage[4] << 16);
		count += sprintf (outputbuf + count, " %d/%d %d:%d\n",
				  version & 0xff, (version >> 8) & 0xff,
				  (version >> 25) & 0xff,
				  (version >> 16) & 0xff);
		MEI_MUTEX_UNLOCK (mei_sema);

		*ppos += count;
	}
	else if (current_reg->flag != (int *) Recent_indicator) {
		if (*ppos > 0)	/* Assume reading completed in previous read */
			return 0;	// indicates end of file
		count = sprintf (outputbuf, "0x%08X\n\n",
				 *(current_reg->flag));
		*ppos += count;
		if (count > nbytes)	/* Assume output can be read at one time */
			return -EINVAL;
	}
	else {
		if ((int) (*ppos) / ((int) 7) == 16)
			return 0;	// indicate end of the message
		count = sprintf (outputbuf, "0x%04X\n\n",
				 *(((u16 *) (current_reg->flag)) +
				   (int) (*ppos) / ((int) 7)));
		*ppos += count;
	}
	if (copy_to_user (buf, outputbuf, count))
		return -EFAULT;
	return count;
}

static ssize_t
proc_write (struct file *file, const char *buffer, size_t count,
	    loff_t * ppos)
{
	int i_ino = (file->f_dentry->d_inode)->i_ino;
	reg_entry_t *current_reg = NULL;
	int i;
	unsigned long newRegValue;
	char *endp;

	for (i = 0; i < NUM_OF_REG_ENTRY; i++) {
		if (regs[i].low_ino == i_ino) {
			current_reg = &regs[i];
			break;
		}
	}
	if ((current_reg == NULL)
	    || (current_reg->flag == (int *) Recent_indicator))
		return -EINVAL;

	newRegValue = simple_strtoul (buffer, &endp, 0);
	*(current_reg->flag) = (int) newRegValue;
	return (count + endp - buffer);
}
#endif //CONFIG_PROC_FS

//TODO, for loopback test
#ifdef DFE_LOOPBACK
#define mte_reg_base	(0x4800*4+0x20000)

/* Iridia Registers Address Constants */
#define MTE_Reg(r)    	(int)(mte_reg_base + (r*4))

#define IT_AMODE       	MTE_Reg(0x0004)

#define OMBOX_BASE 	0xDF80
#define OMBOX1 	(OMBOX_BASE+0x4)
#define IMBOX_BASE 	0xDFC0

#define TIMER_DELAY   	(1024)
#define BC0_BYTES     	(32)
#define BC1_BYTES     	(30)
#define NUM_MB        	(12)
#define TIMEOUT_VALUE 	2000

static void
BFMWait (u32 cycle)
{
	u32 i;
	for (i = 0; i < cycle; i++);
}

static void
WriteRegLong (u32 addr, u32 data)
{
	//*((volatile u32 *)(addr)) =  data; 
	IFXMIPS_WRITE_REGISTER_L (data, addr);
}

static u32
ReadRegLong (u32 addr)
{
	// u32  rd_val;
	//rd_val = *((volatile u32 *)(addr));
	// return rd_val;
	return IFXMIPS_READ_REGISTER_L (addr);
}

/* This routine writes the mailbox with the data in an input array */
static void
WriteMbox (u32 * mboxarray, u32 size)
{
	meiDebugWrite (IMBOX_BASE, mboxarray, size);
	printk ("write to %X\n", IMBOX_BASE);
	meiLongwordWrite ( MEI_TO_ARC_INT, MEI_TO_ARC_MSGAV);
}

/* This routine reads the output mailbox and places the results into an array */
static void
ReadMbox (u32 * mboxarray, u32 size)
{
	meiDebugRead (OMBOX_BASE, mboxarray, size);
	printk ("read from %X\n", OMBOX_BASE);
}

static void
MEIWriteARCValue (u32 address, u32 value)
{
	u32 i, check = 0;

	/* Write address register */
	IFXMIPS_WRITE_REGISTER_L (address, MEI_DEBUG_WAD);

	/* Write data register */
	IFXMIPS_WRITE_REGISTER_L (value, MEI_DEBUG_DATA);

	/* wait until complete - timeout at 40 */
	for (i = 0; i < 40; i++) {
		check = IFXMIPS_READ_REGISTER_L (ARC_TO_MEI_INT);

		if ((check & ARC_TO_MEI_DBG_DONE))
			break;
	}
	/* clear the flag */
	IFXMIPS_WRITE_REGISTER_L (ARC_TO_MEI_DBG_DONE, ARC_TO_MEI_INT);
}

void
arc_code_page_download (uint32_t arc_code_length, uint32_t * start_address)
{
	int count;
	printk ("try to download pages,size=%d\n", arc_code_length);
	meiControlModeSwitch (MEI_MASTER_MODE);
	if (arc_halt_flag == 0) {
		meiHaltArc ();
	}
	meiLongwordWrite ( MEI_XFR_ADDR, 0);
	for (count = 0; count < arc_code_length; count++) {
		meiLongwordWrite ( MEI_DATA_XFR,
				  *(start_address + count));
	}
	meiControlModeSwitch (JTAG_MASTER_MODE);
}
static int
load_jump_table (unsigned long addr)
{
	int i;
	uint32_t addr_le, addr_be;
	uint32_t jump_table[32];
	for (i = 0; i < 16; i++) {
		addr_le = i * 8 + addr;
		addr_be = ((addr_le >> 16) & 0xffff);
		addr_be |= ((addr_le & 0xffff) << 16);
		jump_table[i * 2 + 0] = 0x0f802020;
		jump_table[i * 2 + 1] = addr_be;
		//printk("jt %X %08X %08X\n",i,jump_table[i*2+0],jump_table[i*2+1]);
	}
	arc_code_page_download (32, &jump_table[0]);
	return 0;
}

void
dfe_loopback_irq_handler (void)
{
	uint32_t rd_mbox[10];

	memset (&rd_mbox[0], 0, 10 * 4);
	ReadMbox (&rd_mbox[0], 6);
	if (rd_mbox[0] == 0x0) {
		printk ("Get ARC_ACK\n");
		got_int = 1;
	}
	else if (rd_mbox[0] == 0x5) {
		printk ("Get ARC_BUSY\n");
		got_int = 2;
	}
	else if (rd_mbox[0] == 0x3) {
		printk ("Get ARC_EDONE\n");
		if (rd_mbox[1] == 0x0) {
			got_int = 3;
			printk ("Get E_MEMTEST\n");
			if (rd_mbox[2] != 0x1) {
				got_int = 4;
				printk ("Get Result %X\n",
						 rd_mbox[2]);
			}
		}
	}
	meiLongwordWrite ( ARC_TO_MEI_INT, ARC_TO_MEI_DBG_DONE);
	MEI_MASK_AND_ACK_IRQ (IFXMIPS_MEI_INT);
	disable_irq (IFXMIPS_MEI_INT);
	//got_int = 1;
	return;
}

static void
wait_mem_test_result (void)
{
	uint32_t mbox[5];
	mbox[0] = 0;
	printk ("Waiting Starting\n");
	while (mbox[0] == 0) {
		ReadMbox (&mbox[0], 5);
	}
	printk ("Try to get mem test result.\n");
	ReadMbox (&mbox[0], 5);
	if (mbox[0] == 0xA) {
		printk ("Success.\n");
	}
	else if (mbox[0] == 0xA) {
		printk
			("Fail,address %X,except data %X,receive data %X\n",
			 mbox[1], mbox[2], mbox[3]);
	}
	else {
		printk ("Fail\n");
	}
}

static int
arc_ping_testing (void)
{
#define MEI_PING 0x00000001
	uint32_t wr_mbox[10], rd_mbox[10];
	int i;
	for (i = 0; i < 10; i++) {
		wr_mbox[i] = 0;
		rd_mbox[i] = 0;
	}

	printk ("send ping msg\n");
	wr_mbox[0] = MEI_PING;
	WriteMbox (&wr_mbox[0], 10);

	while (got_int == 0) {
		MEI_WAIT (100);
	}

	printk ("send start event\n");
	got_int = 0;

	wr_mbox[0] = 0x4;
	wr_mbox[1] = 0;
	wr_mbox[2] = 0;
	wr_mbox[3] = (uint32_t) 0xf5acc307e;
	wr_mbox[4] = 5;
	wr_mbox[5] = 2;
	wr_mbox[6] = 0x1c000;
	wr_mbox[7] = 64;
	wr_mbox[8] = 0;
	wr_mbox[9] = 0;
	WriteMbox (&wr_mbox[0], 10);
	enable_irq (IFXMIPS_MEI_INT);
	//printk("meiMailboxWrite ret=%d\n",i);
	meiLongwordWrite ( MEI_TO_ARC_INT, MEI_TO_ARC_MSGAV);
	printk ("sleeping\n");
	while (1) {
		if (got_int > 0) {

			if (got_int > 3)
				printk ("got_int >>>> 3\n");
			else
				printk ("got int = %d\n", got_int);
			got_int = 0;
			//schedule();
			enable_irq (IFXMIPS_MEI_INT);
		}
		//mbox_read(&rd_mbox[0],6);
		MEI_WAIT (100);
	}
}

static MEI_ERROR
DFE_Loopback_Test (void)
{
	int i = 0;
	u32 arc_debug_data = 0, temp;

	meiResetARC ();
	// start the clock
	arc_debug_data = ACL_CLK_MODE_ENABLE;
	meiDebugWrite (CRI_CCR0, &arc_debug_data, 1);

#if defined( DFE_PING_TEST )|| defined( DFE_ATM_LOOPBACK)
	// WriteARCreg(AUX_XMEM_LTEST,0);
	meiControlModeSwitch (MEI_MASTER_MODE);
#define AUX_XMEM_LTEST 0x128
	_meiDebugLongWordWrite (MEI_DEBUG_DEC_AUX_MASK, AUX_XMEM_LTEST, 0);
	meiControlModeSwitch (JTAG_MASTER_MODE);

	// WriteARCreg(AUX_XDMA_GAP,0); 
	meiControlModeSwitch (MEI_MASTER_MODE);
#define AUX_XDMA_GAP 0x114
	_meiDebugLongWordWrite (MEI_DEBUG_DEC_AUX_MASK, AUX_XDMA_GAP, 0);
	meiControlModeSwitch (JTAG_MASTER_MODE);

	meiControlModeSwitch (MEI_MASTER_MODE);
	temp = 0;
	_meiDebugLongWordWrite (MEI_DEBUG_DEC_AUX_MASK,
				(u32) MEI_XDATA_BASE_SH, temp);
	meiControlModeSwitch (JTAG_MASTER_MODE);

	i = alloc_processor_memory (SDRAM_SEGMENT_SIZE * 16, adsl_mem_info);
	if (i >= 0) {
		int idx;

		for (idx = 0; idx < i; idx++) {
			adsl_mem_info[idx].type = FREE_RELOAD;
			IFXMIPS_WRITE_REGISTER_L ((((uint32_t)
						   adsl_mem_info[idx].
						   address) & 0x0fffffff),
						 MEI_XMEM_BAR_BASE + idx * 4);
			printk ("bar%d(%X)=%X\n", idx,
					 MEI_XMEM_BAR_BASE + idx * 4,
					 (((uint32_t) adsl_mem_info[idx].
					   address) & 0x0fffffff));
			memset ((u8 *) adsl_mem_info[idx].address, 0,
				SDRAM_SEGMENT_SIZE);
		}

		meiLongwordWrite ( MEI_XDATA_BASE_SH, ((unsigned long)
							    adsl_mem_info
							    [XDATA_REGISTER].
							    address) &
				  0x0FFFFFFF);

	}
	else {
		printk ("cannot load image: no memory\n\n");
		return MEI_FAILURE;
	}
	//WriteARCreg(AUX_IC_CTRL,2);
	meiControlModeSwitch (MEI_MASTER_MODE);
#define AUX_IC_CTRL 0x11
	_meiDebugLongWordWrite (MEI_DEBUG_DEC_AUX_MASK, AUX_IC_CTRL, 2);
	meiControlModeSwitch (JTAG_MASTER_MODE);

	meiHaltArc ();

#ifdef DFE_PING_TEST

	printk ("ping test image size=%d\n", sizeof (code_array));
	memcpy ((u8 *) (adsl_mem_info[0].address + 0x1004), &code_array[0],
		sizeof (code_array));
	load_jump_table (0x80000 + 0x1004);

#endif //DFE_PING_TEST

	printk ("ARC ping test code download complete\n");
#endif //defined( DFE_PING_TEST )|| defined( DFE_ATM_LOOPBACK)
#ifdef DFE_MEM_TEST
	meiLongwordWrite (ARC_TO_MEI_INT_MASK, MSGAV_EN);

	arc_code_page_download (1537, &mem_test_code_array[0]);
	printk ("ARC mem test code download complete\n");
#endif //DFE_MEM_TEST
#ifdef DFE_ATM_LOOPBACK
	arc_debug_data = 0xf;
	arc_code_page_download (1077, &code_array[0]);
	// Start Iridia IT_AMODE (in dmp access) why is it required?
	meiDebugWrite (0x32010, &arc_debug_data, 1);
#endif //DFE_ATM_LOOPBACK
	meiMailboxInterruptsEnable ();
	meiRunArc ();

#ifdef DFE_PING_TEST
	arc_ping_testing ();
#endif //DFE_PING_TEST
#ifdef DFE_MEM_TEST
	wait_mem_test_result ();
#endif //DFE_MEM_TEST

	free_image_buffer (FREE_ALL);
	return MEI_SUCCESS;
}

#endif //DFE_LOOPBACK
//end of TODO, for loopback test

#if defined(CONFIG_IFXMIPS_MEI_LED) && defined(DATA_LED_SUPPORT)

/* 
 *  Led Thread Main function
 */
static int
led_poll (void *unused)
{
	struct task_struct *tsk = current;

	daemonize("mei_led_poll");
	strcpy (tsk->comm, "atm_led");
	sigfillset (&tsk->blocked);

	stop_led_module = 0;	//begin polling ...

	while (!stop_led_module) {
		if (led_status_on || led_need_to_flash) {
			adsl_led_flash_task ();
		}
		if (led_status_on)	//sleep 200 ms to check if need to turn led off
		{
			interruptible_sleep_on_timeout
				(&wait_queue_led_polling, 25);
		}
		else {
			interruptible_sleep_on (&wait_queue_led_polling);
		}
	}
	return 0;
}

/* 
 * API for atm driver to notify led thread a data coming/sending 
 */
#if defined (CONFIG_ATM_IFXMIPS)
static int
adsl_led_flash (void)
{
	if (!modem_ready)
		return 0;

	if (led_status_on == 0 && led_need_to_flash == 0)
	{
		wake_up_interruptible (&wait_queue_led_polling);	//wake up and clean led module 
	}
	led_need_to_flash = 1;	//asking to flash led

	return 0;
}
#endif
/*
 * Main task for led controlling.
 */
static int
adsl_led_flash_task (void)
{
#ifdef DATA_LED_ADSL_FW_HANDLE
	u16 one = 1;
	u16 zero = 0;
	u16 data = 0x0600;
	u16 CMVMSG[MSG_LENGTH];
#endif

//      printk("Task Running...\n");    //joelin  test

	if (!showtime) {
		led_need_to_flash = 0;
		led_status_on = 0;
		return 0;
	}

	if (led_status_on == 0 && led_need_to_flash == 1) {

#ifdef DATA_LED_ADSL_FW_HANDLE
		data = 0x0901;	//flash
		send_cmv (H2D_CMV_WRITE, INFO, 91, 5, 1, &data, CMVMSG);	//use GPIO9 for TR68 data led .flash.
#else
		ifxmips_led_blink_set(0x0); // data
		ifxmips_led_blink_set(0x1); // link
#endif
		led_status_on = 1;

	}
	else if (led_status_on == 1 && led_need_to_flash == 0) {
#ifdef DATA_LED_ADSL_FW_HANDLE
#ifdef DATA_LED_ON_MODE
		data = 0x0903;	//use GPIO9 for TR68 data led .turn on.
#else
		data = 0x0900;	//off
#endif
		printk ("off %04X\n", data);
		send_cmv (H2D_CMV_WRITE, INFO, 91, 5, 1, &data, CMVMSG);	//use GPIO9 for TR68 data led .off.
#else
#endif
		led_status_on = 0;
	}
	led_need_to_flash = 0;
	return 0;
}

/* 
 * Led initialization function
 * This function create a thread to polling atm traffic and do led blanking
 */
static int
ifxmips_mei_led_init (void)
{
	init_waitqueue_head (&wait_queue_led_polling);	// adsl led for led function
	kernel_thread (led_poll, NULL, CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD);
	return 0;
}

/* 
 * Led destory function
 */
static int
ifxmips_mei_led_cleanup (void)
{
	stop_led_module = 1;	//wake up and clean led module 
	wake_up_interruptible (&wait_queue_led_polling);	//wake up and clean led module   
	return 0;
}
#endif //#ifdef CONFIG_IFXMIPS_MEI_LED

////////////////////////////////////////////////////////////////////////////
int __init
ifxmips_mei_init_module (void)
{
	struct proc_dir_entry *entry;
	int i;
	u32 temp;
#ifdef CONFIG_DEVFS_FS
	char buf[10];
#endif
	reg_entry_t regs_temp[PROC_ITEMS] =	// Items being debugged
	{
		/*  {   flag,                   name,              description } */
		{&arcmsgav, "arcmsgav", "arc to mei message ", 0},
		{&cmv_reply, "cmv_reply", "cmv needs reply", 0},
		{&cmv_waiting, "cmv_waiting",
		 "waiting for cmv reply from arc", 0},
		{&indicator_count, "indicator_count",
		 "ARC to MEI indicator count", 0},
		{&cmv_count, "cmv_count", "MEI to ARC CMVs", 0},
		{&reply_count, "reply_count", "ARC to MEI Reply", 0},
		{(int *) Recent_indicator, "Recent_indicator",
		 "most recent indicator", 0},
		{(int *) 8, "version", "version of firmware", 0},
	};
	do_gettimeofday (&time_disconnect);

	printk ("Danube MEI version:%s\n", IFXMIPS_MEI_VERSION);

	memcpy ((char *) regs, (char *) regs_temp, sizeof (regs_temp));
	MEI_MUTEX_INIT (mei_sema, 1);	// semaphore initialization, mutex
	MEI_INIT_WAKELIST ("arcq", wait_queue_arcmsgav);	// for ARCMSGAV
	MEI_INIT_WAKELIST ("arcldq", wait_queue_loop_diagnostic);	// for loop diagnostic function
#ifdef IFX_ADSL_L3_MODE_SUPPORT
	MEI_INIT_WAKELIST ("arcl3q", wait_queue_l3);	// for l3 power mode
#endif //IFX_ADSL_L3_MODE_SUPPORT


	memset (&adsl_mem_info[0], 0, sizeof (smmu_mem_info_t) * MAX_BAR_REGISTERS);
#if defined(CONFIG_IFXMIPS_MEI_LED) && defined(DATA_LED_SUPPORT)
	printk("not enabling mei leds due to bug that makes the board hang\n");
//	ifxmips_mei_led_init ();
#endif

#ifdef CONFIG_IFXMIPS_MEI_MIB
	ifxmips_mei_mib_init ();
#endif

#ifdef IFXMIPS_CLEAR_EOC
	MEI_INIT_WAKELIST ("arceoc", wait_queue_hdlc_poll);
	ifxmips_mei_ceoc_init ();
#endif
	// power up mei 
	temp = ifxmips_r32(IFXMIPS_PMU_PWDCR);
	temp &= 0xffff7dbe;
	ifxmips_w32(temp, IFXMIPS_PMU_PWDCR);

#if defined (CONFIG_ATM_IFXMIPS)
	IFX_ATM_LED_Callback_Register (adsl_led_flash);
#endif
	if (register_chrdev (major, IFXMIPS_MEI_DEVNAME, &mei_operations) != 0) {
		printk("\n\n unable to register major for ifxmips_mei!!!");
		return -ENODEV;
	} else {
		printk("registered ifxmips_mei on #%d\n", major);
	}

	disable_irq(IFXMIPS_MEI_INT);

	if (request_irq(IFXMIPS_MEI_INT, mei_interrupt_arcmsgav, 0, "ifxmips_mei_arcmsgav", NULL) != 0) {
		printk("\n\n unable to register irq(%d) for ifxmips_mei!!!",
			IFXMIPS_MEI_INT);
		return -1;
	}

//	enable_irq(IFXMIPS_MEI_INT);
	// procfs
	meidir = proc_mkdir(MEI_DIRNAME, &proc_root);
	if (meidir == NULL)
	{
		printk(": can't create /proc/" MEI_DIRNAME "\n\n");
		return -ENOMEM;
	}

	for (i = 0; i < NUM_OF_REG_ENTRY; i++) {
		entry = create_proc_entry (regs[i].name,
					   S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH, meidir);
		if (entry)
		{
			regs[i].low_ino = entry->low_ino;
			entry->proc_fops = &proc_operations;
		} else {
			printk (": can't create /proc/" MEI_DIRNAME "/%s\n\n", regs[i].name);
			return -ENOMEM;
		}
	}

	///////////////////////////////// register net device ////////////////////////////
#ifdef DFE_LOOPBACK
	DFE_Loopback_Test ();
#endif //DFE_LOOPBACK
	return 0;
}

void __exit
ifxmips_mei_cleanup_module (void)
{
	int i;

#if defined(CONFIG_IFXMIPS_MEI_LED) && defined(DATA_LED_SUPPORT)
	ifxmips_mei_led_cleanup ();
#endif
	showtime = 0;		//joelin,clear task

#ifdef CONFIG_PROC_FS
	for (i = 0; i < NUM_OF_REG_ENTRY; i++)
		remove_proc_entry (regs[i].name, meidir);

	remove_proc_entry (MEI_DIRNAME, &proc_root);
#endif //CONFIG_PROC_FS

#if defined (CONFIG_ATM_IFXMIPS)
	IFX_ATM_LED_Callback_Unregister (adsl_led_flash);
#endif
	disable_irq (IFXMIPS_MEI_INT);
	free_irq(IFXMIPS_MEI_INT, NULL);

#ifdef CONFIG_DEVFS_FS
	devfs_unregister (mei_devfs_handle);
#else
	unregister_chrdev (major, "ifxmips_mei");
#endif
#ifdef CONFIG_IFXMIPS_MEI_MIB
	ifxmips_mei_mib_cleanup ();
#endif

	free_image_buffer (FREE_ALL);
	return;
}

EXPORT_SYMBOL (meiDebugRead);
EXPORT_SYMBOL (meiDebugWrite);
EXPORT_SYMBOL (ifx_me_hdlc_send);
EXPORT_SYMBOL (ifx_mei_hdlc_read);
MODULE_LICENSE ("GPL");

module_init (ifxmips_mei_init_module);
module_exit (ifxmips_mei_cleanup_module);
