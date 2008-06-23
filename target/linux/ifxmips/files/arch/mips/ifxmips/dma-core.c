#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/fs.h>
#include <linux/errno.h>
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

#include <asm/ifxmips/ifxmips.h>
#include <asm/ifxmips/ifxmips_irq.h>
#include <asm/ifxmips/ifxmips_dma.h>
#include <asm/ifxmips/ifxmips_pmu.h>

/*25 descriptors for each dma channel,4096/8/20=25.xx*/
#define IFXMIPS_DMA_DESCRIPTOR_OFFSET 25

#define MAX_DMA_DEVICE_NUM  6	/*max ports connecting to dma */
#define MAX_DMA_CHANNEL_NUM 20	/*max dma channels */
#define DMA_INT_BUDGET      100	/*budget for interrupt handling */
#define DMA_POLL_COUNTER    4	/*fix me, set the correct counter value here! */

extern void ifxmips_mask_and_ack_irq (unsigned int irq_nr);
extern void ifxmips_enable_irq (unsigned int irq_nr);
extern void ifxmips_disable_irq (unsigned int irq_nr);

u64 *g_desc_list;
_dma_device_info dma_devs[MAX_DMA_DEVICE_NUM];
_dma_channel_info dma_chan[MAX_DMA_CHANNEL_NUM];

char global_device_name[MAX_DMA_DEVICE_NUM][20] =
	{ {"PPE"}, {"DEU"}, {"SPI"}, {"SDIO"}, {"MCTRL0"}, {"MCTRL1"} };

_dma_chan_map default_dma_map[MAX_DMA_CHANNEL_NUM] = {
	{"PPE", IFXMIPS_DMA_RX, 0, IFXMIPS_DMA_CH0_INT, 0},
	{"PPE", IFXMIPS_DMA_TX, 0, IFXMIPS_DMA_CH1_INT, 0},
	{"PPE", IFXMIPS_DMA_RX, 1, IFXMIPS_DMA_CH2_INT, 1},
	{"PPE", IFXMIPS_DMA_TX, 1, IFXMIPS_DMA_CH3_INT, 1},
	{"PPE", IFXMIPS_DMA_RX, 2, IFXMIPS_DMA_CH4_INT, 2},
	{"PPE", IFXMIPS_DMA_TX, 2, IFXMIPS_DMA_CH5_INT, 2},
	{"PPE", IFXMIPS_DMA_RX, 3, IFXMIPS_DMA_CH6_INT, 3},
	{"PPE", IFXMIPS_DMA_TX, 3, IFXMIPS_DMA_CH7_INT, 3},
	{"DEU", IFXMIPS_DMA_RX, 0, IFXMIPS_DMA_CH8_INT, 0},
	{"DEU", IFXMIPS_DMA_TX, 0, IFXMIPS_DMA_CH9_INT, 0},
	{"DEU", IFXMIPS_DMA_RX, 1, IFXMIPS_DMA_CH10_INT, 1},
	{"DEU", IFXMIPS_DMA_TX, 1, IFXMIPS_DMA_CH11_INT, 1},
	{"SPI", IFXMIPS_DMA_RX, 0, IFXMIPS_DMA_CH12_INT, 0},
	{"SPI", IFXMIPS_DMA_TX, 0, IFXMIPS_DMA_CH13_INT, 0},
	{"SDIO", IFXMIPS_DMA_RX, 0, IFXMIPS_DMA_CH14_INT, 0},
	{"SDIO", IFXMIPS_DMA_TX, 0, IFXMIPS_DMA_CH15_INT, 0},
	{"MCTRL0", IFXMIPS_DMA_RX, 0, IFXMIPS_DMA_CH16_INT, 0},
	{"MCTRL0", IFXMIPS_DMA_TX, 0, IFXMIPS_DMA_CH17_INT, 0},
	{"MCTRL1", IFXMIPS_DMA_RX, 1, IFXMIPS_DMA_CH18_INT, 1},
	{"MCTRL1", IFXMIPS_DMA_TX, 1, IFXMIPS_DMA_CH19_INT, 1}
};

