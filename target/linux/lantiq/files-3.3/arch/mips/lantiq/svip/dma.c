/*
 ** Copyright (C) 2005 Wu Qi Ming <Qi-Ming.Wu@infineon.com>
 **
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 2 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
/*
 * Description:
 * Driver for SVIP DMA
 * Author:	Wu Qi Ming[Qi-Ming.Wu@infineon.com]
 * Created:	26-September-2005
 */

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
#include <linux/errno.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/semaphore.h>

#include <base_reg.h>
#include <mps_reg.h>
#include <dma_reg.h>
#include <svip_dma.h>
#include <lantiq_soc.h>
#include <irq.h>
#include <sys1_reg.h>

static struct svip_reg_sys1 *const sys1 = (struct svip_reg_sys1 *)LTQ_SYS1_BASE;
static struct svip_reg_dma *const dma = (struct svip_reg_dma *)LTQ_DMA_BASE;
static struct svip_reg_mbs *const mbs = (struct svip_reg_mbs *)LTQ_MBS_BASE;

#define DRV_NAME "ltq_dma"
extern void ltq_mask_and_ack_irq(struct irq_data *data);
extern void ltq_enable_irq(struct irq_data *data);

static inline void mask_and_ack_irq(unsigned int irq_nr)
{
	static int i = 0;
	struct irq_data data;
	data.irq = irq_nr;
	if ((i < 2) && (irq_nr == 137)) {
		printk("eth delay hack\n");
		i++;
	}
	ltq_mask_and_ack_irq(&data);
}

static inline void svip_enable_irq(unsigned int irq_nr)
{
	struct irq_data data;
	data.irq = irq_nr;
	ltq_enable_irq(&data);
}

#define DMA_EMSG(fmt, args...) \
	printk(KERN_ERR  "%s: " fmt, __func__, ## args)

static inline void mbs_grab(void)
{
	while (mbs_r32(mbsr0) != 0);
}

static inline void mbs_release(void)
{
	mbs_w32(0, mbsr0);
	asm("sync");
}

/* max ports connecting to dma */
#define LTQ_MAX_DMA_DEVICE_NUM		ARRAY_SIZE(dma_devices)
/* max dma channels */
#define LTQ_MAX_DMA_CHANNEL_NUM		ARRAY_SIZE(dma_chan)

/* bytes per descriptor */
#define DMA_DESCR_SIZE		8

#define DMA_DESCR_CH_SIZE	(DMA_DESCR_NUM * DMA_DESCR_SIZE)
#define DMA_DESCR_TOTAL_SIZE	(LTQ_MAX_DMA_CHANNEL_NUM * DMA_DESCR_CH_SIZE)
#define DMA_DESCR_MEM_PAGES	((DMA_DESCR_TOTAL_SIZE / PAGE_SIZE) + \
				 (((DMA_DESCR_TOTAL_SIZE % PAGE_SIZE) > 0)))

/* budget for interrupt handling */
#define DMA_INT_BUDGET		100
/* set the correct counter value here! */
#define DMA_POLL_COUNTER	32

struct proc_dir_entry *g_dma_dir;

/* device_name | max_rx_chan_num | max_tx_chan_num | drop_enable */
struct dma_device_info dma_devices[] = {
	{ "SW",    4, 4, 0 },
	{ "DEU",   1, 1, 0 },
	{ "SSC0",  1, 1, 0 },
	{ "SSC1",  1, 1, 0 },
	{ "MCTRL", 1, 1, 0 },
	{ "PCM0",  1, 1, 0 },
	{ "PCM1",  1, 1, 0 },
	{ "PCM2",  1, 1, 0 },
	{ "PCM3",  1, 1, 0 }
};

/*  *dma_dev   | dir  | pri |    irq        | rel_chan_no   */
struct dma_channel_info dma_chan[] = {
	{ &dma_devices[0], DIR_RX, 0, INT_NUM_IM4_IRL0 + 0,  0 },
	{ &dma_devices[0], DIR_TX, 0, INT_NUM_IM4_IRL0 + 1,  0 },
	{ &dma_devices[0], DIR_RX, 1, INT_NUM_IM4_IRL0 + 2,  1 },
	{ &dma_devices[0], DIR_TX, 1, INT_NUM_IM4_IRL0 + 3,  1 },
	{ &dma_devices[0], DIR_RX, 2, INT_NUM_IM4_IRL0 + 4,  2 },
	{ &dma_devices[0], DIR_TX, 2, INT_NUM_IM4_IRL0 + 5,  2 },
	{ &dma_devices[0], DIR_RX, 3, INT_NUM_IM4_IRL0 + 6,  3 },
	{ &dma_devices[0], DIR_TX, 3, INT_NUM_IM4_IRL0 + 7,  3 },
	{ &dma_devices[1], DIR_RX, 0, INT_NUM_IM4_IRL0 + 8,  0 },
	{ &dma_devices[1], DIR_TX, 0, INT_NUM_IM4_IRL0 + 9,  0 },
	{ &dma_devices[2], DIR_RX, 0, INT_NUM_IM4_IRL0 + 10, 0 },
	{ &dma_devices[2], DIR_TX, 0, INT_NUM_IM4_IRL0 + 11, 0 },
	{ &dma_devices[3], DIR_RX, 0, INT_NUM_IM4_IRL0 + 12, 0 },
	{ &dma_devices[3], DIR_TX, 0, INT_NUM_IM4_IRL0 + 13, 0 },
	{ &dma_devices[4], DIR_RX, 0, INT_NUM_IM4_IRL0 + 14, 0 },
	{ &dma_devices[4], DIR_TX, 0, INT_NUM_IM4_IRL0 + 15, 0 },
	{ &dma_devices[5], DIR_RX, 0, INT_NUM_IM4_IRL0 + 16, 0 },
	{ &dma_devices[5], DIR_TX, 0, INT_NUM_IM4_IRL0 + 17, 0 },
	{ &dma_devices[6], DIR_RX, 1, INT_NUM_IM3_IRL0 + 18, 0 },
	{ &dma_devices[6], DIR_TX, 1, INT_NUM_IM3_IRL0 + 19, 0 },
	{ &dma_devices[7], DIR_RX, 2, INT_NUM_IM4_IRL0 + 20, 0 },
	{ &dma_devices[7], DIR_TX, 2, INT_NUM_IM4_IRL0 + 21, 0 },
	{ &dma_devices[8], DIR_RX, 3, INT_NUM_IM4_IRL0 + 22, 0 },
	{ &dma_devices[8], DIR_TX, 3, INT_NUM_IM4_IRL0 + 23, 0 }
};

