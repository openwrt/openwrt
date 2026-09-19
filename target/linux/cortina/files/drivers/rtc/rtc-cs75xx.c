// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
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

#define G2_RTC_RTCALM		0x04
#define G2_RTC_RTCALM_ALMEN	BIT(7)
#define G2_RTC_RTCALM_YEAREN	BIT(6)
#define G2_RTC_RTCALM_MONEN	BIT(5)
#define G2_RTC_RTCALM_DAYEN	BIT(4)
#define G2_RTC_RTCALM_DATEEN	BIT(3)
#define G2_RTC_RTCALM_HOUREN	BIT(2)
#define G2_RTC_RTCALM_MINEN	BIT(1)
#define G2_RTC_RTCALM_SECEN	BIT(0)

#define G2_RTC_ALMSEC		0x08
#define G2_RTC_ALMMIN		0x0c
#define G2_RTC_ALMHOUR		0x10
#define G2_RTC_ALMDATE		0x14
#define G2_RTC_ALMDAY		0x18
#define G2_RTC_ALMMON		0x1c
#define G2_RTC_ALMYEAR		0x20

#define G2_RTC_RTCSEC		0x24
#define G2_RTC_RTCMIN		0x28
#define G2_RTC_RTCHOUR		0x2c
#define G2_RTC_RTCDATE		0x30
#define G2_RTC_RTCDAY		0x34
#define G2_RTC_RTCMON		0x38
#define G2_RTC_RTCYEAR		0x3c
#define G2_RTC_RTCIM		0x40
#define G2_RTC_RTCPEND		0x44
#define G2_RTC_PRIPEND		0x48
#define G2_RTC_WKUPPEND		0x4c

struct g2_rtc {
	void __iomem		*base;
	struct rtc_device	*rtc;
	int			alarm_irq;
	int			tick_irq;
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
		      tmp | G2_RTC_RTCCON_RTCEN | G2_RTC_RTCCON_STARTB, 0xff);
}

static void g2_rtc_write_disable(struct g2_rtc *p)
{
	u32 tmp = g2_rtc_readb(p, G2_RTC_RTCCON);

	g2_rtc_writel(p, G2_RTC_RTCCON,
		      tmp & ~(G2_RTC_RTCCON_STARTB | G2_RTC_RTCCON_RTCEN), 0xff);
}

static irqreturn_t g2_rtc_alarmirq(int irq, void *id)
{
	struct g2_rtc *p = id;

	g2_rtc_writel(p, G2_RTC_RTCPEND, 0, 0xff);
	rtc_update_irq(p->rtc, 1, RTC_AF | RTC_IRQF);
	return IRQ_HANDLED;
}

static irqreturn_t g2_rtc_tickirq(int irq, void *id)
{
	struct g2_rtc *p = id;

	g2_rtc_writel(p, G2_RTC_PRIPEND, 0, 0xff);
	rtc_update_irq(p->rtc, 1, RTC_PF | RTC_IRQF);
	return IRQ_HANDLED;
}

static void g2_rtc_setaie(struct g2_rtc *p, int to)
{
	u32 tmp;

	g2_rtc_write_enable(p);
	tmp = g2_rtc_readl(p, G2_RTC_RTCALM);
	if (to)
		tmp |= G2_RTC_RTCALM_ALMEN;
	else
		tmp &= ~G2_RTC_RTCALM_ALMEN;
	g2_rtc_writel(p, G2_RTC_RTCALM, tmp, 0xff);
	g2_rtc_write_disable(p);
}

