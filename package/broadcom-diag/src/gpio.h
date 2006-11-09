#ifndef __DIAG_GPIO_H
#define __DIAG_GPIO_H

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

#define EXTIF_ADDR 0x1f000000
#define EXTIF_UART (EXTIF_ADDR + 0x00800000)

#define GPIO_TYPE_NORMAL	(0x0 << 24)
#define GPIO_TYPE_EXTIF 	(0x1 << 24)
#define GPIO_TYPE_MASK  	(0xf << 24)

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

static inline void gpio_set_extif(int gpio, int value)
{
	volatile u8 *addr = (volatile u8 *) KSEG1ADDR(EXTIF_UART) + (gpio & ~GPIO_TYPE_MASK);
	if (value)
		*addr = 0xFF;
	else
		*addr;
}


#endif
