/************************************************************************
 *
 * Copyright (c) 2007
 * Infineon Technologies AG
 * St. Martin Strasse 53; 81669 Muenchen; Germany
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 ************************************************************************/

#ifndef __SVIP_DMA_H
#define __SVIP_DMA_H

#define LTQ_DMA_CH_ON  1
#define LTQ_DMA_CH_OFF 0
#define LTQ_DMA_CH_DEFAULT_WEIGHT 100;

#define DMA_OWN   1
#define CPU_OWN   0
#define DMA_MAJOR 250

/* Descriptors */
#define DMA_DESC_OWN_CPU		0x0
#define DMA_DESC_OWN_DMA		0x80000000
#define DMA_DESC_CPT_SET		0x40000000
#define DMA_DESC_SOP_SET		0x20000000
#define DMA_DESC_EOP_SET		0x10000000

struct rx_desc {
	union {
		struct {
#ifdef CONFIG_CPU_LITTLE_ENDIAN
			volatile u32 data_length:16;
			volatile u32 reserve2:7;
			volatile u32 byte_offset:2;
			volatile u32 reserve1:3;
			volatile u32 eop:1;
			volatile u32 sop:1;
			volatile u32 c:1;
			volatile u32 own:1;
#else
			volatile u32 own:1;
			volatile u32 c:1;
			volatile u32 sop:1;
			volatile u32 eop:1;
			volatile u32 reserve1:3;
			volatile u32 byte_offset:2;
			volatile u32 reserve2:7;
			volatile u32 data_length:16;
#endif
		} field;

		volatile u32 word;
	} status;

	volatile u32 data_pointer;
};

struct tx_desc {
	union {
		struct {
#ifdef CONFIG_CPU_LITTLE_ENDIAN
			volatile u32 data_length:16;
			volatile u32 reserved:7;
			volatile u32 byte_offset:5;
			volatile u32 eop:1;
			volatile u32 sop:1;
			volatile u32 c:1;
			volatile u32 own:1;
#else
			volatile u32 own:1;
			volatile u32 c:1;
			volatile u32 sop:1;
			volatile u32 eop:1;
			volatile u32 byte_offset:5;
			volatile u32 reserved:7;
			volatile u32 data_length:16;
#endif
		} field;

		volatile u32 word;
	} status;

	volatile u32 data_pointer;
};

/* DMA pseudo interrupts notified to switch driver */
#define RCV_INT          0x01
#define TX_BUF_FULL_INT  0x02
#define TRANSMIT_CPT_INT 0x04
#define CHANNEL_CLOSED   0x10

/* Parameters for switch DMA device */
#define DEFAULT_SW_CHANNEL_WEIGHT 3
#define DEFAULT_SW_PORT_WEIGHT    7

#define DEFAULT_SW_TX_BURST_LEN 2 /* 2 words, 4 words, 8 words */
#define DEFAULT_SW_RX_BURST_LEN 2 /* 2 words, 4 words, 8 words */

#define DEFAULT_SW_TX_CHANNEL_NUM 4
#define DEFAULT_SW_RX_CHANNEL_NUM 4

#define DEFAULT_SW_TX_CHANNEL_DESCR_NUM 20
#define DEFAULT_SW_RX_CHANNEL_DESCR_NUM 20

/* Parameters for SSC DMA device */
#define DEFAULT_SSC_CHANNEL_WEIGHT 3
#define DEFAULT_SSC_PORT_WEIGHT    7

#define DEFAULT_SSC_TX_CHANNEL_CLASS 3
#define DEFAULT_SSC_RX_CHANNEL_CLASS 0

#define DEFAULT_SSC_TX_BURST_LEN 2 /* 2 words, 4 words, 8 words */
#define DEFAULT_SSC_RX_BURST_LEN 2 /* 2 words, 4 words, 8 words */

#define DEFAULT_SSC0_TX_CHANNEL_NUM 1
#define DEFAULT_SSC0_RX_CHANNEL_NUM 1
#define DEFAULT_SSC1_TX_CHANNEL_NUM 1
#define DEFAULT_SSC1_RX_CHANNEL_NUM 1

#define DEFAULT_SSC_TX_CHANNEL_DESCR_NUM 10
#define DEFAULT_SSC_RX_CHANNEL_DESCR_NUM 10