static int g2_rtc_gettime(struct device *dev, struct rtc_time *tm)
{
	struct g2_rtc *p = dev_get_drvdata(dev);
	unsigned int have_retried = 0;
	u32 year;

retry:
	tm->tm_min  = g2_rtc_readb(p, G2_RTC_RTCMIN);
	tm->tm_hour = g2_rtc_readb(p, G2_RTC_RTCHOUR);
	tm->tm_mday = g2_rtc_readb(p, G2_RTC_RTCDATE);
	tm->tm_mon  = g2_rtc_readb(p, G2_RTC_RTCMON);
	year        = g2_rtc_readl(p, G2_RTC_RTCYEAR) & 0xffff;
	tm->tm_sec  = g2_rtc_readb(p, G2_RTC_RTCSEC);

	if (tm->tm_sec == 0 && !have_retried) {
		have_retried = 1;
		goto retry;
	}

	tm->tm_sec  = bcd2bin(tm->tm_sec);
	tm->tm_min  = bcd2bin(tm->tm_min);
	tm->tm_hour = bcd2bin(tm->tm_hour);
	tm->tm_mday = bcd2bin(tm->tm_mday);
	tm->tm_mon  = bcd2bin(tm->tm_mon) - 1;
	tm->tm_year = bcd2bin(year & 0xff) + bcd2bin(year >> 8) * 100 - 1900;

	return rtc_valid_tm(tm);
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
	g2_rtc_writel(p, G2_RTC_RTCYEAR, (bin2bcd(year / 100) << 8) | bin2bcd(year % 100), 0xffff);
	g2_rtc_write_disable(p);

	return 0;
}

static int g2_rtc_getalarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct g2_rtc *p = dev_get_drvdata(dev);
	struct rtc_time *t = &alrm->time;
	u32 en;

	g2_rtc_write_enable(p);
	t->tm_sec  = g2_rtc_readb(p, G2_RTC_ALMSEC);
	t->tm_min  = g2_rtc_readb(p, G2_RTC_ALMMIN);
	t->tm_hour = g2_rtc_readb(p, G2_RTC_ALMHOUR);
	t->tm_mon  = g2_rtc_readb(p, G2_RTC_ALMMON);
	t->tm_mday = g2_rtc_readb(p, G2_RTC_ALMDATE);
	t->tm_year = g2_rtc_readb(p, G2_RTC_ALMYEAR);
	en = g2_rtc_readb(p, G2_RTC_RTCALM);
	g2_rtc_write_disable(p);

	alrm->enabled = !!(en & G2_RTC_RTCALM_ALMEN);

	t->tm_sec  = (en & G2_RTC_RTCALM_SECEN)  ? bcd2bin(t->tm_sec)  : -1;
	t->tm_min  = (en & G2_RTC_RTCALM_MINEN)  ? bcd2bin(t->tm_min)  : -1;
	t->tm_hour = (en & G2_RTC_RTCALM_HOUREN) ? bcd2bin(t->tm_hour) : -1;
	t->tm_mday = (en & G2_RTC_RTCALM_DAYEN)  ? bcd2bin(t->tm_mday) : -1;
	t->tm_mon  = (en & G2_RTC_RTCALM_MONEN)  ? bcd2bin(t->tm_mon) - 1 : -1;
	t->tm_year = (en & G2_RTC_RTCALM_YEAREN) ?
		     (t->tm_year & 0xff) + ((t->tm_year >> 8) * 100) + 100 : -1;

	return 0;
}

static int g2_rtc_setalarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct g2_rtc *p = dev_get_drvdata(dev);
	struct rtc_time *tm = &alrm->time;
	u32 en;

	g2_rtc_write_enable(p);

	en = g2_rtc_readb(p, G2_RTC_RTCALM) & G2_RTC_RTCALM_ALMEN;
	g2_rtc_writel(p, G2_RTC_RTCALM, 0, 0xff);

	if (tm->tm_sec >= 0 && tm->tm_sec < 60) {
		en |= G2_RTC_RTCALM_SECEN;
		g2_rtc_writel(p, G2_RTC_ALMSEC, bin2bcd(tm->tm_sec), 0xff);
	}
	if (tm->tm_min >= 0 && tm->tm_min < 60) {
		en |= G2_RTC_RTCALM_MINEN;
		g2_rtc_writel(p, G2_RTC_ALMMIN, bin2bcd(tm->tm_min), 0xff);
	}
	if (tm->tm_hour >= 0 && tm->tm_hour < 24) {
		en |= G2_RTC_RTCALM_HOUREN;
		g2_rtc_writel(p, G2_RTC_ALMHOUR, bin2bcd(tm->tm_hour), 0xff);
	}

	if (alrm->enabled)
		en |= G2_RTC_RTCALM_ALMEN;
	else
		en &= ~G2_RTC_RTCALM_ALMEN;
	g2_rtc_writel(p, G2_RTC_RTCALM, en, 0xff);

	g2_rtc_write_disable(p);
	g2_rtc_setaie(p, alrm->enabled);

	return 0;
}