_dma_chan_map *chan_map = default_dma_map;
volatile u32 g_ifxmips_dma_int_status = 0;
volatile int g_ifxmips_dma_in_process = 0;/*0=not in process,1=in process*/

void do_dma_tasklet (unsigned long);
DECLARE_TASKLET (dma_tasklet, do_dma_tasklet, 0);

u8*
common_buffer_alloc (int len, int *byte_offset, void **opt)
{
	u8 *buffer = (u8 *) kmalloc (len * sizeof (u8), GFP_KERNEL);

	*byte_offset = 0;

	return buffer;
}

void
common_buffer_free (u8 *dataptr, void *opt)
{
	if (dataptr)
		kfree(dataptr);
}

void
enable_ch_irq (_dma_channel_info *pCh)
{
	int chan_no = (int)(pCh - dma_chan);
	int flag;

	local_irq_save(flag);
	ifxmips_w32(chan_no, IFXMIPS_DMA_CS);
	ifxmips_w32(0x4a, IFXMIPS_DMA_CIE);
	ifxmips_w32(ifxmips_r32(IFXMIPS_DMA_IRNEN) | (1 << chan_no), IFXMIPS_DMA_IRNEN);
	local_irq_restore(flag);
	ifxmips_enable_irq(pCh->irq);
}

void
disable_ch_irq (_dma_channel_info *pCh)
{
	int flag;
	int chan_no = (int) (pCh - dma_chan);

	local_irq_save(flag);
	g_ifxmips_dma_int_status &= ~(1 << chan_no);
	ifxmips_w32(chan_no, IFXMIPS_DMA_CS);
	ifxmips_w32(0, IFXMIPS_DMA_CIE);
	ifxmips_w32(ifxmips_r32(IFXMIPS_DMA_IRNEN) & ~(1 << chan_no), IFXMIPS_DMA_IRNEN);
	local_irq_restore(flag);
	ifxmips_mask_and_ack_irq(pCh->irq);
}

void
open_chan (_dma_channel_info *pCh)
{
	int flag;
	int chan_no = (int)(pCh - dma_chan);

	local_irq_save(flag);
	ifxmips_w32(chan_no, IFXMIPS_DMA_CS);
	ifxmips_w32(ifxmips_r32(IFXMIPS_DMA_CCTRL) | 1, IFXMIPS_DMA_CCTRL);
	if(pCh->dir == IFXMIPS_DMA_RX)
		enable_ch_irq(pCh);
	local_irq_restore(flag);
}

void
close_chan(_dma_channel_info *pCh)
{
	int flag;
	int chan_no = (int) (pCh - dma_chan);

	local_irq_save(flag);
	ifxmips_w32(chan_no, IFXMIPS_DMA_CS);
	ifxmips_w32(ifxmips_r32(IFXMIPS_DMA_CCTRL) & ~1, IFXMIPS_DMA_CCTRL);
	disable_ch_irq(pCh);
	local_irq_restore(flag);
}

void
reset_chan (_dma_channel_info *pCh)
{
	int chan_no = (int) (pCh - dma_chan);

	ifxmips_w32(chan_no, IFXMIPS_DMA_CS);
	ifxmips_w32(ifxmips_r32(IFXMIPS_DMA_CCTRL) | 2, IFXMIPS_DMA_CCTRL);
}

