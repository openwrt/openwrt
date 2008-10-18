#ifndef __BCM947XX_GPIO_H
#define __BCM947XX_GPIO_H

#include <linux/ssb/ssb_embedded.h>

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
	ssb_gpio_outen(&ssb, 1 << gpio, 0);
	return 0;
}

static inline int gpio_direction_output(unsigned gpio, int value)
{
	ssb_gpio_out(&ssb, 1 << gpio, (value ? 1 << gpio : 0));
	ssb_gpio_outen(&ssb, 1 << gpio, 1 << gpio);
	return 0;
}


static inline int gpio_to_irq(unsigned gpio)
{
	struct ssb_device *dev;

	dev = ssb.chipco.dev;
	if (!dev)
		dev = ssb.extif.dev;
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
	return !!ssb_gpio_in(&ssb, 1 << gpio);
}

static inline int gpio_set_value(unsigned gpio, int value)
{
	ssb_gpio_out(&ssb, 1 << gpio, (value ? 1 << gpio : 0));
	return 0;
}


/* cansleep wrappers */
#include <asm-generic/gpio.h>


#endif /* __BCM947XX_GPIO_H */

