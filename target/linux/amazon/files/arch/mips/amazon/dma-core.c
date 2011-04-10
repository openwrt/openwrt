/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */
//-----------------------------------------------------------------------
/*
 * Description:
 *	Driver for Infineon Amazon DMA
 */
//-----------------------------------------------------------------------
/* Author:	Wu Qi Ming[Qi-Ming.Wu@infineon.com]
 * Created:	7-April-2004
 */
//-----------------------------------------------------------------------
/* History
 * Last changed on: 4-May-2004
 * Last changed by: <peng.liu@infineon.com>
 * Reason: debug
 */
//----------------------------------------------------------------------- 
/* Last changed on: 03-Dec-2004
 * Last changed by: peng.liu@infineon.com
 * Reason: recover from TPE bug 
 */

//000004:fchang 2005/6/2 Modified by Linpeng as described below
//----------------------------------------------------------------------- 
/* Last changed on: 28-Jan-2004
 * Last changed by: peng.liu@infineon.com
 * Reason: 
 * - handle "out of memory" bug
 */
//000003:tc.chen 2005/06/16 fix memory leak when Tx buffer full (heaving traffic).
//507261:tc.chen 2005/07/26 re-organize code address map to improve performance.

#if defined(CONFIG_MODVERSIONS) && !defined(MODVERSIONS)
#define MODVERSIONS
#endif

#if defined(MODVERSIONS) && !defined(__GENKSYMS__)
#include <linux/modversions.h>
#endif

#ifndef EXPORT_SYMTAB
#define EXPORT_SYMTAB			/* need this one 'cause we export symbols */
#endif

#undef DMA_NO_POLLING

/* no TX interrupt handling */
#define NO_TX_INT
/* need for DMA workaround */
#undef AMAZON_DMA_TPE_AAL5_RECOVERY

#ifdef AMAZON_DMA_TPE_AAL5_RECOVERY
#define MAX_SYNC_FAILS 1000000	// 000004:fchang
unsigned int dma_sync_fails = 0;
unsigned int total_dma_tpe_reset = 0;
int (*tpe_reset) (void);
int (*tpe_start) (void);
int (*tpe_inject) (void);
#endif							// AMAZON_DMA_TPE_AAL5_RECOVERY


#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/stat.h>
#include <linux/mm.h>
#include <linux/tty.h>
#include <linux/selection.h>
#include <linux/kmod.h>
#include <linux/vmalloc.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <linux/errno.h>
#include <asm/io.h>

#include <asm/amazon/amazon.h>
#include <asm/amazon/irq.h>
#include <asm/amazon/amazon_dma.h>
#include "dma-core.h"

#define AMAZON_DMA_EMSG(fmt, args...) printk( KERN_ERR  "%s: " fmt,__FUNCTION__, ## args)

static irqreturn_t dma_interrupt(int irq, void *dev_id);
extern void mask_and_ack_amazon_irq(unsigned int irq_nr);

/***************************************** global data *******************************************/
u64 *g_desc_list;
dev_list *g_current_dev = NULL;
dev_list *g_head_dev = NULL;
dev_list *g_tail_dev = NULL;
channel_info g_log_chan[CHAN_TOTAL_NUM + 1];
struct proc_dir_entry *g_amazon_dma_dir;
static u8 rx_chan_list_len = 0;
static u8 tx_chan_list_len = 0;
static int rx_chan_list[RX_CHAN_NUM + 1];
static int tx_chan_list[TX_CHAN_NUM + 1];
static u32 comb_isr_mask[CHAN_TOTAL_NUM];

static inline int is_rx_chan(int chan_no)
/*judge if this is an rx channel*/
{
	int result = 0;
	if (chan_no < RX_CHAN_NUM)
		result = 1;
	return result;
}

/* Ugly, Channel ON register is badly mapped to channel no. */
static u8 ch_on_mapping[CHAN_TOTAL_NUM] =
	{ 0, 1, 2, 3, 6, 7, 10, 4, 5, 8, 9, 11 };

/* Brief: 	check wether the chan_no is legal
 * Parameter:  	chan_no: logical channel number
 * Return: 	0 if is not valid
 *		1 if is valid
 */
static inline int is_valid_dma_ch(int chan_no)
{
	return ((chan_no >= 0) && (chan_no < CHAN_TOTAL_NUM));
}

/* Brief:	check whether a channel is open through Channel ON register
 * Parameter:  chan_no: logical channel number
 * Return: 	1 channel is open
 *		0 not yet
 *		EINVAL: invalid parameter
 */
static inline int is_channel_open(int chan_no)
{
	return (AMAZON_DMA_REG32(AMAZON_DMA_CH_ON) &
			(1 << ch_on_mapping[chan_no]));
}

/* Brief: add a list entry
 * Description: 
 * 	always add to the tail and no redundancy allowed. (i.e. entries are unique)
 *	0	: entry deleted
 *	<0	: not deleted (due to not unique)
 */
static inline int _add_list_entry(int *list, int size_of_list, int entry)
{
	int i;
	for (i = 0; i < size_of_list; i++) {
		if (list[i] == entry)
			break;
		if (list[i] < 0) {
			list[i] = entry;
			return 0;
		}
	}
	return -1;
}

/* Brief: delete a list entry
 * Description:
 *	find the entry and remove it. shift all entries behind it one step forward if necessary\
 * Return:
 *	0	: entry deleted
 *	<0	: not deleted (due to not found?)
 */
static inline int _delete_list_entry(int *list, int size_of_list,
									 int entry)
{
	int i, j;
	for (i = 0; i < size_of_list; i++) {
		if (list[i] == entry) {
			for (j = i; j < size_of_list; j++) {
				list[j] = list[j + 1];
				if (list[j + 1] < 0) {
					break;
				}
			}
			return 0;
		}
	}
	return -1;
}

/* Brief:	enable a channel through Channel ON register
 * Parameter:  chan_no: logical channel number
 * Description:	
 * 	Please don't open a channel without a valid descriptor (hardware pitfall)
 */
static inline void open_channel(int chan_no)
{
	AMAZON_DMA_REG32(AMAZON_DMA_CH_ON) |= (1 << ch_on_mapping[chan_no]);
	if (is_rx_chan(chan_no)) {
		if (_add_list_entry(rx_chan_list, RX_CHAN_NUM, chan_no) == 0) {
			rx_chan_list_len++;
		} else {
			AMAZON_DMA_DMSG("cannot add chan %d to open list\n", chan_no);
		}
	} else {
		if (_add_list_entry(tx_chan_list, TX_CHAN_NUM, chan_no) == 0) {
			tx_chan_list_len++;
		} else {
			AMAZON_DMA_DMSG("cannot add chan %d to open list\n", chan_no);
		}
	}
}

/* Brief:	disable a channel through Channel ON register
 * Parameter:  chan_no: logical channel number
 */

