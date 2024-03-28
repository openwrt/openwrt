#include <linux/bitfield.h>
#include <linux/clk.h>
#include <linux/iopoll.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_platform.h>

#define SE_CLR_RAM_CTRL			0x180004
#define SE_CLR_RAM_CTRL_MASK		GENMASK(20, 0)
#define SE_CONFIG0			0x180008
#define SE_CONFIG0_IPSPL_ZERO_LIMIT	BIT(19)
#define SE_CONFIG0_IPORT_VALID		BIT(8)
#define SE_TB_OP			0x18003c
#define SE_TB_OP_BUSY			BIT(31)
#define SE_TB_OP_WR			BIT(24)
#define SE_TB_OP_REQ_ID			GENMASK(21, 16)
#define SE_TB_OP_REQ_ADDR		GENMASK(15, 0)
#define SE_TB_WRDATA0			0x180040

#define SE_TB_IPORT_ID			1

struct dpns_priv {
	void __iomem *ioaddr;
	struct clk *clk;
};

static int dpns_populate_table(struct dpns_priv *priv)
{
	void __iomem *ioaddr = priv->ioaddr;
	int ret, i;
	u32 reg;

	writel(SE_CLR_RAM_CTRL_MASK, ioaddr + SE_CLR_RAM_CTRL);
	ret = readl_poll_timeout(ioaddr + SE_CLR_RAM_CTRL, reg, !reg, 0, 1000);
	if (ret)
		return ret;

	reg = readl(ioaddr + SE_CONFIG0);
	reg |= SE_CONFIG0_IPORT_VALID;
	reg &= ~SE_CONFIG0_IPSPL_ZERO_LIMIT;
	writel(reg, ioaddr + SE_CONFIG0);

	writel(0xa0000, ioaddr + SE_TB_WRDATA0);
	for (i = 0; i < 6; i++) {
		reg = SE_TB_OP_WR | FIELD_PREP(SE_TB_OP_REQ_ADDR, i) |
		      FIELD_PREP(SE_TB_OP_REQ_ID, SE_TB_IPORT_ID);
		writel(reg, ioaddr + SE_TB_OP);
		ret = readl_poll_timeout(ioaddr + SE_TB_OP, reg,
					 !(reg & SE_TB_OP_BUSY), 0, 100);
		if (ret)
			return ret;
	}

	return 0;
}

static int dpns_probe(struct platform_device *pdev)
{
	struct dpns_priv *priv;

	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	platform_set_drvdata(pdev, priv);

	priv->ioaddr = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(priv->ioaddr))
		return PTR_ERR(priv->ioaddr);

	priv->clk = devm_clk_get_enabled(&pdev->dev, NULL);
	if (IS_ERR(priv->clk))
		return PTR_ERR(priv->clk);

	return dpns_populate_table(priv);
}

static const struct of_device_id dpns_match[] = {
	{ .compatible = "siflower,sf21a6826p-dpns" },
	{},
};
MODULE_DEVICE_TABLE(of, dpns_match);

static struct platform_driver dpns_driver = {
	.probe	= dpns_probe,
	.driver	= {
		.name		= "sfdpns",
		.of_match_table	= dpns_match,
	},
};
module_platform_driver(dpns_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Qingfang Deng <qingfang.deng@siflower.com.cn>");
MODULE_DESCRIPTION("NPU stub driver for SF21A6826P SoC");
