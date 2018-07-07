/*
 *  Realtek SoCs SPI controller device
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef _REALTEK_DEV_SPI_H
#define _REALTEK_DEV_SPI_H

#include <linux/spi/spi.h>

void realtek_register_spi(struct spi_board_info const *info, unsigned n);

#endif /* _REALTEK_DEV_SPI_H */