static inline void close_channel(int chan_no)
{
	AMAZON_DMA_REG32(AMAZON_DMA_CH_ON) &= ~(1 << ch_on_mapping[chan_no]);
	if (is_rx_chan(chan_no)) {
		if (_delete_list_entry(rx_chan_list, RX_CHAN_NUM, chan_no) == 0) {
			rx_chan_list_len--;
		} else {
			AMAZON_DMA_DMSG("cannot remove chan %d from open list \n",
							chan_no);
		}
	} else {
		if (_delete_list_entry(tx_chan_list, TX_CHAN_NUM, chan_no) == 0) {
			tx_chan_list_len--;
		} else {
			AMAZON_DMA_DMSG("cannot remove chan %d from open list \n",
							chan_no);
		}
	}
}

/* Brief: clear RX interrupt
 */
inline void rx_chan_clear_isr(int chan_no)
{
#ifdef DMA_NO_POLLING
	AMAZON_DMA_REG32(AMAZON_DMA_CH0_ISR + chan_no * AMAZON_DMA_CH_STEP) =
		(AMAZON_DMA_REG32
		 (AMAZON_DMA_CH0_ISR +
		  chan_no *
		  AMAZON_DMA_CH_STEP) & (DMA_ISR_CPT | DMA_ISR_EOP | DMA_ISR_CMDCPT
								 | DMA_ISR_DURR));
#else
	AMAZON_DMA_REG32(AMAZON_DMA_CH0_ISR + chan_no * AMAZON_DMA_CH_STEP) =
		(AMAZON_DMA_REG32
		 (AMAZON_DMA_CH0_ISR +
		  chan_no *
		  AMAZON_DMA_CH_STEP) & (DMA_ISR_CPT | DMA_ISR_EOP |
								 DMA_ISR_CMDCPT));
#endif
}

#ifdef AMAZON_DMA_TPE_AAL5_RECOVERY
/* Brief:	hacking function, this will reset all descriptors back to DMA
 */
static void dma_reset_all_descriptors(int chan_no)
{
	volatile struct rx_desc *rx_desc_p = NULL;
	int i;
	rx_desc_p =
		(struct rx_desc *) g_desc_list +
		g_log_chan[chan_no].offset_from_base;
	for (i = 0; i < g_log_chan[chan_no].desc_len; i++) {
		rx_desc_p->status.word &=
			(~(DMA_DESC_SOP_SET | DMA_DESC_EOP_SET | DMA_DESC_CPT_SET));
		rx_desc_p->status.word |=
			(DMA_DESC_OWN_DMA | g_log_chan[chan_no].packet_size);
		rx_desc_p++;
	}
}

/* Brief:	Reset DMA descriptors 
 */
static void amazon_dma_reset_tpe_rx(int chan_no)
{
	struct tx_desc *tx_desc_p = NULL;
	int j, i = 0;

	// wait until all TX channels stop transmitting
	for (j = 9; j <= 10; j++) {
		tx_desc_p =
			(struct tx_desc *) g_desc_list +
			g_log_chan[j].offset_from_base;
		for (i = 0; i < g_log_chan[j].desc_len; i++) {
			while ((tx_desc_p->status.field.OWN != CPU_OWN)) {
				AMAZON_DMA_DMSG("DMA TX in progress\n");	// 000004:fchang
				udelay(100);
			}
			tx_desc_p++;
		}
	}

	if (tpe_reset) {
		total_dma_tpe_reset++;
		AMAZON_DMA_DMSG
			("\n===============resetting TPE========================== \n");
		if ((*tpe_reset) ()) {
			panic("cannot reset TPE engien\n");	// 000004:fchang
		}
	} else {
		panic("no tpe_reset function\n");	// 000004:fchang
		return;
	}
	dma_reset_all_descriptors(chan_no);
	rx_chan_clear_isr(chan_no);
	mb();

	// send EoP
	if (tpe_inject) {
		if ((*tpe_inject) ()) {
			panic("cannot inject a cell\n");	// 000004:fchang
		}
	} else {
		AMAZON_DMA_EMSG("no tpe_inject function\n");
		return;
	}
	mb();
	while (1) {
		if (AMAZON_DMA_REG32
			(AMAZON_DMA_CH0_ISR +
			 chan_no * AMAZON_DMA_CH_STEP) & (DMA_ISR_CPT)) {
			rx_chan_clear_isr(chan_no);
			mb();
			dma_reset_all_descriptors(chan_no);
			if (g_log_chan[chan_no].current_desc ==
				(g_log_chan[chan_no].desc_len - 1)) {
				g_log_chan[chan_no].current_desc = 0;
			} else {
				g_log_chan[chan_no].current_desc++;
			}
			break;
		}
		mdelay(1);
	}
	mb();
#if 0
	AMAZON_DMA_REG32(AMAZON_DMA_CH_ON) &= ~(1 << ch_on_mapping[chan_no]);
	while (AMAZON_DMA_REG32(AMAZON_DMA_CH_ON) &
		   (1 << ch_on_mapping[chan_no])) {
		printk("TPE channel still on\n");
		mdelay(1);
	}

	// AMAZON_DMA_REG32(AMAZON_DMA_CH_RST) = (1<<chan_no);
	mb();
	AMAZON_DMA_REG32(AMAZON_DMA_CH0_MSK + chan_no * AMAZON_DMA_CH_STEP) =
		0x32;
	mb();
	rx_chan_clear_isr(chan_no);
	dma_reset_all_descriptors(chan_no);
	mb();
	AMAZON_DMA_REG32(AMAZON_DMA_CH_ON) |= (1 << ch_on_mapping[chan_no]);
	// g_log_chan[chan_no].current_desc=0;
	mb();
	mdelay(1);
#endif
	if (tpe_start) {
		(*tpe_start) ();
	} else {
		AMAZON_DMA_EMSG("cannot restart TPE engien\n");
	}
}
#endif							// AMAZON_DMA_TPE_AAL5_RECOVERY


/* Brief:	RX channel interrupt handler 
 * Parameter:	RX channel no
 * Description: the interrupt handler for each RX channel
 *		1. check descriptor, clear ISR if no incoming packet
 *		2. inform upper layer to receive packet (and update descriptors)
 */
