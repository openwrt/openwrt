#ifndef __DIAG_GPIO_H
#define __DIAG_GPIO_H

#include <linux/interrupt.h>
#include <linux/ssb/ssb_embedded.h>
#include <linux/gpio.h>
#include <bcm47xx.h>

#define EXTIF_ADDR 0x1f000000
#define EXTIF_UART (EXTIF_ADDR + 0x00800000)

#define GPIO_TYPE_NORMAL	(0x0 << 24)
#define GPIO_TYPE_EXTIF 	(0x1 << 24)
#define GPIO_TYPE_MASK  	(0xf << 24)

#endif /* __DIAG_GPIO_H */
