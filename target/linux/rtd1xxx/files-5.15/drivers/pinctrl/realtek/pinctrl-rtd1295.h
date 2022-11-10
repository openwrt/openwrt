/* SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause */

/*
 * Realtek pin controller driver
 *
 * Copyright (c) 2019 Realtek Semiconductor Corp.
 */

#ifndef PINCTRL_RTD1295_H
#define PINCTRL_RTD1295_H

enum rtd1295_iso_pins {
	RTD1295_ISO_GPIO_0 = 0,
	RTD1295_ISO_GPIO_1,
	RTD1295_ISO_GPIO_2,
	RTD1295_ISO_GPIO_3,
	RTD1295_ISO_GPIO_4,
	RTD1295_ISO_GPIO_5,
	RTD1295_HDMI_HPD,
	RTD1295_ISO_GPIO_7,
	RTD1295_IR_RX,
	RTD1295_IR_TX,
	RTD1295_UR0_RX,
	RTD1295_UR0_TX,
	RTD1295_UR1_RX,
	RTD1295_UR1_TX,
	RTD1295_UR1_CTS_N,
	RTD1295_UR1_RTS_N,
	RTD1295_I2C_SCL_0,
	RTD1295_I2C_SDA_0,
	RTD1295_I2C_SCL_1,
	RTD1295_I2C_SDA_1,
	RTD1295_I2C_SCL_6,
	RTD1295_ISO_GPIO_21,
	RTD1295_ISO_GPIO_22,
	RTD1295_ISO_GPIO_23,
	RTD1295_ISO_GPIO_24,
	RTD1295_ISO_GPIO_25,
	RTD1295_I2C_SDA_6,
	RTD1295_ETN_LED_LINK,
	RTD1295_ETN_LED_RXTX,
	RTD1295_NAT_LED_0,
	RTD1295_NAT_LED_1,
	RTD1295_NAT_LED_2,
	RTD1295_NAT_LED_3,
	RTD1295_ISO_GPIO_33,
	RTD1295_ISO_GPIO_34,
	RTD1295_PWM_23_LOC0,
	RTD1295_PWM_01_LOC0,
	RTD1295_PWM_23_LOC1,
	RTD1295_PWM_01_LOC1,
	RTD1295_EJTAG_AVCPU_LOC,
	RTD1295_UR2_LOC,
};

static const struct pinctrl_pin_desc rtd1295_iso_pins[] = {
	PINCTRL_PIN(RTD1295_ISO_GPIO_0, "iso_gpio_0"),
	PINCTRL_PIN(RTD1295_ISO_GPIO_1, "iso_gpio_1"),
	PINCTRL_PIN(RTD1295_ISO_GPIO_2, "iso_gpio_2"),
	PINCTRL_PIN(RTD1295_ISO_GPIO_3, "iso_gpio_3"),
	PINCTRL_PIN(RTD1295_ISO_GPIO_4, "iso_gpio_4"),
	PINCTRL_PIN(RTD1295_ISO_GPIO_5, "iso_gpio_5"),
	PINCTRL_PIN(RTD1295_HDMI_HPD, "hdmi_hpd"),
	PINCTRL_PIN(RTD1295_ISO_GPIO_7, "iso_gpio_7"),
	PINCTRL_PIN(RTD1295_IR_RX, "ir_rx"),
	PINCTRL_PIN(RTD1295_IR_TX, "ir_tx"),
	PINCTRL_PIN(RTD1295_UR0_RX, "ur0_rx"),
	PINCTRL_PIN(RTD1295_UR0_TX, "ur0_tx"),
	PINCTRL_PIN(RTD1295_UR1_RX, "ur1_rx"),
	PINCTRL_PIN(RTD1295_UR1_TX, "ur1_tx"),
	PINCTRL_PIN(RTD1295_UR1_CTS_N, "ur1_cts_n"),
	PINCTRL_PIN(RTD1295_UR1_RTS_N, "ur1_rts_n"),
	PINCTRL_PIN(RTD1295_I2C_SCL_0, "i2c_scl_0"),
	PINCTRL_PIN(RTD1295_I2C_SDA_0, "i2c_sda_0"),
	PINCTRL_PIN(RTD1295_I2C_SCL_1, "i2c_scl_1"),
	PINCTRL_PIN(RTD1295_I2C_SDA_1, "i2c_sda_1"),
	PINCTRL_PIN(RTD1295_I2C_SCL_6, "i2c_scl_6"),
	PINCTRL_PIN(RTD1295_ISO_GPIO_21, "iso_gpio_21"),
	PINCTRL_PIN(RTD1295_ISO_GPIO_22, "iso_gpio_22"),
	PINCTRL_PIN(RTD1295_ISO_GPIO_23, "iso_gpio_23"),
	PINCTRL_PIN(RTD1295_ISO_GPIO_24, "iso_gpio_24"),
	PINCTRL_PIN(RTD1295_ISO_GPIO_25, "iso_gpio_25"),
	PINCTRL_PIN(RTD1295_I2C_SDA_6, "i2c_sda_6"),
	PINCTRL_PIN(RTD1295_ETN_LED_LINK, "etn_led_link"),
	PINCTRL_PIN(RTD1295_ETN_LED_RXTX, "etn_led_rxtx"),
	PINCTRL_PIN(RTD1295_NAT_LED_0, "nat_led_0"),
	PINCTRL_PIN(RTD1295_NAT_LED_1, "nat_led_1"),
	PINCTRL_PIN(RTD1295_NAT_LED_2, "nat_led_2"),
	PINCTRL_PIN(RTD1295_NAT_LED_3, "nat_led_3"),
	PINCTRL_PIN(RTD1295_ISO_GPIO_33, "iso_gpio_33"),
	PINCTRL_PIN(RTD1295_ISO_GPIO_34, "iso_gpio_34"),
	PINCTRL_PIN(RTD1295_PWM_23_LOC0, "pwm_23_loc0"),
	PINCTRL_PIN(RTD1295_PWM_01_LOC0, "pwm_01_loc0"),
	PINCTRL_PIN(RTD1295_PWM_23_LOC1, "pwm_23_loc1"),
	PINCTRL_PIN(RTD1295_PWM_01_LOC1, "pwm_01_loc1"),
	PINCTRL_PIN(RTD1295_EJTAG_AVCPU_LOC, "ejtag_avcpu_loc"),
	PINCTRL_PIN(RTD1295_UR2_LOC, "ur2_loc"),
};

static const unsigned int rtd1295_iso_gpio_0_pins[] = { RTD1295_ISO_GPIO_0 };
static const unsigned int rtd1295_iso_gpio_1_pins[] = { RTD1295_ISO_GPIO_1 };
static const unsigned int rtd1295_iso_gpio_2_pins[] = { RTD1295_ISO_GPIO_2 };
static const unsigned int rtd1295_iso_gpio_3_pins[] = { RTD1295_ISO_GPIO_3 };
static const unsigned int rtd1295_iso_gpio_4_pins[] = { RTD1295_ISO_GPIO_4 };
static const unsigned int rtd1295_iso_gpio_5_pins[] = { RTD1295_ISO_GPIO_5 };
static const unsigned int rtd1295_hdmi_hpd_pins[] = { RTD1295_HDMI_HPD };
static const unsigned int rtd1295_iso_gpio_7_pins[] = { RTD1295_ISO_GPIO_7 };
static const unsigned int rtd1295_ir_rx_pins[] = { RTD1295_IR_RX };
static const unsigned int rtd1295_ir_tx_pins[] = { RTD1295_IR_TX };
static const unsigned int rtd1295_ur0_rx_pins[] = { RTD1295_UR0_RX };
static const unsigned int rtd1295_ur0_tx_pins[] = { RTD1295_UR0_TX };
static const unsigned int rtd1295_ur1_rx_pins[] = { RTD1295_UR1_RX };
static const unsigned int rtd1295_ur1_tx_pins[] = { RTD1295_UR1_TX };
static const unsigned int rtd1295_ur1_cts_n_pins[] = { RTD1295_UR1_CTS_N };
static const unsigned int rtd1295_ur1_rts_n_pins[] = { RTD1295_UR1_RTS_N };
static const unsigned int rtd1295_i2c_scl_0_pins[] = { RTD1295_I2C_SCL_0 };
static const unsigned int rtd1295_i2c_sda_0_pins[] = { RTD1295_I2C_SDA_0 };
static const unsigned int rtd1295_i2c_scl_1_pins[] = { RTD1295_I2C_SCL_1 };
static const unsigned int rtd1295_i2c_sda_1_pins[] = { RTD1295_I2C_SDA_1 };
static const unsigned int rtd1295_i2c_scl_6_pins[] = { RTD1295_I2C_SCL_6 };
static const unsigned int rtd1295_iso_gpio_21_pins[] = { RTD1295_ISO_GPIO_21 };
static const unsigned int rtd1295_iso_gpio_22_pins[] = { RTD1295_ISO_GPIO_22 };
static const unsigned int rtd1295_iso_gpio_23_pins[] = { RTD1295_ISO_GPIO_23 };
static const unsigned int rtd1295_iso_gpio_24_pins[] = { RTD1295_ISO_GPIO_24 };
static const unsigned int rtd1295_iso_gpio_25_pins[] = { RTD1295_ISO_GPIO_25 };
static const unsigned int rtd1295_i2c_sda_6_pins[] = { RTD1295_I2C_SDA_6 };
static const unsigned int rtd1295_etn_led_link_pins[] = {
				RTD1295_ETN_LED_LINK };
static const unsigned int rtd1295_etn_led_rxtx_pins[] = {
				RTD1295_ETN_LED_RXTX };
static const unsigned int rtd1295_nat_led_0_pins[] = { RTD1295_NAT_LED_0 };
static const unsigned int rtd1295_nat_led_1_pins[] = { RTD1295_NAT_LED_1 };
static const unsigned int rtd1295_nat_led_2_pins[] = { RTD1295_NAT_LED_2 };
static const unsigned int rtd1295_nat_led_3_pins[] = { RTD1295_NAT_LED_3 };
static const unsigned int rtd1295_iso_gpio_33_pins[] = { RTD1295_ISO_GPIO_33 };
static const unsigned int rtd1295_iso_gpio_34_pins[] = { RTD1295_ISO_GPIO_34 };
static const unsigned int rtd1295_pwm_23_loc0_pins[] = { RTD1295_PWM_23_LOC0 };
static const unsigned int rtd1295_pwm_01_loc0_pins[] = { RTD1295_PWM_01_LOC0 };
static const unsigned int rtd1295_pwm_23_loc1_pins[] = { RTD1295_PWM_23_LOC1 };
static const unsigned int rtd1295_pwm_01_loc1_pins[] = { RTD1295_PWM_01_LOC1 };
static const unsigned int rtd1295_ejtag_avcpu_loc_pins[] = {
				RTD1295_EJTAG_AVCPU_LOC };
static const unsigned int rtd1295_ur2_loc_pins[] = { RTD1295_UR2_LOC };

static const unsigned int rtd1295_i2c0_pins[] = {
	RTD1295_I2C_SCL_0, RTD1295_I2C_SDA_0
};
static const unsigned int rtd1295_i2c1_pins[] = {
	RTD1295_I2C_SCL_1, RTD1295_I2C_SDA_1
};
static const unsigned int rtd1295_i2c6_pins[] = {
	RTD1295_I2C_SCL_6, RTD1295_I2C_SDA_6
};
static const unsigned int rtd1295_uart0_pins[] = {
				RTD1295_UR0_RX, RTD1295_UR0_TX };
static const unsigned int rtd1295_uart1_pins[] = {
				RTD1295_UR1_RX, RTD1295_UR1_TX };
static const unsigned int rtd1295_uart2_0_pins[] = {
	RTD1295_ISO_GPIO_2, RTD1295_ISO_GPIO_3,
	RTD1295_ISO_GPIO_4, RTD1295_ISO_GPIO_5
};
static const unsigned int rtd1295_uart2_1_pins[] = {
	RTD1295_ISO_GPIO_23, RTD1295_ISO_GPIO_24,
	RTD1295_ISO_GPIO_33, RTD1295_ISO_GPIO_34
};