void
rx_chan_intr_handler (int chan_no)
{
	_dma_device_info *pDev = (_dma_device_info *)dma_chan[chan_no].dma_dev;
	_dma_channel_info *pCh = &dma_chan[chan_no];
	struct rx_desc *rx_desc_p;
	int tmp;
	int flag;

	/*handle command complete interrupt */
	rx_desc_p = (struct rx_desc*)pCh->desc_base + pCh->curr_desc;
	if (rx_desc_p->status.field.OWN == CPU_OWN
	    && rx_desc_p->status.field.C
	    && rx_desc_p->status.field.data_length < 1536){
		/*Every thing is correct, then we inform the upper layer */
		pDev->current_rx_chan = pCh->rel_chan_no;
		if(pDev->intr_handler)
			pDev->intr_handler(pDev, RCV_INT);
		pCh->weight--;
	} else {
		local_irq_save(flag);
		tmp = ifxmips_r32(IFXMIPS_DMA_CS);
		ifxmips_w32(chan_no, IFXMIPS_DMA_CS);
		ifxmips_w32(ifxmips_r32(IFXMIPS_DMA_CIS) | 0x7e, IFXMIPS_DMA_CIS);
		ifxmips_w32(tmp, IFXMIPS_DMA_CS);
		g_ifxmips_dma_int_status &= ~(1 << chan_no);
		local_irq_restore(flag);
		ifxmips_enable_irq(dma_chan[chan_no].irq);
	}
}

inline void
tx_chan_intr_handler (int chan_no)
{
	_dma_device_info *pDev = (_dma_device_info*)dma_chan[chan_no].dma_dev;
	_dma_channel_info *pCh = &dma_chan[chan_no];
    int tmp;
    int flag;

    local_irq_save(flag);
    tmp = ifxmips_r32(IFXMIPS_DMA_CS);
    ifxmips_w32(chan_no, IFXMIPS_DMA_CS);
    ifxmips_w32(ifxmips_r32(IFXMIPS_DMA_CIS) | 0x7e, IFXMIPS_DMA_CIS);
    ifxmips_w32(tmp, IFXMIPS_DMA_CS);
    g_ifxmips_dma_int_status &= ~(1 << chan_no);
    local_irq_restore(flag);
	pDev->current_tx_chan = pCh->rel_chan_no;
	if (pDev->intr_handler)
		pDev->intr_handler(pDev, TRANSMIT_CPT_INT);
}

void
do_dma_tasklet (unsigned long unused)
{
	int i;
	int chan_no = 0;
	int budget = DMA_INT_BUDGET;
	int weight = 0;
    int flag;

	while (g_ifxmips_dma_int_status)
	{
		if (budget-- < 0)
		{
			tasklet_schedule(&dma_tasklet);
			return;
		}
		chan_no = -1;
		weight = 0;
		for (i = 0; i < MAX_DMA_CHANNEL_NUM; i++)
		{
			if ((g_ifxmips_dma_int_status & (1 << i)) && dma_chan[i].weight > 0)
			{
				if (dma_chan[i].weight > weight)
				{
					chan_no = i;
                    weight = dma_chan[chan_no].weight;
                }
			}
		}

		if (chan_no >= 0)
		{
			if (chan_map[chan_no].dir == IFXMIPS_DMA_RX)
				rx_chan_intr_handler(chan_no);
			else
				tx_chan_intr_handler(chan_no);
		} else {
			for (i = 0; i < MAX_DMA_CHANNEL_NUM; i++)
			{
				dma_chan[i].weight = dma_chan[i].default_weight;
			}
		}
	}

    local_irq_save(flag);
	g_ifxmips_dma_in_process = 0;
    if (g_ifxmips_dma_int_status)
	{
        g_ifxmips_dma_in_process = 1;
        tasklet_schedule(&dma_tasklet);
    }
    local_irq_restore(flag);
}

irqreturn_t
dma_interrupt (int irq, void *dev_id)
{
	_dma_channel_info *pCh;
	int chan_no = 0;
	int tmp;

	pCh = (_dma_channel_info*)dev_id;
	chan_no = (int)(pCh - dma_chan);
	if (chan_no < 0 || chan_no > 19)
		BUG();

	tmp = ifxmips_r32(IFXMIPS_DMA_IRNEN);
	ifxmips_w32(0, IFXMIPS_DMA_IRNEN);
	g_ifxmips_dma_int_status |= 1 << chan_no;
	ifxmips_w32(tmp, IFXMIPS_DMA_IRNEN);
	ifxmips_mask_and_ack_irq(irq);

    if (!g_ifxmips_dma_in_process)
	{
        g_ifxmips_dma_in_process = 1;
        tasklet_schedule(&dma_tasklet);
    }

	return IRQ_HANDLED;
}