u64 *g_desc_list[DMA_DESCR_MEM_PAGES];

volatile u32 g_dma_int_status = 0;

/* 0 - not in process, 1 - in process */
volatile int g_dma_in_process;

int ltq_dma_init(void);
void do_dma_tasklet(unsigned long);
DECLARE_TASKLET(dma_tasklet, do_dma_tasklet, 0);
irqreturn_t dma_interrupt(int irq, void *dev_id);

u8 *common_buffer_alloc(int len, int *byte_offset, void **opt)
{
	u8 *buffer = kmalloc(len * sizeof(u8), GFP_KERNEL);
	*byte_offset = 0;
	return buffer;
}

void common_buffer_free(u8 *dataptr, void *opt)
{
	kfree(dataptr);
}

void enable_ch_irq(struct dma_channel_info *ch)
{
	int chan_no = (int)(ch - dma_chan);
	unsigned long flag;
	u32 val;

	if (ch->dir == DIR_RX)
		val = DMA_CIE_DESCPT | DMA_CIE_DUR;
	else
		val = DMA_CIE_DESCPT;

	local_irq_save(flag);
	mbs_grab();
	dma_w32(chan_no, cs);
	dma_w32(val, cie);
	dma_w32_mask(0, 1 << chan_no, irnen);
	mbs_release();
	local_irq_restore(flag);

	svip_enable_irq(ch->irq);
}

void disable_ch_irq(struct dma_channel_info *ch)
{
	unsigned long flag;
	int chan_no = (int)(ch - dma_chan);

	local_irq_save(flag);
	g_dma_int_status &= ~(1 << chan_no);
	mbs_grab();
	dma_w32(chan_no, cs);
	dma_w32(0, cie);
	mbs_release();
	dma_w32_mask(1 << chan_no, 0, irnen);
	local_irq_restore(flag);

	mask_and_ack_irq(ch->irq);
}

int open_chan(struct dma_channel_info *ch)
{
	unsigned long flag;
	int j;
	int chan_no = (int)(ch - dma_chan);
	u8 *buffer;
	int byte_offset;
	struct rx_desc *rx_desc_p;
	struct tx_desc *tx_desc_p;

	if (ch->control == LTQ_DMA_CH_ON)
		return -1;

	if (ch->dir == DIR_RX) {
		for (j = 0; j < ch->desc_len; j++) {
			rx_desc_p = (struct rx_desc *)ch->desc_base+j;
			buffer = ch->dma_dev->buffer_alloc(ch->packet_size,
							   &byte_offset,
							   (void *)&ch->opt[j]);
			if (!buffer)
				return -ENOBUFS;

			rx_desc_p->data_pointer = (u32)CPHYSADDR((u32)buffer);
			rx_desc_p->status.word = 0;
			rx_desc_p->status.field.byte_offset = byte_offset;
			rx_desc_p->status.field.data_length = ch->packet_size;
			rx_desc_p->status.field.own = DMA_OWN;
		}
	} else {
		for (j = 0; j < ch->desc_len; j++) {
			tx_desc_p = (struct tx_desc *)ch->desc_base + j;
			tx_desc_p->data_pointer = 0;
			tx_desc_p->status.word = 0;
		}
	}
	ch->xfer_cnt = 0;

	local_irq_save(flag);
	mbs_grab();
	dma_w32(chan_no, cs);
	dma_w32(ch->desc_len, cdlen);
	dma_w32(0x7e, cis);
	dma_w32(DMA_CCTRL_TXWGT_VAL(ch->tx_weight)
		| DMA_CCTRL_CLASS_VAL(ch->pri)
		| (ch->dir == DIR_RX ? DMA_CCTRL_ON_OFF : 0), cctrl);
	mbs_release();
	ch->control = LTQ_DMA_CH_ON;
	local_irq_restore(flag);

	if (request_irq(ch->irq, dma_interrupt,
			IRQF_DISABLED, "dma-core", (void *)ch) != 0) {
		printk(KERN_ERR "error, cannot get dma_irq!\n");
		return -EFAULT;
	}

	enable_ch_irq(ch);
	return 0;
}