inline void rx_chan_intr_handler(int chan_no)
{
	volatile struct rx_desc *rx_desc_p = NULL;

	/* fetch the current descriptor */
	rx_desc_p =
		(struct rx_desc *) g_desc_list +
		g_log_chan[chan_no].offset_from_base +
		g_log_chan[chan_no].current_desc;

	g_log_chan[chan_no].dma_dev->current_rx_chan =
		chan_no - g_log_chan[chan_no].dma_dev->logic_rx_chan_base;

	// workaround for DMA pitfall: complete bit set happends before the
	// other two bits (own,eop) are ready
	if ((rx_desc_p->status.field.EoP != 1)
		|| (rx_desc_p->status.field.OWN != CPU_OWN)
		|| (rx_desc_p->status.field.data_length ==
			g_log_chan[chan_no].packet_size)) {
#ifdef AMAZON_DMA_TPE_AAL5_RECOVERY
		if (chan_no == 4 || chan_no == 5) {
			dma_sync_fails++;
			if (dma_sync_fails > MAX_SYNC_FAILS) {
				// detect bug
				rx_desc_p0 =
					(struct rx_desc *) g_desc_list +
					g_log_chan[chan_no].offset_from_base;
				rx_desc_p1 =
					(struct rx_desc *) g_desc_list +
					g_log_chan[chan_no].offset_from_base + 1;
				if ((rx_desc_p0->status.field.OWN == CPU_OWN
					 && rx_desc_p0->status.field.EoP != 1)
					&& (rx_desc_p1->status.field.OWN == CPU_OWN
						&& rx_desc_p1->status.field.EoP != 1)) {
					amazon_dma_reset_tpe_rx(chan_no);
					dma_sync_fails = 0;
					return;
				}
				dma_sync_fails = 0;
				AMAZON_DMA_DMSG("too many times ch:%d\n", chan_no);	// 000004:fchang
				return;
			}
			udelay(10);			// 000004:fchang
		}
#endif							// //AMAZON_DMA_TPE_AAL5_RECOVERY
		return;
	}

	/* inform the upper layer to receive the packet */
	g_log_chan[chan_no].intr_handler(g_log_chan[chan_no].dma_dev, RCV_INT);
	/* check the next descriptor, if still contains the incoming packet,
	   then do not clear the interrupt status */
	rx_desc_p =
		(struct rx_desc *) g_desc_list +
		g_log_chan[chan_no].offset_from_base +
		g_log_chan[chan_no].current_desc;
	if (!
		((rx_desc_p->status.field.OWN == CPU_OWN)
		 && (rx_desc_p->status.field.C == 1))) {
		rx_chan_clear_isr(chan_no);
	}
}


/* Brief:	TX channel interrupt handler 
 * Parameter:	TX channel no
 * Description: the interrupt handler for each TX channel
 * 1. check all the descripters,if any of them had transmitted a packet, then free buffer
 * because we cannot garantee the which one has already transmitted out, we have to go through all the descriptors here
 * 2. clear the interrupt status bit
 */
inline void tx_chan_intr_handler(int chan_no)
{
	struct tx_desc *tx_desc_p = NULL;
	int i = 0;

	tx_desc_p =
		(struct tx_desc *) g_desc_list +
		g_log_chan[chan_no].offset_from_base;

	for (i = 0; i < g_log_chan[chan_no].desc_len; i++) {
		if ((tx_desc_p->status.field.OWN == CPU_OWN)
			&& (tx_desc_p->status.field.C == 1)) {
			/* if already transmitted, then free the buffer */
			g_log_chan[chan_no].
				buffer_free((u8 *) __va(tx_desc_p->Data_Pointer),
							g_log_chan[chan_no].opt[i]);
			tx_desc_p->status.field.C = 0;
			/* inform the upper layer about the completion of the
			   transmitted packet, the upper layer may want to free the
			   packet */
			g_log_chan[chan_no].intr_handler(g_log_chan[chan_no].dma_dev,
											 TRANSMIT_CPT_INT);
		}
		tx_desc_p++;
	}

	/* after all these operations, clear the interrupt status bit */
	AMAZON_DMA_REG32(AMAZON_DMA_CH0_ISR + chan_no * AMAZON_DMA_CH_STEP) =
		(AMAZON_DMA_REG32
		 (AMAZON_DMA_CH0_ISR +
		  chan_no *
		  AMAZON_DMA_CH_STEP) & (DMA_ISR_CPT | DMA_ISR_EOP |
								 DMA_ISR_CMDCPT));
}

/*	Brief:	DMA interrupt handler
 */
static irqreturn_t dma_interrupt(int irq, void *dev_id)
{
	int i = 0;
	int chan_no;
	u32 isr = 0;
#ifdef NO_TX_INT				// 000004:fchang
	static int cnt = 0;			// 000004:fchang
#endif							// 000004:fchang
	while ((isr =
			AMAZON_DMA_REG32(AMAZON_DMA_COMB_ISR)) & (COMB_ISR_RX_MASK |
													  COMB_ISR_TX_MASK)) {
		if (isr & COMB_ISR_RX_MASK) {
			// RX Channels: start WFQ algorithm
			chan_no = CHAN_TOTAL_NUM;
			for (i = 0; i < RX_CHAN_NUM; i++) {
				if ((isr & (comb_isr_mask[i]))
					&& (g_log_chan[i].weight > 0)) {
					if (g_log_chan[chan_no].weight < g_log_chan[i].weight) {
						chan_no = i;
					}
				}
			}
			if (chan_no < CHAN_TOTAL_NUM) {
				rx_chan_intr_handler(chan_no);
			} else {
				for (i = 0; i < RX_CHAN_NUM; i++) {
					g_log_chan[i].weight = g_log_chan[i].default_weight;
				}
			}
		}
#ifdef NO_TX_INT
		cnt++;
		if (cnt == 10) {
			cnt = 0;
			for (i = 0; i < tx_chan_list_len; i++) {
				if (AMAZON_DMA_REG32
					(AMAZON_DMA_CH0_ISR +
					 tx_chan_list[i] *
					 AMAZON_DMA_CH_STEP) & (DMA_ISR_CPT | DMA_ISR_EOP)) {
					tx_chan_intr_handler(tx_chan_list[i]);
				}
			}
		}
#else
		if (isr & COMB_ISR_TX_MASK) {
			// TX channels: RR
			for (i = 0; i < tx_chan_list_len; i++) {
				if (isr & (comb_isr_mask[tx_chan_list[i]])) {
					tx_chan_intr_handler(tx_chan_list[i]);
				}
			}
		}
#endif
	}							// while 
	return IRQ_HANDLED;
}


/*	Brief:	read a packet from DMA RX channel
 *	Parameter:
 *	Return:	packet length
 *	Description:
 *		This is called back in a context of DMA interrupt
 *		1. prepare new descriptor
 *		2. read data
 *		3. update WFQ weight
 */