#define RTD1295_GROUP(_name) \
	{ \
		.name = # _name, \
		.pins = rtd1295_ ## _name ## _pins, \
		.num_pins = ARRAY_SIZE(rtd1295_ ## _name ## _pins), \
	}

static const struct rtd_pin_group_desc rtd1295_iso_pin_groups[] = {
	RTD1295_GROUP(iso_gpio_0),
	RTD1295_GROUP(iso_gpio_1),
	RTD1295_GROUP(iso_gpio_2),
	RTD1295_GROUP(iso_gpio_3),
	RTD1295_GROUP(iso_gpio_4),
	RTD1295_GROUP(iso_gpio_5),
	RTD1295_GROUP(hdmi_hpd),
	RTD1295_GROUP(iso_gpio_7),
	RTD1295_GROUP(ir_rx),
	RTD1295_GROUP(ir_tx),
	RTD1295_GROUP(ur0_rx),
	RTD1295_GROUP(ur0_tx),
	RTD1295_GROUP(ur1_rx),
	RTD1295_GROUP(ur1_tx),
	RTD1295_GROUP(ur1_cts_n),
	RTD1295_GROUP(ur1_rts_n),
	RTD1295_GROUP(i2c_scl_0),
	RTD1295_GROUP(i2c_sda_0),
	RTD1295_GROUP(i2c_scl_1),
	RTD1295_GROUP(i2c_sda_1),
	RTD1295_GROUP(i2c_scl_6),
	RTD1295_GROUP(iso_gpio_21),
	RTD1295_GROUP(iso_gpio_22),
	RTD1295_GROUP(iso_gpio_23),
	RTD1295_GROUP(iso_gpio_24),
	RTD1295_GROUP(iso_gpio_25),
	RTD1295_GROUP(i2c_sda_6),
	RTD1295_GROUP(etn_led_link),
	RTD1295_GROUP(etn_led_rxtx),
	RTD1295_GROUP(nat_led_0),
	RTD1295_GROUP(nat_led_1),
	RTD1295_GROUP(nat_led_2),
	RTD1295_GROUP(nat_led_3),
	RTD1295_GROUP(iso_gpio_33),
	RTD1295_GROUP(iso_gpio_34),
	RTD1295_GROUP(pwm_23_loc0),
	RTD1295_GROUP(pwm_01_loc0),
	RTD1295_GROUP(pwm_23_loc1),
	RTD1295_GROUP(pwm_01_loc1),
	RTD1295_GROUP(ejtag_avcpu_loc),
	RTD1295_GROUP(ur2_loc),

	RTD1295_GROUP(i2c0),
	RTD1295_GROUP(i2c1),
	RTD1295_GROUP(i2c6),
	RTD1295_GROUP(uart0),
	RTD1295_GROUP(uart1),
	RTD1295_GROUP(uart2_0),
	RTD1295_GROUP(uart2_1),
};

static const char * const rtd1295_iso_gpio_groups[] = {
	"iso_gpio_2", "iso_gpio_3", "iso_gpio_4",
	"iso_gpio_5", "hdmi_hpd", "iso_gpio_7",
	"ir_rx", "ir_tx", "ur0_rx", "ur0_tx",
	"ur1_rx", "ur1_tx", "ur1_cts_n", "ur1_rts_n",
	"i2c_scl_0", "i2c_sda_0", "i2c_scl_1", "i2c_sda_1", "i2c_scl_6",
	"iso_gpio_21", "iso_gpio_22", "iso_gpio_23",
	"iso_gpio_24", "iso_gpio_25",
	"i2c_sda_6", "etn_led_link", "etn_led_rxtx",
	"nat_led_0", "nat_led_1", "nat_led_2", "nat_led_3",
	"iso_gpio_33", "iso_gpio_34"
};
static const char * const rtd1295_iso_acpu_ejtag_loc_iso_groups[] = {
	"iso_gpio_2", "iso_gpio_3", "iso_gpio_4", "iso_gpio_5", "iso_gpio_7"
};
static const char * const rtd1295_iso_edp_hpd_groups[] = { "iso_gpio_7" };
static const char * const rtd1295_iso_etn_led_groups[] = {
	"etn_led_link", "etn_led_rxtx"
};
static const char * const rtd1295_iso_i2c0_groups[] = {
	"i2c_scl_0", "i2c_sda_0", "i2c0"
};
static const char * const rtd1295_iso_i2c1_groups[] = {
	"i2c_scl_1", "i2c_sda_1", "i2c1"
};
static const char * const rtd1295_iso_i2c6_groups[] = {
	"i2c_scl_6", "i2c_sda_6", "i2c6"
};
static const char * const rtd1295_iso_ir_rx_groups[] = { "ir_rx" };
static const char * const rtd1295_iso_ir_tx_groups[] = { "ir_tx" };
static const char * const rtd1295_iso_nat_led_groups[] = {
	"nat_led_0", "nat_led_1", "nat_led_2", "nat_led_3"
};
static const char * const rtd1295_iso_pwm_1_groups[] = {
	"etn_led_link", "etn_led_rxtx", "nat_led_0", "nat_led_1"
};
static const char * const rtd1295_iso_pwm_0_groups[] = {
	"iso_gpio_21", "iso_gpio_22", "iso_gpio_23", "iso_gpio_24"
};
static const char * const rtd1295_iso_rtc_groups[] = { "iso_gpio_25" };
static const char * const rtd1295_iso_sc_groups[] = {
	"nat_led_0", "nat_led_1", "nat_led_2", "nat_led_3"
};
static const char * const rtd1295_iso_standby_dbg_groups[] = {
	"iso_gpio_2", "iso_gpio_3", "ir_rx"
};
static const char * const rtd1295_iso_uart0_groups[] = {
	"ur0_rx", "ur0_tx", "uart0"
};
static const char * const rtd1295_iso_uart1_groups[] = {
	"ur1_rx", "ur1_tx", "ur1_cts_n", "ur1_rts_n", "uart1"
};
static const char * const rtd1295_iso_uart2_0_groups[] = {
	"iso_gpio_2", "iso_gpio_3", "iso_gpio_4", "iso_gpio_5", "ur2_loc", "uart2_0"
};
static const char * const rtd1295_iso_uart2_1_groups[] = {
	"iso_gpio_23", "iso_gpio_24", "iso_gpio_33", "iso_gpio_34", "ur2_loc", "uart2_1"
};
static const char * const rtd1295_iso_pwm_01_loc0_normal_groups[] = {
	"pwm_01_loc0"
};
static const char * const rtd1295_iso_pwm_23_loc0_normal_groups[] = {
	"pwm_23_loc0"
};
static const char * const rtd1295_iso_pwm_01_loc0_open_drain_groups[] = {
	"pwm_01_loc0"
};
static const char * const rtd1295_iso_pwm_23_loc0_open_drain_groups[] = {
	"pwm_23_loc0"
};
static const char * const rtd1295_iso_pwm_01_loc1_normal_groups[] = {
	"pwm_01_loc1"
};
static const char * const rtd1295_iso_pwm_23_loc1_normal_groups[] = {
	"pwm_23_loc1"
};
static const char * const rtd1295_iso_pwm_01_loc1_open_drain_groups[] = {
	"pwm_01_loc1"
};
static const char * const rtd1295_iso_pwm_23_loc1_open_drain_groups[] = {
	"pwm_23_loc1"
};
static const char * const rtd1295_iso_acpu_ejtag_loc_nf_groups[] = {
	"ejtag_avcpu_loc"
};


#define RTD1295_FUNC(_name) \
	{ \
		.name = # _name, \
		.groups = rtd1295_iso_ ## _name ## _groups, \
		.num_groups = ARRAY_SIZE(rtd1295_iso_ ## _name ## _groups), \
	}

static const struct rtd_pin_func_desc rtd1295_iso_pin_functions[] = {
	RTD1295_FUNC(gpio),
	RTD1295_FUNC(acpu_ejtag_loc_iso),
	RTD1295_FUNC(edp_hpd),
	RTD1295_FUNC(etn_led),
	RTD1295_FUNC(i2c0),
	RTD1295_FUNC(i2c1),
	RTD1295_FUNC(i2c6),
	RTD1295_FUNC(ir_rx),
	RTD1295_FUNC(ir_tx),
	RTD1295_FUNC(nat_led),
	RTD1295_FUNC(pwm_0),
	RTD1295_FUNC(pwm_1),
	RTD1295_FUNC(rtc),
	RTD1295_FUNC(sc),
	RTD1295_FUNC(standby_dbg),
	RTD1295_FUNC(uart0),
	RTD1295_FUNC(uart1),
	RTD1295_FUNC(uart2_0),
	RTD1295_FUNC(uart2_1),
	RTD1295_FUNC(pwm_01_loc0_normal),
	RTD1295_FUNC(pwm_23_loc0_normal),
	RTD1295_FUNC(pwm_01_loc0_open_drain),
	RTD1295_FUNC(pwm_23_loc0_open_drain),
	RTD1295_FUNC(pwm_01_loc1_normal),
	RTD1295_FUNC(pwm_23_loc1_normal),
	RTD1295_FUNC(pwm_01_loc1_open_drain),
	RTD1295_FUNC(pwm_23_loc1_open_drain),
	RTD1295_FUNC(acpu_ejtag_loc_nf),
};

#undef RTD1295_FUNC

static const struct rtd_pin_desc rtd1295_iso_muxes[] = {
	[RTD1295_ISO_GPIO_4] = RTK_PIN_MUX(iso_gpio_4, 0x10, GENMASK(1, 0),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 0), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 0), "uart2_0"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 0), "acpu_ejtag_loc_iso")),
	[RTD1295_ISO_GPIO_5] = RTK_PIN_MUX(iso_gpio_5, 0x10, GENMASK(3, 2),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 2), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 2), "uart2_0"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 2), "acpu_ejtag_loc_iso")),
	[RTD1295_ISO_GPIO_7] = RTK_PIN_MUX(iso_gpio_7, 0x10, GENMASK(5, 4),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 4), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 4), "edp_hpd"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 4), "acpu_ejtag_loc_iso")),
	[RTD1295_IR_RX] = RTK_PIN_MUX(ir_rx, 0x10, GENMASK(7, 6),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 6), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 6), "ir_rx"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 6), "standby_dbg")),
	[RTD1295_IR_TX] = RTK_PIN_MUX(ir_tx, 0x10, GENMASK(9, 8),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 8), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 8), "ir_tx")),
	[RTD1295_UR0_RX] = RTK_PIN_MUX(ur0_rx, 0x10, GENMASK(11, 10),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 10), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 10), "uart0")),
	[RTD1295_UR0_TX] = RTK_PIN_MUX(ur0_tx, 0x10, GENMASK(13, 12),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 12), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 12), "uart0")),
	[RTD1295_UR1_RX] = RTK_PIN_MUX(ur1_rx, 0x10, GENMASK(15, 14),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 14), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 14), "uart1")),
	[RTD1295_UR1_TX] = RTK_PIN_MUX(ur1_tx, 0x10, GENMASK(17, 16),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 16), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 16), "uart1")),
	[RTD1295_UR1_CTS_N] = RTK_PIN_MUX(ur1_cts_n, 0x10, GENMASK(19, 18),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 18), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 18), "uart1")),
	[RTD1295_UR1_RTS_N] = RTK_PIN_MUX(ur1_rts_n, 0x10, GENMASK(21, 20),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 20), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 20), "uart1")),
	[RTD1295_I2C_SCL_0] = RTK_PIN_MUX(i2c_scl_0, 0x10, GENMASK(23, 22),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 22), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 22), "i2c0")),
	[RTD1295_I2C_SDA_0] = RTK_PIN_MUX(i2c_sda_0, 0x10, GENMASK(25, 24),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 24), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 24), "i2c0")),
	[RTD1295_ETN_LED_LINK] = RTK_PIN_MUX(etn_led_link, 0x10, GENMASK(27, 26),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 26), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 26), "etn_led"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 26), "pwm_1")),
	[RTD1295_ETN_LED_RXTX] = RTK_PIN_MUX(etn_led_rxtx, 0x10, GENMASK(29, 28),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 28), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 28), "etn_led"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 28), "pwm_1")),

	[RTD1295_I2C_SCL_6] = RTK_PIN_MUX(i2c_scl_6, 0x14, GENMASK(1, 0),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 0), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 0), "i2c6")),
	[RTD1295_I2C_SDA_6] = RTK_PIN_MUX(i2c_sda_6, 0x14, GENMASK(3, 2),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 2), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 2), "i2c6")),
	[RTD1295_HDMI_HPD] = RTK_PIN_MUX(hdmi_hpd, 0x14, GENMASK(5, 4),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 4), "gpio")),
	[RTD1295_ISO_GPIO_2] = RTK_PIN_MUX(iso_gpio_2, 0x14, GENMASK(8, 6),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 6), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 6), "standby_dbg"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 6), "acpu_ejtag_loc_iso"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x4, 6), "uart2_0")),
	[RTD1295_ISO_GPIO_3] = RTK_PIN_MUX(iso_gpio_3, 0x14, GENMASK(11, 9),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 9), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 9), "standby_dbg"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 9), "acpu_ejtag_loc_iso"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x4, 9), "uart2_0")),
	[RTD1295_I2C_SCL_1] = RTK_PIN_MUX(i2c_scl_1, 0x14, GENMASK(13, 12),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 12), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 12), "i2c1")),
	[RTD1295_I2C_SDA_1] = RTK_PIN_MUX(i2c_sda_1, 0x14, GENMASK(15, 14),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 14), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 14), "i2c1")),
	[RTD1295_NAT_LED_0] = RTK_PIN_MUX(nat_led_0, 0x14, GENMASK(17, 16),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 16), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 16), "nat_led"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 16), "pwm_1"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 16), "sc")),
	[RTD1295_NAT_LED_1] = RTK_PIN_MUX(nat_led_1, 0x14, GENMASK(19, 18),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 18), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 18), "nat_led"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 18), "pwm_1"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 18), "sc")),
	[RTD1295_NAT_LED_2] = RTK_PIN_MUX(nat_led_2, 0x14, GENMASK(21, 20),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 20), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 20), "nat_led"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 20), "sc")),
	[RTD1295_NAT_LED_3] = RTK_PIN_MUX(nat_led_3, 0x14, GENMASK(23, 22),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 22), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 22), "nat_led"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 22), "sc")),
	[RTD1295_PWM_23_LOC0] = RTK_PIN_MUX(pwm_23_loc0, 0x14, GENMASK(24, 24),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 24), "pwm_23_loc0_normal"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 24), "pwm_23_loc0_open_drain")),
	[RTD1295_PWM_01_LOC0] = RTK_PIN_MUX(pwm_01_loc0, 0x14, GENMASK(25, 25),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 25), "pwm_01_loc0_normal"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 25), "pwm_01_loc0_open_drain")),
	[RTD1295_PWM_23_LOC1] = RTK_PIN_MUX(pwm_23_loc1, 0x14, GENMASK(26, 26),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 26), "pwm_23_loc1_normal"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 26), "pwm_23_loc1_open_drain")),
	[RTD1295_PWM_01_LOC1] = RTK_PIN_MUX(pwm_01_loc1, 0x14, GENMASK(27, 27),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 27), "pwm_01_loc1_normal"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 27), "pwm_01_loc1_open_drain")),
	[RTD1295_EJTAG_AVCPU_LOC] = RTK_PIN_MUX(ejtag_avcpu_loc, 0x14, GENMASK(29, 28),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 28), "acpu_ejtag_loc_iso"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 28), "acpu_ejtag_loc_nf")),
	[RTD1295_UR2_LOC] = RTK_PIN_MUX(ur2_loc, 0x14, GENMASK(31, 30),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 30), "uart2_0"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 30), "uart2_1")),

	[RTD1295_ISO_GPIO_21] = RTK_PIN_MUX(iso_gpio_21, 0x1c, GENMASK(1, 0),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 0), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 0), "pwm_0")),
	[RTD1295_ISO_GPIO_22] = RTK_PIN_MUX(iso_gpio_22, 0x1c, GENMASK(3, 2),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 2), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 2), "pwm_0")),
	[RTD1295_ISO_GPIO_23] = RTK_PIN_MUX(iso_gpio_23, 0x1c, GENMASK(5, 4),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 4), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 4), "pwm_0"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 4), "uart2_1")),
	[RTD1295_ISO_GPIO_24] = RTK_PIN_MUX(iso_gpio_24, 0x1c, GENMASK(7, 6),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 6), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 6), "pwm_0"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 6), "uart2_1")),
	[RTD1295_ISO_GPIO_25] = RTK_PIN_MUX(iso_gpio_25, 0x1c, GENMASK(9, 8),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 8), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 8), "rtc")),
	[RTD1295_ISO_GPIO_33] = RTK_PIN_MUX(iso_gpio_33, 0x1c, GENMASK(11, 10),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 10), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 10), "uart2_1")),
	[RTD1295_ISO_GPIO_34] = RTK_PIN_MUX(iso_gpio_34, 0x1c, GENMASK(13, 12),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 12), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 12), "uart2_1")),
};

