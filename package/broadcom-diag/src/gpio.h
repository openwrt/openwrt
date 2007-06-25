#ifndef __DIAG_GPIO_H
#define __DIAG_GPIO_H
#include <linux/interrupt.h>

#ifndef BCMDRIVER
#include <linux/ssb/ssb.h>
#include <linux/ssb/ssb_driver_chipcommon.h>
#include <linux/ssb/ssb_driver_extif.h>

extern struct ssb_bus ssb;

#define gpio_op(op, param...) \
	do { \
		if (ssb.chipco.dev) \
			return ssb_chipco_gpio_##op(&ssb.chipco, param); \
		else if (ssb.extif.dev) \
			return ssb_extif_gpio_##op(&ssb.extif, param); \
		else \
			return 0; \
	} while (0);
		

static inline u32 gpio_in(void)
{
	gpio_op(in, ~0);
}

static inline u32 gpio_out(u32 mask, u32 value)
{
	gpio_op(out, mask, value);
}

static inline u32 gpio_outen(u32 mask, u32 value)
{
	gpio_op(outen, mask, value);
}

static inline u32 gpio_control(u32 mask, u32 value)
{
	if (ssb.chipco.dev)
		return ssb_chipco_gpio_control(&ssb.chipco, mask, value);
	else
		return 0;
}

static inline u32 gpio_intmask(u32 mask, u32 value)
{
	gpio_op(intmask, mask, value);
}

static inline u32 gpio_intpolarity(u32 mask, u32 value)
{
	gpio_op(polarity, mask, value);
}

static void gpio_set_irqenable(int enabled, irqreturn_t (*handler)(int, void *))
{
	int irq;

	if (ssb.chipco.dev)
		irq = ssb_mips_irq(ssb.chipco.dev) + 2;
	else if (ssb.extif.dev)
		irq = ssb_mips_irq(ssb.extif.dev) + 2;
	else return;
	
	if (enabled) {
		if (request_irq(irq, handler, IRQF_SHARED | IRQF_SAMPLE_RANDOM, "gpio", handler))
			return;
	} else {
		free_irq(irq, handler);
	}

	if (ssb.chipco.dev)
		ssb_write32_masked(ssb.chipco.dev, SSB_CHIPCO_IRQMASK, SSB_CHIPCO_IRQ_GPIO, (enabled ? SSB_CHIPCO_IRQ_GPIO : 0));
}

#else

#include <typedefs.h>
#include <osl.h>
#include <bcmdevs.h>
#include <sbutils.h>
#include <sbconfig.h>
#include <sbchipc.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
#include <sbmips.h>
#else
#include <hndcpu.h>
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#define sbh bcm947xx_sbh
#define sbh_lock bcm947xx_sbh_lock
#endif

extern void *sbh;
extern spinlock_t sbh_lock;

#define gpio_in()	sb_gpioin(sbh)
#define gpio_out(mask, value) 	sb_gpioout(sbh, mask, ((value) & (mask)), GPIO_DRV_PRIORITY)
#define gpio_outen(mask, value) 	sb_gpioouten(sbh, mask, value, GPIO_DRV_PRIORITY)
#define gpio_control(mask, value) 	sb_gpiocontrol(sbh, mask, value, GPIO_DRV_PRIORITY)
#define gpio_intmask(mask, value) 	sb_gpiointmask(sbh, mask, value, GPIO_DRV_PRIORITY)
#define gpio_intpolarity(mask, value) 	sb_gpiointpolarity(sbh, mask, value, GPIO_DRV_PRIORITY)

static void gpio_set_irqenable(int enabled, irqreturn_t (*handler)(int, void *, struct pt_regs *))
{
	unsigned int coreidx;
	unsigned long flags;
	chipcregs_t *cc;
	int irq;

	spin_lock_irqsave(sbh_lock, flags);
	coreidx = sb_coreidx(sbh);

	irq = sb_irq(sbh) + 2;
	if (enabled)
		request_irq(irq, handler, SA_SHIRQ | SA_SAMPLE_RANDOM, "gpio", handler);
	else
		free_irq(irq, handler);

	if ((cc = sb_setcore(sbh, SB_CC, 0))) {
		int intmask;

		intmask = readl(&cc->intmask);
		if (enabled)
			intmask |= CI_GPIO;
		else
			intmask &= ~CI_GPIO;
		writel(intmask, &cc->intmask);
	}
	sb_setcoreidx(sbh, coreidx);
	spin_unlock_irqrestore(sbh_lock, flags);
}

#endif /* BCMDRIVER */

#define EXTIF_ADDR 0x1f000000
#define EXTIF_UART (EXTIF_ADDR + 0x00800000)

#define GPIO_TYPE_NORMAL	(0x0 << 24)
#define GPIO_TYPE_EXTIF 	(0x1 << 24)
#define GPIO_TYPE_MASK  	(0xf << 24)

static inline void gpio_set_extif(int gpio, int value)
{
	volatile u8 *addr = (volatile u8 *) KSEG1ADDR(EXTIF_UART) + (gpio & ~GPIO_TYPE_MASK);
	if (value)
		*addr = 0xFF;
	else
		*addr;
}

#endif /* __DIAG_GPIO_H */
