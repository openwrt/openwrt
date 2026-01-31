/*******************************************************************************

  Intel SmartPHY DSL PCIe Endpoint/ACA Linux driver
  Copyright(c) 2016 Intel Corporation.

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

*******************************************************************************/

#ifndef REGS_H
#define REGS_H

#include <linux/bitops.h>

/* APPL defined */
#define PCIE_APPL_BASE		0x00048000
#define PCIE_APPL_REG(X)	(PCIE_APPL_BASE + (X))

#define PCIE_APPL_PHY_CFG1	PCIE_APPL_REG(0x3C)
#define PCIE_APPL_PHY_CFG2	PCIE_APPL_REG(0x40)
#define PCIE_APPL_PHY_CFG3	PCIE_APPL_REG(0x58)
#define PCIE_APPL_PHY_CFG4	PCIE_APPL_REG(0x28)
#define PCIE_APPL_INTR_VEC	PCIE_APPL_REG(0x48)
#define PCIE_APPL_MSI_EN	PCIE_APPL_REG(0x4C)

#define PCIE_MSI_EN_ALL		0xFF

/* RCU defined */
#define RCU_BASE		0x00008000
#define RCU_REG(X)		(RCU_BASE + (X))
#define RCU_STAT		RCU_REG(0x00)
#define RCU_REQ			RCU_REG(0x10)

#define RCU_MSI			RCU_REG(0x80)
#define PCI_MSI_4_MODE		1
#define PCI_MSI_8_MODE		0

/* CGU */
#define CGU_BASE		0x00000000
#define CGU_REG(X)		(CGU_BASE + (X))
#define PMU_PWDCR		CGU_REG(0x011C)
#define PMU_SR			CGU_REG(0x0120)
#define PMU_ALL			0x20ec0305

#define PLL_OMCFG		CGU_REG(0x0064)

#define SYS_CLK			0x3
#define SYS_CLK_S		0
#define PPE_CLK			0x700
#define PPE_CLK_S		8

#define IF_CLK			CGU_REG(0x0024)

#define CLK_PD			BIT(10)
#define CLK_OD			BIT(11)
#define PCIE_CLKOD		(BIT(12) | BIT(13))
#define AFE_CLKOD		BIT(14)

#define IF_CLKOD_ALL		(CLK_PD | CLK_OD | PCIE_CLKOD | AFE_CLKOD)

/* GPIO */
#define GPIO_BASE		0x00020000
#define GPIO_REG(X)		(GPIO_BASE + (X))
#define GPIO_OUT		GPIO_REG(0x00)
#define GPIO_IN			GPIO_REG(0x04)
#define GPIO_DIR		GPIO_REG(0x08)
#define GPIO_OUTSET		GPIO_REG(0x40)
#define GPIO_OUTCLR		GPIO_REG(0x44)
#define GPIO_DIRSET		GPIO_REG(0x48)
#define GPIO_DIRCLR		GPIO_REG(0x4c)

/* PADC */
#define PADC_BASE		0x00024000
#define PADC_REG(X)		(PADC_BASE + (X))
#define PADC_MUX(pin)		PADC_REG(((pin) << 2))
#define PADC_PUEN		PADC_REG(0x80)
#define PADC_PDEN		PADC_REG(0x84)
#define PADC_SRC		PADC_REG(0x88)
#define PADC_DCC		PADC_REG(0x8c)
#define PADC_OD			PADC_REG(0x94)
#define PADC_AVAIL		PADC_REG(0x98)
#define PADC_MUX_M		0x7

/* ICU defined */
#define ICU_BASE		0x00010000
#define ICU_REG(X)		(ICU_BASE + (X))
#define ICU_IMSR		ICU_REG(0x40)
#define ICU_IMER		ICU_REG(0x44)
#define ICU_IMOSR		ICU_REG(0x48)
#define ICU_DMA_TX_STATUS	ICU_REG(0x50)
#define ICU_DMA_RX_STATUS	ICU_REG(0x54)
#define ICU_DMA_TX_IMER		ICU_REG(0x58)
#define ICU_DMA_RX_IMER		ICU_REG(0x5C)
#define ICU_DMA_TX_IMOSR	ICU_REG(0x60)
#define ICU_DMA_RX_IMOSR	ICU_REG(0x64)

#define PPE2HOST_INT0		BIT(0)
#define PPE2HOST_INT1		BIT(1)
#define DYING_GASP_INT		BIT(3)
#define MEI_IRQ			BIT(8)
#define ACA_XBAR_INT		BIT(9)
#define MODEM_XBAR_INT		BIT(12)
#define LED0_INT		BIT(13)
#define LED1_INT		BIT(14)
#define NMI_PLL			BIT(15)
#define DMA_TX			BIT(16)
#define DMA_RX			BIT(17)
#define ACA_HOSTIF_TX		BIT(20)
#define ACA_HOSTIF_RX		BIT(21)
#define ACA_RXOUT_PD_RING_FULL	BIT(22)
#define ACA_TXOUT_PD_RING_FULL	BIT(23)

#define ICU_TOP_ALL		0x0003f30B /* Except ACA related */
#define ICU_DMA_TX_ALL		0x003f03FF
#define ICU_DMA_RX_ALL		0x003F03FF

#define wr32(value, reg)	(writel(value, (priv->mem + (reg))))
#define rd32(reg)		(readl(priv->mem + (reg)))
#define wrfl()			((void)rd32(RCU_STAT))

#define wr32_mask(clr, set, reg)		\
	wr32(((rd32(reg) & ~(clr)) | (set)), (reg))

#endif /* REGS_H */