static const struct rtd_pin_config_desc rtd1295_iso_configs[] = {
	RTK_PIN_CONFIG(iso_gpio_2, 0x0, 0, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(iso_gpio_3, 0x0, 4, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(ir_rx, 0x0, 8, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(iso_gpio_4, 0x0, 12, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(iso_gpio_5, 0x0, 16, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(iso_gpio_7, 0x0, 20, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(i2c_sda_0, 0x0, 24, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(i2c_scl_0, 0x0, 28, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(ur1_rx, 0x4, 0, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(ur1_tx, 0x4, 4, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(ur1_rts_n, 0x4, 8, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(ur1_cts_n, 0x4, 12, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(ur0_rx, 0x4, 16, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(ur0_tx, 0x4, 20, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(etn_led_link, 0x4, 24, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(etn_led_rxtx, 0x4, 28, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(i2c_sda_6, 0x8, 12, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(i2c_scl_6, 0x8, 16, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(ir_tx, 0x8, 20, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(i2c_sda_1, 0x8, 24, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(i2c_scl_1, 0x8, 28, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(hdmi_hpd, 0xc, 12, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(nat_led_0, 0xc, 16, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(nat_led_1, 0xc, 20, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(nat_led_2, 0xc, 24, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(nat_led_3, 0xc, 28, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(iso_gpio_21, 0x18, 0, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(iso_gpio_22, 0x18, 4, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(iso_gpio_23, 0x18, 8, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(iso_gpio_24, 0x18, 12, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(iso_gpio_25, 0x18, 16, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(iso_gpio_33, 0x20, 0, 1, 0, 2, 3, PCONF_UNSUPP, 2),
	RTK_PIN_CONFIG(iso_gpio_34, 0x20, 4, 1, 0, 2, 3, PCONF_UNSUPP, 2),
};

static struct rtd_pin_reg_list rtd1295_iso_reg_lists[] = {
	{.reg_offset = 0x0},
	{.reg_offset = 0x4},
	{.reg_offset = 0x8},
	{.reg_offset = 0xc},
	{.reg_offset = 0x10},
	{.reg_offset = 0x14},
	{.reg_offset = 0x18},
	{.reg_offset = 0x1c},
	{.reg_offset = 0x20},
};

static const struct rtd_pinctrl_desc rtd1295_iso_pinctrl_desc = {
	.pins = rtd1295_iso_pins,
	.num_pins = ARRAY_SIZE(rtd1295_iso_pins),
	.groups = rtd1295_iso_pin_groups,
	.num_groups = ARRAY_SIZE(rtd1295_iso_pin_groups),
	.functions = rtd1295_iso_pin_functions,
	.num_functions = ARRAY_SIZE(rtd1295_iso_pin_functions),
	.muxes = rtd1295_iso_muxes,
	.num_muxes = ARRAY_SIZE(rtd1295_iso_muxes),
	.configs = rtd1295_iso_configs,
	.num_configs = ARRAY_SIZE(rtd1295_iso_configs),
	.lists = rtd1295_iso_reg_lists,
	.num_regs = ARRAY_SIZE(rtd1295_iso_reg_lists),
};

/* SB2 */

enum rtd1295_sb2_pins {
	RTD1295_GPIO_0 = 0,
	RTD1295_GPIO_1,
	RTD1295_GPIO_2,
	RTD1295_GPIO_3,
	RTD1295_GPIO_4,
	RTD1295_GPIO_5,
	RTD1295_GPIO_6,
	RTD1295_GPIO_7,
	RTD1295_GPIO_8,
	RTD1295_GPIO_9,
	RTD1295_TP1_SYNC,
	RTD1295_I2C_SCL_4,
	RTD1295_I2C_SDA_4,
	RTD1295_I2C_SCL_5,
	RTD1295_I2C_SDA_5,
	RTD1295_USB_ID,
	RTD1295_SENSOR_CKO_0,
	RTD1295_SENSOR_CKO_1,
	RTD1295_SENSOR_RST,
	RTD1295_SENSOR_STB_0,
	RTD1295_SENSOR_STB_1,
	RTD1295_TP0_DATA,
	RTD1295_TP0_CLK,
	RTD1295_TP0_VALID,
	RTD1295_TP0_SYNC,
	RTD1295_TP1_DATA,
	RTD1295_TP1_CLK,
	RTD1295_TP1_VALID,
	RTD1295_RGMII0_TXC,
	RTD1295_RGMII0_TX_CTL,
	RTD1295_RGMII0_TXD_0,
	RTD1295_RGMII0_TXD_1,
	RTD1295_RGMII0_TXD_2,
	RTD1295_RGMII0_TXD_3,
	RTD1295_RGMII0_RXC,
	RTD1295_RGMII0_RX_CTL,
	RTD1295_RGMII0_RXD_0,
	RTD1295_RGMII0_RXD_1,
	RTD1295_RGMII0_RXD_2,
	RTD1295_RGMII0_RXD_3,
	RTD1295_RGMII0_MDIO,
	RTD1295_RGMII0_MDC,
	RTD1295_RGMII1_TXC,
	RTD1295_RGMII1_TX_CTL,
	RTD1295_RGMII1_TXD_0,
	RTD1295_RGMII1_TXD_1,
	RTD1295_RGMII1_TXD_2,
	RTD1295_RGMII1_TXD_3,
	RTD1295_RGMII1_RXC,
	RTD1295_RGMII1_RX_CTL,
	RTD1295_RGMII1_RXD_0,
	RTD1295_RGMII1_RXD_1,
	RTD1295_RGMII1_RXD_2,
	RTD1295_RGMII1_RXD_3,
	RTD1295_HIF_LOC,
	RTD1295_EJTAG_SCPU_LOC,
	RTD1295_SF_EN,
	RTD1295_TP0_LOC,
	RTD1295_TP1_LOC,
};

static const struct pinctrl_pin_desc rtd1295_sb2_pins[] = {
	PINCTRL_PIN(RTD1295_GPIO_0, "gpio_0"),
	PINCTRL_PIN(RTD1295_GPIO_1, "gpio_1"),
	PINCTRL_PIN(RTD1295_GPIO_2, "gpio_2"),
	PINCTRL_PIN(RTD1295_GPIO_3, "gpio_3"),
	PINCTRL_PIN(RTD1295_GPIO_4, "gpio_4"),
	PINCTRL_PIN(RTD1295_GPIO_5, "gpio_5"),
	PINCTRL_PIN(RTD1295_GPIO_6, "gpio_6"),
	PINCTRL_PIN(RTD1295_GPIO_7, "gpio_7"),
	PINCTRL_PIN(RTD1295_GPIO_8, "gpio_8"),
	PINCTRL_PIN(RTD1295_GPIO_9, "gpio_9"),
	PINCTRL_PIN(RTD1295_TP1_SYNC, "tp1_sync"),
	PINCTRL_PIN(RTD1295_I2C_SCL_4, "i2c_scl_4"),
	PINCTRL_PIN(RTD1295_I2C_SDA_4, "i2c_sda_4"),
	PINCTRL_PIN(RTD1295_I2C_SCL_5, "i2c_scl_5"),
	PINCTRL_PIN(RTD1295_I2C_SDA_5, "i2c_sda_5"),
	PINCTRL_PIN(RTD1295_USB_ID, "usb_id"),
	PINCTRL_PIN(RTD1295_SENSOR_CKO_0, "sensor_cko_0"),
	PINCTRL_PIN(RTD1295_SENSOR_CKO_1, "sensor_cko_1"),
	PINCTRL_PIN(RTD1295_SENSOR_RST,   "sensor_rst"),
	PINCTRL_PIN(RTD1295_SENSOR_STB_0, "sensor_stb_0"),
	PINCTRL_PIN(RTD1295_SENSOR_STB_1, "sensor_stb_1"),
	PINCTRL_PIN(RTD1295_TP0_DATA,  "tp0_data"),
	PINCTRL_PIN(RTD1295_TP0_CLK,   "tp0_clk"),
	PINCTRL_PIN(RTD1295_TP0_VALID, "tp0_valid"),
	PINCTRL_PIN(RTD1295_TP0_SYNC,  "tp0_sync"),
	PINCTRL_PIN(RTD1295_TP1_DATA,  "tp1_data"),
	PINCTRL_PIN(RTD1295_TP1_CLK,   "tp1_clk"),
	PINCTRL_PIN(RTD1295_TP1_VALID, "tp1_valid"),
	PINCTRL_PIN(RTD1295_RGMII0_TXC,    "rgmii0_txc"),
	PINCTRL_PIN(RTD1295_RGMII0_TX_CTL, "rgmii0_tx_ctl"),
	PINCTRL_PIN(RTD1295_RGMII0_TXD_0,  "rgmii0_txd_0"),
	PINCTRL_PIN(RTD1295_RGMII0_TXD_1,  "rgmii0_txd_1"),
	PINCTRL_PIN(RTD1295_RGMII0_TXD_2,  "rgmii0_txd_2"),
	PINCTRL_PIN(RTD1295_RGMII0_TXD_3,  "rgmii0_txd_3"),
	PINCTRL_PIN(RTD1295_RGMII0_RXC,    "rgmii0_rxc"),
	PINCTRL_PIN(RTD1295_RGMII0_RX_CTL, "rgmii0_rx_ctl"),
	PINCTRL_PIN(RTD1295_RGMII0_RXD_0,  "rgmii0_rxd_0"),
	PINCTRL_PIN(RTD1295_RGMII0_RXD_1,  "rgmii0_rxd_1"),
	PINCTRL_PIN(RTD1295_RGMII0_RXD_2,  "rgmii0_rxd_2"),
	PINCTRL_PIN(RTD1295_RGMII0_RXD_3,  "rgmii0_rxd_3"),
	PINCTRL_PIN(RTD1295_RGMII0_MDIO,   "rgmii0_mdio"),
	PINCTRL_PIN(RTD1295_RGMII0_MDC,    "rgmii0_mdc"),
	PINCTRL_PIN(RTD1295_RGMII1_TXC,    "rgmii1_txc"),
	PINCTRL_PIN(RTD1295_RGMII1_TX_CTL, "rgmii1_tx_ctl"),
	PINCTRL_PIN(RTD1295_RGMII1_TXD_0,  "rgmii1_txd_0"),
	PINCTRL_PIN(RTD1295_RGMII1_TXD_1,  "rgmii1_txd_1"),
	PINCTRL_PIN(RTD1295_RGMII1_TXD_2,  "rgmii1_txd_2"),
	PINCTRL_PIN(RTD1295_RGMII1_TXD_3,  "rgmii1_txd_3"),
	PINCTRL_PIN(RTD1295_RGMII1_RXC,    "rgmii1_rxc"),
	PINCTRL_PIN(RTD1295_RGMII1_RX_CTL, "rgmii1_rx_ctl"),
	PINCTRL_PIN(RTD1295_RGMII1_RXD_0,  "rgmii1_rxd_0"),
	PINCTRL_PIN(RTD1295_RGMII1_RXD_1,  "rgmii1_rxd_1"),
	PINCTRL_PIN(RTD1295_RGMII1_RXD_2,  "rgmii1_rxd_2"),
	PINCTRL_PIN(RTD1295_RGMII1_RXD_3,  "rgmii1_rxd_3"),
	PINCTRL_PIN(RTD1295_HIF_LOC,  "hif_loc"),
	PINCTRL_PIN(RTD1295_EJTAG_SCPU_LOC,  "ejtag_scpu_loc"),
	PINCTRL_PIN(RTD1295_SF_EN,  "sf_en"),
	PINCTRL_PIN(RTD1295_TP0_LOC,  "tp0_loc"),
	PINCTRL_PIN(RTD1295_TP1_LOC,  "tp1_loc"),
};

static const unsigned int rtd1295_gpio_0_pins[] = { RTD1295_GPIO_0 };
static const unsigned int rtd1295_gpio_1_pins[] = { RTD1295_GPIO_1 };
static const unsigned int rtd1295_gpio_2_pins[] = { RTD1295_GPIO_2 };
static const unsigned int rtd1295_gpio_3_pins[] = { RTD1295_GPIO_3 };
static const unsigned int rtd1295_gpio_4_pins[] = { RTD1295_GPIO_4 };
static const unsigned int rtd1295_gpio_5_pins[] = { RTD1295_GPIO_5 };
static const unsigned int rtd1295_gpio_6_pins[] = { RTD1295_GPIO_6 };
static const unsigned int rtd1295_gpio_7_pins[] = { RTD1295_GPIO_7 };
static const unsigned int rtd1295_gpio_8_pins[] = { RTD1295_GPIO_8 };
static const unsigned int rtd1295_gpio_9_pins[] = { RTD1295_GPIO_9 };
static const unsigned int rtd1295_tp1_sync_pins[] = { RTD1295_TP1_SYNC };
static const unsigned int rtd1295_i2c_scl_4_pins[] = { RTD1295_I2C_SCL_4 };
static const unsigned int rtd1295_i2c_sda_4_pins[] = { RTD1295_I2C_SDA_4 };
static const unsigned int rtd1295_i2c_scl_5_pins[] = { RTD1295_I2C_SCL_5 };
static const unsigned int rtd1295_i2c_sda_5_pins[] = { RTD1295_I2C_SDA_5 };
static const unsigned int rtd1295_usb_id_pins[] = { RTD1295_USB_ID };
static const unsigned int rtd1295_sensor_cko_0_pins[] = {
				RTD1295_SENSOR_CKO_0 };
static const unsigned int rtd1295_sensor_cko_1_pins[] = {
				RTD1295_SENSOR_CKO_1 };
static const unsigned int rtd1295_sensor_rst_pins[] = { RTD1295_SENSOR_RST };
static const unsigned int rtd1295_sensor_stb_0_pins[] = {
				RTD1295_SENSOR_STB_0 };
static const unsigned int rtd1295_sensor_stb_1_pins[] = {
				RTD1295_SENSOR_STB_1 };
static const unsigned int rtd1295_tp0_data_pins[] = { RTD1295_TP0_DATA };
static const unsigned int rtd1295_tp0_clk_pins[] = { RTD1295_TP0_CLK };
static const unsigned int rtd1295_tp0_valid_pins[] = { RTD1295_TP0_VALID };
static const unsigned int rtd1295_tp0_sync_pins[] = { RTD1295_TP0_SYNC };
static const unsigned int rtd1295_tp1_data_pins[] = { RTD1295_TP1_DATA };
static const unsigned int rtd1295_tp1_clk_pins[] = { RTD1295_TP1_CLK };
static const unsigned int rtd1295_tp1_valid_pins[] = { RTD1295_TP1_VALID };
static const unsigned int rtd1295_rgmii0_txc_pins[] = { RTD1295_RGMII0_TXC };
static const unsigned int rtd1295_rgmii0_tx_ctl_pins[] = {
				RTD1295_RGMII0_TX_CTL };
static const unsigned int rtd1295_rgmii0_txd_0_pins[] = {
				RTD1295_RGMII0_TXD_0 };
static const unsigned int rtd1295_rgmii0_txd_1_pins[] = {
				RTD1295_RGMII0_TXD_1 };
static const unsigned int rtd1295_rgmii0_txd_2_pins[] = {
				RTD1295_RGMII0_TXD_2 };
static const unsigned int rtd1295_rgmii0_txd_3_pins[] = {
				RTD1295_RGMII0_TXD_3 };
static const unsigned int rtd1295_rgmii0_rxc_pins[] = { RTD1295_RGMII0_RXC };
static const unsigned int rtd1295_rgmii0_rx_ctl_pins[] = {
				RTD1295_RGMII0_RX_CTL };
static const unsigned int rtd1295_rgmii0_rxd_0_pins[] = {
				RTD1295_RGMII0_RXD_0 };
static const unsigned int rtd1295_rgmii0_rxd_1_pins[] = {
				RTD1295_RGMII0_RXD_1 };
static const unsigned int rtd1295_rgmii0_rxd_2_pins[] = {
				RTD1295_RGMII0_RXD_2 };
static const unsigned int rtd1295_rgmii0_rxd_3_pins[] = {
				RTD1295_RGMII0_RXD_3 };
static const unsigned int rtd1295_rgmii0_mdio_pins[] = { RTD1295_RGMII0_MDIO };
static const unsigned int rtd1295_rgmii0_mdc_pins[] = { RTD1295_RGMII0_MDC };
static const unsigned int rtd1295_rgmii1_txc_pins[] = { RTD1295_RGMII1_TXC };
static const unsigned int rtd1295_rgmii1_tx_ctl_pins[] = {
				RTD1295_RGMII1_TX_CTL };
static const unsigned int rtd1295_rgmii1_txd_0_pins[] = {
				RTD1295_RGMII1_TXD_0 };
static const unsigned int rtd1295_rgmii1_txd_1_pins[] = {
				RTD1295_RGMII1_TXD_1 };
static const unsigned int rtd1295_rgmii1_txd_2_pins[] = {
				RTD1295_RGMII1_TXD_2 };
static const unsigned int rtd1295_rgmii1_txd_3_pins[] = {
				RTD1295_RGMII1_TXD_3 };
static const unsigned int rtd1295_rgmii1_rxc_pins[] = { RTD1295_RGMII1_RXC };
static const unsigned int rtd1295_rgmii1_rx_ctl_pins[] = {
				RTD1295_RGMII1_RX_CTL };
static const unsigned int rtd1295_rgmii1_rxd_0_pins[] = {
				RTD1295_RGMII1_RXD_0 };
static const unsigned int rtd1295_rgmii1_rxd_1_pins[] = {
				RTD1295_RGMII1_RXD_1 };
static const unsigned int rtd1295_rgmii1_rxd_2_pins[] = {
				RTD1295_RGMII1_RXD_2 };
static const unsigned int rtd1295_rgmii1_rxd_3_pins[] = {
				RTD1295_RGMII1_RXD_3 };
static const unsigned int rtd1295_hif_loc_pins[] = { RTD1295_HIF_LOC };
static const unsigned int rtd1295_ejtag_scpu_loc_pins[] = {
				RTD1295_EJTAG_SCPU_LOC };
static const unsigned int rtd1295_sf_en_pins[] = { RTD1295_SF_EN };
static const unsigned int rtd1295_tp0_loc_pins[] = { RTD1295_TP0_LOC };
static const unsigned int rtd1295_tp1_loc_pins[] = { RTD1295_TP1_LOC };






static const struct rtd_pin_group_desc rtd1295_sb2_pin_groups[] = {
	RTD1295_GROUP(gpio_0),
	RTD1295_GROUP(gpio_1),
	RTD1295_GROUP(gpio_2),
	RTD1295_GROUP(gpio_3),
	RTD1295_GROUP(gpio_4),
	RTD1295_GROUP(gpio_5),
	RTD1295_GROUP(gpio_6),
	RTD1295_GROUP(gpio_7),
	RTD1295_GROUP(gpio_8),
	RTD1295_GROUP(gpio_9),
	RTD1295_GROUP(tp1_sync),
	RTD1295_GROUP(i2c_scl_4),
	RTD1295_GROUP(i2c_sda_4),
	RTD1295_GROUP(i2c_scl_5),
	RTD1295_GROUP(i2c_sda_5),
	RTD1295_GROUP(usb_id),
	RTD1295_GROUP(sensor_cko_0),
	RTD1295_GROUP(sensor_cko_1),
	RTD1295_GROUP(sensor_rst),
	RTD1295_GROUP(sensor_stb_0),
	RTD1295_GROUP(sensor_stb_1),
	RTD1295_GROUP(tp0_data),
	RTD1295_GROUP(tp0_clk),
	RTD1295_GROUP(tp0_valid),
	RTD1295_GROUP(tp0_sync),
	RTD1295_GROUP(tp1_data),
	RTD1295_GROUP(tp1_clk),
	RTD1295_GROUP(tp1_valid),
	RTD1295_GROUP(rgmii0_txc),
	RTD1295_GROUP(rgmii0_tx_ctl),
	RTD1295_GROUP(rgmii0_txd_0),
	RTD1295_GROUP(rgmii0_txd_1),
	RTD1295_GROUP(rgmii0_txd_2),
	RTD1295_GROUP(rgmii0_txd_3),
	RTD1295_GROUP(rgmii0_rxc),
	RTD1295_GROUP(rgmii0_rx_ctl),
	RTD1295_GROUP(rgmii0_rxd_0),
	RTD1295_GROUP(rgmii0_rxd_1),
	RTD1295_GROUP(rgmii0_rxd_2),
	RTD1295_GROUP(rgmii0_rxd_3),
	RTD1295_GROUP(rgmii0_mdio),
	RTD1295_GROUP(rgmii0_mdc),
	RTD1295_GROUP(rgmii1_txc),
	RTD1295_GROUP(rgmii1_tx_ctl),
	RTD1295_GROUP(rgmii1_txd_0),
	RTD1295_GROUP(rgmii1_txd_1),
	RTD1295_GROUP(rgmii1_txd_2),
	RTD1295_GROUP(rgmii1_txd_3),
	RTD1295_GROUP(rgmii1_rxc),
	RTD1295_GROUP(rgmii1_rx_ctl),
	RTD1295_GROUP(rgmii1_rxd_0),
	RTD1295_GROUP(rgmii1_rxd_1),
	RTD1295_GROUP(rgmii1_rxd_2),
	RTD1295_GROUP(rgmii1_rxd_3),
	RTD1295_GROUP(hif_loc),
	RTD1295_GROUP(ejtag_scpu_loc),
	RTD1295_GROUP(sf_en),
	RTD1295_GROUP(tp0_loc),
	RTD1295_GROUP(tp1_loc),
};

static const char * const rtd1295_sb2_gpio_groups[] = {
	"gpio_0", "gpio_1", "gpio_2", "gpio_3", "gpio_4",
	"gpio_5", "gpio_6", "gpio_7", "gpio_8", "gpio_9",
	"i2c_scl_4", "i2c_sda_4", "i2c_scl_5", "i2c_sda_5", "usb_id",
	"sensor_cko_0", "sensor_cko_1", "sensor_rst",
	"sensor_stb_0", "sensor_stb_1",
	"tp0_data", "tp0_clk", "tp0_valid", "tp0_sync",
	"tp1_data", "tp1_clk", "tp1_valid", "tp1_sync",
	"rgmii0_txc", "rgmii0_tx_ctl", "rgmii0_txd_0",
	"rgmii0_txd_1", "rgmii0_txd_2", "rgmii0_txd_3",
	"rgmii0_rxc", "rgmii0_rx_ctl", "rgmii0_rxd_0",
	"rgmii0_rxd_1", "rgmii0_rxd_2", "rgmii0_rxd_3",
	"rgmii0_mdio", "rgmii0_mdc",
	"rgmii1_txc", "rgmii1_tx_ctl", "rgmii1_txd_0",
	"rgmii1_txd_1", "rgmii1_txd_2", "rgmii1_txd_3",
	"rgmii1_rxc", "rgmii1_rx_ctl", "rgmii1_rxd_0",
	"rgmii1_rxd_1", "rgmii1_rxd_2", "rgmii1_rxd_3",
};
static const char * const rtd1295_sb2_ai_groups[] = {
	"tp0_data", "tp0_sync", "tp0_valid", "tp0_clk"
};
static const char * const rtd1295_sb2_dc_fan_sensor_groups[] = { "gpio_9" };
static const char * const rtd1295_sb2_eth_gpy_groups[] = {
	"rgmii0_mdio", "rgmii0_mdc"
};
static const char * const rtd1295_sb2_gspi_groups[] = {
	"gpio_4", "gpio_5", "gpio_6", "gpio_7"
};
static const char * const rtd1295_sb2_i2c2_groups[] = {
	"tp1_sync", "tp1_clk"
};
static const char * const rtd1295_sb2_i2c3_groups[] = {
	"tp1_data", "tp1_valid"
};
static const char * const rtd1295_sb2_i2c4_groups[] = {
	"i2c_scl_4", "i2c_sda_4"
};
static const char * const rtd1295_sb2_i2c5_groups[] = {
	"i2c_scl_5", "i2c_sda_5"
};
static const char * const rtd1295_sb2_nand_groups[] = {
	"i2c_sda_5", "i2c_scl_5"
};
static const char * const rtd1295_sb2_rgmii_groups[] = {
	"rgmii0_txc", "rgmii0_tx_ctl", "rgmii0_txd_0",
	"rgmii0_txd_1", "rgmii0_txd_2", "rgmii0_txd_3",
	"rgmii0_rxc", "rgmii0_rx_ctl", "rgmii0_rxd_0",
	"rgmii0_rxd_1", "rgmii0_rxd_2", "rgmii0_rxd_3",
	"rgmii0_mdio", "rgmii0_mdc",
	"rgmii1_txc", "rgmii1_tx_ctl", "rgmii1_txd_0",
	"rgmii1_txd_1", "rgmii1_txd_2", "rgmii1_txd_3",
	"rgmii1_rxc", "rgmii1_rx_ctl", "rgmii1_rxd_0",
	"rgmii1_rxd_1", "rgmii1_rxd_2", "rgmii1_rxd_3",
};

static const char * const rtd1295_sb2_scpu_ejtag_loc_gpio_groups[] = {
	"gpio_4", "gpio_5", "gpio_6", "gpio_7", "gpio_8", "ejtag_scpu_loc"
};
static const char * const rtd1295_sb2_scpu_ejtag_loc_cr_groups[] = {
	"ejtag_scpu_loc"
};
static const char * const rtd1295_sb2_sensor_cko_output_groups[] = {
	"sensor_cko_0", "sensor_cko_1"
};
static const char * const rtd1295_sb2_spi_groups[] = {
	"gpio_0", "gpio_1", "gpio_2", "gpio_3"
};
static const char * const rtd1295_sb2_test_loop_dis_groups[] = { "usb_id" };
static const char * const rtd1295_sb2_tp0_loc_rgmii0_tx_groups[] = {
	"rgmii0_txd_0", "rgmii0_txd_1", "rgmii0_txd_2",
	"rgmii0_txd_3", "tp0_loc"
};
static const char * const rtd1295_sb2_tp0_loc_tp0_groups[] = {
	"tp0_data", "tp0_sync", "tp0_valid", "tp0_clk", "tp0_loc"
};
static const char * const rtd1295_sb2_tp0_loc_tp1_groups[] = {
	"tp0_data", "tp0_sync", "tp0_valid", "tp0_clk", "tp0_loc"
};
static const char * const rtd1295_sb2_tp1_loc_rgmii0_rx_groups[] = {
	"rgmii0_rxd_0", "rgmii0_rxd_1", "rgmii0_rxd_2",
	"rgmii0_rxd_3", "tp1_loc"
};
static const char * const rtd1295_sb2_tp1_loc_tp0_groups[] = {
	"tp1_data", "tp1_sync", "tp1_valid", "tp1_clk", "tp1_loc"
};
static const char * const rtd1295_sb2_tp1_loc_tp1_groups[] = {
	"tp1_data", "tp1_sync", "tp1_valid", "tp1_clk", "tp1_loc"
};
static const char * const rtd1295_sb2_usb_clock_output_groups[] = {
	"sensor_cko_1"
};
static const char * const rtd1295_sb2_hif_loc_misc_groups[] = { "hif_loc" };
static const char * const rtd1295_sb2_hif_loc_nf_groups[] = { "hif_loc" };



#define RTD1295_FUNC(_name) \
	{ \
		.name = # _name, \
		.groups = rtd1295_sb2_ ## _name ## _groups, \
		.num_groups = ARRAY_SIZE(rtd1295_sb2_ ## _name ## _groups), \
	}

static const struct rtd_pin_func_desc rtd1295_sb2_pin_functions[] = {
	RTD1295_FUNC(gpio),
	RTD1295_FUNC(ai),
	RTD1295_FUNC(dc_fan_sensor),
	RTD1295_FUNC(eth_gpy),
	RTD1295_FUNC(gspi),
	RTD1295_FUNC(i2c2),
	RTD1295_FUNC(i2c3),
	RTD1295_FUNC(i2c4),
	RTD1295_FUNC(i2c5),
	RTD1295_FUNC(nand),
	RTD1295_FUNC(rgmii),
	RTD1295_FUNC(scpu_ejtag_loc_gpio),
	RTD1295_FUNC(sensor_cko_output),
	RTD1295_FUNC(spi),
	RTD1295_FUNC(test_loop_dis),
	RTD1295_FUNC(tp0_loc_rgmii0_tx),
	RTD1295_FUNC(tp0_loc_tp0),
	RTD1295_FUNC(tp0_loc_tp1),
	RTD1295_FUNC(tp1_loc_rgmii0_rx),
	RTD1295_FUNC(tp1_loc_tp0),
	RTD1295_FUNC(tp1_loc_tp1),
	RTD1295_FUNC(usb_clock_output),
	RTD1295_FUNC(hif_loc_misc),
	RTD1295_FUNC(hif_loc_nf),
	RTD1295_FUNC(scpu_ejtag_loc_cr),
};

#undef RTD1295_FUNC

static const struct rtd_pin_desc rtd1295_sb2_muxes[] = {
	[RTD1295_TP0_DATA] = RTK_PIN_MUX(tp0_data, 0x08, GENMASK(2, 0),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 0), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 0), "tp0_loc_tp0"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 0), "tp0_loc_tp1"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 0), "ai")),
	[RTD1295_TP0_SYNC] = RTK_PIN_MUX(tp0_sync, 0x08, GENMASK(5, 3),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 3), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 3), "tp0_loc_tp0"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 3), "tp0_loc_tp1"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 3), "ai")),
	[RTD1295_TP0_VALID] = RTK_PIN_MUX(tp0_valid, 0x08, GENMASK(8, 6),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 6), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 6), "tp0_loc_tp0"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 6), "tp0_loc_tp1"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 6), "ai")),
	[RTD1295_TP0_CLK] = RTK_PIN_MUX(tp0_clk, 0x08, GENMASK(11, 9),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 9), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 9), "tp0_loc_tp0"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 9), "tp0_loc_tp1"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 9), "ai")),
	[RTD1295_TP1_DATA] = RTK_PIN_MUX(tp1_data, 0x08, GENMASK(17, 16),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 16), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 16), "tp1_loc_tp1"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 16), "tp1_loc_tp0"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 16), "i2c3")),
	[RTD1295_TP1_SYNC] = RTK_PIN_MUX(tp1_sync, 0x08, GENMASK(19, 18),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 18), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 18), "tp1_loc_tp1"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 18), "tp1_loc_tp0"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 18), "i2c2")),
	[RTD1295_TP1_VALID] = RTK_PIN_MUX(tp1_valid, 0x08, GENMASK(21, 20),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 20), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 20), "tp1_loc_tp1"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 20), "tp1_loc_tp0"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 20), "i2c3")),
	[RTD1295_TP1_CLK] = RTK_PIN_MUX(tp1_clk, 0x08, GENMASK(23, 22),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 22), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 22), "tp1_loc_tp1"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 22), "tp1_loc_tp0"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 22), "i2c2")),

	[RTD1295_GPIO_0] = RTK_PIN_MUX(gpio_0, 0x10, GENMASK(2, 0),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 0), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 0), "spi")),
	[RTD1295_GPIO_1] = RTK_PIN_MUX(gpio_1, 0x10, GENMASK(5, 3),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 3), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 3), "spi")),
	[RTD1295_GPIO_2] = RTK_PIN_MUX(gpio_2, 0x10, GENMASK(8, 6),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 6), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 6), "spi")),
	[RTD1295_GPIO_3] = RTK_PIN_MUX(gpio_3, 0x10, GENMASK(11, 9),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 9), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 9), "spi")),
	[RTD1295_GPIO_4] = RTK_PIN_MUX(gpio_4, 0x10, GENMASK(13, 12),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 12), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 12), "scpu_ejtag_loc_gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 12), "gspi")),
	[RTD1295_GPIO_5] = RTK_PIN_MUX(gpio_5, 0x10, GENMASK(15, 14),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 14), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 14), "scpu_ejtag_loc_gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 14), "gspi")),
	[RTD1295_GPIO_6] = RTK_PIN_MUX(gpio_6, 0x10, GENMASK(17, 16),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 16), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 15), "scpu_ejtag_loc_gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 15), "gspi")),
	[RTD1295_GPIO_7] = RTK_PIN_MUX(gpio_7, 0x10, GENMASK(19, 18),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 18), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 18), "scpu_ejtag_loc_gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 18), "gspi")),
	[RTD1295_GPIO_8] = RTK_PIN_MUX(gpio_8, 0x10, GENMASK(21, 20),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 20), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 20), "scpu_ejtag_loc_gpio")),
	[RTD1295_GPIO_9] = RTK_PIN_MUX(gpio_9, 0x10, GENMASK(23, 22),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 22), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 22), "dc_fan_sensor")),

	[RTD1295_I2C_SDA_4] = RTK_PIN_MUX(i2c_sda_4, 0x0c, GENMASK(5, 4),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 4), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 4), "i2c4")),
	[RTD1295_I2C_SCL_4] = RTK_PIN_MUX(i2c_scl_4, 0x0c, GENMASK(7, 6),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 6), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 6), "i2c4")),
	[RTD1295_I2C_SDA_5] = RTK_PIN_MUX(i2c_sda_5, 0x0c, GENMASK(9, 8),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 8), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 8), "i2c5"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 8), "nand")),
	[RTD1295_I2C_SCL_5] = RTK_PIN_MUX(i2c_scl_5, 0x0c, GENMASK(11, 10),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 10), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 10), "i2c5"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 10), "nand")),
	[RTD1295_USB_ID] = RTK_PIN_MUX(usb_id, 0x0c, GENMASK(17, 16),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 16), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 16), "test_loop_dis")),
	[RTD1295_HIF_LOC] = RTK_PIN_MUX(hif_loc, 0x0c, GENMASK(19, 18),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 18), "hif_loc_misc"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 18), "hif_loc_nf")),
	[RTD1295_EJTAG_SCPU_LOC] = RTK_PIN_MUX(ejtag_scpu_loc, 0x0c, GENMASK(21, 20),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 20), "scpu_ejtag_loc_gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 20), "scpu_ejtag_loc_cr")),
	[RTD1295_SENSOR_STB_1] = RTK_PIN_MUX(sensor_stb_1, 0x0c, GENMASK(23, 22),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 22), "gpio")),
	[RTD1295_SENSOR_STB_0] = RTK_PIN_MUX(sensor_stb_0, 0x0c, GENMASK(25, 24),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 24), "gpio")),
	[RTD1295_SENSOR_RST] = RTK_PIN_MUX(sensor_rst, 0x0c, GENMASK(27, 26),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 26), "gpio")),
	[RTD1295_SENSOR_CKO_1] = RTK_PIN_MUX(sensor_cko_1, 0x0c, GENMASK(29, 28),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 28), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 28), "sensor_cko_output"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 28), "usb_clock_output")),
	[RTD1295_SENSOR_CKO_0] = RTK_PIN_MUX(sensor_cko_0, 0x0c, GENMASK(31, 30),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 30), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 30), "sensor_cko_output")),

	[RTD1295_SF_EN] = RTK_PIN_MUX(sf_en, 0x14, GENMASK(0, 0),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 0), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 0), "spi")),
	[RTD1295_TP0_LOC] = RTK_PIN_MUX(tp0_loc, 0x14, GENMASK(7, 7),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 7), "tp0_loc_tp0"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 7), "tp0_loc_tp1"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 7), "tp0_loc_rgmii0_tx")),
	[RTD1295_TP1_LOC] = RTK_PIN_MUX(tp1_loc, 0x14, GENMASK(8, 8),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 8), "tp1_loc_tp1"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 8), "tp1_loc_tp0"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 8), "tp1_loc_rgmii0_rx")),

	[RTD1295_RGMII0_TXC] = RTK_PIN_MUX(rgmii0_txc, 0x6c, GENMASK(1, 0),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 0), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 0), "rgmii")),
	[RTD1295_RGMII0_TX_CTL] = RTK_PIN_MUX(rgmii0_tx_ctl, 0x6c, GENMASK(3, 2),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 2), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 2), "rgmii")),
	[RTD1295_RGMII0_TXD_0] = RTK_PIN_MUX(rgmii0_txd_0, 0x6c, GENMASK(5, 4),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 4), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 4), "rgmii"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 4), "tp0_loc_rgmii0_tx")),
	[RTD1295_RGMII0_TXD_1] = RTK_PIN_MUX(rgmii0_txd_1, 0x6c, GENMASK(7, 6),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 6), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 6), "rgmii"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 6), "tp0_loc_rgmii0_tx")),
	[RTD1295_RGMII0_TXD_2] = RTK_PIN_MUX(rgmii0_txd_2, 0x6c, GENMASK(9, 8),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 8), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 8), "rgmii"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 8), "tp0_loc_rgmii0_tx")),
	[RTD1295_RGMII0_TXD_3] = RTK_PIN_MUX(rgmii0_txd_3, 0x6c, GENMASK(11, 10),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 10), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 10), "rgmii"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 10), "tp0_loc_rgmii0_tx")),
	[RTD1295_RGMII0_RXC] = RTK_PIN_MUX(rgmii0_rxc, 0x6c, GENMASK(13, 12),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 12), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 12), "rgmii")),
	[RTD1295_RGMII0_RX_CTL] = RTK_PIN_MUX(rgmii0_rx_ctl, 0x6c, GENMASK(15, 14),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 14), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 14), "rgmii")),
	[RTD1295_RGMII0_RXD_0] = RTK_PIN_MUX(rgmii0_rxd_0, 0x6c, GENMASK(17, 16),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 16), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 16), "rgmii"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 16), "tp1_loc_rgmii0_rx")),
	[RTD1295_RGMII0_RXD_1]  = RTK_PIN_MUX(rgmii0_rxd_1, 0x6c, GENMASK(19, 18),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 18), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 18), "rgmii"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 18), "tp1_loc_rgmii0_rx")),
	[RTD1295_RGMII0_RXD_2] = RTK_PIN_MUX(rgmii0_rxd_2, 0x6c, GENMASK(21, 20),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 20), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 20), "rgmii"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 20), "tp1_loc_rgmii0_rx")),
	[RTD1295_RGMII0_RXD_3] = RTK_PIN_MUX(rgmii0_rxd_3, 0x6c, GENMASK(23, 22),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 22), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 22), "rgmii"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 22), "tp1_loc_rgmii0_rx")),
	[RTD1295_RGMII0_MDIO] = RTK_PIN_MUX(rgmii0_mdio, 0x6c, GENMASK(25, 24),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 24), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 24), "rgmii"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 24), "eth_gpy")),
	[RTD1295_RGMII0_MDC] = RTK_PIN_MUX(rgmii0_mdc, 0x6c, GENMASK(27, 26),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 26), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 26), "rgmii"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 26), "eth_gpy")),

	[RTD1295_RGMII1_TXC] = RTK_PIN_MUX(rgmii1_txc, 0x7c, GENMASK(1, 0),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 0), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 0), "rgmii")),
	[RTD1295_RGMII1_TX_CTL] = RTK_PIN_MUX(rgmii1_tx_ctl, 0x7c, GENMASK(3, 2),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 2), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 2), "rgmii")),
	[RTD1295_RGMII1_TXD_0] = RTK_PIN_MUX(rgmii1_txd_0, 0x7c, GENMASK(5, 4),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 4), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 4), "rgmii")),
	[RTD1295_RGMII1_TXD_1] = RTK_PIN_MUX(rgmii1_txd_1, 0x7c, GENMASK(7, 6),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 6), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 6), "rgmii")),
	[RTD1295_RGMII1_TXD_2] = RTK_PIN_MUX(rgmii1_txd_2, 0x7c, GENMASK(9, 8),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 8), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 8), "rgmii")),
	[RTD1295_RGMII1_TXD_3] = RTK_PIN_MUX(rgmii1_txd_3, 0x7c, GENMASK(11, 10),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 10), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 10), "rgmii")),
	[RTD1295_RGMII1_RXC] = RTK_PIN_MUX(rgmii1_rxc, 0x7c, GENMASK(13, 12),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 12), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 12), "rgmii")),
	[RTD1295_RGMII1_RX_CTL] = RTK_PIN_MUX(rgmii1_rx_ctl, 0x7c, GENMASK(15, 14),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 14), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 14), "rgmii")),
	[RTD1295_RGMII1_RXD_0] = RTK_PIN_MUX(rgmii1_rxd_0, 0x7c, GENMASK(17, 16),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 16), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 16), "rgmii")),
	[RTD1295_RGMII1_RXD_1] = RTK_PIN_MUX(rgmii1_rxd_1, 0x7c, GENMASK(19, 18),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 18), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 18), "rgmii")),
	[RTD1295_RGMII1_RXD_2] = RTK_PIN_MUX(rgmii1_rxd_2, 0x7c, GENMASK(21, 20),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 20), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 20), "rgmii")),
	[RTD1295_RGMII1_RXD_3] = RTK_PIN_MUX(rgmii1_rxd_3, 0x7c, GENMASK(23, 22),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 22), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 22), "rgmii")),
};

