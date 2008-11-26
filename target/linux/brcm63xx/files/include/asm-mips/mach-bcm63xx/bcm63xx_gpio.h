#ifndef BCM63XX_GPIO_H
#define BCM63XX_GPIO_H

/* all helpers will BUG() if gpio count is >= 37. */
#define BCM63XX_GPIO_COUNT	37

void bcm63xx_gpio_set_dataout(int gpio, int val);
int bcm63xx_gpio_get_datain(int gpio);
void bcm63xx_gpio_set_direction(int gpio, int dir);

#define GPIO_DIR_OUT	0x0
#define GPIO_DIR_IN	0x1

#endif /* !BCM63XX_GPIO_H */
