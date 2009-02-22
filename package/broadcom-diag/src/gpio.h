#ifndef __DIAG_GPIO_H
#define __DIAG_GPIO_H
#include <linux/interrupt.h>

#ifndef BCMDRIVER
#include <linux/ssb/ssb_embedded.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,25)
#include <linux/gpio.h>
#define ssb ssb_bcm47xx
#endif
extern struct ssb_bus ssb;


static inline u32 gpio_in(void)
{
	return ssb_gpio_in(&ssb, ~0);
}

static inline u32 gpio_out(u32 mask, u32 value)
{
	return ssb_gpio_out(&ssb, mask, value);
}

static inline u32 gpio_outen(u32 mask, u32 value)
{
	return ssb_gpio_outen(&ssb, mask, value);
}

static inline u32 gpio_control(u32 mask, u32 value)
{
	return ssb_gpio_control(&ssb, mask, value);
}

static inline u32 gpio_setintmask(u32 mask, u32 value)
{
	return ssb_gpio_intmask(&ssb, mask, value);
}

static inline u32 gpio_intpolarity(u32 mask, u32 value)
{
	return ssb_gpio_polarity(&ssb, mask, value);
}

static inline u32 __ssb_write32_masked(struct ssb_device *dev, u16 offset,
				       u32 mask, u32 value)
{
	value &= mask;
	value |= ssb_read32(dev, offset) & ~mask;
	ssb_write32(dev, offset, value);
	return value;
}

static void gpio_set_irqenable(int enabled, irqreturn_t (*handler)(int, void *))
{
	int irq;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,25)
	irq = gpio_to_irq(0);
	if (irq == -EINVAL) return;
#else
	if (ssb.chipco.dev)
		irq = ssb_mips_irq(ssb.chipco.dev) + 2;
	else if (ssb.extif.dev)
		irq = ssb_mips_irq(ssb.extif.dev) + 2;
	else return;
#endif
	
	if (enabled) {
		if (request_irq(irq, handler, IRQF_SHARED | IRQF_SAMPLE_RANDOM, "gpio", handler))
			return;
	} else {
		free_irq(irq, handler);
	}

	if (ssb.chipco.dev)
		__ssb_write32_masked(ssb.chipco.dev, SSB_CHIPCO_IRQMASK, SSB_CHIPCO_IRQ_GPIO, (enabled ? SSB_CHIPCO_IRQ_GPIO : 0));
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
#define gpio_setintmask(mask, value) 	sb_gpiointmask(sbh, mask, value, GPIO_DRV_PRIORITY)
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
