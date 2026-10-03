// SPDX-License-Identifier: GPL-2.0
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/reboot.h>
#include <linux/io.h>
#include <linux/sizes.h>
#include <linux/smp.h>
#include <linux/spinlock.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/smp_scu.h>
#include <asm/smp_plat.h>

#define GG_IO_PHYS		0xF0000000
#define GG_IO_VIRT_OFFSET	0x07000000
#define GG_IO_VIRT		(GG_IO_PHYS + GG_IO_VIRT_OFFSET)
#define GG_IO_SIZE		SZ_8M
#define GG_GLOBAL_SOFTWARE	0xF00000B8
#define GG_SCU_BASE		0xF8000000
#define GG_SCU_VIRT		0xF7800000
#define GG_SCU_SIZE		SZ_8K
#define GG_GLOBAL_CONFIG	(GG_IO_VIRT + 0x10)
#define GG_GLOBAL_SCRATCH	(GG_IO_VIRT + 0xC0)
#define GG_TWD_WDT_LOAD		(GG_SCU_VIRT + 0x620)
#define GG_TWD_WDT_CTRL		(GG_SCU_VIRT + 0x628)
#define GG_TWD_WDT_RSTSTAT	(GG_SCU_VIRT + 0x630)
#define GG_TWD_WDT_DISABLE	(GG_SCU_VIRT + 0x634)

static struct map_desc cortina_io_desc[] __initdata = {
	{
		.virtual = GG_IO_VIRT,
		.pfn     = __phys_to_pfn(GG_IO_PHYS),
		.length  = GG_IO_SIZE,
		.type    = MT_DEVICE,
	},
	{
		.virtual = GG_SCU_VIRT,
		.pfn     = __phys_to_pfn(GG_SCU_BASE),
		.length  = GG_SCU_SIZE,
		.type    = MT_DEVICE,
	},
};

static void cortina_restart(enum reboot_mode mode, const char *cmd)
{
	u32 val;

	val = readl((void __iomem *)GG_GLOBAL_CONFIG);
	val &= ~0x00000300;
	val &= ~0x0000000E;
	val |= 0x000000F0;
	writel(val, (void __iomem *)GG_GLOBAL_CONFIG);

	val = readl((void __iomem *)GG_GLOBAL_SCRATCH);
	writel(val | 0x400, (void __iomem *)GG_GLOBAL_SCRATCH);
	writel(val & ~0x400, (void __iomem *)GG_GLOBAL_SCRATCH);

	writel(0, (void __iomem *)GG_TWD_WDT_CTRL);
	writel(10, (void __iomem *)GG_TWD_WDT_LOAD);
	writel(0x0000FF09, (void __iomem *)GG_TWD_WDT_CTRL);
	mdelay(1000);
}

static void __init cortina_map_io(void)
{
	iotable_init(cortina_io_desc, ARRAY_SIZE(cortina_io_desc));
}

#define GG_GLOBAL_GPIO_MUX_2	(GG_IO_VIRT + 0x24)

static void __init cortina_init_early(void)
{
	u32 ctrl = readl((void __iomem *)GG_TWD_WDT_CTRL);
	u32 mux;

	mux = readl((void __iomem *)GG_GLOBAL_GPIO_MUX_2);
	mux &= ~0x00003F00;
	writel(mux, (void __iomem *)GG_GLOBAL_GPIO_MUX_2);

	if (!(ctrl & 1))
		return;
	writel(0x12345678, (void __iomem *)GG_TWD_WDT_DISABLE);
	writel(0x87654321, (void __iomem *)GG_TWD_WDT_DISABLE);
	writel(0, (void __iomem *)GG_TWD_WDT_CTRL);
	writel(1, (void __iomem *)GG_TWD_WDT_RSTSTAT);
	pr_info("cortina: bootloader watchdog disarmed (ctrl %08x load %08x)\n",
		ctrl, readl((void __iomem *)GG_TWD_WDT_LOAD));
}

#ifdef CONFIG_SMP
static void __init cortina_smp_prepare_cpus(unsigned int max_cpus)
{
	void __iomem *scu = ioremap(GG_SCU_BASE, SZ_256);

	if (!scu) {
		pr_err("cortina: cannot map SCU\n");
		return;
	}
	scu_enable(scu);
	iounmap(scu);
}

static int cortina_boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	void __iomem *reg = ioremap(GG_GLOBAL_SOFTWARE, 4);

	if (!reg)
		return -ENOMEM;
	writel(__pa_symbol(secondary_startup), reg);
	iounmap(reg);
	dsb_sev();
	arch_send_wakeup_ipi_mask(cpumask_of(cpu));
	return 0;
}

static const struct smp_operations cortina_smp_ops __initconst = {
	.smp_prepare_cpus	= cortina_smp_prepare_cpus,
	.smp_boot_secondary	= cortina_boot_secondary,
};
#endif

static const char * const cortina_cs75xx_dt_compat[] __initconst = {
	"cortina,cs75xx",
	NULL,
};

DT_MACHINE_START(CORTINA_CS75XX_DT, "Cortina CS75xx (Device Tree)")
	.l2c_aux_val	= 0,
	.l2c_aux_mask	= ~0,
	.smp		= smp_ops(cortina_smp_ops),
	.map_io		= cortina_map_io,
	.init_early	= cortina_init_early,
	.restart	= cortina_restart,
	.dt_compat	= cortina_cs75xx_dt_compat,
MACHINE_END
