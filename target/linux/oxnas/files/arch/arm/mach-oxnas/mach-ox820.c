#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/bug.h>
#include <linux/of_platform.h>
#include <linux/clocksource.h>
#include <linux/clk-provider.h>
#include <linux/clk.h>
#include <linux/stmmac.h>
#include <linux/slab.h>
#include <linux/gfp.h>
#include <linux/reset.h>
#include <asm/mach-types.h>
#include <asm/mach/map.h>
#include <asm/mach/arch.h>
#include <asm/page.h>
#include <mach/iomap.h>
#include <mach/hardware.h>
#include <mach/utils.h>
#include <mach/smp.h>

static struct map_desc ox820_io_desc[] __initdata = {
	{
		.virtual = (unsigned long)OXNAS_PERCPU_BASE_VA,
		.pfn = __phys_to_pfn(OXNAS_PERCPU_BASE),
		.length = OXNAS_PERCPU_SIZE,
		.type = MT_DEVICE,
	},
	{
		.virtual = (unsigned long)OXNAS_SYSCRTL_BASE_VA,
		.pfn = __phys_to_pfn(OXNAS_SYSCRTL_BASE),
		.length = OXNAS_SYSCRTL_SIZE,
		.type = MT_DEVICE,
	},
	{
		.virtual = (unsigned long)OXNAS_SECCRTL_BASE_VA,
		.pfn = __phys_to_pfn(OXNAS_SECCRTL_BASE),
		.length = OXNAS_SECCRTL_SIZE,
		.type = MT_DEVICE,
	},
	{
		.virtual = (unsigned long)OXNAS_RPSA_BASE_VA,
		.pfn = __phys_to_pfn(OXNAS_RPSA_BASE),
		.length = OXNAS_RPSA_SIZE,
		.type = MT_DEVICE,
	},
	{
		.virtual = (unsigned long)OXNAS_RPSC_BASE_VA,
		.pfn = __phys_to_pfn(OXNAS_RPSC_BASE),
		.length = OXNAS_RPSC_SIZE,
		.type = MT_DEVICE,
	},
};

void __init ox820_map_common_io(void)
{
	debug_ll_io_init();
	iotable_init(ox820_io_desc, ARRAY_SIZE(ox820_io_desc));
}

struct plat_gmac_data {
	struct plat_stmmacenet_data stmmac;
	struct clk *clk;
};

void *ox820_gmac_setup(struct platform_device *pdev)
{
	struct plat_gmac_data *pdata = pdev->dev.platform_data;

	pdata->clk = clk_get(&pdev->dev, "gmac");
	return (void *) pdata->clk;
};

int ox820_gmac_init(struct platform_device *pdev, void *priv)
{
	int ret;
	unsigned value;

	ret = device_reset(&pdev->dev);
	if (ret)
		return ret;

	if (IS_ERR(priv))
		return PTR_ERR(priv);
	clk_prepare_enable(priv);

	value = readl(SYS_CTRL_GMAC_CTRL);

	/* Enable GMII_GTXCLK to follow GMII_REFCLK, required for gigabit PHY */
	value |= BIT(SYS_CTRL_GMAC_CKEN_GTX);
	/* Use simple mux for 25/125 Mhz clock switching */
	value |= BIT(SYS_CTRL_GMAC_SIMPLE_MUX);
	/* set auto switch tx clock source */
	value |= BIT(SYS_CTRL_GMAC_AUTO_TX_SOURCE);
	/* enable tx & rx vardelay */
	value |= BIT(SYS_CTRL_GMAC_CKEN_TX_OUT);
	value |= BIT(SYS_CTRL_GMAC_CKEN_TXN_OUT);
	value |= BIT(SYS_CTRL_GMAC_CKEN_TX_IN);
	value |= BIT(SYS_CTRL_GMAC_CKEN_RX_OUT);
	value |= BIT(SYS_CTRL_GMAC_CKEN_RXN_OUT);
	value |= BIT(SYS_CTRL_GMAC_CKEN_RX_IN);
	writel(value, SYS_CTRL_GMAC_CTRL);

	/* set tx & rx vardelay */
	value = 0;
	value |= SYS_CTRL_GMAC_TX_VARDELAY(4);
	value |= SYS_CTRL_GMAC_TXN_VARDELAY(2);
	value |= SYS_CTRL_GMAC_RX_VARDELAY(10);
	value |= SYS_CTRL_GMAC_RXN_VARDELAY(8);
	writel(value, SYS_CTRL_GMAC_DELAY_CTRL);

	return 0;
}

void ox820_gmac_exit(struct platform_device *pdev, void *priv)
{
	struct reset_control *rstc;

	clk_disable_unprepare(priv);
	clk_put(priv);

	rstc = reset_control_get(&pdev->dev, NULL);
	if (!IS_ERR(rstc)) {
		reset_control_assert(rstc);
		reset_control_put(rstc);
	}
}

static int __init ox820_ether_init(void)
{
	struct device_node *node;
	struct platform_device *pdev;
	struct plat_gmac_data *pdata;

	node = of_find_compatible_node(NULL, NULL, "plxtech,nas782x-gmac");
	if (!node)
		return -ENOENT;

	pdev = of_find_device_by_node(node);
	of_node_put(node);

	if (!pdev)
		return -EINVAL;

	pdata = kzalloc(sizeof(struct plat_gmac_data), GFP_KERNEL);
	if (!pdata)
		return -ENOMEM;

	pdata->stmmac.setup = ox820_gmac_setup;
	pdata->stmmac.init = ox820_gmac_init;
	pdata->stmmac.exit = ox820_gmac_exit;
	pdev->dev.platform_data = pdata;

	return 0;
}