_dma_device_info*
dma_device_reserve (char *dev_name)
{
	int i;

	for (i = 0; i < MAX_DMA_DEVICE_NUM; i++)
	{
		if (strcmp(dev_name, dma_devs[i].device_name) == 0)
		{
			if (dma_devs[i].reserved)
				return NULL;
			dma_devs[i].reserved = 1;
			break;
		}
	}

	return &dma_devs[i];
}

void
dma_device_release (_dma_device_info *dev)
{
	dev->reserved = 0;
}

void
dma_device_register(_dma_device_info *dev)
{
	int i, j;
	int chan_no = 0;
	u8 *buffer;
	int byte_offset;
	int flag;
	_dma_device_info *pDev;
	_dma_channel_info *pCh;
	struct rx_desc *rx_desc_p;
	struct tx_desc *tx_desc_p;

	for (i = 0; i < dev->max_tx_chan_num; i++)
	{
		pCh = dev->tx_chan[i];
		if (pCh->control == IFXMIPS_DMA_CH_ON)
		{
			chan_no = (int)(pCh - dma_chan);
			for (j = 0; j < pCh->desc_len; j++)
			{
				tx_desc_p = (struct tx_desc*)pCh->desc_base + j;
				memset(tx_desc_p, 0, sizeof(struct tx_desc));
			}
			local_irq_save(flag);
			ifxmips_w32(chan_no, IFXMIPS_DMA_CS);
			/*check if the descriptor length is changed */
			if (ifxmips_r32(IFXMIPS_DMA_CDLEN) != pCh->desc_len)
				ifxmips_w32(pCh->desc_len, IFXMIPS_DMA_CDLEN);

			ifxmips_w32(ifxmips_r32(IFXMIPS_DMA_CCTRL) & ~1, IFXMIPS_DMA_CCTRL);
			ifxmips_w32(ifxmips_r32(IFXMIPS_DMA_CCTRL) | 2, IFXMIPS_DMA_CCTRL);
			while (ifxmips_r32(IFXMIPS_DMA_CCTRL) & 2){};
			ifxmips_w32(ifxmips_r32(IFXMIPS_DMA_IRNEN) | (1 << chan_no), IFXMIPS_DMA_IRNEN);
			ifxmips_w32(0x30100, IFXMIPS_DMA_CCTRL);	/*reset and enable channel,enable channel later */
			local_irq_restore(flag);
		}
	}

	for (i = 0; i < dev->max_rx_chan_num; i++)
	{
		pCh = dev->rx_chan[i];
		if (pCh->control == IFXMIPS_DMA_CH_ON)
		{
			chan_no = (int)(pCh - dma_chan);

			for (j = 0; j < pCh->desc_len; j++)
			{
				rx_desc_p = (struct rx_desc*)pCh->desc_base + j;
				pDev = (_dma_device_info*)(pCh->dma_dev);
				buffer = pDev->buffer_alloc(pCh->packet_size, &byte_offset, (void*)&(pCh->opt[j]));
				if (!buffer)
					break;

				dma_cache_inv((unsigned long) buffer, pCh->packet_size);

				rx_desc_p->Data_Pointer = (u32)CPHYSADDR((u32)buffer);
				rx_desc_p->status.word = 0;
				rx_desc_p->status.field.byte_offset = byte_offset;
				rx_desc_p->status.field.OWN = DMA_OWN;
				rx_desc_p->status.field.data_length = pCh->packet_size;
			}

			local_irq_save(flag);
			ifxmips_w32(chan_no, IFXMIPS_DMA_CS);
			/*check if the descriptor length is changed */
			if (ifxmips_r32(IFXMIPS_DMA_CDLEN) != pCh->desc_len)
				ifxmips_w32(pCh->desc_len, IFXMIPS_DMA_CDLEN);
			ifxmips_w32(ifxmips_r32(IFXMIPS_DMA_CCTRL) & ~1, IFXMIPS_DMA_CCTRL);
			ifxmips_w32(ifxmips_r32(IFXMIPS_DMA_CCTRL) | 2, IFXMIPS_DMA_CCTRL);
			while (ifxmips_r32(IFXMIPS_DMA_CCTRL) & 2){};
			ifxmips_w32(0x0a, IFXMIPS_DMA_CIE);	/*fix me, should enable all the interrupts here? */
			ifxmips_w32(ifxmips_r32(IFXMIPS_DMA_IRNEN) | (1 << chan_no), IFXMIPS_DMA_IRNEN);
			ifxmips_w32(0x30000, IFXMIPS_DMA_CCTRL);
			local_irq_restore(flag);
			ifxmips_enable_irq(dma_chan[chan_no].irq);
		}
	}
}

