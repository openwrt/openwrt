/*
 * arch/arm/mach-imx/pcie.c
 *
 * PCIe host controller driver for IMX6 SOCs
 *
 * Copyright (C) 2012 Freescale Semiconductor, Inc. All Rights Reserved.
 * Copyright (C) 2013 Tim Harvey <tharvey@gateworks.com>
 *
 * Bits taken from arch/arm/mach-dove/pcie.c
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>

#include <asm/signal.h>
#include <asm/mach/pci.h>
#include <asm/sizes.h>

#include "msi.h"

/* PCIe Registers */
#define PCIE_ARB_BASE_ADDR  0x01000000
#define PCIE_ARB_END_ADDR   0x01FFFFFF
#define PCIE_RC_IOBLSSR     0x1c

/* Register Definitions */
#define PRT_LOG_R_BaseAddress 0x700

/* Register DB_R0 */
/* Debug Register 0 */
#define DB_R0 (PRT_LOG_R_BaseAddress + 0x28)
#define DB_R0_RegisterSize 32
#define DB_R0_RegisterResetValue 0x0
#define DB_R0_RegisterResetMask 0xFFFFFFFF
/* End of Register Definition for DB_R0 */

/* Register DB_R1 */
/* Debug Register 1 */
#define DB_R1 (PRT_LOG_R_BaseAddress + 0x2c)
#define DB_R1_RegisterSize 32
#define DB_R1_RegisterResetValue 0x0
#define DB_R1_RegisterResetMask 0xFFFFFFFF
/* End of Register Definition for DB_R1 */

#define PCIE_PL_MSICA		0x820
#define PCIE_PL_MSICUA		0x824
#define PCIE_PL_MSIC_INT	0x828

#define MSIC_INT_EN	0x0
#define MSIC_INT_MASK	0x4
#define MSIC_INT_STATUS	0x8

#define ATU_R_BaseAddress 0x900
#define ATU_VIEWPORT_R (ATU_R_BaseAddress + 0x0)
#define ATU_REGION_CTRL1_R (ATU_R_BaseAddress + 0x4)
#define ATU_REGION_CTRL2_R (ATU_R_BaseAddress + 0x8)
#define ATU_REGION_LOWBASE_R (ATU_R_BaseAddress + 0xC)
#define ATU_REGION_UPBASE_R (ATU_R_BaseAddress + 0x10)
#define ATU_REGION_LIMIT_ADDR_R (ATU_R_BaseAddress + 0x14)
#define ATU_REGION_LOW_TRGT_ADDR_R (ATU_R_BaseAddress + 0x18)
#define ATU_REGION_UP_TRGT_ADDR_R (ATU_R_BaseAddress + 0x1C)

/* IOMUXC */
#define IOMUXC_GPR_BASE_ADDR	0x020E0000
#define IOMUXC_GPR1     (imx_pcie.gpr_base + 0x04)
#define IOMUXC_GPR8     (imx_pcie.gpr_base + 0x20)
#define IOMUXC_GPR12    (imx_pcie.gpr_base + 0x30)
/* GPR1: iomuxc_gpr1_pcie_ref_clk_en(iomuxc_gpr1[16]) */
#define iomuxc_gpr1_pcie_ref_clk_en		(1 << 16)
/* GPR1: iomuxc_gpr1_test_powerdown(iomuxc_gpr1_18) */
#define iomuxc_gpr1_test_powerdown		(1 << 18)
/* GPR12: iomuxc_gpr12_los_level(iomuxc_gpr12[8:4]) */
#define iomuxc_gpr12_los_level			(0x1F << 4)
/* GPR12: iomuxc_gpr12_app_ltssm_enable(iomuxc_gpr12[10]) */
#define iomuxc_gpr12_app_ltssm_enable		(1 << 10)
/* GPR12: iomuxc_gpr12_device_type(iomuxc_gpr12[15:12]) */
#define iomuxc_gpr12_device_type		(0xF << 12)
/* GPR8: iomuxc_gpr8_tx_deemph_gen1(iomuxc_gpr8[5:0]) */
#define iomuxc_gpr8_tx_deemph_gen1		(0x3F << 0)
/* GPR8: iomuxc_gpr8_tx_deemph_gen2_3p5db(iomuxc_gpr8[11:6]) */
#define iomuxc_gpr8_tx_deemph_gen2_3p5db	(0x3F << 6)
/* GPR8: iomuxc_gpr8_tx_deemph_gen2_6db(iomuxc_gpr8[17:12]) */
#define iomuxc_gpr8_tx_deemph_gen2_6db		(0x3F << 12)
/* GPR8: iomuxc_gpr8_tx_swing_full(iomuxc_gpr8[24:18]) */
#define iomuxc_gpr8_tx_swing_full		(0x7F << 18)
/* GPR8: iomuxc_gpr8_tx_swing_low(iomuxc_gpr8[31:25]) */
#define iomuxc_gpr8_tx_swing_low		(0x7F << 25)

/* Registers of PHY */
/* Register PHY_STS_R */
/* PHY Status Register */
#define PHY_STS_R (PRT_LOG_R_BaseAddress + 0x110)

