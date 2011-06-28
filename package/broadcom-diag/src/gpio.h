#ifndef __DIAG_GPIO_H
#define __DIAG_GPIO_H
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <asm/mach-bcm47xx/bcm47xx.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,0)
#define ssb_bcm47xx bcm47xx_bus.ssb
#endif

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

	irq = gpio_to_irq(0);
	if (irq == -EINVAL) return;
	
	if (enabled) {
		if (request_irq(irq, handler, IRQF_SHARED | IRQF_SAMPLE_RANDOM, "gpio", handler))
			return;
	} else {
		free_irq(irq, handler);
	}

	if (ssb_bcm47xx.chipco.dev)
		__ssb_write32_masked(ssb_bcm47xx.chipco.dev, SSB_CHIPCO_IRQMASK, SSB_CHIPCO_IRQ_GPIO, (enabled ? SSB_CHIPCO_IRQ_GPIO : 0));
}

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