static const struct rtd_pin_config_desc rtd1295_sb2_configs[] = {
	RTK_PIN_CONFIG(tp0_clk, 0x28, 0, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(tp0_sync, 0x28, 4, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(tp0_valid, 0x28, 8, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(tp0_data, 0x28, 12, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(tp1_clk, 0x2C, 0, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(tp1_sync, 0x2C, 4, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(tp1_valid, 0x2C, 8, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(tp1_data, 0x2C, 12, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(i2c_sda_4, 0x30, 8, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(i2c_scl_4, 0x30, 12, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(i2c_sda_5, 0x30, 16, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(i2c_scl_5, 0x30, 20, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(gpio_0, 0x34, 0, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(gpio_1, 0x34, 4, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(gpio_2, 0x34, 8, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(gpio_3, 0x34, 12, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(gpio_4, 0x34, 16, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(gpio_5, 0x34, 20, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(gpio_6, 0x34, 24, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(gpio_7, 0x34, 28, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(gpio_8, 0x38, 0, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(gpio_9, 0x38, 4, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(usb_id, 0x38, 8, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(sensor_stb_0, 0x38, 12, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(sensor_stb_1, 0x38, 16, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(sensor_rst, 0x38, 20, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(sensor_cko_0, 0x38, 24, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(sensor_cko_1, 0x38, 28, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii0_txc, 0x64, 0, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii0_tx_ctl, 0x64, 4, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii0_txd_0, 0x64, 8, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii0_txd_1, 0x64, 12, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii0_txd_2, 0x64, 16, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii0_txd_3, 0x64, 20, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii0_rxc, 0x64, 24, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii0_rx_ctl, 0x64, 28, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii0_rxd_0, 0x68, 0, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii0_rxd_1, 0x68, 4, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii0_rxd_2, 0x68, 8, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii0_rxd_3, 0x68, 12, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii0_mdio, 0x68, 16, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii0_mdc, 0x68, 20, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii1_txc, 0x74, 0, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii1_tx_ctl, 0x74, 4, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii1_txd_0, 0x74, 8, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii1_txd_1, 0x74, 12, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii1_txd_2, 0x74, 16, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii1_txd_3, 0x74, 20, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii1_rxc, 0x74, 24, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii1_rx_ctl, 0x74, 28, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii1_rxd_0, 0x78, 0, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii1_rxd_1, 0x78, 4, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii1_rxd_2, 0x78, 8, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(rgmii1_rxd_3, 0x78, 12, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
};

static struct rtd_pin_reg_list rtd1295_sb2_reg_lists[] = {
	{.reg_offset = 0x8},
	{.reg_offset = 0xc},
	{.reg_offset = 0x10},
	{.reg_offset = 0x14},
	{.reg_offset = 0x28},
	{.reg_offset = 0x14},
	{.reg_offset = 0x2c},
	{.reg_offset = 0x30},
	{.reg_offset = 0x34},
	{.reg_offset = 0x38},
	{.reg_offset = 0x3c},
	{.reg_offset = 0x60},
	{.reg_offset = 0x64},
	{.reg_offset = 0x68},
	{.reg_offset = 0x6c},
	{.reg_offset = 0x70},
	{.reg_offset = 0x74},
	{.reg_offset = 0x78},
	{.reg_offset = 0x7c},
};


static const struct rtd_pinctrl_desc rtd1295_sb2_pinctrl_desc = {
	.pins = rtd1295_sb2_pins,
	.num_pins = ARRAY_SIZE(rtd1295_sb2_pins),
	.groups = rtd1295_sb2_pin_groups,
	.num_groups = ARRAY_SIZE(rtd1295_sb2_pin_groups),
	.functions = rtd1295_sb2_pin_functions,
	.num_functions = ARRAY_SIZE(rtd1295_sb2_pin_functions),
	.muxes = rtd1295_sb2_muxes,
	.num_muxes = ARRAY_SIZE(rtd1295_sb2_muxes),
	.configs = rtd1295_sb2_configs,
	.num_configs = ARRAY_SIZE(rtd1295_sb2_configs),
	.lists = rtd1295_sb2_reg_lists,
	.num_regs = ARRAY_SIZE(rtd1295_sb2_reg_lists),
};

/* Disp */

enum rtd1295_disp_pins {
	RTD1295_SPDIF = 0,
	RTD1295_DMIC_CLK,
	RTD1295_DMIC_DATA,
	RTD1295_AO_LRCK,
	RTD1295_AO_BCK,
	RTD1295_AOCK,
	RTD1295_AO_SD_0,
	RTD1295_AO_SD_1,
	RTD1295_AO_SD_2,
	RTD1295_AO_SD_3,
};

static const struct pinctrl_pin_desc rtd1295_disp_pins[] = {
	PINCTRL_PIN(RTD1295_SPDIF, "spdif"),
	PINCTRL_PIN(RTD1295_DMIC_CLK, "dmic_clk"),
	PINCTRL_PIN(RTD1295_DMIC_DATA, "dmic_data"),
	PINCTRL_PIN(RTD1295_AO_LRCK, "ao_lrck"),
	PINCTRL_PIN(RTD1295_AO_BCK, "ao_bck"),
	PINCTRL_PIN(RTD1295_AOCK, "aock"),
	PINCTRL_PIN(RTD1295_AO_SD_0, "ao_sd_0"),
	PINCTRL_PIN(RTD1295_AO_SD_1, "ao_sd_1"),
	PINCTRL_PIN(RTD1295_AO_SD_2, "ao_sd_2"),
	PINCTRL_PIN(RTD1295_AO_SD_3, "ao_sd_3"),
};

static const unsigned int rtd1295_spdif_pins[] = { RTD1295_SPDIF };
static const unsigned int rtd1295_dmic_clk_pins[] = { RTD1295_DMIC_CLK };
static const unsigned int rtd1295_dmic_data_pins[] = { RTD1295_DMIC_DATA };
static const unsigned int rtd1295_ao_lrck_pins[] = { RTD1295_AO_LRCK };
static const unsigned int rtd1295_ao_bck_pins[] = { RTD1295_AO_BCK };
static const unsigned int rtd1295_aock_pins[] = { RTD1295_AOCK };
static const unsigned int rtd1295_ao_sd_0_pins[] = { RTD1295_AO_SD_0 };
static const unsigned int rtd1295_ao_sd_1_pins[] = { RTD1295_AO_SD_1 };
static const unsigned int rtd1295_ao_sd_2_pins[] = { RTD1295_AO_SD_2 };
static const unsigned int rtd1295_ao_sd_3_pins[] = { RTD1295_AO_SD_3 };

static const struct rtd_pin_group_desc rtd1295_disp_pin_groups[] = {
	RTD1295_GROUP(spdif),
	RTD1295_GROUP(dmic_clk),
	RTD1295_GROUP(dmic_data),
	RTD1295_GROUP(ao_lrck),
	RTD1295_GROUP(ao_bck),
	RTD1295_GROUP(aock),
	RTD1295_GROUP(ao_sd_0),
	RTD1295_GROUP(ao_sd_1),
	RTD1295_GROUP(ao_sd_2),
	RTD1295_GROUP(ao_sd_3),
};

static const char * const rtd1295_disp_gpio_groups[] = {
	"spdif", "dmic_clk", "dmic_data",
	"ao_lrck", "ao_bck", "aock", "ao_sd_0", "ao_sd_1", "ao_sd_2", "ao_sd_3",
};
static const char * const rtd1295_disp_ai_groups[] = {
	"dmic_clk", "dmic_data", "ao_sd_2", "ao_sd_3"
};
static const char * const rtd1295_disp_ao_groups[] = {
	"ao_lrck", "ao_bck", "aock", "ao_sd_0", "ao_sd_1", "ao_sd_2", "ao_sd_3"
};
static const char * const rtd1295_disp_dmic_groups[] = {
	"dmic_clk", "dmic_data"
};
static const char * const rtd1295_disp_spdif_out_groups[] = { "spdif" };

#define RTD1295_FUNC(_name) \
	{ \
		.name = # _name, \
		.groups = rtd1295_disp_ ## _name ## _groups, \
		.num_groups = ARRAY_SIZE(rtd1295_disp_ ## _name ## _groups), \
	}

static const struct rtd_pin_func_desc rtd1295_disp_pin_functions[] = {
	RTD1295_FUNC(gpio),
	RTD1295_FUNC(ai),
	RTD1295_FUNC(ao),
	RTD1295_FUNC(dmic),
	RTD1295_FUNC(spdif_out),
};

#undef RTD1295_FUNC

static const struct rtd_pin_desc rtd1295_disp_muxes[] = {
	[RTD1295_SPDIF] = RTK_PIN_MUX(spdif, 0x08, GENMASK(1, 0),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 0), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 0), "spdif_out")),
	[RTD1295_DMIC_CLK] = RTK_PIN_MUX(dmic_clk, 0x08, GENMASK(3, 2),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 2), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 2), "dmic"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 2), "ai")),
	[RTD1295_DMIC_DATA] = RTK_PIN_MUX(dmic_data, 0x08, GENMASK(5, 4),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 4), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 4), "dmic"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 4), "ai")),
	[RTD1295_AO_LRCK] = RTK_PIN_MUX(ao_lrck, 0x08, GENMASK(7, 6),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 6), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 6), "ao")),
	[RTD1295_AO_BCK] = RTK_PIN_MUX(ao_bck, 0x08, GENMASK(9, 8),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 8), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 8), "ao")),
	[RTD1295_AOCK] = RTK_PIN_MUX(aock, 0x08, GENMASK(11, 10),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 10), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 10), "ao")),
	[RTD1295_AO_SD_0] = RTK_PIN_MUX(ao_sd_0, 0x08, GENMASK(13, 12),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 12), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 12), "ao")),
	[RTD1295_AO_SD_1] = RTK_PIN_MUX(ao_sd_1, 0x08, GENMASK(15, 14),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 14), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 14), "ao")),
	[RTD1295_AO_SD_2] = RTK_PIN_MUX(ao_sd_2, 0x08, GENMASK(17, 16),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 16), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 16), "ao"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 16), "ai")),
	[RTD1295_AO_SD_3] =  RTK_PIN_MUX(ao_sd_3, 0x08, GENMASK(19, 18),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 18), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 18), "ao"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 18), "ai")),
};

