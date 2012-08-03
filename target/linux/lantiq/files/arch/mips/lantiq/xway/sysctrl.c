/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2011 John Crispin <blogic@openwrt.org>
 */

#include <linux/ioport.h>
#include <linux/export.h>
#include <linux/clkdev.h>

#include <lantiq_soc.h>

#include "../clk.h"
#include "../devices.h"

/* clock control register */
#define CGU_IFCCR	0x0018
/* system clock register */
#define CGU_SYS		0x0010
/* pci control register */
#define CGU_PCICR	0x0034
/* ephy configuration register */
#define CGU_EPHY	0x10
/* power control register */
#define PMU_PWDCR	0x1C
/* power status register */
#define PMU_PWDSR	0x20
/* power control register */
#define PMU_PWDCR1	0x24
/* power status register */
#define PMU_PWDSR1	0x28
/* power control register */
#define PWDCR(x) ((x) ? (PMU_PWDCR1) : (PMU_PWDCR))
/* power status register */
#define PWDSR(x) ((x) ? (PMU_PWDSR1) : (PMU_PWDSR))

/* PMU - power management unit */
#define PMU_USB0_P	BIT(0)
#define PMU_PCI		BIT(4)
#define PMU_DMA		BIT(5)
#define PMU_USB0	BIT(6)
#define PMU_EPHY	BIT(7)	/* ase */
#define PMU_SPI		BIT(8)
#define PMU_DFE		BIT(9)
#define PMU_EBU		BIT(10)
#define PMU_STP		BIT(11)
#define PMU_GPT		BIT(12)
#define PMU_PPE		BIT(13)
#define PMU_AHBS	BIT(13) /* vr9 */
#define PMU_FPI		BIT(14)
#define PMU_AHBM	BIT(15)
#define PMU_PPE_QSB	BIT(18)
#define PMU_PPE_SLL01	BIT(19)
#define PMU_PPE_TC	BIT(21)
#define PMU_PPE_EMA	BIT(22)
#define PMU_PPE_DPLUM	BIT(23)
#define PMU_PPE_DPLUS	BIT(24)
#define PMU_USB1_P	BIT(26)
#define PMU_USB1	BIT(27)
#define PMU_SWITCH	BIT(28)
#define PMU_PPE_TOP	BIT(29)
#define PMU_GPHY	BIT(30)
#define PMU_PCIE_CLK	BIT(31)

#define PMU1_PCIE_PHY	BIT(0)
#define PMU1_PCIE_CTL	BIT(1)
#define PMU1_PCIE_PDI	BIT(4)
#define PMU1_PCIE_MSI	BIT(5)

#define ltq_pmu_w32(x, y)	ltq_w32((x), ltq_pmu_membase + (y))
#define ltq_pmu_r32(x)		ltq_r32(ltq_pmu_membase + (x))

static struct resource ltq_cgu_resource =
	MEM_RES("cgu", LTQ_CGU_BASE_ADDR, LTQ_CGU_SIZE);

static struct resource ltq_pmu_resource =
	MEM_RES("pmu", LTQ_PMU_BASE_ADDR, LTQ_PMU_SIZE);

static struct resource ltq_ebu_resource =
	MEM_RES("ebu", LTQ_EBU_BASE_ADDR, LTQ_EBU_SIZE);

void __iomem *ltq_cgu_membase;
void __iomem *ltq_ebu_membase;
static void __iomem *ltq_pmu_membase;

static int ltq_cgu_enable(struct clk *clk)
{
	ltq_cgu_w32(ltq_cgu_r32(CGU_IFCCR) | clk->bits, CGU_IFCCR);
	return 0;
}

static void ltq_cgu_disable(struct clk *clk)
{
	ltq_cgu_w32(ltq_cgu_r32(CGU_IFCCR) & ~clk->bits, CGU_IFCCR);
}

static int ltq_pmu_enable(struct clk *clk)
{
	int err = 1000000;

	ltq_pmu_w32(ltq_pmu_r32(PWDCR(clk->module)) & ~clk->bits,
		PWDCR(clk->module));
	do {} while (--err && (ltq_pmu_r32(PWDSR(clk->module)) & clk->bits));

	if (!err)
		panic("activating PMU module failed!\n");

	return 0;
}

