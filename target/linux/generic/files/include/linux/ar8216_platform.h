/*
 * AR8216 switch driver platform data
 *
 * Copyright (C) 2012 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef AR8216_PLATFORM_H
#define AR8216_PLATFORM_H

enum ar8327_pad_mode {
	AR8327_PAD_NC = 0,
	AR8327_PAD_MAC2MAC_MII,
	AR8327_PAD_MAC2MAC_GMII,
	AR8327_PAD_MAC_SGMII,
	AR8327_PAD_MAC2PHY_MII,
	AR8327_PAD_MAC2PHY_GMII,
	AR8327_PAD_MAC_RGMII,
	AR8327_PAD_PHY_GMII,
	AR8327_PAD_PHY_RGMII,
	AR8327_PAD_PHY_MII,
};

enum ar8327_clk_delay_sel {
	AR8327_CLK_DELAY_SEL0 = 0,
	AR8327_CLK_DELAY_SEL1,
	AR8327_CLK_DELAY_SEL2,
	AR8327_CLK_DELAY_SEL3,
};

struct ar8327_pad_cfg {
	enum ar8327_pad_mode mode;
	bool rxclk_sel;
	bool txclk_sel;
	bool pipe_rxclk_sel;
	bool txclk_delay_en;
	bool rxclk_delay_en;
	enum ar8327_clk_delay_sel txclk_delay_sel;
	enum ar8327_clk_delay_sel rxclk_delay_sel;
};

enum ar8327_port_speed {
	AR8327_PORT_SPEED_10 = 0,
	AR8327_PORT_SPEED_100,
	AR8327_PORT_SPEED_1000,
};

struct ar8327_port_cfg {
	int force_link:1;
	enum ar8327_port_speed speed;
	int txpause:1;
	int rxpause:1;
	int duplex:1;
};

struct ar8327_led_cfg {
	u32 led_ctrl0;
	u32 led_ctrl1;
	u32 led_ctrl2;
	u32 led_ctrl3;
	bool open_drain;
};

struct ar8327_platform_data {
	struct ar8327_pad_cfg *pad0_cfg;
	struct ar8327_pad_cfg *pad5_cfg;
	struct ar8327_pad_cfg *pad6_cfg;
	struct ar8327_port_cfg port0_cfg;
	struct ar8327_led_cfg *led_cfg;
};

#endif /* AR8216_PLATFORM_H */