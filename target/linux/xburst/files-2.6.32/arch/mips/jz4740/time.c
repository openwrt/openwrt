/*
 *  Copyright (C) 2010, Lars-Peter Clausen <lars@metafoo.de>
 *  	JZ4740 platform timer support
 *
 *  This program is free software; you can redistribute	 it and/or modify it
 *  under  the terms of	 the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the	License, or (at your
 *  option) any later version.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/clockchips.h>
#include <linux/clk.h>

#include <asm/mach-jz4740/irq.h>
#include <asm/mach-jz4740/jz4740.h>
#include <asm/time.h>

#define JZ_REG_TIMER_STOP		0x1C
#define JZ_REG_TIMER_STOP_SET		0x2C
#define JZ_REG_TIMER_STOP_CLEAR		0x3C
#define JZ_REG_TIMER_ENABLE		0x10
#define JZ_REG_TIMER_ENABLE_SET		0x14
#define JZ_REG_TIMER_ENABLE_CLEAR	0x18
#define JZ_REG_TIMER_FLAG		0x20
#define JZ_REG_TIMER_FLAG_SET		0x24
#define JZ_REG_TIMER_FLAG_CLEAR		0x28
#define JZ_REG_TIMER_MASK		0x30
#define JZ_REG_TIMER_MASK_SET		0x34
#define JZ_REG_TIMER_MASK_CLEAR		0x38

#define JZ_REG_TIMER_DFR(x) (((x) * 0x10) + 0x40)
#define JZ_REG_TIMER_DHR(x) (((x) * 0x10) + 0x44)
#define JZ_REG_TIMER_CNT(x) (((x) * 0x10) + 0x48)
#define JZ_REG_TIMER_CTRL(x) (((x) * 0x10) + 0x4C)

#define JZ_TIMER_IRQ_HALF(x) BIT((x) + 0x10)
#define JZ_TIMER_IRQ_FULL(x) BIT(x)

#define JZ_TIMER_CTRL_PWM_ACTIVE_LOW	BIT(8)
#define JZ_TIMER_CTRL_PWM_ENABLE	BIT(7)
#define JZ_TIMER_CTRL_PRESCALE_MASK	0x1c
#define JZ_TIMER_CTRL_PRESCALE_OFFSET	0x3
#define JZ_TIMER_CTRL_PRESCALE_1	(0 << 3)
#define JZ_TIMER_CTRL_PRESCALE_4	(1 << 3)
#define JZ_TIMER_CTRL_PRESCALE_16	(2 << 3)
#define JZ_TIMER_CTRL_PRESCALE_64	(3 << 3)
#define JZ_TIMER_CTRL_PRESCALE_256	(4 << 3)
#define JZ_TIMER_CTRL_PRESCALE_1024	(5 << 3)

#define JZ_TIMER_CTRL_SRC_EXT		BIT(2)
#define JZ_TIMER_CTRL_SRC_RTC		BIT(1)
#define JZ_TIMER_CTRL_SRC_PCLK		BIT(0)

static void __iomem *jz4740_timer_base;
static uint16_t jz4740_jiffies_per_tick;

void jz4740_timer_enable_watchdog(void)
{
	writel(BIT(16), jz4740_timer_base + JZ_REG_TIMER_STOP_CLEAR);
}

void jz4740_timer_disable_watchdog(void)
{
	writel(BIT(16), jz4740_timer_base + JZ_REG_TIMER_STOP_SET);
}

static inline void jz4740_timer_set_period(unsigned int timer, uint16_t period)
{
	writew(period, jz4740_timer_base + JZ_REG_TIMER_DFR(timer));
}

static inline void jz4740_timer_set_duty(unsigned int timer, uint16_t duty)
{
	writew(duty, jz4740_timer_base + JZ_REG_TIMER_DHR(timer));
}

static void jz4740_init_timer(void)
{
	uint16_t val = 0;
	val |= JZ_TIMER_CTRL_PRESCALE_16;
	val |= JZ_TIMER_CTRL_SRC_EXT;

	writew(val, jz4740_timer_base + JZ_REG_TIMER_CTRL(0));
	writew(0xffff, jz4740_timer_base + JZ_REG_TIMER_DFR(0));
	writew(val, jz4740_timer_base + JZ_REG_TIMER_CTRL(1));
	writew(0xffff, jz4740_timer_base + JZ_REG_TIMER_DFR(1));
}

static void jz4740_timer_enable(unsigned int timer)
{
	writel(BIT(timer), jz4740_timer_base + JZ_REG_TIMER_STOP_CLEAR);
	writel(BIT(timer), jz4740_timer_base + JZ_REG_TIMER_ENABLE_SET);
}

static void jz4740_timer_disable(unsigned int timer)
{
	writel(BIT(timer), jz4740_timer_base + JZ_REG_TIMER_ENABLE_CLEAR);
	writel(BIT(timer), jz4740_timer_base + JZ_REG_TIMER_STOP_SET);
}

static void jz4740_timer_irq_full_enable(unsigned int timer)
{
	writel(JZ_TIMER_IRQ_FULL(timer), jz4740_timer_base + JZ_REG_TIMER_FLAG_CLEAR);
	writel(JZ_TIMER_IRQ_FULL(timer), jz4740_timer_base + JZ_REG_TIMER_MASK_CLEAR);
}

static int jz4740_timer_irq_full_is_enabled(unsigned int timer)
{
	return !(readl(jz4740_timer_base + JZ_REG_TIMER_MASK) &
	JZ_TIMER_IRQ_FULL(timer));
}

static void jz4740_timer_irq_full_disable(unsigned int timer)
{
	writel(JZ_TIMER_IRQ_FULL(timer), jz4740_timer_base + JZ_REG_TIMER_MASK_SET);
}

static void jz4740_timer_irq_half_enable(unsigned int timer)
{
	writel(JZ_TIMER_IRQ_HALF(timer), jz4740_timer_base + JZ_REG_TIMER_FLAG_CLEAR);
	writel(JZ_TIMER_IRQ_HALF(timer), jz4740_timer_base + JZ_REG_TIMER_MASK_CLEAR);
}

static void jz4740_timer_irq_half_disable(unsigned int timer)
{
	writel(JZ_TIMER_IRQ_HALF(timer), jz4740_timer_base + JZ_REG_TIMER_MASK_SET);
}

static cycle_t jz4740_clocksource_read(struct clocksource *cs)
{
    uint16_t val;
	val = readw(jz4740_timer_base + JZ_REG_TIMER_CNT(1));
	return val;
}

static struct clocksource jz4740_clocksource = {
	.name = "jz4740-timer",
	.rating = 200,
	.read = jz4740_clocksource_read,
	.mask = CLOCKSOURCE_MASK(16),
	.flags = CLOCK_SOURCE_IS_CONTINUOUS,
};

static irqreturn_t jz4740_clockevent_irq(int irq, void *devid)
{
	struct clock_event_device *cd = devid;

	writel(JZ_TIMER_IRQ_FULL(0), jz4740_timer_base + JZ_REG_TIMER_FLAG_CLEAR);

	if (cd->mode != CLOCK_EVT_MODE_PERIODIC) {
		jz4740_timer_disable(0);
		cd->event_handler(cd);
	 } else {
		cd->event_handler(cd);
	}

    return IRQ_HANDLED;
}

static void jz4740_clockevent_set_mode(enum clock_event_mode mode,
				       struct clock_event_device *cd)
{
	switch(mode) {
	case CLOCK_EVT_MODE_PERIODIC:
		writew(0x0, jz4740_timer_base + JZ_REG_TIMER_CNT(0));
		writew(jz4740_jiffies_per_tick, jz4740_timer_base + JZ_REG_TIMER_DFR(0));
	case CLOCK_EVT_MODE_RESUME:
		jz4740_timer_irq_full_enable(0);
		jz4740_timer_enable(0);
		break;
	case CLOCK_EVT_MODE_ONESHOT:
	case CLOCK_EVT_MODE_SHUTDOWN:
		jz4740_timer_disable(0);
		break;
	default:
		break;
	}
}

static int jz4740_clockevent_set_next(unsigned long evt, struct
clock_event_device *cd)
{
	writew(0x0, jz4740_timer_base + JZ_REG_TIMER_CNT(0));
	writew(evt, jz4740_timer_base + JZ_REG_TIMER_DFR(0));
	jz4740_timer_enable(0);

	return 0;
}

static struct clock_event_device jz4740_clockevent = {
	.name = "jz4740-timer",
	.features = CLOCK_EVT_FEAT_PERIODIC,
	.set_next_event = jz4740_clockevent_set_next,
	.set_mode = jz4740_clockevent_set_mode,
	.rating = 200,
    .irq = JZ_IRQ_TCU0,
};

static struct irqaction jz_irqaction = {
	.handler	= jz4740_clockevent_irq,
	.flags		= IRQF_PERCPU | IRQF_TIMER | IRQF_DISABLED,
	.name		= "jz4740-timerirq",
    .dev_id      = &jz4740_clockevent,
};


void __init plat_time_init(void)
{
    int ret;
	uint32_t clk_rate;
	struct clk *ext_clk;

	jz4740_timer_base = ioremap(CPHYSADDR(TCU_BASE), 0x100);

	if (!jz4740_timer_base) {
	    printk(KERN_ERR "Failed to ioremap timer registers");
	    return;
	}

	/*ext_clk = clk_get(NULL, "ext");
	clk_rate = clk_get_rate(ext_clk) >> 4;
	clk_put(ext_clk);*/


	clk_rate = JZ_EXTAL >> 4;

    jz4740_jiffies_per_tick = DIV_ROUND_CLOSEST(clk_rate, HZ);

	clockevent_set_clock(&jz4740_clockevent, clk_rate);
	jz4740_clockevent.min_delta_ns = clockevent_delta2ns(100, &jz4740_clockevent);
	jz4740_clockevent.max_delta_ns = clockevent_delta2ns(0xffff, &jz4740_clockevent);
	jz4740_clockevent.cpumask = cpumask_of(0);

	clockevents_register_device(&jz4740_clockevent);

	clocksource_set_clock(&jz4740_clocksource, clk_rate);
	ret = clocksource_register(&jz4740_clocksource);

	if (ret)
	    printk(KERN_ERR "Failed to register clocksource: %d\n", ret);

	setup_irq(JZ_IRQ_TCU0, &jz_irqaction);

	jz4740_init_timer();
	writew(jz4740_jiffies_per_tick, jz4740_timer_base + JZ_REG_TIMER_DFR(0));
    jz4740_timer_irq_half_disable(0);
    jz4740_timer_irq_full_enable(0);
	jz4740_timer_enable(0);

    jz4740_timer_irq_half_disable(1);
    jz4740_timer_irq_full_disable(1);

	jz4740_timer_enable(1);
}
