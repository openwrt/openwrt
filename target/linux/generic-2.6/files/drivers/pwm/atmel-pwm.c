/*
 * drivers/pwm/atmel-pwm.c
 *
 * Copyright (C) 2010 Bill Gatliff <bgat@billgatliff.com>
 * Copyright (C) 2007 David Brownell
 *
 * This program is free software; you may redistribute and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/pwm/pwm.h>

enum {
	/* registers common to the PWMC peripheral */
	PWMC_MR = 0,
	PWMC_ENA = 4,
	PWMC_DIS = 8,
	PWMC_SR = 0xc,
	PWMC_IER = 0x10,
	PWMC_IDR = 0x14,
	PWMC_IMR = 0x18,
	PWMC_ISR = 0x1c,

	/* registers per each PWMC channel */
	PWMC_CMR = 0,
	PWMC_CDTY = 4,
	PWMC_CPRD = 8,
	PWMC_CCNT = 0xc,
	PWMC_CUPD = 0x10,

	/* how to find each channel */
	PWMC_CHAN_BASE = 0x200,
	PWMC_CHAN_STRIDE = 0x20,

	/* CMR bits of interest */
	PWMC_CMR_CPD = 10,
	PWMC_CMR_CPOL = 9,
	PWMC_CMR_CALG = 8,
	PWMC_CMR_CPRE_MASK = 0xf,
};

struct atmel_pwm {
	struct pwm_device pwm;
	spinlock_t lock;
	void __iomem *iobase;
	struct clk *clk;
	u32 *sync_mask;
	int irq;
	u32 ccnt_mask;
};

static inline struct atmel_pwm *to_atmel_pwm(const struct pwm_channel *p)
{
	return container_of(p->pwm, struct atmel_pwm, pwm);
}

static inline void
pwmc_writel(const struct atmel_pwm *p,
	    unsigned offset, u32 val)
{
	__raw_writel(val, p->iobase + offset);
}

static inline u32
pwmc_readl(const struct atmel_pwm *p,
	   unsigned offset)
{
	return __raw_readl(p->iobase + offset);
}

static inline void
pwmc_chan_writel(const struct pwm_channel *p,
		 u32 offset, u32 val)
{
	const struct atmel_pwm *ap = to_atmel_pwm(p);

	if (PWMC_CMR == offset)
		val &= ((1 << PWMC_CMR_CPD)
			| (1 << PWMC_CMR_CPOL)
			| (1 << PWMC_CMR_CALG)
			| (PWMC_CMR_CPRE_MASK));
	else
		val &= ap->ccnt_mask;

	pwmc_writel(ap, offset + PWMC_CHAN_BASE
		    + (p->chan * PWMC_CHAN_STRIDE), val);
}

static inline u32
pwmc_chan_readl(const struct pwm_channel *p,
		u32 offset)
{
	const struct atmel_pwm *ap = to_atmel_pwm(p);

	return pwmc_readl(ap, offset + PWMC_CHAN_BASE
			  + (p->chan * PWMC_CHAN_STRIDE));
}

static inline int
__atmel_pwm_is_on(struct pwm_channel *p)
{
	struct atmel_pwm *ap = to_atmel_pwm(p);
	return (pwmc_readl(ap, PWMC_SR) & (1 << p->chan)) ? 1 : 0;
}

static inline void
__atmel_pwm_unsynchronize(struct pwm_channel *p,
			  struct pwm_channel *to_p)
{
	const struct atmel_pwm *ap = to_atmel_pwm(p);
	int wchan;

	if (to_p) {
		ap->sync_mask[p->chan] &= ~(1 << to_p->chan);
		ap->sync_mask[to_p->chan] &= ~(1 << p->chan);
		goto done;
	}

	ap->sync_mask[p->chan] = 0;
	for (wchan = 0; wchan < ap->pwm.nchan; wchan++)
		ap->sync_mask[wchan] &= ~(1 << p->chan);
done:
	dev_dbg(p->pwm->dev, "sync_mask %x\n", ap->sync_mask[p->chan]);
}

static inline void
__atmel_pwm_synchronize(struct pwm_channel *p,
			struct pwm_channel *to_p)
{
	const struct atmel_pwm *ap = to_atmel_pwm(p);

	if (!to_p)
		return;

	ap->sync_mask[p->chan] |= (1 << to_p->chan);
	ap->sync_mask[to_p->chan] |= (1 << p->chan);

	dev_dbg(p->pwm->dev, "sync_mask %x\n", ap->sync_mask[p->chan]);
}