int close_chan(struct dma_channel_info *ch)
{
	unsigned long flag;
	int j;
	int chan_no = (int)(ch - dma_chan);
	struct rx_desc *desc_p;

	if (ch->control == LTQ_DMA_CH_OFF)
		return -1;

	local_irq_save(flag);
	mbs_grab();
	dma_w32(chan_no, cs);
	dma_w32_mask(DMA_CCTRL_ON_OFF, 0, cctrl);
	mbs_release();
	disable_ch_irq(ch);
	free_irq(ch->irq, (void *)ch);
	ch->control = LTQ_DMA_CH_OFF;
	local_irq_restore(flag);

	/* free descriptors in use */
	for (j = 0; j < ch->desc_len; j++) {
		desc_p = (struct rx_desc *)ch->desc_base+j;
		if ((desc_p->status.field.own == CPU_OWN &&
		     desc_p->status.field.c) ||
		    (desc_p->status.field.own == DMA_OWN)) {
			if (desc_p->data_pointer) {
				ch->dma_dev->buffer_free((u8 *)__va(desc_p->data_pointer),
							 (void *)ch->opt[j]);
				desc_p->data_pointer = (u32)NULL;
			}
		}
	}

	return 0;
}

int reset_chan(struct dma_channel_info *ch)
{
	unsigned long flag;
	int val;
	int chan_no = (int)(ch - dma_chan);

	close_chan(ch);

	local_irq_save(flag);
	mbs_grab();
	dma_w32(chan_no, cs);
	dma_w32_mask(0, DMA_CCTRL_RST, cctrl);
	mbs_release();
	local_irq_restore(flag);

	do {
		local_irq_save(flag);
		mbs_grab();
		dma_w32(chan_no, cs);
		val = dma_r32(cctrl);
		mbs_release();
		local_irq_restore(flag);
	} while (val & DMA_CCTRL_RST);

	return 0;
}

static inline void rx_chan_intr_handler(int chan_no)
{
	struct dma_device_info *dma_dev = (struct dma_device_info *)
		dma_chan[chan_no].dma_dev;
	struct dma_channel_info *ch = &dma_chan[chan_no];
	struct rx_desc *rx_desc_p;
	unsigned long flag;
	u32 val;

	local_irq_save(flag);
	mbs_grab();
	dma_w32(chan_no, cs);
	val = dma_r32(cis);
	dma_w32(DMA_CIS_DESCPT, cis);
	mbs_release();

	/* handle command complete interrupt */
	rx_desc_p = (struct rx_desc *)ch->desc_base + ch->curr_desc;
	if ((rx_desc_p->status.word & (DMA_DESC_OWN_DMA | DMA_DESC_CPT_SET)) ==
	    DMA_DESC_CPT_SET) {
		local_irq_restore(flag);
		/* Every thing is correct, then we inform the upper layer */
		dma_dev->current_rx_chan = ch->rel_chan_no;
		if (dma_dev->intr_handler)
			dma_dev->intr_handler(dma_dev, RCV_INT);
		ch->weight--;
	} else {
		g_dma_int_status &= ~(1 << chan_no);
		local_irq_restore(flag);
		svip_enable_irq(dma_chan[chan_no].irq);
	}
}

static inline void tx_chan_intr_handler(int chan_no)
{
	struct dma_device_info *dma_dev = (struct dma_device_info *)
		dma_chan[chan_no].dma_dev;
	struct dma_channel_info *ch = &dma_chan[chan_no];
	struct tx_desc *tx_desc_p;
	unsigned long flag;

	local_irq_save(flag);
	mbs_grab();
	dma_w32(chan_no, cs);
	dma_w32(DMA_CIS_DESCPT, cis);
	mbs_release();

	tx_desc_p = (struct tx_desc *)ch->desc_base+ch->prev_desc;
	if ((tx_desc_p->status.word & (DMA_DESC_OWN_DMA | DMA_DESC_CPT_SET)) ==
	   DMA_DESC_CPT_SET) {
		local_irq_restore(flag);

		dma_dev->buffer_free((u8 *)__va(tx_desc_p->data_pointer),
				  ch->opt[ch->prev_desc]);
		memset(tx_desc_p, 0, sizeof(struct tx_desc));
		dma_dev->current_tx_chan = ch->rel_chan_no;
		if (dma_dev->intr_handler)
			dma_dev->intr_handler(dma_dev, TRANSMIT_CPT_INT);
		ch->weight--;

		ch->prev_desc = (ch->prev_desc + 1) % (ch->desc_len);
	} else {
		g_dma_int_status &= ~(1 << chan_no);
		local_irq_restore(flag);
		svip_enable_irq(dma_chan[chan_no].irq);
	}
}