/* Register PHY_CTRL_R */
/* PHY Control Register */
#define PHY_CTRL_R (PRT_LOG_R_BaseAddress + 0x114)

#define SSP_CR_SUP_DIG_MPLL_OVRD_IN_LO 0x0011
/* FIELD: RES_ACK_IN_OVRD [15:15]
// FIELD: RES_ACK_IN [14:14]
// FIELD: RES_REQ_IN_OVRD [13:13]
// FIELD: RES_REQ_IN [12:12]
// FIELD: RTUNE_REQ_OVRD [11:11]
// FIELD: RTUNE_REQ [10:10]
// FIELD: MPLL_MULTIPLIER_OVRD [9:9]
// FIELD: MPLL_MULTIPLIER [8:2]
// FIELD: MPLL_EN_OVRD [1:1]
// FIELD: MPLL_EN [0:0]
*/

#define SSP_CR_SUP_DIG_ATEOVRD 0x0010
/* FIELD: ateovrd_en [2:2]
// FIELD: ref_usb2_en [1:1]
// FIELD: ref_clkdiv2 [0:0]
*/

#define SSP_CR_LANE0_DIG_RX_OVRD_IN_LO 0x1005
/* FIELD: RX_LOS_EN_OVRD [13:13]
// FIELD: RX_LOS_EN [12:12]
// FIELD: RX_TERM_EN_OVRD [11:11]
// FIELD: RX_TERM_EN [10:10]
// FIELD: RX_BIT_SHIFT_OVRD [9:9]
// FIELD: RX_BIT_SHIFT [8:8]
// FIELD: RX_ALIGN_EN_OVRD [7:7]
// FIELD: RX_ALIGN_EN [6:6]
// FIELD: RX_DATA_EN_OVRD [5:5]
// FIELD: RX_DATA_EN [4:4]
// FIELD: RX_PLL_EN_OVRD [3:3]
// FIELD: RX_PLL_EN [2:2]
// FIELD: RX_INVERT_OVRD [1:1]
// FIELD: RX_INVERT [0:0]
*/

#define SSP_CR_LANE0_DIG_RX_ASIC_OUT 0x100D
/* FIELD: LOS [2:2]
// FIELD: PLL_STATE [1:1]
// FIELD: VALID [0:0]
*/

/* control bus bit definition */
#define PCIE_CR_CTL_DATA_LOC 0
#define PCIE_CR_CTL_CAP_ADR_LOC 16
#define PCIE_CR_CTL_CAP_DAT_LOC 17
#define PCIE_CR_CTL_WR_LOC 18
#define PCIE_CR_CTL_RD_LOC 19
#define PCIE_CR_STAT_DATA_LOC 0
#define PCIE_CR_STAT_ACK_LOC 16

#define PCIE_CAP_STRUC_BaseAddress 0x70

/* Register LNK_CAP */
/* PCIE Link cap */
#define LNK_CAP (PCIE_CAP_STRUC_BaseAddress + 0xc)

/* End of Register Definitions */

enum {
	MemRdWr = 0,
	MemRdLk = 1,
	IORdWr = 2,
	CfgRdWr0 = 4,
	CfgRdWr1 = 5
};

struct imx_pcie_port {
	u8			index;
	u8			root_bus_nr;
	void __iomem		*base;
	void __iomem		*dbi_base;
	spinlock_t		conf_lock;

	char			io_space_name[16];
	char			mem_space_name[16];

	struct resource		res[2];
	struct clk *clk;
};

struct imx_pcie_info {
	struct imx_pcie_port imx_pcie_port[1];
	int num_pcie_ports;

	void __iomem *base;
	void __iomem *dbi_base;
	void __iomem *gpr_base;

	unsigned int pcie_pwr_en;
	unsigned int pcie_rst;
	unsigned int pcie_wake_up;
	unsigned int pcie_dis;
};

static struct imx_pcie_info imx_pcie;

static int pcie_phy_cr_read(int addr, int *data);
static int pcie_phy_cr_write(int addr, int data);
static void change_field(int *in, int start, int end, int val);

/* IMX PCIE GPR configure routines */
static inline void imx_pcie_clrset(u32 mask, u32 val, void __iomem *addr)
{
	writel(((readl(addr) & ~mask) | (val & mask)), addr);
}

