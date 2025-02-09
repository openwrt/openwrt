// SPDX-License-Identifier: GPL-2.0
/*
 * High Performance Timer present on EN75xx MIPS based SoCs.
 *
 * Copyright (C) 2025 by Caleb James DeLisle <cjd@cjdns.fr>
 */

#include <linux/io.h>
#include <linux/cpumask.h>
#include <linux/interrupt.h>
#include <linux/clockchips.h>
#include <linux/sched_clock.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/cpuhotplug.h>
#include <linux/clk.h>

#define EN75_BITS		32
#define EN75_NUM_BLOCKS		DIV_ROUND_UP(NR_CPUS, 2)
#define EN75_MIN_DELTA		0x00001000
#define EN75_MAX_DELTA		GENMASK(EN75_BITS - 2, 0)

static struct {
	void __iomem *membase[EN75_NUM_BLOCKS];
	u32 freq_hz;
} en75_timer_rai __ro_after_init;

static DEFINE_PER_CPU(struct clock_event_device, en75_timer_pcpu_m);

/* Each memory block has 2 timers, the order of registers is:
 * CTL, CMR0, CNT0, CMR1, CNT1
 */
static inline void __iomem *reg_ctl(u32 timer_n)
{
	return en75_timer_rai.membase[timer_n >> 1];
}

static inline void __iomem *reg_compare(u32 timer_n)
{
	return en75_timer_rai.membase[timer_n >> 1] + (timer_n & 1) * 0x08 + 0x04;
}

static inline void __iomem *reg_count(u32 timer_n)
{
	return en75_timer_rai.membase[timer_n >> 1] + (timer_n & 1) * 0x08 + 0x08;
}

static inline u32 ctl_bit_enabled(u32 timer_n)
{
	return 1U << (timer_n & 1);
}

static inline u32 ctl_bit_pending(u32 timer_n)
{
	return 1U << ((timer_n & 1) + 16);
}

static inline bool en75_cevt_is_pending(int cpu_id)
{
	return ioread32(reg_ctl(cpu_id)) & ctl_bit_pending(cpu_id);
}

static irqreturn_t en75_cevt_interrupt(int irq, void *dev_id)
{
	struct clock_event_device *dev = this_cpu_ptr(&en75_timer_pcpu_m);
	const int cpu = cpumask_first(dev->cpumask);

	if (!en75_cevt_is_pending(cpu)) {
		pr_debug("%s IRQ %d on CPU %d is not pending\n", __func__, irq, cpu);
		return IRQ_NONE;
	}

	iowrite32(ioread32(reg_count(cpu)), reg_compare(cpu));
	dev->event_handler(dev);
	return IRQ_HANDLED;
}

static void en75_cevt_enable(const int cpu)
{
	u32 reg = ioread32(reg_ctl(cpu));

	reg |= ctl_bit_enabled(cpu);
	iowrite32(reg, reg_ctl(cpu));
}

static int en75_cevt_set_next_event(const ulong delta, struct clock_event_device *const dev)
{
	const int cpu = cpumask_first(dev->cpumask);
	const u32 next = ioread32(reg_count(cpu)) + delta;
	bool is_etime;

	iowrite32(next, reg_compare(cpu));

	is_etime = (s32)(next - ioread32(reg_count(cpu))) < EN75_MIN_DELTA / 2;

	WARN_ON_ONCE(cpu != smp_processor_id());

	if (is_etime)
		return -ETIME;

	return 0;
}

static int en75_cevt_init_cpu(const uint cpu)
{
	struct clock_event_device *const cd = &per_cpu(en75_timer_pcpu_m, cpu);

	pr_info("%s: Setting up clockevent for CPU %d\n", cd->name, cpu);

	en75_cevt_enable(cpu);

	enable_percpu_irq(cd->irq, IRQ_TYPE_NONE);

	/* Do this last because it synchronously configures the timer */
	clockevents_config_and_register(
		cd, en75_timer_rai.freq_hz,
		EN75_MIN_DELTA, EN75_MAX_DELTA);

	return 0;
}

static u64 notrace en75_sched_clock_read(void)
{
	/* Always read from clock zero no matter the CPU */
	return (u64)ioread32(reg_count(0));
}

/* Init */

static inline void __init en75_cevt_dev_init(const uint cpu)
{
	iowrite32(0, reg_count(cpu));
	iowrite32(U32_MAX, reg_compare(cpu));
}

static int __init en75_cevt_init(struct device_node *np)
{
	int i;
	int irq;
	int ret;

	irq = irq_of_parse_and_map(np, 0);
	if (irq <= 0) {
		pr_err("%pOFn: irq_of_parse_and_map failed", np);
		return -EINVAL;
	}

	ret = request_percpu_irq(
		irq, en75_cevt_interrupt,
		np->name, &en75_timer_pcpu_m);

	if (ret < 0) {
		pr_err("%pOFn: IRQ %d setup failed (%d)\n", np, irq, ret);
		goto err_unmap_irq;
	}

	for_each_possible_cpu(i) {
		struct clock_event_device *const cd = &per_cpu(en75_timer_pcpu_m, i);

		cd->rating		= 310,
		cd->features		= CLOCK_EVT_FEAT_ONESHOT |
					  CLOCK_EVT_FEAT_C3STOP |
					  CLOCK_EVT_FEAT_PERCPU;
		cd->set_next_event	= en75_cevt_set_next_event;
		cd->irq			= irq;
		cd->cpumask		= cpumask_of(i);
		cd->name		= np->name;

		en75_cevt_dev_init(i);
	}

	cpuhp_setup_state(CPUHP_AP_MIPS_GIC_TIMER_STARTING,
			  "clockevents/en75/timer:starting",
			  en75_cevt_init_cpu, NULL);
	return 0;

err_unmap_irq:
	irq_dispose_mapping(irq);
	return ret;
}

static int __init en75_timer_init(struct device_node *np)
{
	int num_blocks = DIV_ROUND_UP(num_possible_cpus(), 2);
	struct clk *clk;
	int ret;

	pr_info("%pOFn: Init for %d CPU(s)\n", np, num_possible_cpus());

	clk = of_clk_get(np, 0);
	if (IS_ERR(clk)) {
		pr_err("%pOFn: Failed to get CPU clock from DT %ld\n", np,
		       PTR_ERR(clk));
		return PTR_ERR(clk);
	}

	en75_timer_rai.freq_hz = clk_get_rate(clk);

	for (int i = 0; i < num_blocks; i++) {
		en75_timer_rai.membase[i] = of_iomap(np, i);
		if (!en75_timer_rai.membase[i]) {
			pr_err("%pOFn: failed to map register [%d]\n", np, i);
			return -ENXIO;
		}
	}

	/* For clocksource purposes always read clock zero, whatever the CPU */
	ret = clocksource_mmio_init(reg_count(0), np->name,
				    en75_timer_rai.freq_hz, 301, EN75_BITS,
				    clocksource_mmio_readl_up);
	if (ret) {
		pr_err("%pOFn: clocksource_mmio_init failed: %d", np, ret);
		return ret;
	}

	ret = en75_cevt_init(np);
	if (ret < 0)
		return ret;

	sched_clock_register(en75_sched_clock_read, EN75_BITS,
			     en75_timer_rai.freq_hz);

	pr_info("%pOFn: using %u.%03u MHz high precision timer\n", np,
		en75_timer_rai.freq_hz / 1000000,
		(en75_timer_rai.freq_hz / 1000) % 1000);

	return 0;
}

TIMER_OF_DECLARE(en75_timer_hpt, "econet,en75-timer-hpt", en75_timer_init);
