/*
 *  Realtek RLX based SoC GPIO device
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef _REALTEK_GPIO_H
#define _REALTEK_GPIO_H

void realtek_gpio_init(void);

void realtek_set_gpio_control(u32 gpio, bool soft_ctrl);
void realtek_set_gpio_mux(u32 clear, u32 set);

#endif /* _REALTEK_GPIO_H */