static int imx_pcie_setup(int nr, struct pci_sys_data *sys)
{
	struct imx_pcie_port *pp;

	if (nr >= imx_pcie.num_pcie_ports)
		return 0;

	pp = &imx_pcie.imx_pcie_port[nr];
	pp->root_bus_nr = sys->busnr;

	/*
	 * IORESOURCE_IO
	 */
	snprintf(pp->io_space_name, sizeof(pp->io_space_name),
		 "PCIe %d I/O", pp->index);
	pp->io_space_name[sizeof(pp->io_space_name) - 1] = 0;
	pp->res[0].name = pp->io_space_name;
	if (pp->index == 0) {
		pp->res[0].start = PCIE_ARB_BASE_ADDR;
		pp->res[0].end = pp->res[0].start + SZ_1M - 1;
	}
	pp->res[0].flags = IORESOURCE_IO;
	if (request_resource(&ioport_resource, &pp->res[0]))
		panic("Request PCIe IO resource failed\n");
	pci_add_resource_offset(&sys->resources, &pp->res[0], sys->io_offset);

	/*
	 * IORESOURCE_MEM
	 */
	snprintf(pp->mem_space_name, sizeof(pp->mem_space_name),
			"PCIe %d MEM", pp->index);
	pp->mem_space_name[sizeof(pp->mem_space_name) - 1] = 0;
	pp->res[1].name = pp->mem_space_name;
	if (pp->index == 0) {
		pp->res[1].start = PCIE_ARB_BASE_ADDR + SZ_1M;
		pp->res[1].end = pp->res[1].start + SZ_16M - SZ_2M - 1;
	}
	pp->res[1].flags = IORESOURCE_MEM;
	if (request_resource(&iomem_resource, &pp->res[1]))
		panic("Request PCIe Memory resource failed\n");
	pci_add_resource_offset(&sys->resources, &pp->res[1], sys->mem_offset);

	return 1;
}

static int imx_pcie_link_up(void __iomem *dbi_base)
{
	/* Check the pcie link up or link down */
	int iterations = 200;
	u32 rc, ltssm, rx_valid, temp;

	do {
		/* link is debug bit 36 debug 1 start in bit 32 */
		rc = readl(dbi_base + DB_R1) & (0x1 << (36 - 32)) ;
		iterations--;
		usleep_range(2000, 3000);

		/* From L0, initiate MAC entry to gen2 if EP/RC supports gen2.
		 * Wait 2ms (LTSSM timeout is 24ms, PHY lock is ~5us in gen2).
		 * If (MAC/LTSSM.state == Recovery.RcvrLock)
		 * && (PHY/rx_valid==0) then pulse PHY/rx_reset. Transition
		 * to gen2 is stuck
		 */
		pcie_phy_cr_read(SSP_CR_LANE0_DIG_RX_ASIC_OUT, &rx_valid);
		ltssm = readl(dbi_base + DB_R0) & 0x3F;
		if ((ltssm == 0x0D) && ((rx_valid & 0x01) == 0)) {
			pr_info("Transition to gen2 is stuck, reset PHY!\n");
			pcie_phy_cr_read(SSP_CR_LANE0_DIG_RX_OVRD_IN_LO, &temp);
			change_field(&temp, 3, 3, 0x1);
			change_field(&temp, 5, 5, 0x1);
			pcie_phy_cr_write(SSP_CR_LANE0_DIG_RX_OVRD_IN_LO,
					0x0028);
			usleep_range(2000, 3000);
			pcie_phy_cr_read(SSP_CR_LANE0_DIG_RX_OVRD_IN_LO, &temp);
			change_field(&temp, 3, 3, 0x0);
			change_field(&temp, 5, 5, 0x0);
			pcie_phy_cr_write(SSP_CR_LANE0_DIG_RX_OVRD_IN_LO,
					0x0000);
		}

		if ((iterations < 0))
			pr_info("link up failed, DB_R0:0x%08x, DB_R1:0x%08x!\n"
					, readl(dbi_base + DB_R0)
					, readl(dbi_base + DB_R1));
	} while (!rc && iterations);

	if (!rc)
		return 0;
	return 1;
}

static void imx_pcie_regions_setup(void __iomem *dbi_base)
{
	unsigned bus;
	unsigned i;
	unsigned untranslated_base = PCIE_ARB_END_ADDR +1 - SZ_1M;
	void __iomem *p = dbi_base + PCIE_PL_MSIC_INT;
	/*
	 * i.MX6 defines 16MB in the AXI address map for PCIe.
	 *
	 * That address space excepted the pcie registers is
	 * split and defined into different regions by iATU,
	 * with sizes and offsets as follows:
	 *
	 * 0x0100_0000 --- 0x010F_FFFF 1MB IORESOURCE_IO
	 * 0x0110_0000 --- 0x01EF_FFFF 14MB IORESOURCE_MEM
	 * 0x01F0_0000 --- 0x01FF_FFFF 1MB Cfg + Registers
	 */

	/* CMD reg:I/O space, MEM space, and Bus Master Enable */
	writel(readl(dbi_base + PCI_COMMAND)
			| PCI_COMMAND_IO
			| PCI_COMMAND_MEMORY
			| PCI_COMMAND_MASTER,
			dbi_base + PCI_COMMAND);

	/* Set the CLASS_REV of RC CFG header to PCI_CLASS_BRIDGE_PCI */
	writel(readl(dbi_base + PCI_CLASS_REVISION)
			| (PCI_CLASS_BRIDGE_PCI << 16),
			dbi_base + PCI_CLASS_REVISION);

	/*
	 * region0-3 outbound used to access target cfg
	 */
	for (bus = 1; bus <= 4; bus++) {
		writel(bus - 1, dbi_base + ATU_VIEWPORT_R);
		writel(untranslated_base, dbi_base + ATU_REGION_LOWBASE_R);
		untranslated_base += (1 << 18);
		if (bus == 4)
			untranslated_base -= (1 << 14); //(remove registers)
		writel(untranslated_base - 1, dbi_base + ATU_REGION_LIMIT_ADDR_R);
		writel(0, dbi_base + ATU_REGION_UPBASE_R);

		writel(bus << 24, dbi_base + ATU_REGION_LOW_TRGT_ADDR_R);
		writel(0, dbi_base + ATU_REGION_UP_TRGT_ADDR_R);
		writel((bus > 1) ? CfgRdWr1 : CfgRdWr0,
				dbi_base + ATU_REGION_CTRL1_R);
		writel((1<<31), dbi_base + ATU_REGION_CTRL2_R);
	}

	writel(MSI_MATCH_ADDR, dbi_base + PCIE_PL_MSICA);
	writel(0, dbi_base + PCIE_PL_MSICUA);
	for (i = 0; i < 8 ; i++) {
		writel(0, p + MSIC_INT_EN);
		writel(0xffffffff, p + MSIC_INT_MASK);
		writel(0xffffffff, p + MSIC_INT_STATUS);
		p += 12;
	}
}