static inline void
__atmel_pwm_stop(struct pwm_channel *p)
{
	struct atmel_pwm *ap = to_atmel_pwm(p);
	u32 chid = 1 << p->chan;

	pwmc_writel(ap, PWMC_DIS, ap->sync_mask[p->chan] | chid);
}

static inline void
__atmel_pwm_start(struct pwm_channel *p)
{
	struct atmel_pwm *ap = to_atmel_pwm(p);
	u32 chid = 1 << p->chan;

	pwmc_writel(ap, PWMC_ENA, ap->sync_mask[p->chan] | chid);
}

static int
atmel_pwm_synchronize(struct pwm_channel *p,
		      struct pwm_channel *to_p)
{
	unsigned long flags;
	spin_lock_irqsave(&p->lock, flags);
	__atmel_pwm_synchronize(p, to_p);
	spin_unlock_irqrestore(&p->lock, flags);
	return 0;
}

static int
atmel_pwm_unsynchronize(struct pwm_channel *p,
			struct pwm_channel *from_p)
{
	unsigned long flags;
	spin_lock_irqsave(&p->lock, flags);
	__atmel_pwm_unsynchronize(p, from_p);
	spin_unlock_irqrestore(&p->lock, flags);
	return 0;
}

static inline int
__atmel_pwm_config_polarity(struct pwm_channel *p,
			    struct pwm_channel_config *c)
{
	u32 cmr = pwmc_chan_readl(p, PWMC_CMR);

	if (c->polarity)
		cmr &= ~BIT(PWMC_CMR_CPOL);
	else
		cmr |= BIT(PWMC_CMR_CPOL);
	pwmc_chan_writel(p, PWMC_CMR, cmr);
	p->active_high = c->polarity ? 1 : 0;

	dev_dbg(p->pwm->dev, "polarity %d\n", c->polarity);
	return 0;
}

static inline int
__atmel_pwm_config_duty_ticks(struct pwm_channel *p,
			      struct pwm_channel_config *c)
{
	u32 cmr, cprd, cpre, cdty;

	cmr = pwmc_chan_readl(p, PWMC_CMR);
	cprd = pwmc_chan_readl(p, PWMC_CPRD);

	cpre = cmr & PWMC_CMR_CPRE_MASK;
	cmr &= ~BIT(PWMC_CMR_CPD);

	cdty = cprd - (c->duty_ticks >> cpre);

	p->duty_ticks = c->duty_ticks;

	if (__atmel_pwm_is_on(p)) {
		pwmc_chan_writel(p, PWMC_CMR, cmr);
		pwmc_chan_writel(p, PWMC_CUPD, cdty);
	} else
		pwmc_chan_writel(p, PWMC_CDTY, cdty);

	dev_dbg(p->pwm->dev, "duty_ticks = %lu cprd = %x"
		" cdty = %x cpre = %x\n", p->duty_ticks,
		cprd, cdty, cpre);

	return 0;
}

static inline int
__atmel_pwm_config_period_ticks(struct pwm_channel *p,
				struct pwm_channel_config *c)
{
	u32 cmr, cprd, cpre;

	cpre = fls(c->period_ticks);
	if (cpre < 16)
		cpre = 0;
	else {
		cpre -= 15;
		if (cpre > 10)
			return -EINVAL;
	}

	cmr = pwmc_chan_readl(p, PWMC_CMR);
	cmr &= ~PWMC_CMR_CPRE_MASK;
	cmr |= cpre;

	cprd = c->period_ticks >> cpre;

	pwmc_chan_writel(p, PWMC_CMR, cmr);
	pwmc_chan_writel(p, PWMC_CPRD, cprd);
	p->period_ticks = c->period_ticks;

	dev_dbg(p->pwm->dev, "period_ticks = %lu cprd = %x cpre = %x\n",
		 p->period_ticks, cprd, cpre);

	return 0;
}

static int
atmel_pwm_config_nosleep(struct pwm_channel *p,
			 struct pwm_channel_config *c)
{
	int ret = 0;
	unsigned long flags;

	spin_lock_irqsave(&p->lock, flags);

	switch (c->config_mask) {

	case PWM_CONFIG_DUTY_TICKS:
		__atmel_pwm_config_duty_ticks(p, c);
		break;

	case PWM_CONFIG_STOP:
		__atmel_pwm_stop(p);
		break;

	case PWM_CONFIG_START:
		__atmel_pwm_start(p);
		break;

	case PWM_CONFIG_POLARITY:
		__atmel_pwm_config_polarity(p, c);
		break;

	default:
		ret = -EINVAL;
		break;
	}

