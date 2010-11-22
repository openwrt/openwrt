/*
 * arch/m68k/coldfire/m547x/dma.c
 *
 * Coldfire M547x/M548x DMA
 *
 * Copyright 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 * Kurt Mahan <kmahan@freescale.com>
 * Shrek Wu b16972@freescale.com
 *
 * This code is based on patches from the Freescale M547x_8x BSP
 * release mcf547x_8x-20070107-ltib.iso
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/dma.h>
#include <asm/coldfire.h>
#include <asm/m5485sram.h>
#include <asm/mcfsim.h>
#include <asm/MCD_dma.h>

/*
 * This global keeps track of which initiators have been
 * used of the available assignments.  Initiators 0-15 are
 * hardwired.  Initiators 16-31 are multiplexed and controlled
 * via the Initiatior Mux Control Registe (IMCR).  The
 * assigned requestor is stored with the associated initiator
 * number.
 */
static int used_reqs[32] = {
	DMA_ALWAYS, DMA_DSPI_RX, DMA_DSPI_TX, DMA_DREQ0,
	DMA_PSC0_RX, DMA_PSC0_TX, DMA_USBEP0, DMA_USBEP1,
	DMA_USBEP2, DMA_USBEP3, DMA_PCI_TX, DMA_PCI_RX,
	DMA_PSC1_RX, DMA_PSC1_TX, DMA_I2C_RX, DMA_I2C_TX,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0
};

/*
 * This global keeps track of which channels have been assigned
 * to tasks.  This methology assumes that no single initiator
 * will be tied to more than one task/channel
 */
static char used_channel[16] = {
	-1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1
};

unsigned int connected_channel[16] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

/**
 * dma_set_initiator - enable initiator
 * @initiator: initiator identifier
 *
 * Returns 0 of successful, non-zero otherwise
 *
 * Attempt to enable the provided Initiator in the Initiator
 * Mux Control Register.
 */