void imx_pcie_mask_irq(unsigned pos, int set)
{
	unsigned mask = 1 << (pos & 0x1f);
	unsigned val, newval;
	void __iomem *p = imx_pcie.dbi_base + PCIE_PL_MSIC_INT + MSIC_INT_MASK + ((pos >> 5) * 12);
	if (pos >= (8 * 32))
		return;
	val = readl(p);
	if (set)
		newval = val | mask;
	else
		newval = val & ~mask;
	if (val != newval)
		writel(newval, p);
}

void imx_pcie_enable_irq(unsigned pos, int set)
{
	unsigned mask = 1 << (pos & 0x1f);
	unsigned val, newval;
	void __iomem *p = imx_pcie.dbi_base + PCIE_PL_MSIC_INT + MSIC_INT_EN + ((pos >> 5) * 12);
	if (pos >= (8 * 32))
		return;
	val = readl(p);
	if (set)
		newval = val | mask;
	else
		newval = val & ~mask;
	if (val != newval)
		writel(newval, p);
	if (set && (val != newval))
		imx_pcie_mask_irq(pos, 0);	/* unmask when enabled */
}

unsigned imx_pcie_msi_pending(unsigned index)
{
	unsigned val, mask;
	void __iomem *p = imx_pcie.dbi_base + PCIE_PL_MSIC_INT + (index * 12);
	if (index >= 8)
		return 0;
	val = readl(p + MSIC_INT_STATUS);
	mask = readl(p + MSIC_INT_MASK);
	val &= ~mask;
	writel(val, p + MSIC_INT_STATUS);
	return val;
}

static char master_abort(struct pci_bus *bus, u32 devfn, int where)
{
	u32 reg;
	void __iomem *dbi_base = imx_pcie.dbi_base;
	int ret = 0;

 	reg = readl(dbi_base + PCIE_RC_IOBLSSR);
	if (reg & 0x71000000) {
		if (reg & 1<<30)
			pr_err("%d:%02d.%d 0x%04x: parity error\n", bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn), where);
		if (reg & 1<<29) {
			pr_err("%d:%02d.%d 0x%04x: master abort\n", bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn), where);
			ret = 1;
		}
		if (reg & 1<<28)
			pr_err("%d:%02d.%d 0x%04x: target abort\n", bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn), where);
		if (reg & 1<<24)
			pr_err("%d:%02d.%d 0x%04x: master data parity error\n", bus->number, PCI_SLOT(devfn), PCI_FUNC(devfn), where);
		writel(reg, dbi_base + PCIE_RC_IOBLSSR);
		udelay(1500); // without this delay subsequent reads through bridge can erroneously return 0???
	}
	return ret;
}

static volatile void *get_cfg_addr(struct pci_bus *bus, u32 devfn, int where)
{
	unsigned busnum;
	void __iomem *base = imx_pcie.base;
	void __iomem *dbi_base = imx_pcie.dbi_base;

	if (!bus->number) {
		if (devfn != 0)
			return 0;
		return (imx_pcie.dbi_base) + (where & 0x0ffc);
	}
	if ((devfn > 0xff) || (bus->number > 15))
		return 0;
	busnum = bus->number - 1;
	if ((busnum < 3) && (devfn <= 3)) {
		return (base) + (busnum << 18) + (devfn << 16) + (where & 0xfffc);
	}
	writel(3, dbi_base + ATU_VIEWPORT_R);
	writel((bus->number << 24) | (devfn << 16),
			dbi_base + ATU_REGION_LOW_TRGT_ADDR_R);
	writel((bus->number > 1) ? CfgRdWr1 : CfgRdWr0,
			dbi_base + ATU_REGION_CTRL1_R);
	return (base) + (3 << 18) + (where & 0xfffc);
}