static void ltq_pmu_disable(struct clk *clk)
{
	ltq_pmu_w32(ltq_pmu_r32(PWDCR(clk->module)) | clk->bits,
		PWDCR(clk->module));
}

static int ltq_pci_enable(struct clk *clk)
{
	unsigned int ifccr = ltq_cgu_r32(CGU_IFCCR);
	/* set clock bus speed */
	if (ltq_is_ar9()) {
		ifccr &= ~0x1f00000;
		if (clk->rate == CLOCK_33M)
			ifccr |= 0xe00000;
		else
			ifccr |= 0x700000; /* 62.5M */
	} else {
		ifccr &= ~0xf00000;
		if (clk->rate == CLOCK_33M)
			ifccr |= 0x800000;
		else
			ifccr |= 0x400000; /* 62.5M */
	}
	ltq_cgu_w32(ifccr, CGU_IFCCR);
	return 0;
}

static int ltq_pci_ext_enable(struct clk *clk)
{
	/* enable external pci clock */
	ltq_cgu_w32(ltq_cgu_r32(CGU_IFCCR) & ~(1 << 16),
		CGU_IFCCR);
	ltq_cgu_w32((1 << 30), CGU_PCICR);
	return 0;
}

static void ltq_pci_ext_disable(struct clk *clk)
{
	/* disable external pci clock (internal) */
	ltq_cgu_w32(ltq_cgu_r32(CGU_IFCCR) | (1 << 16),
		CGU_IFCCR);
	ltq_cgu_w32((1 << 31) | (1 << 30), CGU_PCICR);
}

/* manage the clock gates via PMU */
static inline void clkdev_add_pmu(const char *dev, const char *con,
					unsigned int module, unsigned int bits)
{
	struct clk *clk = kzalloc(sizeof(struct clk), GFP_KERNEL);

	clk->cl.dev_id = dev;
	clk->cl.con_id = con;
	clk->cl.clk = clk;
	clk->enable = ltq_pmu_enable;
	clk->disable = ltq_pmu_disable;
	clk->module = module;
	clk->bits = bits;
	clkdev_add(&clk->cl);
}

/* manage the clock generator */
static inline void clkdev_add_cgu(const char *dev, const char *con,
					unsigned int bits)
{
	struct clk *clk = kzalloc(sizeof(struct clk), GFP_KERNEL);

	clk->cl.dev_id = dev;
	clk->cl.con_id = con;
	clk->cl.clk = clk;
	clk->enable = ltq_cgu_enable;
	clk->disable = ltq_cgu_disable;
	clk->bits = bits;
	clkdev_add(&clk->cl);
}

/* pci needs its own enable function */
static inline void clkdev_add_pci(void)
{
	struct clk *clk = kzalloc(sizeof(struct clk), GFP_KERNEL);
	struct clk *clk_ext = kzalloc(sizeof(struct clk), GFP_KERNEL);

	/* main pci clock */
	clk->cl.dev_id = "ltq_pci";
	clk->cl.con_id = NULL;
	clk->cl.clk = clk;
	clk->rate = CLOCK_33M;
	clk->enable = ltq_pci_enable;
	clk->disable = ltq_pmu_disable;
	clk->module = 0;
	clk->bits = PMU_PCI;
	clkdev_add(&clk->cl);

	/* use internal/external bus clock */
	clk_ext->cl.dev_id = "ltq_pci";
	clk_ext->cl.con_id = "external";
	clk_ext->cl.clk = clk_ext;
	clk_ext->enable = ltq_pci_ext_enable;
	clk_ext->disable = ltq_pci_ext_disable;
	clkdev_add(&clk_ext->cl);

}