int dma_set_initiator(int initiator)
{
	switch (initiator) {
	case DMA_ALWAYS:
	case DMA_DSPI_RX:
	case DMA_DSPI_TX:
	case DMA_DREQ0:
	case DMA_PSC0_RX:
	case DMA_PSC0_TX:
	case DMA_USBEP0:
	case DMA_USBEP1:
	case DMA_USBEP2:
	case DMA_USBEP3:
	case DMA_PCI_TX:
	case DMA_PCI_RX:
	case DMA_PSC1_RX:
	case DMA_PSC1_TX:
	case DMA_I2C_RX:
	case DMA_I2C_TX:
		/*
		 * These initiators are always active
		 */
		break;

	case DMA_FEC0_RX:
		MCF_DMA_IMCR = (MCF_DMA_IMCR & ~MCF_DMA_IMCR_SRC16(3))
		    | MCF_DMA_IMCR_SRC16_FEC0RX;
		used_reqs[16] = DMA_FEC0_RX;
		break;

	case DMA_FEC0_TX:
		MCF_DMA_IMCR = (MCF_DMA_IMCR & ~MCF_DMA_IMCR_SRC17(3))
		    | MCF_DMA_IMCR_SRC17_FEC0TX;
		used_reqs[17] = DMA_FEC0_TX;
		break;

	case DMA_FEC1_RX:
		MCF_DMA_IMCR = (MCF_DMA_IMCR & ~MCF_DMA_IMCR_SRC20(3))
		    | MCF_DMA_IMCR_SRC20_FEC1RX;
		used_reqs[20] = DMA_FEC1_RX;
		break;

	case DMA_FEC1_TX:
		if (used_reqs[21] == 0) {
			MCF_DMA_IMCR = (MCF_DMA_IMCR & ~MCF_DMA_IMCR_SRC21(3))
			    | MCF_DMA_IMCR_SRC21_FEC1TX;
			used_reqs[21] = DMA_FEC1_TX;
		} else if (used_reqs[25] == 0) {
			MCF_DMA_IMCR = (MCF_DMA_IMCR & ~MCF_DMA_IMCR_SRC25(3))
			    | MCF_DMA_IMCR_SRC25_FEC1TX;
			used_reqs[25] = DMA_FEC1_TX;
		} else if (used_reqs[31] == 0) {
			MCF_DMA_IMCR = (MCF_DMA_IMCR & ~MCF_DMA_IMCR_SRC31(3))
			    | MCF_DMA_IMCR_SRC31_FEC1TX;
			used_reqs[31] = DMA_FEC1_TX;
		} else		/* No empty slots */
			return 1;
		break;

	case DMA_DREQ1:
		if (used_reqs[29] == 0) {
			MCF_DMA_IMCR = (MCF_DMA_IMCR & ~MCF_DMA_IMCR_SRC29(3))
			    | MCF_DMA_IMCR_SRC29_DREQ1;
			used_reqs[29] = DMA_DREQ1;
		} else if (used_reqs[21] == 0) {
			MCF_DMA_IMCR = (MCF_DMA_IMCR & ~MCF_DMA_IMCR_SRC21(3))
			    | MCF_DMA_IMCR_SRC21_DREQ1;
			used_reqs[21] = DMA_DREQ1;
		} else		/* No empty slots */
			return 1;
		break;

	case DMA_CTM0:
		if (used_reqs[24] == 0) {
			MCF_DMA_IMCR = (MCF_DMA_IMCR & ~MCF_DMA_IMCR_SRC24(3))
			    | MCF_DMA_IMCR_SRC24_CTM0;
			used_reqs[24] = DMA_CTM0;
		} else		/* No empty slots */
			return 1;
		break;

	case DMA_CTM1:
		if (used_reqs[25] == 0) {
			MCF_DMA_IMCR = (MCF_DMA_IMCR & ~MCF_DMA_IMCR_SRC25(3))
			    | MCF_DMA_IMCR_SRC25_CTM1;
			used_reqs[25] = DMA_CTM1;
		} else		/* No empty slots */
			return 1;
		break;

	case DMA_CTM2:
		if (used_reqs[26] == 0) {
			MCF_DMA_IMCR = (MCF_DMA_IMCR & ~MCF_DMA_IMCR_SRC26(3))
			    | MCF_DMA_IMCR_SRC26_CTM2;
			used_reqs[26] = DMA_CTM2;
		} else		/* No empty slots */
			return 1;
		break;

	case DMA_CTM3:
		if (used_reqs[27] == 0) {
			MCF_DMA_IMCR = (MCF_DMA_IMCR & ~MCF_DMA_IMCR_SRC27(3))
			    | MCF_DMA_IMCR_SRC27_CTM3;
			used_reqs[27] = DMA_CTM3;
		} else		/* No empty slots */
			return 1;
		break;

	case DMA_CTM4:
		if (used_reqs[28] == 0) {
			MCF_DMA_IMCR = (MCF_DMA_IMCR & ~MCF_DMA_IMCR_SRC28(3))
			    | MCF_DMA_IMCR_SRC28_CTM4;
			used_reqs[28] = DMA_CTM4;
		} else		/* No empty slots */
			return 1;
		break;

	case DMA_CTM5:
		if (used_reqs[29] == 0) {
			MCF_DMA_IMCR = (MCF_DMA_IMCR & ~MCF_DMA_IMCR_SRC29(3))
			    | MCF_DMA_IMCR_SRC29_CTM5;
			used_reqs[29] = DMA_CTM5;
		} else		/* No empty slots */
			return 1;
		break;

	case DMA_CTM6:
		if (used_reqs[30] == 0) {
			MCF_DMA_IMCR = (MCF_DMA_IMCR & ~MCF_DMA_IMCR_SRC30(3))
			    | MCF_DMA_IMCR_SRC30_CTM6;
			used_reqs[30] = DMA_CTM6;
		} else		/* No empty slots */
			return 1;
		break;

	case DMA_CTM7:
		if (used_reqs[31] == 0) {
			MCF_DMA_IMCR = (MCF_DMA_IMCR & ~MCF_DMA_IMCR_SRC31(3))
			    | MCF_DMA_IMCR_SRC31_CTM7;
			used_reqs[31] = DMA_CTM7;
		} else		/* No empty slots */
			return 1;
		break;

	case DMA_USBEP4:
		if (used_reqs[26] == 0) {
			MCF_DMA_IMCR = (MCF_DMA_IMCR & ~MCF_DMA_IMCR_SRC26(3))
			    | MCF_DMA_IMCR_SRC26_USBEP4;
			used_reqs[26] = DMA_USBEP4;
		} else		/* No empty slots */
			return 1;
		break;

	case DMA_USBEP5:
		if (used_reqs[27] == 0) {
			MCF_DMA_IMCR = (MCF_DMA_IMCR & ~MCF_DMA_IMCR_SRC27(3))
			    | MCF_DMA_IMCR_SRC27_USBEP5;
			used_reqs[27] = DMA_USBEP5;
		} else		/* No empty slots */
			return 1;
		break;

	case DMA_USBEP6:
		if (used_reqs[28] == 0) {
			MCF_DMA_IMCR = (MCF_DMA_IMCR & ~MCF_DMA_IMCR_SRC28(3))
			    | MCF_DMA_IMCR_SRC28_USBEP6;
			used_reqs[28] = DMA_USBEP6;
		} else		/* No empty slots */
			return 1;
		break;

	case DMA_PSC2_RX:
		if (used_reqs[28] == 0) {
			MCF_DMA_IMCR = (MCF_DMA_IMCR & ~MCF_DMA_IMCR_SRC28(3))
			    | MCF_DMA_IMCR_SRC28_PSC2RX;
			used_reqs[28] = DMA_PSC2_RX;
		} else		/* No empty slots */
			return 1;
		break;

	case DMA_PSC2_TX:
		if (used_reqs[29] == 0) {
			MCF_DMA_IMCR = (MCF_DMA_IMCR & ~MCF_DMA_IMCR_SRC29(3))
			    | MCF_DMA_IMCR_SRC29_PSC2TX;
			used_reqs[29] = DMA_PSC2_TX;
		} else		/* No empty slots */
			return 1;
		break;

	case DMA_PSC3_RX:
		if (used_reqs[30] == 0) {
			MCF_DMA_IMCR = (MCF_DMA_IMCR & ~MCF_DMA_IMCR_SRC30(3))
			    | MCF_DMA_IMCR_SRC30_PSC3RX;
			used_reqs[30] = DMA_PSC3_RX;
		} else		/* No empty slots */
			return 1;
		break;

	case DMA_PSC3_TX:
		if (used_reqs[31] == 0) {
			MCF_DMA_IMCR = (MCF_DMA_IMCR & ~MCF_DMA_IMCR_SRC31(3))
			    | MCF_DMA_IMCR_SRC31_PSC3TX;
			used_reqs[31] = DMA_PSC3_TX;
		} else		/* No empty slots */
			return 1;
		break;

	default:
		return 1;
	}
	return 0;
}