static int imx_pcie_rd_conf(struct pci_bus *bus, u32 devfn, int where,
			int size, u32 *val)
{
	const volatile void *va_address;
	u32 v;

	if (0)
		pr_info("%s: bus=%x, devfn=%x, where=%x size=%x\n", __func__, bus->number, devfn, where, size);
	va_address = get_cfg_addr(bus, devfn, where);
	if (!va_address) {
		*val = 0xffffffff;
		return PCIBIOS_DEVICE_NOT_FOUND;
	}
	v = readl(va_address);
	if (master_abort(bus, devfn, where)) {
		return PCIBIOS_DEVICE_NOT_FOUND;	
	}
	if (0)
		pr_info("%s: bus=%x, devfn=%x, where=%x size=%x v=%x\n", __func__, bus->number, devfn, where, size, v);
	if (size == 4) {
		*val = v;
	} else if (size == 1) {
		*val = (v >> (8 * (where & 3))) & 0xFF;
	} else if (size == 2) {
		*val = (v >> (8 * (where & 3))) & 0xFFFF;
	} else {
		*val = 0xffffffff;
		return PCIBIOS_BAD_REGISTER_NUMBER;
	}
	return PCIBIOS_SUCCESSFUL;
}

static int imx_pcie_wr_conf(struct pci_bus *bus, u32 devfn,
			int where, int size, u32 val)
{
	volatile void *va_address;
	u32 mask, tmp;

	if (0)
		pr_info("%s: bus=%x, devfn=%x, where=%x size=%x val=%x\n", __func__, bus->number, devfn, where, size, val);
	va_address = get_cfg_addr(bus, devfn, where);
	if (!va_address)
		return PCIBIOS_DEVICE_NOT_FOUND;
	if (size == 4) {
		writel(val, va_address);
		return (master_abort(bus, devfn, where))
			?PCIBIOS_DEVICE_NOT_FOUND:PCIBIOS_SUCCESSFUL;
	}
	if (size == 2)
		mask = ~(0xFFFF << ((where & 0x3) * 8));
	else if (size == 1)
		mask = ~(0xFF << ((where & 0x3) * 8));
	else
		return PCIBIOS_BAD_REGISTER_NUMBER;

	tmp = readl(va_address) & mask;
	tmp |= val << ((where & 0x3) * 8);
	writel(tmp, va_address);
	return (master_abort(bus, devfn, where))
		?PCIBIOS_DEVICE_NOT_FOUND:PCIBIOS_SUCCESSFUL;
}

static struct pci_ops imx_pcie_ops = {
	.read = imx_pcie_rd_conf,
	.write = imx_pcie_wr_conf,
};

signed short irq_map[] = {
	-EINVAL,
	MXC_INT_PCIE_3,		/* int a */
	MXC_INT_PCIE_2,		/* int b */
	MXC_INT_PCIE_1,		/* int c */
	MXC_INT_PCIE_0,		/* int d/MSI */
};

static int imx_pcie_map_irq(const struct pci_dev *dev, u8 slot, u8 pin)
{
	int val = -EINVAL;
	if (pin <= 4)
		val = irq_map[pin];
	return val;
}

static struct hw_pci imx_pci __initdata = {
	.nr_controllers	= 1,
	.setup		= imx_pcie_setup,
	.ops    = &imx_pcie_ops,
	.map_irq	= imx_pcie_map_irq,
};

/* PHY CR bus acess routines */
static int pcie_phy_cr_ack_polling(int max_iterations, int exp_val)
{
	u32 temp_rd_data, wait_counter = 0;

	do {
		temp_rd_data = readl(imx_pcie.dbi_base + PHY_STS_R);
		temp_rd_data = (temp_rd_data >> PCIE_CR_STAT_ACK_LOC) & 0x1;
		wait_counter++;
	} while ((wait_counter < max_iterations) && (temp_rd_data != exp_val));

	if (temp_rd_data != exp_val)
		return 0 ;
	return 1 ;
}

static int pcie_phy_cr_cap_addr(int addr)
{
	u32 temp_wr_data;
	void __iomem *dbi_base = imx_pcie.dbi_base;

	/* write addr */
	temp_wr_data = addr << PCIE_CR_CTL_DATA_LOC ;
	writel(temp_wr_data, dbi_base + PHY_CTRL_R);

	/* capture addr */
	temp_wr_data |= (0x1 << PCIE_CR_CTL_CAP_ADR_LOC);
	writel(temp_wr_data, dbi_base + PHY_CTRL_R);

	/* wait for ack */
	if (!pcie_phy_cr_ack_polling(100, 1))
		return 0;

	/* deassert cap addr */
	temp_wr_data = addr << PCIE_CR_CTL_DATA_LOC;
	writel(temp_wr_data, dbi_base + PHY_CTRL_R);

	/* wait for ack de-assetion */
	if (!pcie_phy_cr_ack_polling(100, 0))
		return 0 ;

	return 1 ;
}