void
dma_device_unregister (_dma_device_info *dev)
{
	int i, j;
	int chan_no;
	_dma_channel_info *pCh;
	struct rx_desc *rx_desc_p;
	struct tx_desc *tx_desc_p;
	int flag;

	for (i = 0; i < dev->max_tx_chan_num; i++)
	{
		pCh = dev->tx_chan[i];
		if (pCh->control == IFXMIPS_DMA_CH_ON)
		{
			chan_no = (int)(dev->tx_chan[i] - dma_chan);
			local_irq_save (flag);
			ifxmips_w32(chan_no, IFXMIPS_DMA_CS);
			pCh->curr_desc = 0;
			pCh->prev_desc = 0;
			pCh->control = IFXMIPS_DMA_CH_OFF;
			ifxmips_w32(0, IFXMIPS_DMA_CIE);	/*fix me, should disable all the interrupts here? */
			ifxmips_w32(ifxmips_r32(IFXMIPS_DMA_IRNEN) & ~(1 << chan_no), IFXMIPS_DMA_IRNEN);	/*disable interrupts */
			ifxmips_w32(ifxmips_r32(IFXMIPS_DMA_CCTRL) & ~1, IFXMIPS_DMA_CCTRL);
			while (ifxmips_r32(IFXMIPS_DMA_CCTRL) & 1) {};
			local_irq_restore (flag);

			for (j = 0; j < pCh->desc_len; j++)
			{
				tx_desc_p = (struct tx_desc*)pCh->desc_base + j;
				if ((tx_desc_p->status.field.OWN == CPU_OWN && tx_desc_p->status.field.C)
						|| (tx_desc_p->status.field.OWN == DMA_OWN && tx_desc_p->status.field.data_length > 0))
				{
					dev->buffer_free ((u8 *) __va (tx_desc_p->Data_Pointer), (void*)pCh->opt[j]);
				}
				tx_desc_p->status.field.OWN = CPU_OWN;
				memset (tx_desc_p, 0, sizeof (struct tx_desc));
			}
			//TODO should free buffer that is not transferred by dma
		}
	}

	for (i = 0; i < dev->max_rx_chan_num; i++)
	{
		pCh = dev->rx_chan[i];
		chan_no = (int)(dev->rx_chan[i] - dma_chan);
		ifxmips_disable_irq(pCh->irq);

		local_irq_save(flag);
		g_ifxmips_dma_int_status &= ~(1 << chan_no);
		pCh->curr_desc = 0;
		pCh->prev_desc = 0;
		pCh->control = IFXMIPS_DMA_CH_OFF;

		ifxmips_w32(chan_no, IFXMIPS_DMA_CS);
		ifxmips_w32(0, IFXMIPS_DMA_CIE); /*fix me, should disable all the interrupts here? */
		ifxmips_w32(ifxmips_r32(IFXMIPS_DMA_IRNEN) & ~(1 << chan_no), IFXMIPS_DMA_IRNEN);	/*disable interrupts */
		ifxmips_w32(ifxmips_r32(IFXMIPS_DMA_CCTRL) & ~1, IFXMIPS_DMA_CCTRL);
		while (ifxmips_r32(IFXMIPS_DMA_CCTRL) & 1) {};

		local_irq_restore (flag);
		for (j = 0; j < pCh->desc_len; j++)
		{
			rx_desc_p = (struct rx_desc *) pCh->desc_base + j;
			if ((rx_desc_p->status.field.OWN == CPU_OWN 
			     && rx_desc_p->status.field.C)
			    || (rx_desc_p->status.field.OWN == DMA_OWN
				&& rx_desc_p->status.field.data_length > 0)) {
				dev->buffer_free ((u8 *)
						  __va (rx_desc_p->
							Data_Pointer),
						  (void *) pCh->opt[j]);
			}
		}
	}
}