static const struct rtd_pin_config_desc rtd1295_disp_configs[] = {
	RTK_PIN_CONFIG(spdif, 0x0, 0, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(dmic_clk, 0x0, 4, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(dmic_data, 0x0, 8, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(ao_lrck, 0x4, 0, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(ao_bck, 0x4, 4, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(aock, 0x4, 8, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(ao_sd_0, 0x4, 12, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(ao_sd_1, 0x4, 16, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(ao_sd_2, 0x4, 20, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
	RTK_PIN_CONFIG(ao_sd_3, 0x4, 24, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_2_4),
};

static struct rtd_pin_reg_list rtd1295_disp_reg_lists[] = {
	{.reg_offset = 0x0},
	{.reg_offset = 0x4},
	{.reg_offset = 0x8},
};


static const struct rtd_pinctrl_desc rtd1295_disp_pinctrl_desc = {
	.pins = rtd1295_disp_pins,
	.num_pins = ARRAY_SIZE(rtd1295_disp_pins),
	.groups = rtd1295_disp_pin_groups,
	.num_groups = ARRAY_SIZE(rtd1295_disp_pin_groups),
	.functions = rtd1295_disp_pin_functions,
	.num_functions = ARRAY_SIZE(rtd1295_disp_pin_functions),
	.muxes = rtd1295_disp_muxes,
	.num_muxes = ARRAY_SIZE(rtd1295_disp_muxes),
	.configs = rtd1295_disp_configs,
	.num_configs = ARRAY_SIZE(rtd1295_disp_configs),
	.lists = rtd1295_disp_reg_lists,
	.num_regs = ARRAY_SIZE(rtd1295_disp_reg_lists),
};

/* CR */

enum rtd1295_cr_pins {
	RTD1295_NF_CLE = 0,
	RTD1295_NF_ALE,
	RTD1295_NF_RD_N,
	RTD1295_NF_WR_N,
	RTD1295_NF_RDY,
	RTD1295_NF_DD_7,
	RTD1295_NF_DD_6,
	RTD1295_NF_DD_5,
	RTD1295_NF_DD_4,
	RTD1295_NF_DD_3,
	RTD1295_NF_DD_2,
	RTD1295_NF_DD_1,
	RTD1295_NF_DD_0,
	RTD1295_NF_DQS,
	RTD1295_NF_CE_N_0,
	RTD1295_NF_CE_N_1,
	RTD1295_EMMC_DD_SB,
	RTD1295_MMC_CMD,
	RTD1295_MMC_CLK,
	RTD1295_MMC_WP,
	RTD1295_MMC_CD,
	RTD1295_MMC_DATA_0,
	RTD1295_MMC_DATA_1,
	RTD1295_MMC_DATA_2,
	RTD1295_MMC_DATA_3,
	RTD1295_SDIO_CMD,
	RTD1295_SDIO_CLK,
	RTD1295_SDIO_DATA_0,
	RTD1295_SDIO_DATA_1,
	RTD1295_SDIO_DATA_2,
	RTD1295_SDIO_DATA_3,
	RTD1295_PCIE_CLKREQ_0,
	RTD1295_PCIE_CLKREQ_1,
	RTD1295_PROB_0,
	RTD1295_PROB_1,
	RTD1295_PROB_2,
	RTD1295_PROB_3,
	RTD1295_SDIO_LOC,
};

static const struct pinctrl_pin_desc rtd1295_cr_pins[] = {
	PINCTRL_PIN(RTD1295_NF_CLE, "nf_cle"),
	PINCTRL_PIN(RTD1295_NF_ALE, "nf_ale"),
	PINCTRL_PIN(RTD1295_NF_RD_N, "nf_rd_n"),
	PINCTRL_PIN(RTD1295_NF_WR_N, "nf_wr_n"),
	PINCTRL_PIN(RTD1295_NF_RDY, "nf_rdy"),
	PINCTRL_PIN(RTD1295_NF_DD_7, "nf_dd_7"),
	PINCTRL_PIN(RTD1295_NF_DD_6, "nf_dd_6"),
	PINCTRL_PIN(RTD1295_NF_DD_5, "nf_dd_5"),
	PINCTRL_PIN(RTD1295_NF_DD_4, "nf_dd_4"),
	PINCTRL_PIN(RTD1295_NF_DD_3, "nf_dd_3"),
	PINCTRL_PIN(RTD1295_NF_DD_2, "nf_dd_2"),
	PINCTRL_PIN(RTD1295_NF_DD_1, "nf_dd_1"),
	PINCTRL_PIN(RTD1295_NF_DD_0, "nf_dd_0"),
	PINCTRL_PIN(RTD1295_NF_DQS, "nf_dqs"),
	PINCTRL_PIN(RTD1295_NF_CE_N_0, "nf_ce_n_0"),
	PINCTRL_PIN(RTD1295_NF_CE_N_1, "nf_ce_n_1"),
	PINCTRL_PIN(RTD1295_EMMC_DD_SB, "emmc_dd_sb"),
	PINCTRL_PIN(RTD1295_MMC_CMD, "mmc_cmd"),
	PINCTRL_PIN(RTD1295_MMC_CLK, "mmc_clk"),
	PINCTRL_PIN(RTD1295_MMC_WP, "mmc_wp"),
	PINCTRL_PIN(RTD1295_MMC_CD, "mmc_cd"),
	PINCTRL_PIN(RTD1295_MMC_DATA_0, "mmc_data_0"),
	PINCTRL_PIN(RTD1295_MMC_DATA_1, "mmc_data_1"),
	PINCTRL_PIN(RTD1295_MMC_DATA_2, "mmc_data_2"),
	PINCTRL_PIN(RTD1295_MMC_DATA_3, "mmc_data_3"),
	PINCTRL_PIN(RTD1295_SDIO_CMD, "sdio_cmd"),
	PINCTRL_PIN(RTD1295_SDIO_CLK, "sdio_clk"),
	PINCTRL_PIN(RTD1295_SDIO_DATA_0, "sdio_data_0"),
	PINCTRL_PIN(RTD1295_SDIO_DATA_1, "sdio_data_1"),
	PINCTRL_PIN(RTD1295_SDIO_DATA_2, "sdio_data_2"),
	PINCTRL_PIN(RTD1295_SDIO_DATA_3, "sdio_data_3"),
	PINCTRL_PIN(RTD1295_PCIE_CLKREQ_0, "pcie_clkreq_0"),
	PINCTRL_PIN(RTD1295_PCIE_CLKREQ_1, "pcie_clkreq_1"),
	PINCTRL_PIN(RTD1295_PROB_0, "prob_0"),
	PINCTRL_PIN(RTD1295_PROB_1, "prob_1"),
	PINCTRL_PIN(RTD1295_PROB_2, "prob_2"),
	PINCTRL_PIN(RTD1295_PROB_3, "prob_3"),
	PINCTRL_PIN(RTD1295_SDIO_LOC, "sdio_loc"),
};

static const unsigned int rtd1295_nf_cle_pins[] = { RTD1295_NF_CLE };
static const unsigned int rtd1295_nf_ale_pins[] = { RTD1295_NF_ALE };
static const unsigned int rtd1295_nf_rd_n_pins[] = { RTD1295_NF_RD_N };
static const unsigned int rtd1295_nf_wr_n_pins[] = { RTD1295_NF_WR_N };
static const unsigned int rtd1295_nf_rdy_pins[] = { RTD1295_NF_RDY };
static const unsigned int rtd1295_nf_dd_7_pins[] = { RTD1295_NF_DD_7 };
static const unsigned int rtd1295_nf_dd_6_pins[] = { RTD1295_NF_DD_6 };
static const unsigned int rtd1295_nf_dd_5_pins[] = { RTD1295_NF_DD_5 };
static const unsigned int rtd1295_nf_dd_4_pins[] = { RTD1295_NF_DD_4 };
static const unsigned int rtd1295_nf_dd_3_pins[] = { RTD1295_NF_DD_3 };
static const unsigned int rtd1295_nf_dd_2_pins[] = { RTD1295_NF_DD_2 };
static const unsigned int rtd1295_nf_dd_1_pins[] = { RTD1295_NF_DD_1 };
static const unsigned int rtd1295_nf_dd_0_pins[] = { RTD1295_NF_DD_0 };
static const unsigned int rtd1295_nf_dqs_pins[] = { RTD1295_NF_DQS };
static const unsigned int rtd1295_nf_ce_n_0_pins[] = { RTD1295_NF_CE_N_0 };
static const unsigned int rtd1295_nf_ce_n_1_pins[] = { RTD1295_NF_CE_N_1 };
static const unsigned int rtd1295_emmc_dd_sb_pins[] = { RTD1295_EMMC_DD_SB };
static const unsigned int rtd1295_mmc_cmd_pins[] = { RTD1295_MMC_CMD };
static const unsigned int rtd1295_mmc_clk_pins[] = { RTD1295_MMC_CLK };
static const unsigned int rtd1295_mmc_wp_pins[] = { RTD1295_MMC_WP };
static const unsigned int rtd1295_mmc_cd_pins[] = { RTD1295_MMC_CD };
static const unsigned int rtd1295_mmc_data_0_pins[] = { RTD1295_MMC_DATA_0 };
static const unsigned int rtd1295_mmc_data_1_pins[] = { RTD1295_MMC_DATA_1 };
static const unsigned int rtd1295_mmc_data_2_pins[] = { RTD1295_MMC_DATA_2 };
static const unsigned int rtd1295_mmc_data_3_pins[] = { RTD1295_MMC_DATA_3 };
static const unsigned int rtd1295_sdio_cmd_pins[] = { RTD1295_SDIO_CMD };
static const unsigned int rtd1295_sdio_clk_pins[] = { RTD1295_SDIO_CLK };
static const unsigned int rtd1295_sdio_data_0_pins[] = { RTD1295_SDIO_DATA_0 };
static const unsigned int rtd1295_sdio_data_1_pins[] = { RTD1295_SDIO_DATA_1 };
static const unsigned int rtd1295_sdio_data_2_pins[] = { RTD1295_SDIO_DATA_2 };
static const unsigned int rtd1295_sdio_data_3_pins[] = { RTD1295_SDIO_DATA_3 };
static const unsigned int rtd1295_pcie_clkreq_0_pins[] = {
				RTD1295_PCIE_CLKREQ_0 };
static const unsigned int rtd1295_pcie_clkreq_1_pins[] = {
				RTD1295_PCIE_CLKREQ_1 };
static const unsigned int rtd1295_prob_0_pins[] = { RTD1295_PROB_0 };
static const unsigned int rtd1295_prob_1_pins[] = { RTD1295_PROB_1 };
static const unsigned int rtd1295_prob_2_pins[] = { RTD1295_PROB_2 };
static const unsigned int rtd1295_prob_3_pins[] = { RTD1295_PROB_3 };
static const unsigned int rtd1295_sdio_loc_pins[] = { RTD1295_SDIO_LOC };


static const struct rtd_pin_group_desc rtd1295_cr_pin_groups[] = {
	RTD1295_GROUP(nf_cle),
	RTD1295_GROUP(nf_ale),
	RTD1295_GROUP(nf_rd_n),
	RTD1295_GROUP(nf_wr_n),
	RTD1295_GROUP(nf_rdy),
	RTD1295_GROUP(nf_dd_7),
	RTD1295_GROUP(nf_dd_6),
	RTD1295_GROUP(nf_dd_5),
	RTD1295_GROUP(nf_dd_4),
	RTD1295_GROUP(nf_dd_3),
	RTD1295_GROUP(nf_dd_2),
	RTD1295_GROUP(nf_dd_1),
	RTD1295_GROUP(nf_dd_0),
	RTD1295_GROUP(nf_dqs),
	RTD1295_GROUP(nf_ce_n_0),
	RTD1295_GROUP(nf_ce_n_1),
	RTD1295_GROUP(emmc_dd_sb),
	RTD1295_GROUP(mmc_cmd),
	RTD1295_GROUP(mmc_clk),
	RTD1295_GROUP(mmc_wp),
	RTD1295_GROUP(mmc_cd),
	RTD1295_GROUP(mmc_data_0),
	RTD1295_GROUP(mmc_data_1),
	RTD1295_GROUP(mmc_data_2),
	RTD1295_GROUP(mmc_data_3),
	RTD1295_GROUP(sdio_cmd),
	RTD1295_GROUP(sdio_clk),
	RTD1295_GROUP(sdio_data_0),
	RTD1295_GROUP(sdio_data_1),
	RTD1295_GROUP(sdio_data_2),
	RTD1295_GROUP(sdio_data_3),
	RTD1295_GROUP(pcie_clkreq_0),
	RTD1295_GROUP(pcie_clkreq_1),
	RTD1295_GROUP(prob_0),
	RTD1295_GROUP(prob_1),
	RTD1295_GROUP(prob_2),
	RTD1295_GROUP(prob_3),
	RTD1295_GROUP(sdio_loc),
};

static const char * const rtd1295_cr_gpio_groups[] = {
	"nf_cle", "nf_ale", "nf_rd_n", "nf_wr_n", "nf_rdy",
	"nf_dd_7", "nf_dd_6", "nf_dd_5", "nf_dd_4",
	"nf_dd_3", "nf_dd_2", "nf_dd_1", "nf_dd_0",
	"nf_dqs", "nf_ce_n_0", "nf_ce_n_1",
	"emmc_dd_sb",
	"mmc_cmd", "mmc_clk", "mmc_wp", "mmc_cd", "mmc_data_0",
	"mmc_data_1", "mmc_data_2", "mmc_data_3",
	"sdio_cmd", "sdio_clk", "sdio_data_0", "sdio_data_1",
	"sdio_data_2", "sdio_data_3",
	"pcie_clkreq_0", "pcie_clkreq_1",
	"prob_0", "prob_1", "prob_2", "prob_3",
};
static const char * const rtd1295_cr_avcpu_ej_groups[] = {
	"nf_rdy", "nf_rd_n", "nf_dd_5", "nf_dd_6", "nf_dd_7"
};
static const char * const rtd1295_cr_emmc_groups[] = {
	"nf_rdy", "nf_rd_n", "nf_cle", "nf_dd_0", "nf_dd_1",
	"nf_dd_2", "nf_dd_3", "nf_dd_4", "nf_dd_5", "nf_dd_6",
	"nf_dd_7", "emmc_dd_sb"
};
static const char * const rtd1295_cr_hif_groups[] = {
	"nf_wr_n", "nf_ale", "nf_cle", "nf_dd_4"
};
static const char * const rtd1295_cr_nand_groups[] = {
	"nf_dqs", "nf_rdy", "nf_rd_n", "nf_wr_n", "nf_ale", "nf_cle",
	"nf_ce_n_0", "nf_ce_n_1", "nf_dd_0", "nf_dd_1", "nf_dd_2",
	"nf_dd_3", "nf_dd_4", "nf_dd_5", "nf_dd_6", "nf_dd_7"
};
static const char * const rtd1295_cr_p2s_groups[] = {
	"prob_0", "prob_1"
};
static const char * const rtd1295_cr_pcie_groups[] = {
	"pcie_clkreq_0", "pcie_clkreq_1"
};
static const char * const rtd1295_cr_pll_test_groups[] = {
	"prob_0", "prob_1", "prob_2", "prob_3"
};
static const char * const rtd1295_cr_scpu_ejtag_loc_cr_groups[] = {
	"mmc_cmd", "mmc_clk", "mmc_wp", "mmc_data_0", "mmc_data_3"
};
static const char * const rtd1295_cr_sd_card_groups[] = {
	"mmc_cmd", "mmc_clk", "mmc_wp", "mmc_cd", "mmc_data_0",
	"mmc_data_1", "mmc_data_2", "mmc_data_3"
};
static const char * const rtd1295_cr_sdio_0_groups[] = {
	"sdio_cmd", "sdio_clk", "sdio_data_0",
	"sdio_data_1", "sdio_data_2", "sdio_data_3"
};
static const char * const rtd1295_cr_sdio_1_groups[] = {
	"mmc_cmd", "mmc_clk", "mmc_data_0",
	"mmc_data_1", "mmc_data_2", "mmc_data_3"
};


#define RTD1295_FUNC(_name) \
	{ \
		.name = # _name, \
		.groups = rtd1295_cr_ ## _name ## _groups, \
		.num_groups = ARRAY_SIZE(rtd1295_cr_ ## _name ## _groups), \
	}

static const struct rtd_pin_func_desc rtd1295_cr_pin_functions[] = {
	RTD1295_FUNC(gpio),
	RTD1295_FUNC(avcpu_ej),
	RTD1295_FUNC(emmc),
	RTD1295_FUNC(hif),
	RTD1295_FUNC(nand),
	RTD1295_FUNC(p2s),
	RTD1295_FUNC(pcie),
	RTD1295_FUNC(pll_test),
	RTD1295_FUNC(scpu_ejtag_loc_cr),
	RTD1295_FUNC(sd_card),
	RTD1295_FUNC(sdio_0),
	RTD1295_FUNC(sdio_1),
};

#undef RTD1295_FUNC

static const struct rtd_pin_desc rtd1295_cr_muxes[] = {
	[RTD1295_NF_DQS] = RTK_PIN_MUX(nf_dqs, 0x00, GENMASK(1, 0),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 0), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 0), "nand")),
	[RTD1295_NF_RDY] = RTK_PIN_MUX(nf_rdy, 0x00, GENMASK(3, 2),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 2), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 2), "nand"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 2), "emmc"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 2), "avcpu_ej")),
	[RTD1295_NF_RD_N] = RTK_PIN_MUX(nf_rd_n, 0x00, GENMASK(5, 4),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 4), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 4), "nand"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 4), "emmc"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 4), "avcpu_ej")),
	[RTD1295_NF_WR_N] = RTK_PIN_MUX(nf_wr_n, 0x00, GENMASK(7, 6),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 6), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 6), "nand"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 6), "hif")),
	[RTD1295_NF_ALE] = RTK_PIN_MUX(nf_ale, 0x00, GENMASK(9, 8),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 8), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 8), "nand"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 8), "hif")),
	[RTD1295_NF_CLE] = RTK_PIN_MUX(nf_cle, 0x00, GENMASK(11, 10),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 10), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 10), "nand"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 10), "emmc"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 10), "hif")),
	[RTD1295_NF_CE_N_0] = RTK_PIN_MUX(nf_ce_n_0, 0x00, GENMASK(13, 12),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 12), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 12), "nand")),
	[RTD1295_NF_CE_N_1] = RTK_PIN_MUX(nf_ce_n_1, 0x00, GENMASK(15, 14),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 14), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 14), "nand")),
	[RTD1295_NF_DD_0] = RTK_PIN_MUX(nf_dd_0, 0x00, GENMASK(17, 16),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 16), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 16), "nand"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 16), "emmc")),
	[RTD1295_NF_DD_1] = RTK_PIN_MUX(nf_dd_1, 0x00, GENMASK(19, 18),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 18), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 18), "nand"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 18), "emmc")),
	[RTD1295_NF_DD_2] = RTK_PIN_MUX(nf_dd_2, 0x00, GENMASK(21, 20),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 20), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 20), "nand"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 20), "emmc")),
	[RTD1295_NF_DD_3] = RTK_PIN_MUX(nf_dd_3, 0x00, GENMASK(23, 22),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 22), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 22), "nand"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 22), "emmc")),
	[RTD1295_NF_DD_4] = RTK_PIN_MUX(nf_dd_4, 0x00, GENMASK(25, 24),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 24), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 24), "nand"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 24), "emmc"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 24), "hif")),
	[RTD1295_NF_DD_5] = RTK_PIN_MUX(nf_dd_5, 0x00, GENMASK(27, 26),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 26), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 26), "nand"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 26), "emmc"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 26), "avcpu_ej")),
	[RTD1295_NF_DD_6] = RTK_PIN_MUX(nf_dd_6, 0x00, GENMASK(29, 28),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 28), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 28), "nand"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 28), "emmc"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 28), "avcpu_ej")),
	[RTD1295_NF_DD_7] = RTK_PIN_MUX(nf_dd_7, 0x00, GENMASK(31, 30),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 30), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 30), "nand"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 30), "emmc"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 30), "avcpu_ej")),

	[RTD1295_SDIO_CMD] = RTK_PIN_MUX(sdio_cmd, 0x04, GENMASK(1, 0),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 0), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 0), "sdio_0")),
	[RTD1295_SDIO_CLK] = RTK_PIN_MUX(sdio_clk, 0x04, GENMASK(3, 2),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 2), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 2), "sdio_0")),
	[RTD1295_SDIO_DATA_0] = RTK_PIN_MUX(sdio_data_0, 0x04, GENMASK(5, 4),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 4), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 4), "sdio_0")),
	[RTD1295_SDIO_DATA_1] = RTK_PIN_MUX(sdio_data_1, 0x04, GENMASK(7, 6),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 6), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 6), "sdio_0")),
	[RTD1295_SDIO_DATA_2] = RTK_PIN_MUX(sdio_data_2, 0x04, GENMASK(9, 8),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 8), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 8), "sdio_0")),
	[RTD1295_SDIO_DATA_3] = RTK_PIN_MUX(sdio_data_3, 0x04, GENMASK(11, 10),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 10), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 10), "sdio_0")),
	[RTD1295_EMMC_DD_SB] = RTK_PIN_MUX(emmc_dd_sb, 0x04, GENMASK(13, 12),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 12), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 12), "emmc")),
	[RTD1295_MMC_CMD] = RTK_PIN_MUX(mmc_cmd, 0x04, GENMASK(17, 16),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 16), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 16), "sd_card"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 16), "sdio_1"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 16), "scpu_ejtag_loc_cr")),
	[RTD1295_MMC_CLK] = RTK_PIN_MUX(mmc_clk, 0x04, GENMASK(19, 18),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 18), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 18), "sd_card"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 18), "sdio_1"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 18), "scpu_ejtag_loc_cr")),
	[RTD1295_MMC_WP] = RTK_PIN_MUX(mmc_wp, 0x04, GENMASK(21, 20),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 20), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 20), "sd_card"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 20), "scpu_ejtag_loc_cr")),
	[RTD1295_MMC_CD] = RTK_PIN_MUX(mmc_cd, 0x04, GENMASK(23, 22),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 22), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 22), "sd_card")),
	[RTD1295_MMC_DATA_0] = RTK_PIN_MUX(mmc_data_0, 0x04, GENMASK(25, 24),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 24), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 24), "sd_card"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 24), "sdio_1"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 24), "scpu_ejtag_loc_cr")),
	[RTD1295_MMC_DATA_1] = RTK_PIN_MUX(mmc_data_1, 0x04, GENMASK(27, 26),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 26), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 26), "sd_card"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 26), "sdio_1")),
	[RTD1295_MMC_DATA_2] = RTK_PIN_MUX(mmc_data_2, 0x04, GENMASK(29, 28),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 28), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 28), "sd_card"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 28), "sdio_1")),
	[RTD1295_MMC_DATA_3] = RTK_PIN_MUX(mmc_data_3, 0x04, GENMASK(31, 30),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 30), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 30), "sd_card"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 30), "sdio_1"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x3, 30), "scpu_ejtag_loc_cr")),

	[RTD1295_SDIO_LOC] = RTK_PIN_MUX(sdio_loc, 0x1c, GENMASK(1, 0),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 0), "sdio_0"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 0), "sdio_1")),
	[RTD1295_PCIE_CLKREQ_0] = RTK_PIN_MUX(pcie_clkreq_0, 0x1c, GENMASK(3, 2),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 2), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 2), "pcie")),
	[RTD1295_PCIE_CLKREQ_1] = RTK_PIN_MUX(pcie_clkreq_1, 0x1c, GENMASK(5, 4),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 4), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 4), "pcie")),
	[RTD1295_PROB_0] = RTK_PIN_MUX(prob_0, 0x1c, GENMASK(7, 6),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 6), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 6), "pll_test"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 6), "p2s")),
	[RTD1295_PROB_1] = RTK_PIN_MUX(prob_1, 0x1c, GENMASK(9, 8),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 8), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 8), "pll_test"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x2, 8), "p2s")),
	[RTD1295_PROB_2] = RTK_PIN_MUX(prob_2, 0x1c, GENMASK(11, 10),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 10), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 10), "pll_test")),
	[RTD1295_PROB_3] = RTK_PIN_MUX(prob_3, 0x1c, GENMASK(13, 12),
		RTK_PIN_FUNC(SHIFT_LEFT(0x0, 12), "gpio"),
		RTK_PIN_FUNC(SHIFT_LEFT(0x1, 12), "pll_test")),
};