/**
 * dma_get_initiator - get the initiator for the given requestor
 * @requestor: initiator identifier
 *
 * Returns initiator number (0-31) if assigned or just 0
 */
unsigned int dma_get_initiator(int requestor)
{
	u32 i;

	for (i = 0; i < sizeof(used_reqs); ++i) {
		if (used_reqs[i] == requestor)
			return i;
	}
	return 0;
}

/**
 * dma_remove_initiator - remove the given initiator from active list
 * @requestor: requestor to remove
 */
void dma_remove_initiator(int requestor)
{
	u32 i;

	for (i = 0; i < sizeof(used_reqs); ++i) {
		if (used_reqs[i] == requestor) {
			used_reqs[i] = -1;
			break;
		}
	}
}

/**
 * dma_set_channel_fec: find available channel for fec and mark
 * @requestor: initiator/requestor identifier
 *
 * Returns first avaialble channel (0-5) or -1 if all occupied
 */
int dma_set_channel_fec(int requestor)
{
	u32 i, t;

#ifdef CONFIG_FEC_548x_ENABLE_FEC2
	t = 4;
#else
	t = 2;
#endif

	for (i = 0; i < t ; ++i) {
		if (used_channel[i] == -1) {
			used_channel[i] = requestor;
			return i;
		}
	}
	/* All channels taken */
	return -1;
}

/**
 * dma_set_channel - find an available channel and mark as used
 * @requestor: initiator/requestor identifier
 *
 * Returns first available channel (6-15) or -1 if all occupied
 */