void do_dma_tasklet(unsigned long unused)
{
	int i;
	int chan_no = 0;
	int budget = DMA_INT_BUDGET;
	int weight = 0;
	unsigned long flag;

	while (g_dma_int_status) {
		if (budget-- < 0) {
			tasklet_schedule(&dma_tasklet);
			return;
		}
		chan_no = -1;
		weight = 0;
		/* WFQ algorithm to select the channel */
		for (i = 0; i < LTQ_MAX_DMA_CHANNEL_NUM; i++) {
			if (g_dma_int_status & (1 << i) &&
			    dma_chan[i].weight > 0) {
				if (dma_chan[i].weight > weight) {
					chan_no = i;
					weight = dma_chan[chan_no].weight;
				}
			}
		}
		if (chan_no >= 0) {
			if (dma_chan[chan_no].dir == DIR_RX)
				rx_chan_intr_handler(chan_no);
			else
				tx_chan_intr_handler(chan_no);
		} else {
			/* reset all the channels */
			for (i = 0; i < LTQ_MAX_DMA_CHANNEL_NUM; i++)
				dma_chan[i].weight = dma_chan[i].default_weight;
		}
	}

	local_irq_save(flag);
	g_dma_in_process = 0;
	if (g_dma_int_status) {
		g_dma_in_process = 1;
		tasklet_schedule(&dma_tasklet);
	}
	local_irq_restore(flag);
}

irqreturn_t dma_interrupt(int irq, void *dev_id)
{
	struct dma_channel_info *ch;
	int chan_no = 0;

	ch = (struct dma_channel_info *)dev_id;
	chan_no = (int)(ch - dma_chan);

	if ((unsigned)chan_no >= LTQ_MAX_DMA_CHANNEL_NUM) {
		printk(KERN_ERR "error: dma_interrupt irq=%d chan_no=%d\n",
		       irq, chan_no);
	}

	g_dma_int_status |= 1 << chan_no;
	dma_w32(1 << chan_no, irncr);
	mask_and_ack_irq(irq);

	if (!g_dma_in_process) {
		g_dma_in_process = 1;
		tasklet_schedule(&dma_tasklet);
	}

	return IRQ_RETVAL(1);
}

struct dma_device_info *dma_device_reserve(char *dev_name)
{
	int i;

	ltq_dma_init();
	for (i = 0; i < LTQ_MAX_DMA_DEVICE_NUM; i++) {
		if (strcmp(dev_name, dma_devices[i].device_name) == 0) {
			if (dma_devices[i].reserved)
				return NULL;
			dma_devices[i].reserved = 1;
			break;
		}
	}

	if (i == LTQ_MAX_DMA_DEVICE_NUM)
		return NULL;

	return &dma_devices[i];
}
EXPORT_SYMBOL(dma_device_reserve);

int dma_device_release(struct dma_device_info *dma_dev)
{
	dma_dev->reserved = 0;

	return 0;
}
EXPORT_SYMBOL(dma_device_release);

int dma_device_register(struct dma_device_info *dma_dev)
{
	int port_no = (int)(dma_dev - dma_devices);
	int txbl, rxbl;
	unsigned long flag;

	switch (dma_dev->tx_burst_len) {
	case 8:
		txbl = 3;
		break;
	case 4:
		txbl = 2;
		break;
	default:
		txbl = 1;
		break;
	}

	switch (dma_dev->rx_burst_len) {
	case 8:
		rxbl = 3;
		break;
	case 4:
		rxbl = 2;
		break;
	default:
		rxbl = 1;
	}

	local_irq_save(flag);
	mbs_grab();
	dma_w32(port_no, ps);
	dma_w32(DMA_PCTRL_TXWGT_VAL(dma_dev->tx_weight)
		| DMA_PCTRL_TXENDI_VAL(dma_dev->tx_endianness_mode)
		| DMA_PCTRL_RXENDI_VAL(dma_dev->rx_endianness_mode)
		| DMA_PCTRL_PDEN_VAL(dma_dev->drop_enable)
		| DMA_PCTRL_TXBL_VAL(txbl)
		| DMA_PCTRL_RXBL_VAL(rxbl), pctrl);
	mbs_release();
	local_irq_restore(flag);

	return 0;
}
EXPORT_SYMBOL(dma_device_register);

int dma_device_unregister(struct dma_device_info *dma_dev)
{
	int i;
	int port_no = (int)(dma_dev - dma_devices);
	unsigned long flag;

	/* flush memcopy module; has no effect for other ports */
	local_irq_save(flag);
	mbs_grab();
	dma_w32(port_no, ps);
	dma_w32_mask(0, DMA_PCTRL_GPC, pctrl);
	mbs_release();
	local_irq_restore(flag);

	for (i = 0; i < dma_dev->max_tx_chan_num; i++)
		reset_chan(dma_dev->tx_chan[i]);

	for (i = 0; i < dma_dev->max_rx_chan_num; i++)
		reset_chan(dma_dev->rx_chan[i]);

	return 0;
}
EXPORT_SYMBOL(dma_device_unregister);

