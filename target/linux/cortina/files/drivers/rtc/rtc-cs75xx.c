// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/bits.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/rtc.h>
#include <linux/bcd.h>
#include <linux/io.h>
#include <linux/of.h>

#define DRV_NAME	"cs75xx-rtc"

#define G2_RTC_RTCCON		0x00
#define G2_RTC_RTCCON_STARTB	BIT(0)
#define G2_RTC_RTCCON_RTCEN	BIT(1)
#define G2_RTC_RTCCON_CLKRST	BIT(2)
#define G2_RTC_RTCCON_OSCEN	BIT(3)

#define G2_RTC_RTCSEC		0x24
#define G2_RTC_RTCMIN		0x28
#define G2_RTC_RTCHOUR		0x2c
#define G2_RTC_RTCDATE		0x30
#define G2_RTC_RTCDAY		0x34
#define G2_RTC_RTCMON		0x38
#define G2_RTC_RTCYEAR		0x3c
struct g2_rtc {
	void __iomem		*base;
	struct rtc_device	*rtc;
};

static u32 g2_rtc_readl(struct g2_rtc *p, unsigned int addr)
{
	return readl(p->base + addr);
}

static u8 g2_rtc_readb(struct g2_rtc *p, unsigned int addr)
{
	return readl(p->base + addr) & 0xff;
}

static void g2_rtc_writel(struct g2_rtc *p, unsigned int addr, u32 val, u32 mask)
{
	u32 reg = (readl(p->base + addr) & ~mask) | (val & mask);

	writel(reg, p->base + addr);
}

static void g2_rtc_write_enable(struct g2_rtc *p)
{
	u32 tmp = g2_rtc_readb(p, G2_RTC_RTCCON);

	g2_rtc_writel(p, G2_RTC_RTCCON,
		      (tmp & ~G2_RTC_RTCCON_CLKRST) | G2_RTC_RTCCON_RTCEN |
		      G2_RTC_RTCCON_STARTB, 0xff);
}

static void g2_rtc_write_disable(struct g2_rtc *p)
{
	u32 tmp = g2_rtc_readb(p, G2_RTC_RTCCON);

	g2_rtc_writel(p, G2_RTC_RTCCON,
		      tmp & ~G2_RTC_RTCCON_STARTB, 0xff);
}

static int g2_rtc_gettime(struct device *dev, struct rtc_time *tm)
{
	struct g2_rtc *p = dev_get_drvdata(dev);
	u32 year, sec, ctrl;
	int retry;

	ctrl = g2_rtc_readb(p, G2_RTC_RTCCON);
	if (!(ctrl & G2_RTC_RTCCON_RTCEN) || (ctrl & G2_RTC_RTCCON_CLKRST))
		return -EINVAL;

	for (retry = 0; retry < 3; retry++) {
		sec = g2_rtc_readb(p, G2_RTC_RTCSEC);
		tm->tm_min = bcd2bin(g2_rtc_readb(p, G2_RTC_RTCMIN));
		tm->tm_hour = bcd2bin(g2_rtc_readb(p, G2_RTC_RTCHOUR));
		tm->tm_mday = bcd2bin(g2_rtc_readb(p, G2_RTC_RTCDATE));
		tm->tm_mon = bcd2bin(g2_rtc_readb(p, G2_RTC_RTCMON)) - 1;
		year = g2_rtc_readl(p, G2_RTC_RTCYEAR) & 0xffff;
		if (sec == g2_rtc_readb(p, G2_RTC_RTCSEC)) {
			tm->tm_sec = bcd2bin(sec);
			tm->tm_year = bcd2bin(year & 0xff) +
				      bcd2bin(year >> 8) * 100 - 1900;
			return 0;
		}
	}

	return -EAGAIN;
}

static int g2_rtc_settime(struct device *dev, struct rtc_time *tm)
{
	struct g2_rtc *p = dev_get_drvdata(dev);
	int year = tm->tm_year + 1900;

	g2_rtc_write_enable(p);
	g2_rtc_writel(p, G2_RTC_RTCSEC,  bin2bcd(tm->tm_sec), 0xffffffff);
	g2_rtc_writel(p, G2_RTC_RTCMIN,  bin2bcd(tm->tm_min), 0xffffffff);
	g2_rtc_writel(p, G2_RTC_RTCHOUR, bin2bcd(tm->tm_hour), 0xffffffff);
	g2_rtc_writel(p, G2_RTC_RTCDATE, bin2bcd(tm->tm_mday), 0xffffffff);
	g2_rtc_writel(p, G2_RTC_RTCMON,  bin2bcd(tm->tm_mon + 1), 0xffffffff);
	g2_rtc_writel(p, G2_RTC_RTCYEAR,
		      (bin2bcd(year / 100) << 8) | bin2bcd(year % 100), 0xffff);
	g2_rtc_write_disable(p);

	return 0;
}

static const struct rtc_class_ops g2_rtcops = {
	.read_time		= g2_rtc_gettime,
	.set_time		= g2_rtc_settime,
};

static int g2_rtc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct g2_rtc *p;

	p = devm_kzalloc(dev, sizeof(*p), GFP_KERNEL);
	if (!p)
		return -ENOMEM;

	p->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(p->base))
		return PTR_ERR(p->base);

	platform_set_drvdata(pdev, p);

	p->rtc = devm_rtc_allocate_device(dev);
	if (IS_ERR(p->rtc))
		return PTR_ERR(p->rtc);
	p->rtc->ops = &g2_rtcops;
	p->rtc->range_min = RTC_TIMESTAMP_BEGIN_0000;
	p->rtc->range_max = RTC_TIMESTAMP_END_9999;

	return devm_rtc_register_device(p->rtc);
}

static const struct of_device_id cs75xx_rtc_of_match_table[] = {
	{ .compatible = "cortina,cs7542-rtc" },
	{}
};
MODULE_DEVICE_TABLE(of, cs75xx_rtc_of_match_table);

static struct platform_driver g2_rtc_driver = {
	.probe		= g2_rtc_probe,
	.driver		= {
		.name	= DRV_NAME,
		.of_match_table = cs75xx_rtc_of_match_table,
	},
};
module_platform_driver(g2_rtc_driver);

MODULE_DESCRIPTION("Cortina CS75xx RTC driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:cs75xx-rtc");
