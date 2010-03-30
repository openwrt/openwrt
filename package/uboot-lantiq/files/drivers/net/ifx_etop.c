/*
 * Lantiq CPE device ethernet driver.
 * Supposed to work on Twinpass/Danube.
 *
 * Based on INCA-IP driver:
 * (C) Copyright 2003-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2010
 * Thomas Langer, Ralph Hempel 
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>

#include <malloc.h>
#include <net.h>
#include <miiphy.h>
#include <asm/types.h>
#include <asm/io.h>
#include <asm/addrspace.h>
#include <config.h>

#include "ifx_etop.h"

#if defined(CONFIG_AR9)
#define TX_CHAN_NO   1
#define RX_CHAN_NO   0
#else
#define TX_CHAN_NO   7
#define RX_CHAN_NO   6
#endif

#define NUM_RX_DESC	PKTBUFSRX
#define NUM_TX_DESC	8
#define TOUT_LOOP	100

typedef struct
{
	union
	{
		struct
		{
			volatile u32 OWN	:1;
			volatile u32 C		:1;
			volatile u32 Sop	:1;
			volatile u32 Eop	:1;
			volatile u32 reserved	:3;
			volatile u32 Byteoffset	:2;
			volatile u32 reserve	:7;
			volatile u32 DataLen	:16;
		}field;

		volatile u32 word;
	}status;

	volatile u32 DataPtr;
} dma_rx_descriptor_t;

typedef struct
{
	union
	{
		struct
		{
			volatile u32 OWN	:1;
			volatile u32 C		:1;
			volatile u32 Sop	:1;
			volatile u32 Eop	:1;
			volatile u32 Byteoffset	:5;
			volatile u32 reserved	:7;
			volatile u32 DataLen	:16;
		}field;

		volatile u32 word;
	}status;

	volatile u32 DataPtr;
} dma_tx_descriptor_t;

static volatile dma_rx_descriptor_t rx_des_ring[NUM_RX_DESC] __attribute__ ((aligned(8)));
static volatile dma_tx_descriptor_t tx_des_ring[NUM_TX_DESC] __attribute__ ((aligned(8)));
static int tx_num, rx_num;

static volatile IfxDMA_t *pDma = (IfxDMA_t *)CKSEG1ADDR(DANUBE_DMA_BASE);

static int lq_eth_init(struct eth_device *dev, bd_t * bis);
static int lq_eth_send(struct eth_device *dev, volatile void *packet,int length);
static int lq_eth_recv(struct eth_device *dev);
static void lq_eth_halt(struct eth_device *dev);
static void lq_eth_init_chip(void);
static void lq_eth_init_dma(void);

static int lq_eth_miiphy_read(char *devname, u8 phyAddr, u8 regAddr, u16 * retVal)
{
	u32 timeout = 50000;
	u32 phy, reg;

	if ((phyAddr > 0x1F) || (regAddr > 0x1F) || (retVal == NULL))
		return -1;

	phy = (phyAddr & 0x1F) << 21;
	reg = (regAddr & 0x1F) << 16;

	*ETOP_MDIO_ACC = 0xC0000000 | phy | reg;
	while ((timeout--) && (*ETOP_MDIO_ACC & 0x80000000))
		udelay(10);

	if (timeout==0) {
		*retVal = 0;
		return -1;
	}
	*retVal = *ETOP_MDIO_ACC & 0xFFFF;
	return 0;
}

static int lq_eth_miiphy_write(char *devname, u8 phyAddr, u8 regAddr, u16 data)
{
	u32 timeout = 50000;
	u32 phy, reg;

	if ((phyAddr > 0x1F) || (regAddr > 0x1F))
		return -1;

	phy = (phyAddr & 0x1F) << 21;
	reg = (regAddr & 0x1F) << 16;

	*ETOP_MDIO_ACC = 0x80000000 | phy | reg | data;
	while ((timeout--) && (*ETOP_MDIO_ACC & 0x80000000))
		udelay(10);

	if (timeout==0)
		return -1;
	return 0;
}


int lq_eth_initialize(bd_t * bis)
{
	struct eth_device *dev;

	debug("Entered lq_eth_initialize()\n");

	if (!(dev = malloc (sizeof *dev))) {
		printf("Failed to allocate memory\n");
		return -1;
	}
	memset(dev, 0, sizeof(*dev));

	sprintf(dev->name, "lq_cpe_eth");
	dev->init = lq_eth_init;
	dev->halt = lq_eth_halt;
	dev->send = lq_eth_send;
	dev->recv = lq_eth_recv;

	eth_register(dev);

#if defined (CONFIG_MII) || defined(CONFIG_CMD_MII)
	/* register mii command access routines */
	miiphy_register(dev->name,
			lq_eth_miiphy_read, lq_eth_miiphy_write);