/**
 * Read Packet from DMA Rx channel.
 * The function gets the data from the current rx descriptor assigned
 * to the passed DMA device and passes it back to the caller.
 * The function is called in the context of DMA interrupt.
 * In detail the following actions are done:
 * - get current receive descriptor
 * - allocate memory via allocation callback function
 * - pass data from descriptor to allocated memory
 * - update channel weight
 * - release descriptor
 * - update current descriptor position
 *
 * \param *dma_dev    - pointer to DMA device structure
 * \param **dataptr   - pointer to received data
 * \param **opt
 * \return packet length - length of received data
 * \ingroup Internal
 */
int dma_device_read(struct dma_device_info *dma_dev, u8 **dataptr, void **opt)
{
	u8 *buf;
	int len;
	int byte_offset = 0;
	void *p = NULL;

	struct dma_channel_info *ch =
		dma_dev->rx_chan[dma_dev->current_rx_chan];

	struct rx_desc *rx_desc_p;

	/* get the rx data first */
	rx_desc_p = (struct rx_desc *)ch->desc_base+ch->curr_desc;
	buf = (u8 *)__va(rx_desc_p->data_pointer);
	*(u32 *)dataptr = (u32)buf;
	len = rx_desc_p->status.field.data_length;
#ifndef CONFIG_MIPS_UNCACHED
	dma_cache_inv((unsigned long)buf, len);
#endif
	if (opt)
		*(int *)opt = (int)ch->opt[ch->curr_desc];

	/* replace with a new allocated buffer */
	buf = dma_dev->buffer_alloc(ch->packet_size, &byte_offset, &p);
	if (buf) {
		ch->opt[ch->curr_desc] = p;

		wmb();
		rx_desc_p->data_pointer = (u32)CPHYSADDR((u32)buf);
		rx_desc_p->status.word = (DMA_OWN << 31)  \
					 |(byte_offset << 23) \
					 | ch->packet_size;

		wmb();
	} else {
		*(u32 *)dataptr = 0;
		if (opt)
			*(int *)opt = 0;
	}

	ch->xfer_cnt++;
	/* increase the curr_desc pointer */
	ch->curr_desc++;
	if (ch->curr_desc == ch->desc_len)
		ch->curr_desc = 0;
	/* return the length of the received packet */
	return len;
}
EXPORT_SYMBOL(dma_device_read);

/**
 * Write Packet through DMA Tx channel to peripheral.
 *
 * \param *dma_dev   - pointer to DMA device structure
 * \param *dataptr   - pointer to data to be sent
 * \param  len       - amount of data bytes to be sent
 * \param *opt
 * \return len       - length of transmitted data
 * \ingroup Internal
 */
int dma_device_write(struct dma_device_info *dma_dev, u8 *dataptr, int len,
		     void *opt)
{
	unsigned long flag;
	u32 byte_offset;
	struct dma_channel_info *ch;
	int chan_no;
	struct tx_desc *tx_desc_p;
	local_irq_save(flag);

	ch = dma_dev->tx_chan[dma_dev->current_tx_chan];
	chan_no = (int)(ch - dma_chan);

	if (ch->control == LTQ_DMA_CH_OFF) {
		local_irq_restore(flag);
		printk(KERN_ERR "%s: dma channel %d not enabled!\n",
		       __func__, chan_no);
		return 0;
	}

	tx_desc_p = (struct tx_desc *)ch->desc_base+ch->curr_desc;
	/* Check whether this descriptor is available */
	if (tx_desc_p->status.word & (DMA_DESC_OWN_DMA | DMA_DESC_CPT_SET)) {
		/* if not , the tell the upper layer device */
		dma_dev->intr_handler(dma_dev, TX_BUF_FULL_INT);
		local_irq_restore(flag);
		return 0;
	}
	ch->opt[ch->curr_desc] = opt;
	/* byte offset----to adjust the starting address of the data buffer,
	 * should be multiple of the burst length.*/
	byte_offset = ((u32)CPHYSADDR((u32)dataptr)) %
		(dma_dev->tx_burst_len * 4);
#ifndef CONFIG_MIPS_UNCACHED
	dma_cache_wback((unsigned long)dataptr, len);
	wmb();
#endif
	tx_desc_p->data_pointer = (u32)CPHYSADDR((u32)dataptr) - byte_offset;
	wmb();
	tx_desc_p->status.word = (DMA_OWN << 31)
		| DMA_DESC_SOP_SET
		| DMA_DESC_EOP_SET
		| (byte_offset << 23)
		| len;
	wmb();

	if (ch->xfer_cnt == 0) {
		mbs_grab();
		dma_w32(chan_no, cs);
		dma_w32_mask(0, DMA_CCTRL_ON_OFF, cctrl);
		mbs_release();
	}

	ch->xfer_cnt++;
	ch->curr_desc++;
	if (ch->curr_desc == ch->desc_len)
		ch->curr_desc = 0;

	local_irq_restore(flag);
	return len;
}
EXPORT_SYMBOL(dma_device_write);

/**
 * Display descriptor list via proc file
 *
 * \param chan_no   - logical channel number
 * \ingroup Internal
 */