//507261:tc.chen int dma_device_read(struct dma_device_info* dma_dev, u8** dataptr, void** opt)
int asmlinkage dma_device_read(struct dma_device_info *dma_dev,
							   u8 ** dataptr, void **opt)
{
	u8 *buf;
	int len;
	int chan_no = 0;
	int byte_offset = 0;

	struct rx_desc *rx_desc_p;
	void *p = NULL;
	int current_desc;

	chan_no = dma_dev->logic_rx_chan_base + dma_dev->current_rx_chan;
	current_desc = g_log_chan[chan_no].current_desc;
	rx_desc_p =
		(struct rx_desc *) (g_desc_list +
							g_log_chan[chan_no].offset_from_base +
							current_desc);
	buf = (u8 *) __va(rx_desc_p->Data_Pointer);	/* extract the virtual
												   address of the data
												   pointer */
	len = rx_desc_p->status.field.data_length;	/* extract the data length */
#ifndef	CONFIG_MIPS_UNCACHED
	dma_cache_inv((unsigned long) buf, len);
#endif							// CONFIG_MIPS_UNCACHED
	*(u32 *) dataptr = (u32) buf;
	if (opt) {
		*(int *) opt = (int) g_log_chan[chan_no].opt[current_desc];	/* read 
																	   out 
																	   the 
																	   opt 
																	   information */
	}

	buf =
		(u8 *) g_log_chan[chan_no].buffer_alloc(g_log_chan[chan_no].
												packet_size, &byte_offset,
												&p);
	// should check null!!!!
	if (buf == NULL || p == NULL) {
		*(u32 *) dataptr = 0;
		*(int *) opt = 0;
		len = 0;
	} else {
		g_log_chan[chan_no].opt[current_desc] = p;
		/* reduce the weight for WFQ algorithm */
		g_log_chan[chan_no].weight -= len;
		rx_desc_p->Data_Pointer = (u32) CPHYSADDR((u32) buf);
	}
	if (current_desc == g_log_chan[chan_no].desc_len - 1) {
		current_desc = 0;
	} else {
		current_desc++;
	}
	g_log_chan[chan_no].current_desc = current_desc;

	rx_desc_p->status.word = DMA_DESC_OWN_DMA
		| (byte_offset << DMA_DESC_BYTEOFF_SHIFT)
		| g_log_chan[chan_no].packet_size;
	return len;
}

/*	Brief:	write a packet through DMA RX channel to peripheral
 *	Parameter:
 *	Return:	packet length
 *	Description:
 *
 */
u64 dma_tx_drop = 0;
//507261:tc.chen int dma_device_write(struct dma_device_info* dma_dev, u8* dataptr, int len,void* opt)
int asmlinkage dma_device_write(struct dma_device_info *dma_dev,
								u8 * dataptr, int len, void *opt)
{
	int chan_no = 0;
	struct tx_desc *tx_desc_p;

	int byte_offset = 0;
	int current_desc;
	static int cnt = 0;			// 000004:fchang

	unsigned long flag;
	local_irq_save(flag);

	chan_no = dma_dev->logic_tx_chan_base + dma_dev->current_tx_chan;
	current_desc = g_log_chan[chan_no].current_desc;
	tx_desc_p =
		(struct tx_desc *) (g_desc_list +
							g_log_chan[chan_no].offset_from_base +
							current_desc);
	// 000003:tc.chen if(tx_desc_p->status.field.OWN==DMA_OWN){
	if (tx_desc_p->status.field.OWN == DMA_OWN || tx_desc_p->status.field.C == 1) {	// 000003:tc.chen
		AMAZON_DMA_DMSG("no TX desc for CPU, drop packet\n");
		dma_tx_drop++;
		g_log_chan[chan_no].intr_handler(dma_dev, TX_BUF_FULL_INT);
		local_irq_restore(flag);
		return 0;
	}
	g_log_chan[chan_no].opt[current_desc] = opt;

	/* byte offset----to adjust the starting address of the data buffer,
	   should be multiple of the burst length. */
	byte_offset =
		((u32) CPHYSADDR((u32) dataptr)) % (g_log_chan[chan_no].burst_len *
											4);
#ifndef	CONFIG_MIPS_UNCACHED
	dma_cache_wback((unsigned long) dataptr, len);
	wmb();
#endif							// CONFIG_MIPS_UNCACHED

	tx_desc_p->Data_Pointer = (u32) CPHYSADDR((u32) dataptr) - byte_offset;
	wmb();
	tx_desc_p->status.word = DMA_DESC_OWN_DMA
		| DMA_DESC_SOP_SET
		| DMA_DESC_EOP_SET | (byte_offset << DMA_DESC_BYTEOFF_SHIFT)
		| len;
	wmb();
	if (is_channel_open(chan_no) == 0) {
		// turn on if necessary
		open_channel(chan_no);
	}
#ifdef DMA_NO_POLLING
	if ((AMAZON_DMA_REG32
		 (AMAZON_DMA_CH0_ISR +
		  chan_no * AMAZON_DMA_CH_STEP) & (DMA_ISR_DURR | DMA_ISR_CPT)) ==
		(DMA_ISR_DURR)) {
		// clear DURR if (CPT is AND set and DURR is set)
		AMAZON_DMA_REG32(AMAZON_DMA_CH0_ISR +
						 chan_no * AMAZON_DMA_CH_STEP) = DMA_ISR_DURR;
	}
#endif

	if (current_desc == (g_log_chan[chan_no].desc_len - 1)) {
		current_desc = 0;
	} else {
		current_desc++;
	}


	g_log_chan[chan_no].current_desc = current_desc;
	tx_desc_p =
		(struct tx_desc *) (g_desc_list +
							g_log_chan[chan_no].offset_from_base +
							current_desc);
	// 000003:tc.chen if(tx_desc_p->status.field.OWN==DMA_OWN){
	if (tx_desc_p->status.field.OWN == DMA_OWN || tx_desc_p->status.field.C == 1) {	// 000003:tc.chen
		g_log_chan[chan_no].intr_handler(dma_dev, TX_BUF_FULL_INT);
	}
#ifdef NO_TX_INT
//000004:fchang Start
	cnt++;
	if (cnt == 5) {
		cnt = 0;
		tx_chan_intr_handler(chan_no);
	}
//000004:fchang End
#endif
	local_irq_restore(flag);	// 000004:fchang
	return len;
}



int desc_list_proc_read(char *buf, char **start, off_t offset,
						int count, int *eof, void *data)
{
	int i;
	u32 *p = (u32 *) g_desc_list;
	int len = 0;
	len += sprintf(buf + len, "descriptor list:\n");
	for (i = 0; i < 120; i++) {
		len += sprintf(buf + len, "%d\n", i);
		len += sprintf(buf + len, "%08x\n", *(p + i * 2 + 1));
		len += sprintf(buf + len, "%08x\n", *(p + i * 2));

	}

	return len;

}

