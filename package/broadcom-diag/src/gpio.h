#ifndef __DIAG_GPIO_H
#define __DIAG_GPIO_H

#include <linux/interrupt.h>
#include <linux/ssb/ssb_embedded.h>
#include <linux/gpio.h>
#include <bcm47xx.h>

static inline u32 gpio_in(void)
{
	switch (bcm47xx_bus_type) {
#ifdef CONFIG_BCM47XX_SSB
	case BCM47XX_BUS_TYPE_SSB:
		return ssb_gpio_in(&bcm47xx_bus.ssb, ~0);
#endif
#ifdef CONFIG_BCM47XX_BCMA
	case BCM47XX_BUS_TYPE_BCMA:
		return bcma_chipco_gpio_in(&bcm47xx_bus.bcma.bus.drv_cc, ~0);
#endif
	}
	return -EINVAL;
}

static inline u32 gpio_out(u32 mask, u32 value)
{
	switch (bcm47xx_bus_type) {
#ifdef CONFIG_BCM47XX_SSB
	case BCM47XX_BUS_TYPE_SSB:
		return ssb_gpio_out(&bcm47xx_bus.ssb, mask, value);
#endif
#ifdef CONFIG_BCM47XX_BCMA
	case BCM47XX_BUS_TYPE_BCMA:
		return bcma_chipco_gpio_out(&bcm47xx_bus.bcma.bus.drv_cc, mask, value);
#endif
	}
	return -EINVAL;
}

static inline u32 gpio_outen(u32 mask, u32 value)
{
	switch (bcm47xx_bus_type) {
#ifdef CONFIG_BCM47XX_SSB
	case BCM47XX_BUS_TYPE_SSB:
		ssb_gpio_outen(&bcm47xx_bus.ssb, mask, value);
		return 0;
#endif
#ifdef CONFIG_BCM47XX_BCMA
	case BCM47XX_BUS_TYPE_BCMA:
		bcma_chipco_gpio_outen(&bcm47xx_bus.bcma.bus.drv_cc, mask, value);
		return 0;
#endif
	}
	return -EINVAL;
}

static inline u32 gpio_control(u32 mask, u32 value)
{
	switch (bcm47xx_bus_type) {
#ifdef CONFIG_BCM47XX_SSB
	case BCM47XX_BUS_TYPE_SSB:
		return ssb_gpio_control(&bcm47xx_bus.ssb, mask, value);
#endif
#ifdef CONFIG_BCM47XX_BCMA
	case BCM47XX_BUS_TYPE_BCMA:
		return bcma_chipco_gpio_control(&bcm47xx_bus.bcma.bus.drv_cc, mask, value);
#endif
	}
	return -EINVAL;
}

static inline u32 gpio_setintmask(u32 mask, u32 value)
{
	switch (bcm47xx_bus_type) {
#ifdef CONFIG_BCM47XX_SSB
	case BCM47XX_BUS_TYPE_SSB:
		return ssb_gpio_intmask(&bcm47xx_bus.ssb, mask, value);
#endif
#ifdef CONFIG_BCM47XX_BCMA
	case BCM47XX_BUS_TYPE_BCMA:
		return bcma_chipco_gpio_intmask(&bcm47xx_bus.bcma.bus.drv_cc, mask, value);
#endif
	}
	return -EINVAL;
}

static inline u32 gpio_intpolarity(u32 mask, u32 value)
{
	switch (bcm47xx_bus_type) {
#ifdef CONFIG_BCM47XX_SSB
	case BCM47XX_BUS_TYPE_SSB:
		return ssb_gpio_polarity(&bcm47xx_bus.ssb, mask, value);
#endif
#ifdef CONFIG_BCM47XX_BCMA
	case BCM47XX_BUS_TYPE_BCMA:
		return bcma_chipco_gpio_polarity(&bcm47xx_bus.bcma.bus.drv_cc, mask, value);
#endif
	}
	return -EINVAL;
}

#ifdef CONFIG_BCM47XX_SSB
static inline u32 __ssb_write32_masked(struct ssb_device *dev, u16 offset,
				       u32 mask, u32 value)
{
	value &= mask;
	value |= ssb_read32(dev, offset) & ~mask;
	ssb_write32(dev, offset, value);
	return value;
}
#endif

#ifdef CONFIG_BCM47XX_BCMA
static inline u32 __bcma_write32_masked(struct bcma_device *dev, u16 offset,
				       u32 mask, u32 value)
{
	value &= mask;
	value |= bcma_read32(dev, offset) & ~mask;
	bcma_write32(dev, offset, value);
	return value;
}
#endif

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

	switch (bcm47xx_bus_type) {
#ifdef CONFIG_BCM47XX_SSB
	case BCM47XX_BUS_TYPE_SSB:
		if (bcm47xx_bus.ssb.chipco.dev)
			__ssb_write32_masked(bcm47xx_bus.ssb.chipco.dev, SSB_CHIPCO_IRQMASK, SSB_CHIPCO_IRQ_GPIO, (enabled ? SSB_CHIPCO_IRQ_GPIO : 0));
#endif
#ifdef CONFIG_BCM47XX_BCMA
	case BCM47XX_BUS_TYPE_BCMA:
		if (bcm47xx_bus.bcma.bus.drv_cc.core)
			__bcma_write32_masked(bcm47xx_bus.bcma.bus.drv_cc.core, BCMA_CC_IRQMASK, BCMA_CC_IRQ_GPIO, (enabled ? BCMA_CC_IRQ_GPIO : 0));
#endif
	}
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