static void __init ox820_dt_init(void)
{
	int ret;

	ret = of_platform_populate(NULL, of_default_bus_match_table, NULL,
					NULL);

	if (ret) {
		pr_err("of_platform_populate failed: %d\n", ret);
		BUG();
	}

	ret = ox820_ether_init();

	if (ret)
		pr_info("ox820_ether_init failed: %d\n", ret);
}

static void __init ox820_timer_init(void)
{
	of_clk_init(NULL);
	clocksource_of_init();
}

void ox820_init_early(void)
{

}

void ox820_assert_system_reset(enum reboot_mode mode, const char *cmd)
{
	u32 value;

/* Assert reset to cores as per power on defaults
 * Don't touch the DDR interface as things will come to an impromptu stop
 * NB Possibly should be asserting reset for PLLB, but there are timing
 *    concerns here according to the docs */
	value = BIT(SYS_CTRL_RST_COPRO)		|
		BIT(SYS_CTRL_RST_USBHS)		|
		BIT(SYS_CTRL_RST_USBHSPHYA)	|
		BIT(SYS_CTRL_RST_MACA)		|
		BIT(SYS_CTRL_RST_PCIEA)		|
		BIT(SYS_CTRL_RST_SGDMA)		|
		BIT(SYS_CTRL_RST_CIPHER)	|
		BIT(SYS_CTRL_RST_SATA)		|
		BIT(SYS_CTRL_RST_SATA_LINK)	|
		BIT(SYS_CTRL_RST_SATA_PHY)	|
		BIT(SYS_CTRL_RST_PCIEPHY)	|
		BIT(SYS_CTRL_RST_STATIC)	|
		BIT(SYS_CTRL_RST_UART1)		|
		BIT(SYS_CTRL_RST_UART2)		|
		BIT(SYS_CTRL_RST_MISC)		|
		BIT(SYS_CTRL_RST_I2S)		|
		BIT(SYS_CTRL_RST_SD)		|
		BIT(SYS_CTRL_RST_MACB)		|
		BIT(SYS_CTRL_RST_PCIEB)		|
		BIT(SYS_CTRL_RST_VIDEO)		|
		BIT(SYS_CTRL_RST_USBHSPHYB)	|
		BIT(SYS_CTRL_RST_USBDEV);

	writel(value, SYS_CTRL_RST_SET_CTRL);

	/* Release reset to cores as per power on defaults */
	writel(BIT(SYS_CTRL_RST_GPIO), SYS_CTRL_RST_CLR_CTRL);

	/* Disable clocks to cores as per power-on defaults - must leave DDR
	 * related clocks enabled otherwise we'll stop rather abruptly. */
	value =
		BIT(SYS_CTRL_CLK_COPRO)		|
		BIT(SYS_CTRL_CLK_DMA)		|
		BIT(SYS_CTRL_CLK_CIPHER)	|
		BIT(SYS_CTRL_CLK_SD)		|
		BIT(SYS_CTRL_CLK_SATA)		|
		BIT(SYS_CTRL_CLK_I2S)		|
		BIT(SYS_CTRL_CLK_USBHS)		|
		BIT(SYS_CTRL_CLK_MAC)		|
		BIT(SYS_CTRL_CLK_PCIEA)		|
		BIT(SYS_CTRL_CLK_STATIC)	|
		BIT(SYS_CTRL_CLK_MACB)		|
		BIT(SYS_CTRL_CLK_PCIEB)		|
		BIT(SYS_CTRL_CLK_REF600)	|
		BIT(SYS_CTRL_CLK_USBDEV);

	writel(value, SYS_CTRL_CLK_CLR_CTRL);

	/* Enable clocks to cores as per power-on defaults */

	/* Set sys-control pin mux'ing as per power-on defaults */
	writel(0, SYS_CTRL_SECONDARY_SEL);
	writel(0, SYS_CTRL_TERTIARY_SEL);
	writel(0, SYS_CTRL_QUATERNARY_SEL);
	writel(0, SYS_CTRL_DEBUG_SEL);
	writel(0, SYS_CTRL_ALTERNATIVE_SEL);
	writel(0, SYS_CTRL_PULLUP_SEL);

	writel(0, SYS_CTRL_SECONDARY_SEL);
	writel(0, SYS_CTRL_TERTIARY_SEL);
	writel(0, SYS_CTRL_QUATERNARY_SEL);
	writel(0, SYS_CTRL_DEBUG_SEL);
	writel(0, SYS_CTRL_ALTERNATIVE_SEL);
	writel(0, SYS_CTRL_PULLUP_SEL);

	/* No need to save any state, as the ROM loader can determine whether
	 * reset is due to power cycling or programatic action, just hit the
	 * (self-clearing) CPU reset bit of the block reset register */
	value =
		BIT(SYS_CTRL_RST_SCU) |
		BIT(SYS_CTRL_RST_ARM0) |
		BIT(SYS_CTRL_RST_ARM1);

	writel(value, SYS_CTRL_RST_SET_CTRL);
}

static const char * const ox820_dt_board_compat[] = {
	"plxtech,nas7820",
	"plxtech,nas7821",
	"plxtech,nas7825",
	NULL
};

DT_MACHINE_START(OX820_DT, "PLXTECH NAS782X SoC (Flattened Device Tree)")
	.map_io		= ox820_map_common_io,
	.smp		= smp_ops(ox820_smp_ops),
	.init_early	= ox820_init_early,
	.init_time	= ox820_timer_init,
	.init_machine	= ox820_dt_init,
	.restart	= ox820_assert_system_reset,
	.dt_compat	= ox820_dt_board_compat,
MACHINE_END