	spin_unlock_irqrestore(&p->lock, flags);
	return ret;
}

static int
atmel_pwm_stop_sync(struct pwm_channel *p)
{
	struct atmel_pwm *ap = container_of(p->pwm, struct atmel_pwm, pwm);
	int ret;
	int was_on = __atmel_pwm_is_on(p);

	if (was_on) {
		do {
			init_completion(&p->complete);
			set_bit(FLAG_STOP, &p->flags);
			pwmc_writel(ap, PWMC_IER, 1 << p->chan);

			dev_dbg(p->pwm->dev, "waiting on stop_sync completion...\n");

			ret = wait_for_completion_interruptible(&p->complete);

			dev_dbg(p->pwm->dev, "stop_sync complete (%d)\n", ret);

			if (ret)
				return ret;
		} while (p->flags & BIT(FLAG_STOP));
	}

	return was_on;
}

static int
atmel_pwm_config(struct pwm_channel *p,
		 struct pwm_channel_config *c)
{
	int was_on = 0;

	if (p->pwm->config_nosleep) {
		if (!p->pwm->config_nosleep(p, c))
			return 0;
	}

	might_sleep();

	dev_dbg(p->pwm->dev, "config_mask %x\n", c->config_mask);

	was_on = atmel_pwm_stop_sync(p);
	if (was_on < 0)
		return was_on;

	if (c->config_mask & PWM_CONFIG_PERIOD_TICKS) {
		__atmel_pwm_config_period_ticks(p, c);
		if (!(c->config_mask & PWM_CONFIG_DUTY_TICKS)) {
			struct pwm_channel_config d = {
				.config_mask = PWM_CONFIG_DUTY_TICKS,
				.duty_ticks = p->duty_ticks,
			};
			__atmel_pwm_config_duty_ticks(p, &d);
		}
	}

	if (c->config_mask & PWM_CONFIG_DUTY_TICKS)
		__atmel_pwm_config_duty_ticks(p, c);

	if (c->config_mask & PWM_CONFIG_POLARITY)
		__atmel_pwm_config_polarity(p, c);

	if ((c->config_mask & PWM_CONFIG_START)
	    || (was_on && !(c->config_mask & PWM_CONFIG_STOP)))
		__atmel_pwm_start(p);

	return 0;
}

static void
__atmel_pwm_set_callback(struct pwm_channel *p,
			 pwm_callback_t callback)
{
	struct atmel_pwm *ap = container_of(p->pwm, struct atmel_pwm, pwm);

	p->callback = callback;
	pwmc_writel(ap, p->callback ? PWMC_IER : PWMC_IDR, 1 << p->chan);
}

static int
atmel_pwm_set_callback(struct pwm_channel *p,
		       pwm_callback_t callback)
{
	struct atmel_pwm *ap = to_atmel_pwm(p);
	unsigned long flags;

	spin_lock_irqsave(&ap->lock, flags);
	__atmel_pwm_set_callback(p, callback);
	spin_unlock_irqrestore(&ap->lock, flags);

	return 0;
}

static int
atmel_pwm_request(struct pwm_channel *p)
{
	struct atmel_pwm *ap = to_atmel_pwm(p);
	unsigned long flags;

	spin_lock_irqsave(&p->lock, flags);
	clk_enable(ap->clk);
	p->tick_hz = clk_get_rate(ap->clk);
	__atmel_pwm_unsynchronize(p, NULL);
	__atmel_pwm_stop(p);
	spin_unlock_irqrestore(&p->lock, flags);

	return 0;
}

static void
atmel_pwm_free(struct pwm_channel *p)
{
	struct atmel_pwm *ap = to_atmel_pwm(p);
	clk_disable(ap->clk);
}

static irqreturn_t
atmel_pwmc_irq(int irq, void *data)
{
	struct atmel_pwm *ap = data;
	struct pwm_channel *p;
	u32 isr;
	int chid;
	unsigned long flags;

	spin_lock_irqsave(&ap->lock, flags);

	isr = pwmc_readl(ap, PWMC_ISR);
	for (chid = 0; isr; chid++, isr >>= 1) {
		p = &ap->pwm.channels[chid];
		if (isr & 1) {
			if (p->callback)
				p->callback(p);
			if (p->flags & BIT(FLAG_STOP)) {
				__atmel_pwm_stop(p);
				clear_bit(FLAG_STOP, &p->flags);
			}
			complete_all(&p->complete);
		}
	}

	spin_unlock_irqrestore(&ap->lock, flags);

	return IRQ_HANDLED;
}