int desc_list_proc_read(char *buf, char **start, off_t offset,
			int count, int *eof, void *data)
{
	int len = 0;
	int i;
	static int chan_no;
	u32 *p;

	if ((chan_no == 0) && (offset > count)) {
		*eof = 1;
		return 0;
	}

	if (chan_no != 0) {
		*start = buf;
	} else {
		buf = buf + offset;
		*start = buf;
	}

	p = (u32 *)dma_chan[chan_no].desc_base;

	if (dma_chan[chan_no].dir == DIR_RX)
		len += sprintf(buf + len,
			       "channel %d %s Rx descriptor list:\n",
			       chan_no, dma_chan[chan_no].dma_dev->device_name);
	else
		len += sprintf(buf + len,
			       "channel %d %s Tx descriptor list:\n",
			       chan_no, dma_chan[chan_no].dma_dev->device_name);
	len += sprintf(buf + len,
		       " no  address        data pointer command bits "
		       "(Own, Complete, SoP, EoP, Offset) \n");
	len += sprintf(buf + len,
		       "----------------------------------------------"
		       "-----------------------------------\n");
	for (i = 0; i < dma_chan[chan_no].desc_len; i++) {
		len += sprintf(buf + len, "%3d  ", i);
		len += sprintf(buf + len, "0x%08x     ", (u32)(p + (i * 2)));
		len += sprintf(buf + len, "%08x     ", *(p + (i * 2 + 1)));
		len += sprintf(buf + len, "%08x     ", *(p + (i * 2)));

		if (*(p + (i * 2)) & 0x80000000)
			len += sprintf(buf + len, "D ");
		else
			len += sprintf(buf + len, "C ");
		if (*(p + (i * 2)) & 0x40000000)
			len += sprintf(buf + len, "C ");
		else
			len += sprintf(buf + len, "c ");
		if (*(p + (i * 2)) & 0x20000000)
			len += sprintf(buf + len, "S ");
		else
			len += sprintf(buf + len, "s ");
		if (*(p + (i * 2)) & 0x10000000)
			len += sprintf(buf + len, "E ");
		else
			len += sprintf(buf + len, "e ");

		/* byte offset is different for rx and tx descriptors*/
		if (dma_chan[chan_no].dir == DIR_RX) {
			len += sprintf(buf + len, "%01x ",
				       (*(p + (i * 2)) & 0x01800000) >> 23);
		} else {
			len += sprintf(buf + len, "%02x ",
				       (*(p + (i * 2)) & 0x0F800000) >> 23);
		}

		if (dma_chan[chan_no].curr_desc == i)
			len += sprintf(buf + len, "<- CURR");

		if (dma_chan[chan_no].prev_desc == i)
			len += sprintf(buf + len, "<- PREV");

		len += sprintf(buf + len, "\n");

	}

	len += sprintf(buf + len, "\n");
	chan_no++;
	if (chan_no > LTQ_MAX_DMA_CHANNEL_NUM - 1)
		chan_no = 0;

	*eof = 1;
	return len;
}

/**
 * Displays the weight of all DMA channels via proc file
 *
 *
 *
 * \param *buf
 * \param **start
 * \param offset
 * \param count
 * \param *eof
 * \param *data
 * \return len - amount of bytes written to file
 */
int channel_weight_proc_read(char *buf, char **start, off_t offset,
			     int count, int *eof, void *data)
{
	int i;
	int len = 0;
	len += sprintf(buf + len, "Qos dma channel weight list\n");
	len += sprintf(buf + len, "channel_num default_weight "
		       "current_weight    device    Tx/Rx\n");
	len += sprintf(buf + len, "---------------------------"
		       "---------------------------------\n");
	for (i = 0; i < LTQ_MAX_DMA_CHANNEL_NUM; i++) {
		struct dma_channel_info *ch = &dma_chan[i];

		if (ch->dir == DIR_RX) {
			len += sprintf(buf + len,
				       "     %2d      %08x        "
				       "%08x      %10s   Rx\n",
				      i, ch->default_weight, ch->weight,
				      ch->dma_dev->device_name);
		} else {
			len += sprintf(buf + len,
				       "     %2d      %08x        "
				       "%08x      %10s   Tx\n",
				      i, ch->default_weight, ch->weight,
				      ch->dma_dev->device_name);
		}
	}

	return len;
}

/**
 * Provides DMA Register Content to proc file
 * This function reads the content of general DMA Registers, DMA Channel
 * Registers and DMA Port Registers and performs a structures output to the
 * DMA proc file
 *
 * \param *buf
 * \param **start
 * \param offset
 * \param count
 * \param *eof
 * \param *data
 * \return len - amount of bytes written to file
 */