static int g2_rtc_alarm_irq_enable(struct device *dev, unsigned int enabled)
{
	g2_rtc_setaie(dev_get_drvdata(dev), enabled);
	return 0;
}

static const struct rtc_class_ops g2_rtcops = {
	.read_time		= g2_rtc_gettime,
	.set_time		= g2_rtc_settime,
	.read_alarm		= g2_rtc_getalarm,
	.set_alarm		= g2_rtc_setalarm,
	.alarm_irq_enable	= g2_rtc_alarm_irq_enable,
};

static void g2_rtc_enable(struct device *dev, struct g2_rtc *p)
{
	u32 tmp;

	if ((g2_rtc_readb(p, G2_RTC_RTCCON) & G2_RTC_RTCCON_RTCEN) == 0) {
		dev_info(dev, "rtc disabled, re-enabling\n");
		tmp = g2_rtc_readb(p, G2_RTC_RTCCON);
		g2_rtc_writel(p, G2_RTC_RTCCON, tmp | G2_RTC_RTCCON_RTCEN, 0xff);
	}
	if (g2_rtc_readb(p, G2_RTC_RTCCON) & G2_RTC_RTCCON_CLKRST) {
		dev_info(dev, "removing RTCCON_CLKRST\n");
		tmp = g2_rtc_readb(p, G2_RTC_RTCCON);
		g2_rtc_writel(p, G2_RTC_RTCCON, tmp & ~G2_RTC_RTCCON_CLKRST, 0xff);
	}
	g2_rtc_writel(p, G2_RTC_WKUPPEND, 0, 0xff);
	g2_rtc_writel(p, G2_RTC_RTCIM, 0x43, 0xff);
}

static int g2_rtc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct g2_rtc *p;
	int ret;

	p = devm_kzalloc(dev, sizeof(*p), GFP_KERNEL);
	if (!p)
		return -ENOMEM;

	p->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(p->base))
		return PTR_ERR(p->base);

	p->alarm_irq = platform_get_irq(pdev, 0);
	if (p->alarm_irq < 0)
		return p->alarm_irq;
	p->tick_irq = platform_get_irq(pdev, 1);
	if (p->tick_irq < 0)
		return p->tick_irq;

	platform_set_drvdata(pdev, p);

	g2_rtc_enable(dev, p);
	device_init_wakeup(dev, 1);

	p->rtc = devm_rtc_allocate_device(dev);
	if (IS_ERR(p->rtc))
		return PTR_ERR(p->rtc);
	p->rtc->ops = &g2_rtcops;
	p->rtc->range_min = RTC_TIMESTAMP_BEGIN_2000;
	p->rtc->range_max = RTC_TIMESTAMP_END_2099;
	p->rtc->max_user_freq = 128;

	ret = devm_request_irq(dev, p->alarm_irq, g2_rtc_alarmirq, 0,
			       "g2-rtc alarm", p);
	if (ret)
		return ret;
	ret = devm_request_irq(dev, p->tick_irq, g2_rtc_tickirq, 0,
			       "g2-rtc tick", p);
	if (ret)
		return ret;

	return devm_rtc_register_device(p->rtc);
}

static const struct of_device_id cs75xx_rtc_of_match_table[] = {
	{ .compatible = "cortina,cs75xx-rtc" },
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
