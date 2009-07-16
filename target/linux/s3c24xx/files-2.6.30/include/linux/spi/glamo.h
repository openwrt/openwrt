#ifndef __GLAMO_SPI_H
#define __GLAMO_SPI_H

#include <linux/glamo-gpio.h>

struct glamo_core;

struct glamo_spigpio_platform_data {
	unsigned int pin_clk;
	unsigned int pin_mosi;
	unsigned int pin_miso;
	unsigned int pin_cs;
	int          bus_num;

	struct glamo_core *core;
};


#endif