int dma_register_proc_read(char *buf, char **start, off_t offset,
			   int count, int *eof, void *data)
{
	int len = 0;
	int i;
	int limit = count;
	unsigned long flags;
	static int blockcount;
	static int channel_no;

	if ((blockcount == 0) && (offset > count)) {
		*eof = 1;
		return 0;
	}

	switch (blockcount) {
	case 0:
		len += sprintf(buf + len, "\nGeneral DMA Registers\n");
		len += sprintf(buf + len, "-------------------------"
			       "----------------\n");
		len += sprintf(buf + len, "CLC=        %08x\n", dma_r32(clc));
		len += sprintf(buf + len, "ID=         %08x\n", dma_r32(id));
		len += sprintf(buf + len, "DMA_CPOLL=  %08x\n", dma_r32(cpoll));
		len += sprintf(buf + len, "DMA_CS=     %08x\n", dma_r32(cs));
		len += sprintf(buf + len, "DMA_PS=     %08x\n", dma_r32(ps));
		len += sprintf(buf + len, "DMA_IRNEN=  %08x\n", dma_r32(irnen));
		len += sprintf(buf + len, "DMA_IRNCR=  %08x\n", dma_r32(irncr));
		len += sprintf(buf + len, "DMA_IRNICR= %08x\n",
			       dma_r32(irnicr));
		len += sprintf(buf + len, "\nDMA Channel Registers\n");
		blockcount = 1;
		return len;
		break;
	case 1:
		/* If we had an overflow start at beginning of buffer
		 * otherwise use offset */
		if (channel_no != 0) {
			*start = buf;
		} else {
			buf = buf + offset;
			*start = buf;
		}

		local_irq_save(flags);
		for (i = channel_no; i < LTQ_MAX_DMA_CHANNEL_NUM; i++) {
			struct dma_channel_info *ch = &dma_chan[i];

			if (len + 300 > limit) {
				local_irq_restore(flags);
				channel_no = i;
				blockcount = 1;
				return len;
			}
			len += sprintf(buf + len, "----------------------"
				       "-------------------\n");
			if (ch->dir == DIR_RX) {
				len += sprintf(buf + len,
					       "Channel %d - Device %s Rx\n",
					       i, ch->dma_dev->device_name);
			} else {
				len += sprintf(buf + len,
					       "Channel %d - Device %s Tx\n",
					       i, ch->dma_dev->device_name);
			}
			dma_w32(i, cs);
			len += sprintf(buf + len, "DMA_CCTRL=  %08x\n",
				       dma_r32(cctrl));
			len += sprintf(buf + len, "DMA_CDBA=   %08x\n",
				       dma_r32(cdba));
			len += sprintf(buf + len, "DMA_CIE=    %08x\n",
				       dma_r32(cie));
			len += sprintf(buf + len, "DMA_CIS=    %08x\n",
				       dma_r32(cis));
			len += sprintf(buf + len, "DMA_CDLEN=  %08x\n",
				       dma_r32(cdlen));
		}
		local_irq_restore(flags);
		blockcount = 2;
		channel_no = 0;
		return len;
		break;
	case 2:
		*start = buf;
		/*
		 * display port dependent registers
		 */
		len += sprintf(buf + len, "\nDMA Port Registers\n");
		len += sprintf(buf + len,
			       "-----------------------------------------\n");
		local_irq_save(flags);
		for (i = 0; i < LTQ_MAX_DMA_DEVICE_NUM; i++) {
			dma_w32(i, ps);
			len += sprintf(buf + len,
				       "Port %d DMA_PCTRL= %08x\n",
				       i, dma_r32(pctrl));
		}
		local_irq_restore(flags);
		blockcount = 0;
		*eof = 1;
		return len;
		break;
	}

	blockcount = 0;
	*eof = 1;
	return 0;
}

/**
 * Open Method of DMA Device Driver
 * This function increments the device driver's use counter.
 *
 *
 * \param
 * \return
 */
static int dma_open(struct inode *inode, struct file *file)
{
	return 0;
}

/**
 * Release Method of DMA Device driver.
 * This function decrements the device driver's use counter.
 *
 *
 * \param
 * \return
 */
static int dma_release(struct inode *inode, struct file *file)
{
	/* release the resources */
	return 0;
}

/**
 * Ioctl Interface to DMA Module
 *
 * \param  None
 * \return 0  - initialization successful
 *         <0 - failed initialization
 */
static long dma_ioctl(struct file *file,
		     unsigned int cmd, unsigned long arg)
{
	int result = 0;
	/* TODO: add some user controled functions here */
	return result;
}

const static struct file_operations dma_fops = {
	.owner = THIS_MODULE,
	.open = dma_open,
	.release = dma_release,
	.unlocked_ioctl = dma_ioctl,
};

void map_dma_chan(struct dma_channel_info *map)
{
	int i;

	/* assign default values for channel settings */
	for (i = 0; i < LTQ_MAX_DMA_CHANNEL_NUM; i++) {
		dma_chan[i].byte_offset = 0;
		dma_chan[i].open = &open_chan;
		dma_chan[i].close = &close_chan;
		dma_chan[i].reset = &reset_chan;
		dma_chan[i].enable_irq = enable_ch_irq;
		dma_chan[i].disable_irq = disable_ch_irq;
		dma_chan[i].tx_weight = 1;
		dma_chan[i].control = 0;
		dma_chan[i].default_weight  =  LTQ_DMA_CH_DEFAULT_WEIGHT;
		dma_chan[i].weight = dma_chan[i].default_weight;
		dma_chan[i].curr_desc = 0;
		dma_chan[i].prev_desc = 0;
	}

	/* assign default values for port settings */
	for (i = 0; i < LTQ_MAX_DMA_DEVICE_NUM; i++) {
		/*set default tx channel number to be one*/
		dma_devices[i].num_tx_chan = 1;
		/*set default rx channel number to be one*/
		dma_devices[i].num_rx_chan = 1;
		dma_devices[i].buffer_alloc = common_buffer_alloc;
		dma_devices[i].buffer_free = common_buffer_free;
		dma_devices[i].intr_handler = NULL;
		dma_devices[i].tx_burst_len = 4;
		dma_devices[i].rx_burst_len = 4;
#ifdef CONFIG_CPU_LITTLE_ENDIAN
		dma_devices[i].tx_endianness_mode = 0;
		dma_devices[i].rx_endianness_mode = 0;
#else
		dma_devices[i].tx_endianness_mode = 3;
		dma_devices[i].rx_endianness_mode = 3;
#endif
	}
}

