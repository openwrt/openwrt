/*
 * spi_gpio interface to platform code
 *
 * Copyright (c) 2008 Piotr Skamruk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef _LINUX_SPI_SPI_GPIO
#define _LINUX_SPI_SPI_GPIO

#include <linux/types.h>
#include <linux/spi/spi.h>


/** struct spi_gpio_platform_data - Data definitions for a SPI-GPIO device.
 * This structure holds information about a GPIO-based SPI device.
 *
 * @pin_clk: The GPIO pin number of the CLOCK pin.
 *
 * @pin_miso: The GPIO pin number of the MISO pin.
 *
 * @pin_mosi: The GPIO pin number of the MOSI pin.
 *
 * @pin_cs: The GPIO pin number of the CHIPSELECT pin.
 *
 * @cs_activelow: If true, the chip is selected when the CS line is low.
 *
 * @no_spi_delay: If true, no delay is done in the lowlevel bitbanging.
 *                Note that doing no delay is not standards compliant,
 *                but it might be needed to speed up transfers on some
 *                slow embedded machines.
 *
 * @boardinfo_setup: This callback is called after the
 *                   SPI master device was registered, but before the
 *                   device is registered.
 * @boardinfo_setup_data: Data argument passed to boardinfo_setup().
 */
struct spi_gpio_platform_data {
	unsigned int pin_clk;
	unsigned int pin_miso;
	unsigned int pin_mosi;
	unsigned int pin_cs;
	bool cs_activelow;
	bool no_spi_delay;
	int (*boardinfo_setup)(struct spi_board_info *bi,
			       struct spi_master *master,
			       void *data);
	void *boardinfo_setup_data;
};

#endif /* _LINUX_SPI_SPI_GPIO */