static int pcie_phy_cr_read(int addr , int *data)
{
	u32 temp_rd_data, temp_wr_data;
	void __iomem *dbi_base = imx_pcie.dbi_base;

	/*  write addr */
	/* cap addr */
	if (!pcie_phy_cr_cap_addr(addr))
		return 0;

	/* assert rd signal */
	temp_wr_data = 0x1 << PCIE_CR_CTL_RD_LOC;
	writel(temp_wr_data, dbi_base + PHY_CTRL_R);

	/* wait for ack */
	if (!pcie_phy_cr_ack_polling(100, 1))
		return 0;

	/* after got ack return data */
	temp_rd_data = readl(dbi_base + PHY_STS_R);
	*data = (temp_rd_data & (0xffff << PCIE_CR_STAT_DATA_LOC)) ;

	/* deassert rd signal */
	temp_wr_data = 0x0;
	writel(temp_wr_data, dbi_base + PHY_CTRL_R);

	/* wait for ack de-assetion */
	if (!pcie_phy_cr_ack_polling(100, 0))
		return 0 ;

	return 1 ;

}

static int pcie_phy_cr_write(int addr, int data)
{
	u32 temp_wr_data;
	void __iomem *dbi_base = imx_pcie.dbi_base;

	/* write addr */
	/* cap addr */
	if (!pcie_phy_cr_cap_addr(addr))
		return 0 ;

	temp_wr_data = data << PCIE_CR_CTL_DATA_LOC;
	writel(temp_wr_data, dbi_base + PHY_CTRL_R);

	/* capture data */
	temp_wr_data |= (0x1 << PCIE_CR_CTL_CAP_DAT_LOC);
	writel(temp_wr_data, dbi_base + PHY_CTRL_R);

	/* wait for ack */
	if (!pcie_phy_cr_ack_polling(100, 1))
		return 0 ;

	/* deassert cap data */
	temp_wr_data = data << PCIE_CR_CTL_DATA_LOC;
	writel(temp_wr_data, dbi_base + PHY_CTRL_R);

	/* wait for ack de-assetion */
	if (!pcie_phy_cr_ack_polling(100, 0))
		return 0;

	/* assert wr signal */
	temp_wr_data = 0x1 << PCIE_CR_CTL_WR_LOC;
	writel(temp_wr_data, dbi_base + PHY_CTRL_R);

	/* wait for ack */
	if (!pcie_phy_cr_ack_polling(100, 1))
		return 0;

	/* deassert wr signal */
	temp_wr_data = data << PCIE_CR_CTL_DATA_LOC;
	writel(temp_wr_data, dbi_base + PHY_CTRL_R);

	/* wait for ack de-assetion */
	if (!pcie_phy_cr_ack_polling(100, 0))
		return 0;

	temp_wr_data = 0x0 ;
	writel(temp_wr_data, dbi_base + PHY_CTRL_R);

	return 1 ;
}

static void change_field(int *in, int start, int end, int val)
{
	int mask;

	mask = ((0xFFFFFFFF << start) ^ (0xFFFFFFFF << (end + 1))) & 0xFFFFFFFF;
	*in = (*in & ~mask) | (val << start);
}

static int imx_pcie_enable_controller(struct device *dev)
{
	struct clk *clk;
	struct device_node *np = dev->of_node;

	if (gpio_is_valid(imx_pcie.pcie_pwr_en)) {
		/* Enable PCIE power */
		gpio_request(imx_pcie.pcie_pwr_en, "PCIE POWER_EN");

		/* activate PCIE_PWR_EN */
		gpio_direction_output(imx_pcie.pcie_pwr_en, 1);
	}

	// power up PCIe PHY
	imx_pcie_clrset(iomuxc_gpr1_test_powerdown, 0 << 18, IOMUXC_GPR1);

	/* enable the clks */
	if (np)
		clk = of_clk_get(np, 0);
	else
		clk = devm_clk_get(dev, "pcie_clk");
	if (IS_ERR(clk)) {
		pr_err("no pcie clock.\n");
		return -EINVAL;
	}

	if (clk_prepare_enable(clk)) {
		pr_err("can't enable pcie clock.\n");
		clk_put(clk);
		return -EINVAL;
	}

	// Enable PCIe PHY ref clock
	imx_pcie_clrset(iomuxc_gpr1_pcie_ref_clk_en, 1 << 16, IOMUXC_GPR1);

	return 0;
}

static void card_reset(struct device *dev)
{
	if (gpio_is_valid(imx_pcie.pcie_rst)) {
		/* PCIE RESET */
		gpio_request(imx_pcie.pcie_rst, "PCIE RESET");

		/* activate PERST_B */
		gpio_direction_output(imx_pcie.pcie_rst, 0);

		/* Add one reset to the pcie external device */
		msleep(100);

		/* deactive PERST_B */
		gpio_direction_output(imx_pcie.pcie_rst, 1);
	}
}