/* Parameters for memory DMA device */
#define DEFAULT_MEM_CHANNEL_WEIGHT 3
#define DEFAULT_MEM_PORT_WEIGHT    7

#define DEFAULT_MEM_TX_BURST_LEN 4 /* 2 words, 4 words, 8 words */
#define DEFAULT_MEM_RX_BURST_LEN 4 /* 2 words, 4 words, 8 words */

#define DEFAULT_MEM_TX_CHANNEL_NUM 1
#define DEFAULT_MEM_RX_CHANNEL_NUM 1

#define DEFAULT_MEM_TX_CHANNEL_DESCR_NUM 2
#define DEFAULT_MEM_RX_CHANNEL_DESCR_NUM 2

/* Parameters for DEU DMA device */
#define DEFAULT_DEU_CHANNEL_WEIGHT 1
#define DEFAULT_DEU_PORT_WEIGHT    1

#define DEFAULT_DEU_TX_BURST_LEN 4 /* 2 words, 4 words, 8 words */
#define DEFAULT_DEU_RX_BURST_LEN 4 /* 2 words, 4 words, 8 words */

#define DEFAULT_DEU_TX_CHANNEL_DESCR_NUM 20
#define DEFAULT_DEU_RX_CHANNEL_DESCR_NUM 20

#define DMA_DESCR_NUM     30 /* number of descriptors per channel */

enum dma_dir_t {
	DIR_RX = 0,
	DIR_TX = 1,
};

struct dma_device_info;

struct dma_channel_info {
	/*Pointer to the peripheral device who is using this channel*/
	/*const*/ struct dma_device_info *dma_dev;
	/*direction*/
	const enum dma_dir_t dir; /*RX or TX*/
	/*class for this channel for QoS*/
	int pri;
	/*irq number*/
	const int irq;
	/*relative channel number*/
	const int rel_chan_no;
	/*absolute channel number*/
	int abs_chan_no;

	/*specify byte_offset*/
	int byte_offset;
	int tx_weight;

	/*descriptor parameter*/
	int desc_base;
	int desc_len;
	int curr_desc;
	int prev_desc;/*only used if it is a tx channel*/

	/*weight setting for WFQ algorithm*/
	int weight;
	int default_weight;

	int packet_size;

	/*status of this channel*/
	int control; /*on or off*/
	int xfer_cnt;
	int dur; /*descriptor underrun*/

	/**optional information for the upper layer devices*/
	void *opt[DMA_DESCR_NUM];

	/*channel operations*/
	int (*open)(struct dma_channel_info *ch);
	int (*close)(struct dma_channel_info *ch);
	int (*reset)(struct dma_channel_info *ch);
	void (*enable_irq)(struct dma_channel_info *ch);
	void (*disable_irq)(struct dma_channel_info *ch);
};


struct dma_device_info {
	/*device name of this peripheral*/
	const char device_name[16];
	const int  max_rx_chan_num;
	const int  max_tx_chan_num;
	int drop_enable;

	int reserved;

	int tx_burst_len;
	int rx_burst_len;
	int tx_weight;

	int  current_tx_chan;
	int  current_rx_chan;
	int  num_tx_chan;
	int  num_rx_chan;
	int  tx_endianness_mode;
	int  rx_endianness_mode;
	struct dma_channel_info *tx_chan[4];
	struct dma_channel_info *rx_chan[4];

	/*functions, optional*/
	u8 *(*buffer_alloc)(int len,int *offset, void **opt);
	void (*buffer_free)(u8 *dataptr, void *opt);
	int (*intr_handler)(struct dma_device_info *dma_dev, int status);

	/* used by peripheral driver only */
	void *priv;
};

struct dma_device_info *dma_device_reserve(char *dev_name);
int dma_device_release(struct dma_device_info *dma_dev);
int dma_device_register(struct dma_device_info *dma_dev);
int dma_device_unregister(struct dma_device_info *dma_dev);
int dma_device_read(struct dma_device_info *dma_dev, u8 **dataptr, void **opt);
int dma_device_write(struct dma_device_info *dma_dev, u8 *dataptr,
		     int len, void *opt);

#endif