#endif

	lq_eth_init_dma();
	lq_eth_init_chip();

	return 0;
}

static int lq_eth_init(struct eth_device *dev, bd_t * bis)
{
	int i;
	uchar *enetaddr = dev->enetaddr;

	debug("lq_eth_init %x:%x:%x:%x:%x:%x\n",
		enetaddr[0], enetaddr[1], enetaddr[2], enetaddr[3], enetaddr[4], enetaddr[5]);

	*ENET_MAC_DA0 = (enetaddr[0]<<24) + (enetaddr[1]<<16) + (enetaddr[2]<< 8) + enetaddr[3];
	*ENET_MAC_DA1 = (enetaddr[4]<<24) + (enetaddr[5]<<16);
	*ENETS_CFG |= 1<<28;	/* enable filter for unicast packets */

	tx_num=0;
	rx_num=0;

	for(i=0;i < NUM_RX_DESC; i++) {
		dma_rx_descriptor_t * rx_desc = (dma_rx_descriptor_t *)CKSEG1ADDR(&rx_des_ring[i]);
		rx_desc->status.word=0;
		rx_desc->status.field.OWN=1;
		rx_desc->status.field.DataLen=PKTSIZE_ALIGN;   /* 1536  */
		rx_desc->DataPtr=(u32)CKSEG1ADDR(NetRxPackets[i]);
		NetRxPackets[i][0] = 0xAA;
	}

	/* Reset DMA */
	dma_writel(dma_cs, RX_CHAN_NO);
	dma_writel(dma_cctrl, 0x2);/*fix me, need to reset this channel first?*/
	dma_writel(dma_cpoll, 0x80000040);
	/*set descriptor base*/
	dma_writel(dma_cdba, (u32)rx_des_ring);
	dma_writel(dma_cdlen, NUM_RX_DESC);
	dma_writel(dma_cie, 0);
	dma_writel(dma_cctrl, 0x30000);

	for(i=0;i < NUM_TX_DESC; i++) {
		dma_tx_descriptor_t * tx_desc = (dma_tx_descriptor_t *)CKSEG1ADDR(&tx_des_ring[i]);
		memset(tx_desc, 0, sizeof(tx_des_ring[0]));
	}

	dma_writel(dma_cs, TX_CHAN_NO);
	dma_writel(dma_cctrl, 0x2);/*fix me, need to reset this channel first?*/
	dma_writel(dma_cpoll, 0x80000040);
	dma_writel(dma_cdba, (u32)tx_des_ring);
	dma_writel(dma_cdlen, NUM_TX_DESC);
	dma_writel(dma_cie, 0);
	dma_writel(dma_cctrl, 0x30100);

	/* turn on DMA rx & tx channel
	*/
	dma_writel(dma_cs, RX_CHAN_NO);
	dma_writel(dma_cctrl, dma_readl(dma_cctrl) | 1); /*reset and turn on the channel*/

	return 0;
}

static void lq_eth_halt(struct eth_device *dev)
{
	int i;

	debug("lq_eth_halt()\n");

	for(i=0;i<8;i++) {
		dma_writel(dma_cs, i);
		dma_writel(dma_cctrl, dma_readl(dma_cctrl) & ~1);/*stop the dma channel*/
	}
}

#ifdef DEBUG
static void lq_dump(const u8 *data, const u32 length)
{
	u32 i;
	debug("\n");
	for(i=0;i<length;i++) {
		debug("%02x ", data[i]);
	}
	debug("\n");
}
#endif

static int lq_eth_send(struct eth_device *dev, volatile void *packet, int length)
{
	int i;
	int res = -1;
	volatile dma_tx_descriptor_t * tx_desc = (dma_tx_descriptor_t *)CKSEG1ADDR(&tx_des_ring[tx_num]);

	if (length <= 0) {
		printf ("%s: bad packet size: %d\n", dev->name, length);
		goto Done;
	}

	for(i=0; tx_desc->status.field.OWN==1; i++) {
		if (i>=TOUT_LOOP) {
			printf("NO Tx Descriptor...");
			goto Done;
		}
	}

	tx_desc->status.field.Sop=1;
	tx_desc->status.field.Eop=1;
	tx_desc->status.field.C=0;
	tx_desc->DataPtr = (u32)CKSEG1ADDR(packet);
	if (length<60)
		tx_desc->status.field.DataLen = 60;
	else
		tx_desc->status.field.DataLen = (u32)length;

	flush_cache((u32)packet, tx_desc->status.field.DataLen);
	asm("SYNC");
	tx_desc->status.field.OWN=1;

	res=length;
	tx_num++;
	if (tx_num==NUM_TX_DESC) tx_num=0;

#ifdef DEBUG
	lq_dump(tx_desc->DataPtr, tx_desc->status.field.DataLen);
#endif

	dma_writel(dma_cs, TX_CHAN_NO);
	if (!(dma_readl(dma_cctrl) & 1)) {
		dma_writel(dma_cctrl, dma_readl(dma_cctrl) | 1);
	}

Done:
	return res;
}