static void add_pcie_port(struct platform_device *pdev, void __iomem *base, void __iomem *dbi_base)
{
	struct clk *clk;
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;

	if (imx_pcie_link_up(dbi_base)) {
		struct imx_pcie_port *pp = &imx_pcie.imx_pcie_port[imx_pcie.num_pcie_ports++];

		pr_info("IMX PCIe port: link up.\n");

		pp->index = 0;
		pp->root_bus_nr = -1;
		pp->base = base;
		pp->dbi_base = dbi_base;
		spin_lock_init(&pp->conf_lock);
		memset(pp->res, 0, sizeof(pp->res));
	} else {
		pr_info("IMX PCIe port: link down!\n");
		/* Release the clocks, and disable the power */

		if (np)
			clk = of_clk_get(np, 0);
		else
			clk = clk_get(NULL, "pcie_clk");
		if (IS_ERR(clk)) {
			pr_err("no pcie clock.\n");
			return;
		}

		clk_disable_unprepare(clk);
		clk_put(clk);

		// Disable the PCIE PHY Ref Clock
		imx_pcie_clrset(iomuxc_gpr1_pcie_ref_clk_en, 0 << 16, IOMUXC_GPR1);

		if (gpio_is_valid(imx_pcie.pcie_pwr_en)) {
			/* Disable PCIE power */
			gpio_request(imx_pcie.pcie_pwr_en, "PCIE POWER_EN");

			/* activate PCIE_PWR_EN */
			gpio_direction_output(imx_pcie.pcie_pwr_en, 0);
		}

		// Power down PCIE PHY
		imx_pcie_clrset(iomuxc_gpr1_test_powerdown, 1 << 18, IOMUXC_GPR1);
	}
}

static int imx_pcie_abort_handler(unsigned long addr, unsigned int fsr,
		struct pt_regs *regs)
{
	unsigned long instr;
	unsigned long pc = instruction_pointer(regs) - 4;

	instr = *(unsigned long *)pc;
/* imprecise aborts are no longer enabled in 3.7+ during init it would appear.
 * We now using PCIE_RC_IOBLSSR to detect master abort however we will still get
 * at least one imprecise abort and need to have a handler.
 */
#if 0
	if (instr == 0xf57ff04f) {
		/* dsb sy */
		pc -= 4;
		instr = *(unsigned long *)pc;
	}
	pr_info("PCIe abort: address = 0x%08lx fsr = 0x%03x PC = 0x%08lx LR = 0x%08lx instr=%08lx\n",
		addr, fsr, regs->ARM_pc, regs->ARM_lr, instr);


	/*
	 * If the instruction being executed was a read,
	 * make it look like it read all-ones.
	 */
	if ((instr & 0x0c500000) == 0x04100000) {
		/* LDR instruction */
		int reg = (instr >> 12) & 15;

		regs->uregs[reg] = -1;
		regs->ARM_pc = pc + 4;
		return 0;
	}
	return 1;
#else
	pr_info("PCIe abort: address = 0x%08lx fsr = 0x%03x PC = 0x%08lx LR = 0x%08lx instr=%08lx\n",
		addr, fsr, regs->ARM_pc, regs->ARM_lr, instr);

	return 0;
#endif
}