int
dma_device_read (struct dma_device_info *dma_dev, u8 ** dataptr, void **opt)
{
	u8 *buf;
	int len;
	int byte_offset = 0;
	void *p = NULL;
	_dma_channel_info *pCh = dma_dev->rx_chan[dma_dev->current_rx_chan];
	struct rx_desc *rx_desc_p;

	/*get the rx data first */
	rx_desc_p = (struct rx_desc *) pCh->desc_base + pCh->curr_desc;
	if (!(rx_desc_p->status.field.OWN == CPU_OWN && rx_desc_p->status.field.C))
	{
		return 0;
	}

	buf = (u8 *) __va (rx_desc_p->Data_Pointer);
	*(u32*)dataptr = (u32)buf;
	len = rx_desc_p->status.field.data_length;

	if (opt)
	{
		*(int*)opt = (int)pCh->opt[pCh->curr_desc];
	}

	/*replace with a new allocated buffer */
	buf = dma_dev->buffer_alloc(pCh->packet_size, &byte_offset, &p);

	if (buf)
	{
		dma_cache_inv ((unsigned long) buf,
		pCh->packet_size);
		pCh->opt[pCh->curr_desc] = p;
		wmb ();

		rx_desc_p->Data_Pointer = (u32) CPHYSADDR ((u32) buf);
		rx_desc_p->status.word = (DMA_OWN << 31) | ((byte_offset) << 23) | pCh->packet_size;
		wmb ();
	} else {
		*(u32 *) dataptr = 0;
		if (opt)
			*(int *) opt = 0;
		len = 0;
	}

	/*increase the curr_desc pointer */
	pCh->curr_desc++;
	if (pCh->curr_desc == pCh->desc_len)
		pCh->curr_desc = 0;

	return len;
}

int
dma_device_write (struct dma_device_info *dma_dev, u8 * dataptr, int len, void *opt)
{
	int flag;
	u32 tmp, byte_offset;
	_dma_channel_info *pCh;
	int chan_no;
	struct tx_desc *tx_desc_p;
	local_irq_save (flag);

	pCh = dma_dev->tx_chan[dma_dev->current_tx_chan];
	chan_no = (int)(pCh - (_dma_channel_info *) dma_chan);

	tx_desc_p = (struct tx_desc*)pCh->desc_base + pCh->prev_desc;
	while (tx_desc_p->status.field.OWN == CPU_OWN && tx_desc_p->status.field.C)
	{
		dma_dev->buffer_free((u8 *) __va (tx_desc_p->Data_Pointer), pCh->opt[pCh->prev_desc]);
		memset(tx_desc_p, 0, sizeof (struct tx_desc));
		pCh->prev_desc = (pCh->prev_desc + 1) % (pCh->desc_len);
		tx_desc_p = (struct tx_desc*)pCh->desc_base + pCh->prev_desc;
	}
	tx_desc_p = (struct tx_desc*)pCh->desc_base + pCh->curr_desc;
	/*Check whether this descriptor is available */
	if (tx_desc_p->status.field.OWN == DMA_OWN || tx_desc_p->status.field.C)
	{
		/*if not , the tell the upper layer device */
		dma_dev->intr_handler (dma_dev, TX_BUF_FULL_INT);
		local_irq_restore(flag);
		printk (KERN_INFO "%s %d: failed to write!\n", __func__, __LINE__);

		return 0;
	}
	pCh->opt[pCh->curr_desc] = opt;
	/*byte offset----to adjust the starting address of the data buffer, should be multiple of the burst length. */
	byte_offset = ((u32) CPHYSADDR ((u32) dataptr)) % ((dma_dev->tx_burst_len) * 4);
	dma_cache_wback ((unsigned long) dataptr, len);
	wmb ();
	tx_desc_p->Data_Pointer = (u32) CPHYSADDR ((u32) dataptr) - byte_offset;
	wmb ();
	tx_desc_p->status.word = (DMA_OWN << 31) | DMA_DESC_SOP_SET | DMA_DESC_EOP_SET | ((byte_offset) << 23) | len;
	wmb ();

	pCh->curr_desc++;
	if (pCh->curr_desc == pCh->desc_len)
		pCh->curr_desc = 0;

	/*Check whether this descriptor is available */
	tx_desc_p = (struct tx_desc *) pCh->desc_base + pCh->curr_desc;
	if (tx_desc_p->status.field.OWN == DMA_OWN)
	{
		/*if not , the tell the upper layer device */
		dma_dev->intr_handler (dma_dev, TX_BUF_FULL_INT);
	}

	ifxmips_w32(chan_no, IFXMIPS_DMA_CS);
	tmp = ifxmips_r32(IFXMIPS_DMA_CCTRL);

	if (!(tmp & 1))
		pCh->open (pCh);

	local_irq_restore (flag);

	return len;
}