static int lq_eth_recv(struct eth_device *dev)
{
	int length  = 0;
	volatile dma_rx_descriptor_t * rx_desc;

	rx_desc = (dma_rx_descriptor_t *)CKSEG1ADDR(&rx_des_ring[rx_num]);

	if ((rx_desc->status.field.C == 0) || (rx_desc->status.field.OWN == 1)) {
		return 0;
	}
	debug("rx");
#ifdef DEBUG
	lq_dump(rx_desc->DataPtr, rx_desc->status.field.DataLen);
#endif
	length = rx_desc->status.field.DataLen;
	if (length > 4) {
		invalidate_dcache_range((u32)CKSEG0ADDR(rx_desc->DataPtr), (u32) CKSEG0ADDR(rx_desc->DataPtr) + length);
		NetReceive(NetRxPackets[rx_num], length);
	} else {
		printf("ERROR: Invalid rx packet length.\n");
	}

	rx_desc->status.field.Sop=0;
	rx_desc->status.field.Eop=0;
	rx_desc->status.field.C=0;
	rx_desc->status.field.DataLen=PKTSIZE_ALIGN;
	rx_desc->status.field.OWN=1;

	rx_num++;
	if (rx_num == NUM_RX_DESC)
		rx_num=0;

	return length;
}

static void lq_eth_init_chip(void)
{
	*ETOP_MDIO_CFG &= ~0x6;
	*ENET_MAC_CFG = 0x187;

	// turn on port0, set to rmii and turn off port1.
#ifdef CONFIG_RMII
	*ETOP_CFG = (*ETOP_CFG & 0xFFFFFFFC) | 0x0000000A;
#else
	*ETOP_CFG = (*ETOP_CFG & 0xFFFFFFFC) | 0x00000008;
#endif

	*ETOP_IG_PLEN_CTRL = 0x004005EE; // set packetlen.
	*ENET_MAC_CFG |= 1<<11; /*enable the crc*/
	return;
}

static void lq_eth_init_dma(void)
{
	/* Reset DMA */
	dma_writel(dma_ctrl, dma_readl(dma_ctrl) | 1);
	dma_writel(dma_irnen, 0);/*disable all the interrupts first*/

	/* Clear Interrupt Status Register */
	dma_writel(dma_irncr, 0xfffff);
	/*disable all the dma interrupts*/
	dma_writel(dma_irnen, 0);
	/*disable channel 0 and channel 1 interrupts*/

	dma_writel(dma_cs, RX_CHAN_NO);
	dma_writel(dma_cctrl, 0x2);/*fix me, need to reset this channel first?*/
	dma_writel(dma_cpoll, 0x80000040);
	/*set descriptor base*/
	dma_writel(dma_cdba, (u32)rx_des_ring);
	dma_writel(dma_cdlen, NUM_RX_DESC);
	dma_writel(dma_cie, 0);
	dma_writel(dma_cctrl, 0x30000);

	dma_writel(dma_cs, TX_CHAN_NO);
	dma_writel(dma_cctrl, 0x2);/*fix me, need to reset this channel first?*/
	dma_writel(dma_cpoll, 0x80000040);
	dma_writel(dma_cdba, (u32)tx_des_ring);
	dma_writel(dma_cdlen, NUM_TX_DESC);
	dma_writel(dma_cie, 0);
	dma_writel(dma_cctrl, 0x30100);
	/*enable the poll function and set the poll counter*/
	//dma_writel(DMA_CPOLL=DANUBE_DMA_POLL_EN | (DANUBE_DMA_POLL_COUNT<<4);
	/*set port properties, enable endian conversion for switch*/
	dma_writel(dma_ps, 0);
	dma_writel(dma_pctrl, dma_readl(dma_pctrl) | (0xf<<8));/*enable 32 bit endian conversion*/

	return;
}
