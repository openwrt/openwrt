#ifndef _ASM_X86_MACH_RDC321X_GPIO_H
#define _ASM_X86_MACH_RDC321X_GPIO_H

#include <linux/kernel.h>

#define gpio_to_irq(gpio)       NULL

#define gpio_get_value __gpio_get_value
#define gpio_set_value __gpio_set_value

#define gpio_cansleep __gpio_cansleep

/* For cansleep */
#include <asm-generic/gpio.h>

#endif /* _ASM_X86_MACH_RDC321X_GPIO_H */