int
map_dma_chan(_dma_chan_map *map)
{
	int i, j;
	int result;

	for (i = 0; i < MAX_DMA_DEVICE_NUM; i++)
	{
		strcpy(dma_devs[i].device_name, global_device_name[i]);
	}

	for (i = 0; i < MAX_DMA_CHANNEL_NUM; i++)
	{
		dma_chan[i].irq = map[i].irq;
		result = request_irq(dma_chan[i].irq, dma_interrupt, IRQF_DISABLED, "dma-core", (void*)&dma_chan[i]);
		if (result)
		{
			printk("error, cannot get dma_irq!\n");
			free_irq(dma_chan[i].irq, (void *) &dma_interrupt);

			return -EFAULT;
		}
	}

	for (i = 0; i < MAX_DMA_DEVICE_NUM; i++)
	{
		dma_devs[i].num_tx_chan = 0;	/*set default tx channel number to be one */
		dma_devs[i].num_rx_chan = 0;	/*set default rx channel number to be one */
		dma_devs[i].max_rx_chan_num = 0;
		dma_devs[i].max_tx_chan_num = 0;
		dma_devs[i].buffer_alloc = &common_buffer_alloc;
		dma_devs[i].buffer_free = &common_buffer_free;
		dma_devs[i].intr_handler = NULL;
		dma_devs[i].tx_burst_len = 4;
		dma_devs[i].rx_burst_len = 4;
		if (i == 0)
		{
			ifxmips_w32(0, IFXMIPS_DMA_PS);
			ifxmips_w32(ifxmips_r32(IFXMIPS_DMA_PCTRL) | ((0xf << 8) | (1 << 6)), IFXMIPS_DMA_PCTRL);	/*enable dma drop */
		}

		if (i == 1)
		{
			ifxmips_w32(1, IFXMIPS_DMA_PS);
			ifxmips_w32(0x14, IFXMIPS_DMA_PCTRL);	/*deu port setting */
		}

		for (j = 0; j < MAX_DMA_CHANNEL_NUM; j++)
		{
			dma_chan[j].byte_offset = 0;
			dma_chan[j].open = &open_chan;
			dma_chan[j].close = &close_chan;
			dma_chan[j].reset = &reset_chan;
			dma_chan[j].enable_irq = &enable_ch_irq;
			dma_chan[j].disable_irq = &disable_ch_irq;
			dma_chan[j].rel_chan_no = map[j].rel_chan_no;
			dma_chan[j].control = IFXMIPS_DMA_CH_OFF;
			dma_chan[j].default_weight = IFXMIPS_DMA_CH_DEFAULT_WEIGHT;
			dma_chan[j].weight = dma_chan[j].default_weight;
			dma_chan[j].curr_desc = 0;
			dma_chan[j].prev_desc = 0;
		}

		for (j = 0; j < MAX_DMA_CHANNEL_NUM; j++)
		{
			if (strcmp(dma_devs[i].device_name, map[j].dev_name) == 0)
			{
				if (map[j].dir == IFXMIPS_DMA_RX)
				{
					dma_chan[j].dir = IFXMIPS_DMA_RX;
					dma_devs[i].max_rx_chan_num++;
					dma_devs[i].rx_chan[dma_devs[i].max_rx_chan_num - 1] = &dma_chan[j];
					dma_devs[i].rx_chan[dma_devs[i].max_rx_chan_num - 1]->pri = map[j].pri;
					dma_chan[j].dma_dev = (void*)&dma_devs[i];
				} else if(map[j].dir == IFXMIPS_DMA_TX)
				{ /*TX direction */
					dma_chan[j].dir = IFXMIPS_DMA_TX;
					dma_devs[i].max_tx_chan_num++;
					dma_devs[i].tx_chan[dma_devs[i].max_tx_chan_num - 1] = &dma_chan[j];
					dma_devs[i].tx_chan[dma_devs[i].max_tx_chan_num - 1]->pri = map[j].pri;
					dma_chan[j].dma_dev = (void*)&dma_devs[i];
				} else {
					printk ("WRONG DMA MAP!\n");
				}
			}
		}
	}

	return 0;
}