static int imx_pcie_pltfm_probe(struct platform_device *pdev)
{
	struct resource *mem;
	struct device *dev = &pdev->dev;
	struct device_node *np = pdev->dev.of_node;
	struct resource res;
	int ret;

	if (!np) {
		dev_err(&pdev->dev, "No of data found\n");
		return -EINVAL;
	}

	res.start = res.end = 0;
	ret = of_address_to_resource(np, 0, &res);
	if (ret)
		goto err;
	mem = &res;
	imx_pcie.pcie_pwr_en = of_get_named_gpio(np, "pwren-gpios", 0);
	imx_pcie.pcie_rst = of_get_named_gpio(np, "rst-gpios", 0);
	imx_pcie.pcie_wake_up = of_get_named_gpio(np, "wake-gpios", 0);
	imx_pcie.pcie_dis = of_get_named_gpio(np, "dis-gpios", 0);
	//pdev->dev.platform_data = pdata;

	imx_pcie.base = ioremap_nocache(PCIE_ARB_END_ADDR - SZ_1M + 1, SZ_1M - SZ_16K);
	if (!imx_pcie.base) {
		pr_err("error with ioremap in function %s\n", __func__);
		return -EIO;
	}

	imx_pcie.dbi_base = devm_ioremap(dev, mem->start, resource_size(mem));
	if (!imx_pcie.dbi_base) {
		dev_err(dev, "can't map %pR\n", mem);
		return -ENOMEM;
	}

	np = of_find_compatible_node(NULL, NULL, "fsl,imx6q-iomuxc-gpr");
	if (!np) {
		dev_err(dev, "can't find iomux\n");
		return -ENOMEM;
	}
	ret = of_address_to_resource(np, 0, &res);
	of_node_put(np);
	if (ret)
		goto err;
	mem = &res;
	imx_pcie.gpr_base = devm_ioremap(dev, mem->start, resource_size(mem));
	if (!imx_pcie.gpr_base) {
		dev_err(dev, "can't map %pR\n", mem);
		return -ENOMEM;
	}

	// hold LTSSM in detect state
	imx_pcie_clrset(iomuxc_gpr12_app_ltssm_enable, 0 << 10, IOMUXC_GPR12);

	/* configure constant input signal to the pcie ctrl and phy */
	// set device type to RC (PCI_EXP_TYPE_ROOT_PORT=4 is from pcie_regs.h)
	imx_pcie_clrset(iomuxc_gpr12_device_type, PCI_EXP_TYPE_ROOT_PORT << 12, IOMUXC_GPR12);
	// loss of signal detect sensitivity function - must be 0x9
	imx_pcie_clrset(iomuxc_gpr12_los_level, 9 << 4, IOMUXC_GPR12);
	// not clear what values these should have from RM
	imx_pcie_clrset(iomuxc_gpr8_tx_deemph_gen1, 0 << 0, IOMUXC_GPR8);
	imx_pcie_clrset(iomuxc_gpr8_tx_deemph_gen2_3p5db, 0 << 6, IOMUXC_GPR8);
	imx_pcie_clrset(iomuxc_gpr8_tx_deemph_gen2_6db, 20 << 12, IOMUXC_GPR8);
	imx_pcie_clrset(iomuxc_gpr8_tx_swing_full, 127 << 18, IOMUXC_GPR8);
	imx_pcie_clrset(iomuxc_gpr8_tx_swing_low, 127 << 25, IOMUXC_GPR8);

	/* Enable the pwr, clks and so on */
	ret = imx_pcie_enable_controller(dev);
	if (ret)
		goto err;

	/* togle the external card's reset */
	card_reset(dev) ;

	usleep_range(3000, 4000);
	imx_pcie_regions_setup(imx_pcie.dbi_base);
	usleep_range(3000, 4000);

	/*
	 * Force to GEN1 because of PCIE2USB storage stress tests
	 * would be failed when GEN2 is enabled
	 */
	writel(((readl(imx_pcie.dbi_base + LNK_CAP) & 0xfffffff0) | 0x1),
			imx_pcie.dbi_base + LNK_CAP);

	/* start link up */
	imx_pcie_clrset(iomuxc_gpr12_app_ltssm_enable, 1 << 10, IOMUXC_GPR12);

	hook_fault_code(16 + 6, imx_pcie_abort_handler, SIGBUS, 0,
			"imprecise external abort");

	/* add the pcie port */
	add_pcie_port(pdev, imx_pcie.base, imx_pcie.dbi_base);

	pci_common_init(&imx_pci);

	return 0;

err:
	return ret;
}

static int imx_pcie_pltfm_remove(struct platform_device *pdev)
{
	struct clk *clk;
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct resource *iomem = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	/* Release clocks, and disable power  */
	if (np)
		clk = of_clk_get(np, 0);
	else
		clk = devm_clk_get(dev, "pcie_clk");
	if (IS_ERR(clk))
		pr_err("no pcie clock.\n");

	if (clk) {
		clk_disable_unprepare(clk);
		clk_put(clk);
	}

	// disable PCIe PHY clock ref
	imx_pcie_clrset(iomuxc_gpr1_pcie_ref_clk_en, 0 << 16, IOMUXC_GPR1);

	if (gpio_is_valid(imx_pcie.pcie_pwr_en)) {
		/* Disable PCIE power */
		gpio_request(imx_pcie.pcie_pwr_en, "PCIE POWER_EN");

		/* activate PCIE_PWR_EN */
		gpio_direction_output(imx_pcie.pcie_pwr_en, 0);
	}

	// power down PCIe PHY
	imx_pcie_clrset(iomuxc_gpr1_test_powerdown, 1 << 18, IOMUXC_GPR1);

	iounmap(imx_pcie.base);
	iounmap(imx_pcie.dbi_base);
	iounmap(imx_pcie.gpr_base);
	release_mem_region(iomem->start, resource_size(iomem));
	//platform_set_drvdata(pdev, NULL);

	return 0;
}

static const struct of_device_id of_imx_pcie_match[] = {
	{ .compatible = "fsl,pcie" },
	{}
};
MODULE_DEVICE_TABLE(of, of_imx_pcie_match);

static struct platform_driver imx_pcie_pltfm_driver = {
	.driver = {
		.name	= "imx-pcie",
		.owner	= THIS_MODULE,
		.of_match_table = of_imx_pcie_match,
	},
	.probe		= imx_pcie_pltfm_probe,
	.remove		= imx_pcie_pltfm_remove,
};

/*****************************************************************************\
 *                                                                           *
 * Driver init/exit                                                          *
 *                                                                           *
\*****************************************************************************/

static int __init imx_pcie_drv_init(void)
{
	pcibios_min_io = 0;
	pcibios_min_mem = 0;

	return platform_driver_register(&imx_pcie_pltfm_driver);
}

static void __exit imx_pcie_drv_exit(void)
{
	platform_driver_unregister(&imx_pcie_pltfm_driver);
}

//module_init(imx_pcie_drv_init);
//module_exit(imx_pcie_drv_exit);
late_initcall(imx_pcie_drv_init);

MODULE_DESCRIPTION("i.MX PCIE platform driver");
MODULE_LICENSE("GPL v2");