void dma_chip_init(void)
{
	int i;

	sys1_w32(SYS1_CLKENR_DMA, clkenr);
	wmb();
	/* reset DMA */
	dma_w32(DMA_CTRL_RST, ctrl);
	wmb();
	/* disable all the interrupts first */
	dma_w32(0, irnen);

	/* enable polling for all channels */
	dma_w32(DMA_CPOLL_EN | DMA_CPOLL_CNT_VAL(DMA_POLL_COUNTER), cpoll);

	/****************************************************/
	for (i = 0; i < LTQ_MAX_DMA_CHANNEL_NUM; i++)
		disable_ch_irq(&dma_chan[i]);
}

int ltq_dma_init(void)
{
	int result = 0;
	int i;
	unsigned long flag;
	static int dma_initialized;

	if (dma_initialized == 1)
		return 0;
	dma_initialized = 1;

	result = register_chrdev(DMA_MAJOR, "dma-core", &dma_fops);
	if (result) {
		DMA_EMSG("cannot register device dma-core!\n");
		return result;
	}

	dma_chip_init();
	map_dma_chan(dma_chan);

	/* allocate DMA memory for buffer descriptors */
	for (i = 0; i < DMA_DESCR_MEM_PAGES; i++) {
		g_desc_list[i] = (u64 *)__get_free_page(GFP_DMA);
		if (g_desc_list[i] == NULL) {
			DMA_EMSG("no memory for desriptor\n");
			return -ENOMEM;
		}
		g_desc_list[i] = (u64 *)KSEG1ADDR(g_desc_list[i]);
		memset(g_desc_list[i], 0, PAGE_SIZE);
	}

	for (i = 0; i < LTQ_MAX_DMA_CHANNEL_NUM; i++) {
		int page_index, ch_per_page;
		/* cross-link relative channels of a port to
		 * corresponding absolute channels */
		if (dma_chan[i].dir == DIR_RX) {
			((struct dma_device_info *)(dma_chan[i].dma_dev))->
				rx_chan[dma_chan[i].rel_chan_no] = &dma_chan[i];
		} else {
			((struct dma_device_info *)(dma_chan[i].dma_dev))->
				tx_chan[dma_chan[i].rel_chan_no] = &dma_chan[i];
		}
		dma_chan[i].abs_chan_no = i;

		page_index = i * DMA_DESCR_CH_SIZE / PAGE_SIZE;
		ch_per_page = PAGE_SIZE / DMA_DESCR_CH_SIZE +
			((PAGE_SIZE % DMA_DESCR_CH_SIZE) > 0);
		dma_chan[i].desc_base =
			(u32)g_desc_list[page_index] +
			(i - page_index*ch_per_page) * DMA_DESCR_NUM*8;
		dma_chan[i].curr_desc = 0;
		dma_chan[i].desc_len = DMA_DESCR_NUM;

		local_irq_save(flag);
		mbs_grab();
		dma_w32(i, cs);
		dma_w32((u32)CPHYSADDR(dma_chan[i].desc_base), cdba);
		mbs_release();
		local_irq_restore(flag);
	}

	g_dma_dir = proc_mkdir("driver/" DRV_NAME, NULL);

	create_proc_read_entry("dma_register",
			       0,
			       g_dma_dir,
			       dma_register_proc_read,
			       NULL);

	create_proc_read_entry("g_desc_list",
			       0,
			       g_dma_dir,
			       desc_list_proc_read,
			       NULL);

	create_proc_read_entry("channel_weight",
			       0,
			       g_dma_dir,
			       channel_weight_proc_read,
			       NULL);

	printk(KERN_NOTICE "SVIP DMA engine initialized\n");

	return 0;
}

/**
 * Cleanup DMA device
 * This function releases all resources used by the DMA device driver on
 * module removal.
 *
 *
 * \param  None
 * \return Nothing
 */
void dma_cleanup(void)
{
	int i;
	unregister_chrdev(DMA_MAJOR, "dma-core");

	for (i = 0; i < DMA_DESCR_MEM_PAGES; i++)
		free_page(KSEG0ADDR((unsigned long)g_desc_list[i]));
	remove_proc_entry("channel_weight", g_dma_dir);
	remove_proc_entry("g_desc_list", g_dma_dir);
	remove_proc_entry("dma_register", g_dma_dir);
	remove_proc_entry("driver/" DRV_NAME, NULL);
	/* release the resources */
	for (i = 0; i < LTQ_MAX_DMA_CHANNEL_NUM; i++)
		free_irq(dma_chan[i].irq, (void *)&dma_chan[i]);
}

arch_initcall(ltq_dma_init);

MODULE_LICENSE("GPL");
