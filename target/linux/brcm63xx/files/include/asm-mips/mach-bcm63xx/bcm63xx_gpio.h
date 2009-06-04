#ifndef BCM63XX_GPIO_H
#define BCM63XX_GPIO_H

#include <linux/init.h>

int __init bcm63xx_gpio_init(void);

/* all helpers will BUG() if gpio count is >= 37. */
#define BCM63XX_GPIO_COUNT	37

#define GPIO_DIR_OUT	0x0
#define GPIO_DIR_IN	0x1

#endif /* !BCM63XX_GPIO_H */