static int __devinit
atmel_pwmc_probe(struct platform_device *pdev)
{
	struct atmel_pwm *ap;
	struct resource *r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	int ret = 0;

	ap = kzalloc(sizeof(*ap), GFP_KERNEL);
	if (!ap) {
		ret = -ENOMEM;
		goto err_atmel_pwm_alloc;
	}

	spin_lock_init(&ap->lock);
	platform_set_drvdata(pdev, ap);

	ap->pwm.dev = &pdev->dev;
	ap->pwm.bus_id = dev_name(&pdev->dev);

	ap->pwm.nchan = 4; /* TODO: true only for SAM9263 and AP7000 */
	ap->ccnt_mask = 0xffffUL; /* TODO: true only for SAM9263 */

	ap->sync_mask = kzalloc(ap->pwm.nchan * sizeof(u32), GFP_KERNEL);
	if (!ap->sync_mask) {
		ret = -ENOMEM;
		goto err_alloc_sync_masks;
	}

	ap->pwm.owner = THIS_MODULE;
	ap->pwm.request = atmel_pwm_request;
	ap->pwm.free = atmel_pwm_free;
	ap->pwm.config_nosleep = atmel_pwm_config_nosleep;
	ap->pwm.config = atmel_pwm_config;
	ap->pwm.synchronize = atmel_pwm_synchronize;
	ap->pwm.unsynchronize = atmel_pwm_unsynchronize;
	ap->pwm.set_callback = atmel_pwm_set_callback;

	ap->clk = clk_get(&pdev->dev, "pwm_clk");
	if (PTR_ERR(ap->clk)) {
		ret = -ENODEV;
		goto err_clk_get;
	}

	ap->iobase = ioremap_nocache(r->start, r->end - r->start + 1);
	if (!ap->iobase) {
		ret = -ENODEV;
		goto err_ioremap;
	}

	clk_enable(ap->clk);
	pwmc_writel(ap, PWMC_DIS, -1);
	pwmc_writel(ap, PWMC_IDR, -1);
	clk_disable(ap->clk);

	ap->irq = platform_get_irq(pdev, 0);
	if (ap->irq != -ENXIO) {
		ret = request_irq(ap->irq, atmel_pwmc_irq, 0,
				  ap->pwm.bus_id, ap);
		if (ret)
			goto err_request_irq;
	}

	ret = pwm_register(&ap->pwm);
	if (ret)
		goto err_pwm_register;

	return 0;

err_pwm_register:
	if (ap->irq != -ENXIO)
		free_irq(ap->irq, ap);
err_request_irq:
	iounmap(ap->iobase);
err_ioremap:
	clk_put(ap->clk);
err_clk_get:
	platform_set_drvdata(pdev, NULL);
err_alloc_sync_masks:
	kfree(ap);
err_atmel_pwm_alloc:
	return ret;
}

static int __devexit
atmel_pwmc_remove(struct platform_device *pdev)
{
	struct atmel_pwm *ap = platform_get_drvdata(pdev);
	int ret;

	/* TODO: what can we do if this fails? */
	ret = pwm_unregister(&ap->pwm);

	clk_enable(ap->clk);
	pwmc_writel(ap, PWMC_IDR, -1);
	pwmc_writel(ap, PWMC_DIS, -1);
	clk_disable(ap->clk);

	if (ap->irq != -ENXIO)
		free_irq(ap->irq, ap);

	clk_put(ap->clk);
	iounmap(ap->iobase);

	kfree(ap);

	return 0;
}

static struct platform_driver atmel_pwm_driver = {
	.driver = {
		.name = "atmel_pwmc",
		.owner = THIS_MODULE,
	},
	.probe = atmel_pwmc_probe,
	.remove = __devexit_p(atmel_pwmc_remove),
};

static int __init atmel_pwm_init(void)
{
	return platform_driver_register(&atmel_pwm_driver);
}
module_init(atmel_pwm_init);

static void __exit atmel_pwm_exit(void)
{
	platform_driver_unregister(&atmel_pwm_driver);
}
module_exit(atmel_pwm_exit);

MODULE_AUTHOR("Bill Gatliff <bgat@billgatliff.com>");
MODULE_DESCRIPTION("Driver for Atmel PWMC peripheral");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:atmel_pwmc");
