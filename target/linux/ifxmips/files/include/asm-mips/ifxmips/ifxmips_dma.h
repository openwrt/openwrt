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
 *
 *   Copyright (C) 2005 infineon
 *   Copyright (C) 2007 John Crispin <blogic@openwrt.org>
 *
 */
#ifndef _IFXMIPS_DMA_H__
#define _IFXMIPS_DMA_H__

#define RCV_INT						1
#define TX_BUF_FULL_INT					2
#define TRANSMIT_CPT_INT				4
#define IFXMIPS_DMA_CH_ON				1
#define IFXMIPS_DMA_CH_OFF				0
#define IFXMIPS_DMA_CH_DEFAULT_WEIGHT			100

enum attr_t{
	TX = 0,
	RX = 1,
	RESERVED = 2,
	DEFAULT = 3,
};

#define DMA_OWN						1
#define CPU_OWN						0
#define DMA_MAJOR					250

#define DMA_DESC_OWN_CPU				0x0
#define DMA_DESC_OWN_DMA				0x80000000
#define DMA_DESC_CPT_SET				0x40000000
#define DMA_DESC_SOP_SET				0x20000000
#define DMA_DESC_EOP_SET				0x10000000

#define MISCFG_MASK					0x40
#define RDERR_MASK					0x20
#define CHOFF_MASK					0x10
#define DESCPT_MASK					0x8
#define DUR_MASK					0x4
#define EOP_MASK					0x2

#define DMA_DROP_MASK					(1<<31)

#define IFXMIPS_DMA_RX					-1
#define IFXMIPS_DMA_TX					1

struct dma_chan_map {
	const char *dev_name;
	enum attr_t dir;
	int pri;
	int irq;
	int rel_chan_no;
};

#ifdef CONFIG_CPU_LITTLE_ENDIAN
struct rx_desc {
	u32 data_length:16;
	volatile u32 reserved:7;
	volatile u32 byte_offset:2;
	volatile u32 Burst_length_offset:3;
	volatile u32 EoP:1;
	volatile u32 Res:1;
	volatile u32 C:1;
	volatile u32 OWN:1;
	volatile u32 Data_Pointer; /* fixme: should be 28 bits here */
};

struct tx_desc {
	volatile u32 data_length:16;
	volatile u32 reserved1:7;
	volatile u32 byte_offset:5;
	volatile u32 EoP:1;
	volatile u32 SoP:1;
	volatile u32 C:1;
	volatile u32 OWN:1;
	volatile u32 Data_Pointer; /* fixme: should be 28 bits here */
};
#else /* BIG */
struct rx_desc {
	union {
		struct {
			volatile u32 OWN:1;
			volatile u32 C:1;
			volatile u32 SoP:1;
			volatile u32 EoP:1;
			volatile u32 Burst_length_offset:3;
			volatile u32 byte_offset:2;
			volatile u32 reserve:7;
			volatile u32 data_length:16;
		} field;
		volatile u32 word;
	} status;
	volatile u32 Data_Pointer;
};

struct tx_desc {
	union {
		struct {
			volatile u32 OWN:1;
			volatile u32 C:1;
			volatile u32 SoP:1;
			volatile u32 EoP:1;
			volatile u32 byte_offset:5;
			volatile u32 reserved:7;
			volatile u32 data_length:16;
		} field;
		volatile u32 word;
	} status;
	volatile u32 Data_Pointer;
};
#endif /* ENDIAN */

struct dma_channel_info {
	/* relative channel number */
	int rel_chan_no;
	/* class for this channel for QoS */
	int pri;
	/* specify byte_offset */
	int byte_offset;
	/* direction */
	int dir;
	/* irq number */
	int irq;
	/* descriptor parameter */
	int desc_base;
	int desc_len;
	int curr_desc;
	int prev_desc; /* only used if it is a tx channel*/
	/* weight setting for WFQ algorithm*/
	int weight;
	int default_weight;
	int packet_size;
	int burst_len;
	/* on or off of this channel */
	int control;
	/* optional information for the upper layer devices */
#if defined(CONFIG_IFXMIPS_ETHERNET_D2) || defined(CONFIG_IFXMIPS_PPA)
	void *opt[64];
#else
	void *opt[25];
#endif
	/* Pointer to the peripheral device who is using this channel */
	void *dma_dev;
	/* channel operations */
	void (*open)(struct dma_channel_info *pCh);
	void (*close)(struct dma_channel_info *pCh);
	void (*reset)(struct dma_channel_info *pCh);
	void (*enable_irq)(struct dma_channel_info *pCh);
	void (*disable_irq)(struct dma_channel_info *pCh);
};

struct dma_device_info {
	/* device name of this peripheral */
	char device_name[15];
	int reserved;
	int tx_burst_len;
	int rx_burst_len;
	int default_weight;
	int current_tx_chan;
	int current_rx_chan;
	int num_tx_chan;
	int num_rx_chan;
	int max_rx_chan_num;
	int max_tx_chan_num;
	struct dma_channel_info *tx_chan[20];
	struct dma_channel_info *rx_chan[20];
	/*functions, optional*/
	u8 *(*buffer_alloc)(int len, int *offset, void **opt);
	void (*buffer_free)(u8 *dataptr, void *opt);
	int (*intr_handler)(struct dma_device_info *info, int status);
	void *priv;		/* used by peripheral driver only */
};

struct dma_device_info *dma_device_reserve(char *dev_name);
void dma_device_release(struct dma_device_info *dev);
void dma_device_register(struct dma_device_info *info);
void dma_device_unregister(struct dma_device_info *info);
int dma_device_read(struct dma_device_info *info, u8 **dataptr, void **opt);
int dma_device_write(struct dma_device_info *info, u8 *dataptr, int len,
	void *opt);

#endif

