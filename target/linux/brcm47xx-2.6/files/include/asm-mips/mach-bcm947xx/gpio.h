#ifndef __BCM947XX_GPIO_H
#define __BCM947XX_GPIO_H

#include <linux/ssb/ssb.h>
#include <linux/ssb/ssb_driver_chipcommon.h>
#include <linux/ssb/ssb_driver_extif.h>

extern struct ssb_bus ssb;

static inline int gpio_request(unsigned gpio, const char *label)
{
	return 0;
}

static inline void gpio_free(unsigned gpio)
{
}

static inline int gpio_direction_input(unsigned gpio)
{
	if (ssb.chipco->dev)
		ssb_chipco_gpioouten(&ssb.chipco, 1 << gpio, 0);
	else if (ssb.extif->dev)
		ssb_extif_gpioouten(&ssb.extif, 1 << gpio, 0);
	else
		return -EINVAL;
	return 0;
}

static inline int gpio_direction_output(unsigned gpio)
{
	if (ssb.chipco->dev)
		ssb_chipco_gpioouten(&ssb.chipco, 1 << gpio, 1 << gpio);
	else if (ssb.extif->dev)
		ssb_extif_gpioouten(&ssb.extif, 1 << gpio, 1 << gpio);
	else
		return -EINVAL;

	return 0;
}


static inline int gpio_to_irq(unsigned gpio)
{
	struct ssb_device *dev;
	
	dev = ssb.chipco->dev;
	if (!dev)
		dev = ssb.extif->dev;
	if (!dev)
		return -EINVAL;

	return ssb_mips_irq(dev) + 2;
}

static inline int irq_to_gpio(unsigned gpio)
{
	return -EINVAL;
}


static inline int gpio_get_value(unsigned gpio)
{
	if (ssb.chipco->dev)
		return ssb_chipco_gpioin(&ssb.chipco, 1 << gpio) ? 1 : 0;
	else if (ssb.extif->dev)
		return ssb_extif_gpioin(&ssb.extif, 1 << gpio) ? 1 : 0;
	else
		return 0;
}

static inline int gpio_set_value(unsigned gpio, int value)
{
	if (ssb.chipco->dev)
		ssb_chipco_gpioout(&ssb.chipco, 1 << gpio, (value ? 1 << gpio : 0));
	else if (ssb.extif->dev)
		ssb_extif_gpioout(&ssb.extif, 1 << gpio, (value ? 1 << gpio : 0));
	
	return 0;
}


/* cansleep wrappers */
#include <asm-generic/gpio.h>


#endif /* __BCM947XX_GPIO_H */

