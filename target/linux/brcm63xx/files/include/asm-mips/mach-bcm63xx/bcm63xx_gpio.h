#ifndef BCM63XX_GPIO_H
#define BCM63XX_GPIO_H

/* all helpers will BUG() if gpio count is >= 37. */
#define BCM63XX_GPIO_COUNT	37

#define GPIO_DIR_OUT	0x0
#define GPIO_DIR_IN	0x1

#endif /* !BCM63XX_GPIO_H */
