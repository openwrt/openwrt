#ifndef __ASM_MIPS_MACH_BCM63XX_GPIO_H
#define __ASM_MIPS_MACH_BCM63XX_GPIO_H

#include <bcm63xx_gpio.h>

static inline int gpio_is_valid(int number)
{
	return (number >= BCM63XX_GPIO_COUNT) ? 0 : 1;
}

static inline int gpio_request(unsigned gpio, const char *label)
{
	return 0;
}

static inline void gpio_free(unsigned gpio)
{
}

static inline int gpio_direction_input(unsigned gpio)
{
	bcm63xx_gpio_set_direction(gpio, GPIO_DIR_IN);
	return 0;
}

static inline int gpio_direction_output(unsigned gpio, int value)
{
	bcm63xx_gpio_set_direction(gpio, GPIO_DIR_OUT);
	return 0;
}

static inline int gpio_get_value(unsigned gpio)
{
	return bcm63xx_gpio_get_datain(gpio);
}

static inline void gpio_set_value(unsigned gpio, int value)
{
	bcm63xx_gpio_set_dataout(gpio, value);
}

static inline int gpio_to_irq(unsigned gpio)
{
	return gpio;
}

static inline int irq_to_gpio(unsigned irq)
{
	return irq;
}

#endif /* __ASM_MIPS_MACH_BCM63XX_GPIO_H */
