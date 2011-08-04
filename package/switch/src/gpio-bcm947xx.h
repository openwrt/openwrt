#ifndef __SWITCH_GPIO_H
#define __SWITCH_GPIO_H

#include <linux/ssb/ssb_embedded.h>
#include <linux/bcma/bcma_driver_chipcommon.h>
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

#endif /* __SWITCH_GPIO_H */