int channel_weight_proc_read(char *buf, char **start, off_t offset,
							 int count, int *eof, void *data)
{

	// int i=0;
	int len = 0;
	len += sprintf(buf + len, "Qos dma channel weight list\n");
	len +=
		sprintf(buf + len,
				"channel_num default_weight current_weight device Tx/Rx\n");
	len +=
		sprintf(buf + len,
				"     0      %08x        %08x      Switch   Rx0\n",
				g_log_chan[0].default_weight, g_log_chan[0].weight);
	len +=
		sprintf(buf + len,
				"     1      %08x        %08x      Switch   Rx1\n",
				g_log_chan[1].default_weight, g_log_chan[1].weight);
	len +=
		sprintf(buf + len,
				"     2      %08x        %08x      Switch   Rx2\n",
				g_log_chan[2].default_weight, g_log_chan[2].weight);
	len +=
		sprintf(buf + len,
				"     3      %08x        %08x      Switch   Rx3\n",
				g_log_chan[3].default_weight, g_log_chan[3].weight);
	len +=
		sprintf(buf + len,
				"     4      %08x        %08x      Switch   Tx0\n",
				g_log_chan[4].default_weight, g_log_chan[4].weight);
	len +=
		sprintf(buf + len,
				"     5      %08x        %08x      Switch   Tx1\n",
				g_log_chan[5].default_weight, g_log_chan[5].weight);
	/* 
	   len+=sprintf(buf+len," 6 %08x %08x TPE
	   Rx0\n",g_log_chan[6].default_weight, g_log_chan[6].weight);
	   len+=sprintf(buf+len," 7 %08x %08x TPE
	   Rx0\n",g_log_chan[7].default_weight, g_log_chan[7].weight);
	   len+=sprintf(buf+len," 8 %08x %08x TPE
	   Tx0\n",g_log_chan[8].default_weight, g_log_chan[8].weight);
	   len+=sprintf(buf+len," 9 %08x %08x TPE
	   Rx0\n",g_log_chan[9].default_weight, g_log_chan[9].weight);
	   len+=sprintf(buf+len," 10 %08x %08x DPLUS
	   Rx0\n",g_log_chan[10].default_weight, g_log_chan[10].weight);
	   len+=sprintf(buf+len," 11 %08x %08x DPLUS
	   Rx0\n",g_log_chan[11].default_weight, g_log_chan[11].weight); */
	return len;
}