void __init ltq_soc_init(void)
{
	ltq_pmu_membase = ltq_remap_resource(&ltq_pmu_resource);
	if (!ltq_pmu_membase)
		panic("Failed to remap pmu memory\n");

	ltq_cgu_membase = ltq_remap_resource(&ltq_cgu_resource);
	if (!ltq_cgu_membase)
		panic("Failed to remap cgu memory\n");

	ltq_ebu_membase = ltq_remap_resource(&ltq_ebu_resource);
	if (!ltq_ebu_membase)
		panic("Failed to remap ebu memory\n");

	/* make sure to unprotect the memory region where flash is located */
	ltq_ebu_w32(ltq_ebu_r32(LTQ_EBU_BUSCON0) & ~EBU_WRDIS, LTQ_EBU_BUSCON0);

	/* add our clocks */
	clkdev_add_pmu("ltq_fpi", NULL, 0, PMU_FPI);
	clkdev_add_pmu("ltq_dma", NULL, 0, PMU_DMA);
	clkdev_add_pmu("ltq_stp", NULL, 0, PMU_STP);
	clkdev_add_pmu("ltq_spi.0", NULL, 0, PMU_SPI);
        clkdev_add_pmu("ltq_gptu", NULL, 0, PMU_GPT);
        clkdev_add_pmu("ltq_ebu", NULL, 0, PMU_EBU);
	if (!ltq_is_vr9())
		clkdev_add_pmu("ltq_etop", NULL, 0, PMU_PPE);
	if (!ltq_is_ase())
		clkdev_add_pci();
	if (ltq_is_ase()) {
		if (ltq_cgu_r32(CGU_SYS) & (1 << 5))
			clkdev_add_static(CLOCK_266M, CLOCK_133M, CLOCK_133M);
		else
			clkdev_add_static(CLOCK_133M, CLOCK_133M, CLOCK_133M);
		clkdev_add_cgu("ltq_etop", "ephycgu", CGU_EPHY),
		clkdev_add_pmu("ltq_etop", "ephy", 0, PMU_EPHY);
		clkdev_add_pmu("ltq_dsl", NULL, 0,
			PMU_PPE_EMA | PMU_PPE_TC | PMU_PPE_SLL01 |
			PMU_AHBS | PMU_DFE);
	} else if (ltq_is_vr9()) {
		clkdev_add_static(ltq_vr9_cpu_hz(), ltq_vr9_fpi_hz(),
			ltq_vr9_fpi_hz());
		clkdev_add_pmu("ltq_pcie", "phy", 1, PMU1_PCIE_PHY);
		clkdev_add_pmu("ltq_pcie", "bus", 0, PMU_PCIE_CLK);
		clkdev_add_pmu("ltq_pcie", "msi", 1, PMU1_PCIE_MSI);
		clkdev_add_pmu("ltq_pcie", "pdi", 1, PMU1_PCIE_PDI);
		clkdev_add_pmu("ltq_pcie", "ctl", 1, PMU1_PCIE_CTL);
		clkdev_add_pmu("ltq_pcie", "ahb", 0, PMU_AHBM | PMU_AHBS);
		clkdev_add_pmu("usb0", NULL, 0, PMU_USB0 | PMU_USB0_P);
		clkdev_add_pmu("usb1", NULL, 0, PMU_USB1 | PMU_USB1_P);
		clkdev_add_pmu("ltq_vrx200", NULL, 0,
			PMU_SWITCH | PMU_PPE_DPLUS | PMU_PPE_DPLUM |
			PMU_PPE_EMA | PMU_PPE_TC | PMU_PPE_SLL01 |
			PMU_PPE_QSB);
		clkdev_add_pmu("ltq_dsl", NULL, 0, PMU_DFE | PMU_AHBS);
	} else if (ltq_is_ar9()) {
		clkdev_add_static(ltq_ar9_cpu_hz(), ltq_ar9_fpi_hz(),
			ltq_ar9_fpi_hz());
		clkdev_add_pmu("ltq_etop", "switch", 0, PMU_SWITCH);
		clkdev_add_pmu("ltq_dsl", NULL, 0,
			PMU_PPE_EMA | PMU_PPE_TC | PMU_PPE_SLL01 |
			PMU_PPE_QSB | PMU_AHBS | PMU_DFE);
	} else {
		clkdev_add_static(ltq_danube_cpu_hz(), ltq_danube_fpi_hz(),
			ltq_danube_io_region_clock());
		clkdev_add_pmu("ltq_dsl", NULL, 0,
			PMU_PPE_EMA | PMU_PPE_TC | PMU_PPE_SLL01 |
			PMU_PPE_QSB | PMU_AHBS | PMU_DFE);
	}
}