static const struct rtd_pin_config_desc rtd1295_cr_configs[] = {
	RTK_PIN_CONFIG(nf_dqs, 0x8, 0, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(nf_ale, 0x8, 4, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(nf_ce_n_0, 0x8, 8, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(nf_ce_n_1, 0x8, 12, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(nf_rdy, 0x8, 16, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(nf_rd_n, 0x8, 20, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(nf_wr_n, 0x8, 24, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(nf_cle, 0x8, 28, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(nf_dd_0, 0xc, 0, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(nf_dd_1, 0xc, 4, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(nf_dd_2, 0xc, 8, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(nf_dd_3, 0xc, 12, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(nf_dd_4, 0xc, 16, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(nf_dd_5, 0xc, 20, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(nf_dd_6, 0xc, 24, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(nf_dd_7, 0xc, 28, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(mmc_cmd, 0x10, 0, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(mmc_clk, 0x10, 4, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(mmc_wp, 0x10, 8, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(mmc_cd, 0x10, 12, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(mmc_data_0, 0x10, 16, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(mmc_data_1, 0x10, 20, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(mmc_data_2, 0x10, 24, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(mmc_data_3, 0x10, 28, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(sdio_cmd, 0x14, 0, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(sdio_clk, 0x14, 4, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(sdio_data_0, 0x14, 16, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(sdio_data_1, 0x14, 20, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(sdio_data_2, 0x14, 24, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(sdio_data_3, 0x14, 28, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(emmc_dd_sb, 0x18, 0, 1, 0, 2, 3, PCONF_UNSUPP, PCONF_UNSUPP),
	RTK_PIN_CONFIG(pcie_clkreq_0, 0x18, 4, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(pcie_clkreq_1, 0x18, 8, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(prob_0, 0x18, 12, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(prob_1, 0x18, 16, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(prob_2, 0x18, 20, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
	RTK_PIN_CONFIG(prob_3, 0x18, 24, 1, 0, 2, 3, PCONF_UNSUPP, PADDRI_4_8),
};

static const struct rtd_pin_sconfig_desc rtd1295_cr_sconfigs[] = {
	RTK_PIN_SCONFIG(nf_ce_n_1, 0x20, PCONF_UNSUPP, 0, 0, 4, 4, 4),
	RTK_PIN_SCONFIG(nf_ce_n_0, 0x20, PCONF_UNSUPP, 0, 8, 4, 12, 4),
	RTK_PIN_SCONFIG(nf_rdy, 0x20, PCONF_UNSUPP, 0, 16, 4, 20, 4),
	RTK_PIN_SCONFIG(nf_dqs, 0x20, PCONF_UNSUPP, 0, 24, 4, 28, 4),
	RTK_PIN_SCONFIG(nf_dd_0, 0x24, PCONF_UNSUPP, 0, 0, 4, 4, 4),
	RTK_PIN_SCONFIG(nf_dd_1, 0x24, PCONF_UNSUPP, 0, 8, 4, 12, 4),
	RTK_PIN_SCONFIG(nf_dd_2, 0x24, PCONF_UNSUPP, 0, 16, 4, 20, 4),
	RTK_PIN_SCONFIG(nf_dd_3, 0x24, PCONF_UNSUPP, 0, 24, 4, 28, 4),
	RTK_PIN_SCONFIG(nf_dd_4, 0x28, PCONF_UNSUPP, 0, 0, 4, 4, 4),
	RTK_PIN_SCONFIG(nf_dd_5, 0x28, PCONF_UNSUPP, 0, 8, 4, 12, 4),
	RTK_PIN_SCONFIG(nf_dd_6, 0x28, PCONF_UNSUPP, 0, 16, 4, 20, 4),
	RTK_PIN_SCONFIG(nf_dd_7, 0x28, PCONF_UNSUPP, 0, 24, 4, 28, 4),
	RTK_PIN_SCONFIG(nf_ale, 0x2c, PCONF_UNSUPP, 0, 0, 4, 4, 4),
	RTK_PIN_SCONFIG(nf_cle, 0x2c, PCONF_UNSUPP, 0, 8, 4, 12, 4),
	RTK_PIN_SCONFIG(nf_wr_n, 0x2c, PCONF_UNSUPP, 0, 16, 4, 20, 4),
	RTK_PIN_SCONFIG(nf_rd_n, 0x2c, PCONF_UNSUPP, 0, 24, 4, 28, 4),
	RTK_PIN_SCONFIG(emmc_dd_sb, 0x30, PCONF_UNSUPP, 0, 0, 4, 4, 4),
	RTK_PIN_SCONFIG(mmc_cmd, 0x34, PCONF_UNSUPP, 0, 0, 4, 4, 4),
	RTK_PIN_SCONFIG(mmc_clk, 0x34, PCONF_UNSUPP, 0, 8, 4, 12, 4),
	RTK_PIN_SCONFIG(mmc_data_0, 0x38, PCONF_UNSUPP, 0, 0, 4, 4, 4),
	RTK_PIN_SCONFIG(mmc_data_1, 0x38, PCONF_UNSUPP, 0, 8, 4, 12, 4),
	RTK_PIN_SCONFIG(mmc_data_2, 0x38, PCONF_UNSUPP, 0, 16, 4, 20, 4),
	RTK_PIN_SCONFIG(mmc_data_3, 0x38, PCONF_UNSUPP, 0, 24, 4, 28, 4),
	RTK_PIN_SCONFIG(sdio_cmd, 0x3c, PCONF_UNSUPP, 0, 0, 4, 4, 4),
	RTK_PIN_SCONFIG(sdio_clk, 0x3c, PCONF_UNSUPP, 0, 8, 4, 12, 4),
	RTK_PIN_SCONFIG(sdio_data_0, 0x40, PCONF_UNSUPP, 0, 0, 4, 4, 4),
	RTK_PIN_SCONFIG(sdio_data_1, 0x40, PCONF_UNSUPP, 0, 8, 4, 12, 4),
	RTK_PIN_SCONFIG(sdio_data_2, 0x40, PCONF_UNSUPP, 0, 16, 4, 20, 4),
	RTK_PIN_SCONFIG(sdio_data_3, 0x40, PCONF_UNSUPP, 0, 24, 4, 28, 4),
};

static struct rtd_pin_reg_list rtd1295_cr_reg_lists[] = {
	{.reg_offset = 0x0},
	{.reg_offset = 0x4},
	{.reg_offset = 0x8},
	{.reg_offset = 0xc},
	{.reg_offset = 0x10},
	{.reg_offset = 0x14},
	{.reg_offset = 0x18},
	{.reg_offset = 0x1c},
	{.reg_offset = 0x20},
	{.reg_offset = 0x24},
	{.reg_offset = 0x28},
	{.reg_offset = 0x2c},
	{.reg_offset = 0x30},
	{.reg_offset = 0x34},
	{.reg_offset = 0x38},
	{.reg_offset = 0x3c},
	{.reg_offset = 0x40},
};


static const struct rtd_pinctrl_desc rtd1295_cr_pinctrl_desc = {
	.pins = rtd1295_cr_pins,
	.num_pins = ARRAY_SIZE(rtd1295_cr_pins),
	.groups = rtd1295_cr_pin_groups,
	.num_groups = ARRAY_SIZE(rtd1295_cr_pin_groups),
	.functions = rtd1295_cr_pin_functions,
	.num_functions = ARRAY_SIZE(rtd1295_cr_pin_functions),
	.muxes = rtd1295_cr_muxes,
	.num_muxes = ARRAY_SIZE(rtd1295_cr_muxes),
	.configs = rtd1295_cr_configs,
	.num_configs = ARRAY_SIZE(rtd1295_cr_configs),
	.sconfigs = rtd1295_cr_sconfigs,
	.num_sconfigs = ARRAY_SIZE(rtd1295_cr_sconfigs),
	.lists = rtd1295_cr_reg_lists,
	.num_regs = ARRAY_SIZE(rtd1295_cr_reg_lists),
};

#endif