int dma_register_proc_read(char *buf, char **start, off_t offset,
						   int count, int *eof, void *data)
{
	dev_list *temp_dev;
	int len = 0;;

	len += sprintf(buf + len, "amazon dma driver\n");
	len += sprintf(buf + len, "version 1.0\n");
	len += sprintf(buf + len, "devices registered:\n");
	for (temp_dev = g_head_dev; temp_dev; temp_dev = temp_dev->next) {
		len += sprintf(buf + len, "%s ", temp_dev->dev->device_name);
	}
	len += sprintf(buf + len, "\n");
	len += sprintf(buf + len, "CH_ON=%08x\n", AMAZON_DMA_REG32(AMAZON_DMA_CH_ON));
	len += sprintf(buf + len, "CH_RST=%08x\n", AMAZON_DMA_REG32(AMAZON_DMA_CH_RST));
	len += sprintf(buf + len, "CH0_ISR=%08x\n",	AMAZON_DMA_REG32(AMAZON_DMA_CH0_ISR));
	len += sprintf(buf + len, "CH1_ISR=%08x\n",	AMAZON_DMA_REG32(AMAZON_DMA_CH1_ISR));
	len += sprintf(buf + len, "CH2_ISR=%08x\n",	AMAZON_DMA_REG32(AMAZON_DMA_CH2_ISR));
	len += sprintf(buf + len, "CH3_ISR=%08x\n",	AMAZON_DMA_REG32(AMAZON_DMA_CH3_ISR));
	len += sprintf(buf + len, "CH4_ISR=%08x\n",	AMAZON_DMA_REG32(AMAZON_DMA_CH4_ISR));
	len += sprintf(buf + len, "CH5_ISR=%08x\n",	AMAZON_DMA_REG32(AMAZON_DMA_CH5_ISR));
	len += sprintf(buf + len, "CH6_ISR=%08x\n",	AMAZON_DMA_REG32(AMAZON_DMA_CH6_ISR));
	len += sprintf(buf + len, "CH7_ISR=%08x\n", AMAZON_DMA_REG32(AMAZON_DMA_CH7_ISR));
	len +=		sprintf(buf + len, "CH8_ISR=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH8_ISR));
	len +=
		sprintf(buf + len, "CH9_ISR=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH9_ISR));
	len +=
		sprintf(buf + len, "CH10_ISR=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH10_ISR));
	len +=
		sprintf(buf + len, "CH11_ISR=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH11_ISR));
	len +=
		sprintf(buf + len, "LCH0_MSK=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH0_MSK));
	len +=
		sprintf(buf + len, "LCH1_MSK=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH1_MSK));
	len +=
		sprintf(buf + len, "LCH2_MSK=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH2_MSK));
	len +=
		sprintf(buf + len, "LCH3_MSK=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH3_MSK));
	len +=
		sprintf(buf + len, "LCH4_MSK=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH4_MSK));
	len +=
		sprintf(buf + len, "LCH5_MSK=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH5_MSK));
	len +=
		sprintf(buf + len, "LCH6_MSK=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH6_MSK));
	len +=
		sprintf(buf + len, "LCH7_MSK=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH7_MSK));
	len +=
		sprintf(buf + len, "LCH8_MSK=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH8_MSK));
	len +=
		sprintf(buf + len, "LCH9_MSK=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH9_MSK));
	len +=
		sprintf(buf + len, "LCH10_MSK=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH10_MSK));
	len +=
		sprintf(buf + len, "LCH11_MSK=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH11_MSK));
	len +=
		sprintf(buf + len, "Desc_BA=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_Desc_BA));
	len +=
		sprintf(buf + len, "LCH0_DES_LEN=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH0_DES_LEN));
	len +=
		sprintf(buf + len, "LCH1_DES_LEN=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH1_DES_LEN));
	len +=
		sprintf(buf + len, "LCH2_DES_LEN=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH2_DES_LEN));
	len +=
		sprintf(buf + len, "LCH3_DES_LEN=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH3_DES_LEN));
	len +=
		sprintf(buf + len, "LCH4_DES_LEN=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH4_DES_LEN));
	len +=
		sprintf(buf + len, "LCH5_DES_LEN=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH5_DES_LEN));
	len +=
		sprintf(buf + len, "LCH6_DES_LEN=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH6_DES_LEN));
	len +=
		sprintf(buf + len, "LCH7_DES_LEN=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH7_DES_LEN));
	len +=
		sprintf(buf + len, "LCH8_DES_LEN=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH8_DES_LEN));
	len +=
		sprintf(buf + len, "LCH9_DES_LEN=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH9_DES_LEN));
	len +=
		sprintf(buf + len, "LCH10_DES_LEN=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH10_DES_LEN));
	len +=
		sprintf(buf + len, "LCH11_DES_LEN=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH11_DES_LEN));
	len +=
		sprintf(buf + len, "LCH1_DES_OFST=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH1_DES_OFST));
	len +=
		sprintf(buf + len, "LCH2_DES_OFST=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH2_DES_OFST));
	len +=
		sprintf(buf + len, "LCH3_DES_OFST=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH3_DES_OFST));
	len +=
		sprintf(buf + len, "LCH4_DES_OFST=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH4_DES_OFST));
	len +=
		sprintf(buf + len, "LCH5_DES_OFST=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH5_DES_OFST));
	len +=
		sprintf(buf + len, "LCH6_DES_OFST=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH6_DES_OFST));
	len +=
		sprintf(buf + len, "LCH7_DES_OFST=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH7_DES_OFST));
	len +=
		sprintf(buf + len, "LCH8_DES_OFST=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH8_DES_OFST));
	len +=
		sprintf(buf + len, "LCH9_DES_OFST=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH9_DES_OFST));
	len +=
		sprintf(buf + len, "LCH10_DES_OFST=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH10_DES_OFST));
	len +=
		sprintf(buf + len, "LCH11_DES_OFST=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH11_DES_OFST));
	len +=
		sprintf(buf + len, "AMAZON_DMA_SW_BL=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_SW_BL));
	len +=
		sprintf(buf + len, "AMAZON_DMA_TPE_BL=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_TPE_BL));
	len +=
		sprintf(buf + len, "DPlus2FPI_BL=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_DPlus2FPI_BL));
	len +=
		sprintf(buf + len, "GRX_BUF_LEN=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_GRX_BUF_LEN));
	len +=
		sprintf(buf + len, "DMA_ECON_REG=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_DMA_ECON_REG));
	len +=
		sprintf(buf + len, "POLLING_REG=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_POLLING_REG));
	len +=
		sprintf(buf + len, "CH_WGT=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_CH_WGT));
	len +=
		sprintf(buf + len, "TX_WGT=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_TX_WGT));
	len +=
		sprintf(buf + len, "DPlus2FPI_CLASS=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_DPLus2FPI_CLASS));
	len +=
		sprintf(buf + len, "COMB_ISR=%08x\n",
				AMAZON_DMA_REG32(AMAZON_DMA_COMB_ISR));
#ifdef AMAZON_DMA_TPE_AAL5_RECOVERY
	len += sprintf(buf + len, "TPE fails:%u\n", total_dma_tpe_reset);	// 000004:fchang
#endif
	return len;
}

/*	Brief:	initialize DMA registers
 *	Description:
 */
static void dma_chip_init(void)
{
	int i;
	for (i = 0; i < CHAN_TOTAL_NUM; i++) {
		AMAZON_DMA_REG32(AMAZON_DMA_CH1_DES_OFST +
						 i * AMAZON_DMA_CH_STEP) = DEFAULT_OFFSET;
	}
#ifdef DMA_NO_POLLING
	AMAZON_DMA_REG32(AMAZON_DMA_POLLING_REG) = 0;
#else
	// enable poll mode and set polling counter
	AMAZON_DMA_REG32(AMAZON_DMA_POLLING_REG) = DMA_POLLING_CNT | DMA_POLLING_ENABLE;
#endif
	// to enable DMA drop
	AMAZON_DMA_REG32(AMAZON_DMA_GRX_BUF_LEN) = 0x10000;
}

int insert_dev_list(dev_list * dev)
{
	dev_list *temp_dev;
	if (g_head_dev == NULL) {
		g_head_dev = dev;
		g_tail_dev = dev;
		dev->prev = NULL;
		dev->next = NULL;
	} else {
		for (temp_dev = g_head_dev; temp_dev; temp_dev = temp_dev->next) {
			if (temp_dev->weight < dev->weight) {
				if (temp_dev->prev)
					temp_dev->prev->next = dev;

				dev->prev = temp_dev->prev;
				dev->next = temp_dev;
				temp_dev->prev = dev;
				if (temp_dev == g_head_dev)
					g_head_dev = dev;
				break;
			}
		}

		if (!temp_dev) {
			g_tail_dev->next = dev;
			dev->prev = g_tail_dev;
			dev->next = NULL;
			g_tail_dev = dev;
		}

	}

	return 1;
}

u8 *common_buffer_alloc(int len, int *byte_offset, void **opt)
{
	u8 *buffer = (u8 *) kmalloc(len * sizeof(u8), GFP_KERNEL);
	*byte_offset = 0;
	return buffer;

}

int common_buffer_free(u8 * dataptr, void *opt)
{
	if (dataptr)
		kfree(dataptr);
	return 0;
}


int register_dev(struct dma_device_info *dma_dev)
{
	int i, j, temp;
	int burst_reg = 0;
	u8 *buffer;
	void *p = NULL;
	int byte_offset = 0;

	struct rx_desc *rx_desc_p;
	struct tx_desc *tx_desc_p;
	if (strcmp(dma_dev->device_name, "switch1") == 0) {
		AMAZON_DMA_REG32(AMAZON_DMA_CH_RST) = SWITCH1_RST_MASK;	// resest
																// channel 
																// 1st 
		AMAZON_DMA_REG32(AMAZON_DMA_DMA_ECON_REG) |= 0x3;	// endian
															// conversion
															// for Switch
		burst_reg = AMAZON_DMA_SW_BL;
		dma_dev->logic_rx_chan_base = switch_rx_chan_base;
		dma_dev->logic_tx_chan_base = switch_tx_chan_base;
	}

	else if (strcmp(dma_dev->device_name, "switch2") == 0) {
		AMAZON_DMA_REG32(AMAZON_DMA_CH_RST) = SWITCH2_RST_MASK;	// resest
																// channel 
																// 1st
		AMAZON_DMA_REG32(AMAZON_DMA_DMA_ECON_REG) |= 0x3;	// endian
															// conversion
															// for Switch
		burst_reg = AMAZON_DMA_SW_BL;
		dma_dev->logic_rx_chan_base = switch2_rx_chan_base;
		dma_dev->logic_tx_chan_base = switch2_tx_chan_base;

	} else if (strcmp(dma_dev->device_name, "TPE") == 0) {
		AMAZON_DMA_REG32(AMAZON_DMA_CH_RST) = TPE_RST_MASK;	// resest
															// channel 1st 
															// 
		burst_reg = AMAZON_DMA_TPE_BL;
		dma_dev->logic_rx_chan_base = TPE_rx_chan_base;
		dma_dev->logic_tx_chan_base = TPE_tx_chan_base;
	}

	else if (strcmp(dma_dev->device_name, "DPlus") == 0) {
		AMAZON_DMA_REG32(AMAZON_DMA_CH_RST) = DPlus2FPI_RST_MASK;	// resest 
																	// channel 
																	// 1st
		dma_dev->logic_rx_chan_base = DPLus2FPI_rx_chan_base;
		dma_dev->logic_tx_chan_base = DPLus2FPI_tx_chan_base;

	}

	i = 0;
	for (temp = dma_dev->tx_burst_len; temp > 2; temp /= 2) {
		i += 1;
	}


	AMAZON_DMA_REG32(burst_reg) = i << 1;
	i = 0;
	for (temp = dma_dev->rx_burst_len; temp > 2; temp /= 2) {
		i += 1;
	}
	AMAZON_DMA_REG32(burst_reg) += i;

	for (i = 0; i < dma_dev->num_rx_chan; i++) {

		temp = dma_dev->logic_rx_chan_base + i;
		g_log_chan[temp].dma_dev = dma_dev;
		g_log_chan[temp].weight = dma_dev->rx_chan[i].weight;
		g_log_chan[temp].default_weight = dma_dev->rx_chan[i].weight;
		g_log_chan[temp].current_desc = 0;
		g_log_chan[temp].desc_ofst = DEFAULT_OFFSET;
		g_log_chan[temp].desc_len = dma_dev->rx_chan[i].desc_num;
		g_log_chan[temp].offset_from_base = temp * DEFAULT_OFFSET;
		g_log_chan[temp].packet_size = dma_dev->rx_chan[i].packet_size;

		AMAZON_DMA_REG32(AMAZON_DMA_CH0_DES_LEN + temp * AMAZON_DMA_CH_STEP) = dma_dev->rx_chan[i].desc_num;
		// enable interrupt mask
		if (temp == 4 || temp == 5) {
			AMAZON_DMA_REG32(AMAZON_DMA_CH0_MSK + temp * AMAZON_DMA_CH_STEP) = 0x32;
		} else {
			AMAZON_DMA_REG32(AMAZON_DMA_CH0_MSK + temp * AMAZON_DMA_CH_STEP) = 0x36;
		}
		strcpy(g_log_chan[temp].device_name, dma_dev->device_name);
		g_log_chan[temp].burst_len = dma_dev->rx_burst_len;
		g_log_chan[temp].control = dma_dev->rx_chan[i].control;


		/* specify the buffer allocation and free method */
		if (dma_dev->buffer_alloc)
			g_log_chan[temp].buffer_alloc = dma_dev->buffer_alloc;
		else
			g_log_chan[temp].buffer_alloc = common_buffer_alloc;

		if (dma_dev->buffer_free)
			g_log_chan[temp].buffer_free = dma_dev->buffer_free;
		else
			g_log_chan[temp].buffer_free = common_buffer_free;

		if (dma_dev->intr_handler)
			g_log_chan[temp].intr_handler = dma_dev->intr_handler;
		else
			g_log_chan[temp].intr_handler = NULL;

		for (j = 0; j < g_log_chan[temp].desc_len; j++) {
			rx_desc_p = (struct rx_desc *) (g_desc_list + g_log_chan[temp].offset_from_base + j);
			rx_desc_p->status.word = 0;
			rx_desc_p->status.field.data_length = g_log_chan[temp].packet_size;
			buffer = (u8 *) g_log_chan[temp].buffer_alloc(g_log_chan[temp].packet_size, &byte_offset, &p);
			rx_desc_p->Data_Pointer = (u32) CPHYSADDR((u32) buffer);
			rx_desc_p->status.field.byte_offset = byte_offset;
			/* fix me, should check if the addresss comply with the burst
			   lenght requirment */
			g_log_chan[temp].opt[j] = p;
			rx_desc_p->status.field.OWN = DMA_OWN;

		}
		/* open or close the channel */
		if (g_log_chan[temp].control)
			open_channel(temp);
		else
			close_channel(temp);
	}

	for (i = 0; i < dma_dev->num_tx_chan; i++) {
		temp = dma_dev->logic_tx_chan_base + i;
		g_log_chan[temp].dma_dev = dma_dev;
		g_log_chan[temp].weight = dma_dev->tx_chan[i].weight;
		g_log_chan[temp].default_weight = dma_dev->tx_chan[i].weight;
		g_log_chan[temp].current_desc = 0;
		g_log_chan[temp].desc_ofst = DEFAULT_OFFSET;
		g_log_chan[temp].desc_len = dma_dev->tx_chan[i].desc_num;
		g_log_chan[temp].offset_from_base = temp * DEFAULT_OFFSET;
		g_log_chan[temp].packet_size = dma_dev->tx_chan[i].packet_size;

		AMAZON_DMA_REG32(AMAZON_DMA_CH0_DES_LEN + temp * AMAZON_DMA_CH_STEP) = dma_dev->tx_chan[i].desc_num;
		// enable interrupt mask
#ifdef NO_TX_INT
		AMAZON_DMA_REG32(AMAZON_DMA_CH0_MSK + temp * AMAZON_DMA_CH_STEP) = 0x3e;
#else
		AMAZON_DMA_REG32(AMAZON_DMA_CH0_MSK + temp * AMAZON_DMA_CH_STEP) = 0x36;
#endif

		strcpy(g_log_chan[temp].device_name, dma_dev->device_name);
		g_log_chan[temp].burst_len = dma_dev->tx_burst_len;
		g_log_chan[temp].control = dma_dev->tx_chan[i].control;

		if (dma_dev->buffer_alloc)
			g_log_chan[temp].buffer_alloc = dma_dev->buffer_alloc;
		else
			g_log_chan[temp].buffer_alloc = common_buffer_alloc;

		if (dma_dev->buffer_free)
			g_log_chan[temp].buffer_free = dma_dev->buffer_free;
		else
			g_log_chan[temp].buffer_free = common_buffer_free;

		if (dma_dev->intr_handler)
			g_log_chan[temp].intr_handler = dma_dev->intr_handler;
		else
			g_log_chan[temp].intr_handler = NULL;

		for (j = 0; j < g_log_chan[temp].desc_len; j++) {

			tx_desc_p =
				(struct tx_desc *) (g_desc_list +
									g_log_chan[temp].offset_from_base + j);
			tx_desc_p->status.word = 0;
			tx_desc_p->status.field.data_length =
				g_log_chan[temp].packet_size;
			tx_desc_p->status.field.OWN = CPU_OWN;

		}
		/* workaround DMA pitfall, we never turn on channel if we don't
		   have proper descriptors */
		if (!g_log_chan[temp].control) {
			close_channel(temp);
		}

	}

	return 0;
}

int dma_device_register(struct dma_device_info *dma_dev)
{
	dev_list *temp_dev;
	temp_dev = (dev_list *) kmalloc(sizeof(dev_list), GFP_KERNEL);
	temp_dev->dev = dma_dev;
	temp_dev->weight = dma_dev->weight;
	insert_dev_list(temp_dev);
	/* check whether this is a known device */
	if ((strcmp(dma_dev->device_name, "switch1") == 0)
		|| (strcmp(dma_dev->device_name, "TPE") == 0)
		|| (strcmp(dma_dev->device_name, "switch2") == 0)
		|| (strcmp(dma_dev->device_name, "DPlus") == 0)) {
		register_dev(dma_dev);
	}

	return 0;
}


int unregister_dev(struct dma_device_info *dma_dev)
{
	int i, j, temp;
	u8 *buffer;
	struct rx_desc *rx_desc_p;

	for (i = 0; i < dma_dev->num_rx_chan; i++) {
		temp = dma_dev->logic_rx_chan_base + i;
		close_channel(temp);
		for (j = 0; j < g_log_chan[temp].desc_len; j++) {
			rx_desc_p =
				(struct rx_desc *) (g_desc_list +
									g_log_chan[temp].offset_from_base + j);
			buffer = (u8 *) __va(rx_desc_p->Data_Pointer);
			g_log_chan[temp].buffer_free(buffer, g_log_chan[temp].opt[j]);
		}
	}
	for (i = 0; i < dma_dev->num_tx_chan; i++) {
		temp = dma_dev->logic_tx_chan_base + i;
		close_channel(temp);
	}
	return 0;
}

int dma_device_unregister(struct dma_device_info *dev)
{
	dev_list *temp_dev;
	for (temp_dev = g_head_dev; temp_dev; temp_dev = temp_dev->next) {
		if (strcmp(dev->device_name, temp_dev->dev->device_name) == 0) {
			if ((strcmp(dev->device_name, "switch1") == 0)
				|| (strcmp(dev->device_name, "TPE") == 0)
				|| (strcmp(dev->device_name, "switch2") == 0)
				|| (strcmp(dev->device_name, "DPlus") == 0))
				unregister_dev(dev);
			if (temp_dev == g_head_dev) {
				g_head_dev = temp_dev->next;
				kfree(temp_dev);
			} else {
				if (temp_dev == g_tail_dev)
					g_tail_dev = temp_dev->prev;
				if (temp_dev->prev)
					temp_dev->prev->next = temp_dev->next;
				if (temp_dev->next)
					temp_dev->next->prev = temp_dev->prev;
				kfree(temp_dev);
			}
			break;
		}

	}
	return 0;
}

void dma_device_update_rx(struct dma_device_info *dma_dev)
{
	int i, temp;
	for (i = 0; i < dma_dev->num_rx_chan; i++) {
		temp = dma_dev->logic_rx_chan_base + i;
		g_log_chan[temp].control = dma_dev->rx_chan[i].control;

		if (g_log_chan[temp].control)
			open_channel(temp);
		else
			close_channel(temp);
	}

}

void dma_device_update_tx(struct dma_device_info *dma_dev)
{
	int i, temp;
	for (i = 0; i < dma_dev->num_tx_chan; i++) {
		temp = dma_dev->logic_tx_chan_base + i;
		g_log_chan[temp].control = dma_dev->tx_chan[i].control;
		if (g_log_chan[temp].control) {
			/* we turn on channel when send out the very first packet */
			// open_channel(temp);
		} else
			close_channel(temp);
	}
}

int dma_device_update(struct dma_device_info *dma_dev)
{
	dma_device_update_rx(dma_dev);
	dma_device_update_tx(dma_dev);
	return 0;
}

static int dma_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int dma_release(struct inode *inode, struct file *file)
{
	/* release the resources */
	return 0;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))
static long dma_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
#else
static int dma_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
#endif
{
	int value = 0;
	int chan_no = 0;

	switch (cmd) {
	case 0:					/* get register value */
		break;
	case 1:					/* return channel weight */
		chan_no = *((int *) arg);
		*((int *) arg + 1) = g_log_chan[chan_no].default_weight;
		break;
	case 2:					/* set channel weight */
		chan_no = *((int *) arg);
		value = *((int *) arg + 1);
		printk("new weight=%08x\n", value);
		g_log_chan[chan_no].default_weight = value;
		break;
	default:
		break;
	}
	return 0;
}


static struct file_operations dma_fops = {
  owner:THIS_MODULE,
  open:dma_open,
  release:dma_release,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))
  unlocked_ioctl:dma_ioctl,
#else
  ioctl:dma_ioctl,
#endif
};

static int dma_init(void)
{
	int result = 0;
	int i;
	printk("initialising dma core\n");
	result = register_chrdev(DMA_MAJOR, "dma-core", &dma_fops);
	if (result) {
		AMAZON_DMA_EMSG("cannot register device dma-core!\n");
		return result;
	}
	result = request_irq(AMAZON_DMA_INT, dma_interrupt, IRQF_DISABLED, "dma-core", (void *) &dma_interrupt);
	if (result) {
		AMAZON_DMA_EMSG("error, cannot get dma_irq!\n");
		free_irq(AMAZON_DMA_INT, (void *) &dma_interrupt);
		return -EFAULT;
	}

	g_desc_list = (u64 *) KSEG1ADDR(__get_free_page(GFP_DMA));

	if (g_desc_list == NULL) {
		AMAZON_DMA_EMSG("no memory for desriptor\n");
		return -ENOMEM;
	}
	memset(g_desc_list, 0, PAGE_SIZE);
	AMAZON_DMA_REG32(AMAZON_DMA_Desc_BA) = (u32) CPHYSADDR((u32) g_desc_list);
	g_amazon_dma_dir = proc_mkdir("amazon_dma", NULL);
	create_proc_read_entry("dma_register", 0, g_amazon_dma_dir, dma_register_proc_read, NULL);
	create_proc_read_entry("g_desc_list", 0, g_amazon_dma_dir, desc_list_proc_read, NULL);
	create_proc_read_entry("channel_weight", 0, g_amazon_dma_dir, channel_weight_proc_read, NULL);

	dma_chip_init();
	for (i = 0; i < (RX_CHAN_NUM + 1); i++) {
		rx_chan_list[i] = -1;
	}
	for (i = 0; i < (TX_CHAN_NUM + 1); i++) {
		tx_chan_list[i] = -1;
	}

	for (i = 0; i < CHAN_TOTAL_NUM; i++) {
		comb_isr_mask[i] = 0x80000000 >> (i);
	}

	g_log_chan[CHAN_TOTAL_NUM].weight = 0;
	printk("initialising dma core ... done\n");

	return 0;
}

arch_initcall(dma_init);


void dma_cleanup(void)
{
	dev_list *temp_dev;

	unregister_chrdev(DMA_MAJOR, "dma-core");
	for (temp_dev = g_head_dev; temp_dev; temp_dev = temp_dev->next) {
		kfree(temp_dev);
	}
	free_page(KSEG0ADDR((unsigned long) g_desc_list));
	remove_proc_entry("channel_weight", g_amazon_dma_dir);
	remove_proc_entry("dma_list", g_amazon_dma_dir);
	remove_proc_entry("dma_register", g_amazon_dma_dir);
	remove_proc_entry("amazon_dma", NULL);
	/* release the resources */
	free_irq(AMAZON_DMA_INT, (void *) &dma_interrupt);
}

EXPORT_SYMBOL(dma_device_register);
EXPORT_SYMBOL(dma_device_unregister);
EXPORT_SYMBOL(dma_device_read);
EXPORT_SYMBOL(dma_device_write);
EXPORT_SYMBOL(dma_device_update);
EXPORT_SYMBOL(dma_device_update_rx);

MODULE_LICENSE("GPL");
