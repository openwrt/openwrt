/*
 * arch/ubicom32/include/asm/ubicom32-spi-gpio.h
 *   Platform driver data definitions for GPIO based SPI driver.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#ifndef _ASM_UBICOM32_UBICOM32_SPI_GPIO_H
#define _ASM_UBICOM32_UBICOM32_SPI_GPIO_H

struct ubicom32_spi_gpio_platform_data {
	/*
	 * GPIO to use for MOSI, MISO, CLK
	 */
	int	pin_mosi;
	int	pin_miso;
	int	pin_clk;

	/*
	 * Default state of CLK line
	 */
	int	clk_default;

	/*
	 * Number of chip selects on this bus
	 */
	int	num_chipselect;

	/*
	 * The bus number of this chip
	 */
	int	bus_num;
};

struct ubicom32_spi_gpio_controller_data {
	/*
	 * GPIO to use for chip select
	 */
	int	pin_cs;
};

#endif /* _ASM_UBICOM32_UBICOM32_SPI_GPIO_H */