void
dma_chip_init(void)
{
	int i;

	// enable DMA from PMU
	ifxmips_pmu_enable(IFXMIPS_PMU_PWDCR_DMA);

	// reset DMA
	ifxmips_w32(ifxmips_r32(IFXMIPS_DMA_CTRL) | 1, IFXMIPS_DMA_CTRL);

	// diable all interrupts
	ifxmips_w32(0, IFXMIPS_DMA_IRNEN);

	for (i = 0; i < MAX_DMA_CHANNEL_NUM; i++)
	{
		ifxmips_w32(i, IFXMIPS_DMA_CS);
		ifxmips_w32(0x2, IFXMIPS_DMA_CCTRL);
		ifxmips_w32(0x80000040, IFXMIPS_DMA_CPOLL);
		ifxmips_w32(ifxmips_r32(IFXMIPS_DMA_CCTRL) & ~0x1, IFXMIPS_DMA_CCTRL);

	}
}

int
ifxmips_dma_init (void)
{
	int i;

	dma_chip_init();
	if (map_dma_chan(default_dma_map))
		BUG();

	g_desc_list = (u64*)KSEG1ADDR(__get_free_page(GFP_DMA));

	if (g_desc_list == NULL)
	{
		printk("no memory for desriptor\n");
		return -ENOMEM;
	}

	memset(g_desc_list, 0, PAGE_SIZE);

	for (i = 0; i < MAX_DMA_CHANNEL_NUM; i++)
	{
		dma_chan[i].desc_base = (u32)g_desc_list + i * IFXMIPS_DMA_DESCRIPTOR_OFFSET * 8;
		dma_chan[i].curr_desc = 0;
		dma_chan[i].desc_len = IFXMIPS_DMA_DESCRIPTOR_OFFSET;

		ifxmips_w32(i, IFXMIPS_DMA_CS);
		ifxmips_w32((u32)CPHYSADDR(dma_chan[i].desc_base), IFXMIPS_DMA_CDBA);
		ifxmips_w32(dma_chan[i].desc_len, IFXMIPS_DMA_CDLEN);
	}

	return 0;
}

arch_initcall(ifxmips_dma_init);

void
dma_cleanup(void)
{
	int i;

	free_page(KSEG0ADDR((unsigned long) g_desc_list));
	for (i = 0; i < MAX_DMA_CHANNEL_NUM; i++)
		free_irq(dma_chan[i].irq, (void*)&dma_interrupt);
}

EXPORT_SYMBOL (dma_device_reserve);
EXPORT_SYMBOL (dma_device_release);
EXPORT_SYMBOL (dma_device_register);
EXPORT_SYMBOL (dma_device_unregister);
EXPORT_SYMBOL (dma_device_read);
EXPORT_SYMBOL (dma_device_write);

MODULE_LICENSE ("GPL");
