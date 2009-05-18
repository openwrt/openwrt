#ifndef __GLAMO_SPI_H
#define __GLAMO_SPI_H

#include <linux/glamo-gpio.h>

struct spi_board_info;
struct glamofb_handle;
struct glamo_core;

struct glamo_spi_info {
	unsigned long 		board_size;
	struct spi_board_info	*board_info;
	struct glamofb_handle	*glamofb_handle;
};

struct glamo_spigpio_info {
	unsigned int		pin_clk;
	unsigned int		pin_mosi;
	unsigned int		pin_miso;
	unsigned int 		pin_cs;
	int 			bus_num;

	struct glamo_core	*glamo;
};


#endif