int dma_set_channel(int requestor)
{
	u32 i;
#ifdef CONFIG_NET_FEC2
	i = 4;
#else
	i = 2;
#endif

	for (; i < 16; ++i)
		if (used_channel[i] == -1) {
			used_channel[i] = requestor;
			return i;
		}

	/* All channels taken */
	return -1;
}

/**
 * dma_get_channel - get the channel being initiated by the requestor
 * @requestor: initiator/requestor identifier
 *
 * Returns Initiator for requestor or -1 if not found
 */
int dma_get_channel(int requestor)
{
	u32 i;

	for (i = 0; i < sizeof(used_channel); ++i) {
		if (used_channel[i] == requestor)
			return i;
	}
	return -1;
}

/**
 * dma_connect - connect a channel with reference on data
 * @channel: channel number
 * @address: reference address of data
 *
 * Returns 0 if success or -1 if invalid channel
 */
int dma_connect(int channel, int address)
{
	if ((channel < 16) && (channel >= 0)) {
		connected_channel[channel] = address;
		return 0;
	}
	return -1;
}

/**
 * dma_disconnect - disconnect a channel
 * @channel: channel number
 *
 * Returns 0 if success or -1 if invalid channel
 */
int dma_disconnect(int channel)
{
	if ((channel < 16) && (channel >= 0)) {
		connected_channel[channel] = 0;
		return 0;
	}
	return -1;
}

/**
 * dma_remove_channel - remove channel from the active list
 * @requestor: initiator/requestor identifier
 */
void dma_remove_channel(int requestor)
{
	u32 i;

	for (i = 0; i < sizeof(used_channel); ++i) {
		if (used_channel[i] == requestor) {
			used_channel[i] = -1;
			break;
		}
	}
}

/**
 * dma_interrupt_handler - dma interrupt handler
 * @irq: interrupt number
 * @dev_id: data
 *
 * Returns IRQ_HANDLED
 */
irqreturn_t dma_interrupt_handler(int irq, void *dev_id)
{
	u32 i, interrupts;

	/*
	 * Determine which interrupt(s) triggered by AND'ing the
	 * pending interrupts with those that aren't masked.
	 */
	interrupts = MCF_DMA_DIPR;
	MCF_DMA_DIPR = interrupts;

	for (i = 0; i < 16; ++i, interrupts >>= 1) {
		if (interrupts & 0x1)
			if (connected_channel[i] != 0)
				((void (*)(void)) (connected_channel[i])) ();
	}

	return IRQ_HANDLED;
}

/**
 * dma_remove_channel_by_number - clear dma channel
 * @channel: channel number to clear
 */
void dma_remove_channel_by_number(int channel)
{
	if ((channel < sizeof(used_channel)) && (channel >= 0))
		used_channel[channel] = -1;
}

/**
 * dma_init - initialize the dma subsystem
 *
 * Returns 0 if success non-zero if failure
 *
 * Handles the DMA initialization during device setup.
 */
int __devinit dma_init()
{
	int result;
	char *dma_version_str;

	MCD_getVersion(&dma_version_str);
	printk(KERN_INFO "m547x_8x DMA: Initialize %s\n", dma_version_str);

	/* attempt to setup dma interrupt handler */
	if (request_irq(64 + ISC_DMA, dma_interrupt_handler, IRQF_DISABLED,
			"MCD-DMA", NULL)) {
		printk(KERN_ERR "MCD-DMA: Cannot allocate the DMA IRQ(48)\n");
		return 1;
	}

	MCF_DMA_DIMR = 0;
	MCF_DMA_DIPR = 0xFFFFFFFF;

	MCF_ICR(ISC_DMA) = ILP_DMA;

	result = MCD_initDma((dmaRegs *) (MCF_MBAR + 0x8000),
			(void *) SYS_SRAM_DMA_START, MCD_RELOC_TASKS);
	if (result != MCD_OK) {
		printk(KERN_ERR "MCD-DMA: Cannot perform DMA initialization\n");
		free_irq(64 + ISC_DMA, NULL);
		return 1;
	}

	return 0;
}
device_initcall(dma_init);
