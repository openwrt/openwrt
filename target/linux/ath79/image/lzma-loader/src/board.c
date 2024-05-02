/*
 * LZMA compressed kernel loader for Atheros AR7XXX/AR9XXX based boards
 *
 * Copyright (C) 2011 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <stddef.h>
#include "config.h"
#include "printf.h"
#include "ar71xx_regs.h"

#define READREG(r)	*(volatile unsigned int *)(r)
#define WRITEREG(r,v)	*(volatile unsigned int *)(r) = v

#define KSEG1ADDR(_x)	(((_x) & 0x1fffffff) | 0xa0000000)

#define UART_BASE	0xb8020000

#define UART_TX		0
#define UART_LSR	5

#define UART_LSR_THRE   0x20

#define UART_READ(r)		READREG(UART_BASE + 4 * (r))
#define UART_WRITE(r,v)		WRITEREG(UART_BASE + 4 * (r), (v))

void board_putc(int ch)
{
	while (((UART_READ(UART_LSR)) & UART_LSR_THRE) == 0);
	UART_WRITE(UART_TX, ch);
	while (((UART_READ(UART_LSR)) & UART_LSR_THRE) == 0);
}

#ifdef CONFIG_BOARD_TL_WR1043ND_V1
static void tlwr1043nd_init(void)
{
	unsigned int reg = KSEG1ADDR(AR71XX_RESET_BASE);
	unsigned int t;

	t = READREG(reg + AR913X_RESET_REG_RESET_MODULE);
	t |= AR71XX_RESET_GE0_PHY;
	WRITEREG(reg + AR913X_RESET_REG_RESET_MODULE, t);
	/* flush write */
	t = READREG(reg + AR913X_RESET_REG_RESET_MODULE);
}
#else
static inline void tlwr1043nd_init(void) {}
#endif

#ifdef CONFIG_BOARD_MERAKI_MR18

static int mr18_extract_sgmii_res_cal(void)
{
	unsigned int base;
	unsigned int reversed_sgmii_value;

	unsigned int otp_value, otp_per_val, rbias_per, read_data;
	unsigned int rbias_pos_or_neg;
	unsigned int sgmii_res_cal_value;
	int res_cal_val;

	base = KSEG1ADDR(QCA955X_OTP_BASE);

	WRITEREG(base + QCA955X_OTP_REG_INTF2, 0x7d);
	WRITEREG(base + QCA955X_OTP_REG_LDO_CTRL, 0x00);

	while (READREG(base + QCA955X_OTP_REG_LDO_STATUS) &
		QCA955X_OTP_LDO_STATUS_POWER_ON)
		;

	READREG(base + QCA955X_OTP_REG_MEM_0 + 4);

	while (!(READREG(base + QCA955X_OTP_REG_STATUS0) &
		QCA955X_OTP_STATUS0_EFUSE_VALID))
		;

	read_data = READREG(base + QCA955X_OTP_REG_STATUS1);

	if (!(read_data & 0x1fff))
		return 0;

	if (read_data & 0x00001000)
		otp_value = (read_data & 0xfc0) >> 6;
	else
		otp_value = read_data & 0x3f;

	if (otp_value > 31) {
		otp_per_val = 63 - otp_value;
		rbias_pos_or_neg = 1;
	} else {
		otp_per_val = otp_value;
		rbias_pos_or_neg = 0;
	}

	rbias_per = otp_per_val * 15;

	if (rbias_pos_or_neg == 1)
		res_cal_val = (rbias_per + 34) / 21;
	else if (rbias_per > 34)
		res_cal_val = -((rbias_per - 34) / 21);
	else
		res_cal_val = (34 - rbias_per) / 21;

	sgmii_res_cal_value = (8 + res_cal_val) & 0xf;

	reversed_sgmii_value  = (sgmii_res_cal_value & 8) >> 3;
	reversed_sgmii_value |= (sgmii_res_cal_value & 4) >> 1;
	reversed_sgmii_value |= (sgmii_res_cal_value & 2) << 1;
	reversed_sgmii_value |= (sgmii_res_cal_value & 1) << 3;
	printf("SGMII cal value = 0x%x\n", reversed_sgmii_value);
	return reversed_sgmii_value;
}

#define QCA955X_SGMII_SERDES_RES_CALIBRATION		BIT(23)
#define QCA955X_SGMII_SERDES_RES_CALIBRATION_MASK	0xf
#define QCA955X_SGMII_SERDES_RES_CALIBRATION_SHIFT	23
#define QCA955X_SGMII_SERDES_LOCK_DETECT_STATUS		BIT(15)
#define QCA955X_PLL_ETH_SGMII_SERDES_LOCK_DETECT	BIT(2)
#define QCA955X_PLL_ETH_SGMII_SERDES_PLL_REFCLK		BIT(1)
#define QCA955X_PLL_ETH_SGMII_SERDES_EN_PLL		BIT(0)
#define QCA955X_PLL_CLK_CTRL_REG			0x08
#define QCA955X_PLL_ETH_XMII_CONTROL_REG		0x28
#define QCA955X_PLL_ETH_SGMII_CONTROL_REG		0x48
#define QCA955X_PLL_ETH_SGMII_SERDES_REG		0x4c

static void qca955x_device_reset_clear(unsigned int mask)
{
	unsigned int t, reg;

	reg = KSEG1ADDR(AR71XX_RESET_BASE +
			QCA955X_RESET_REG_RESET_MODULE);

	t = READREG(reg);
	WRITEREG(reg, t & ~mask);
}

static void mr18_setup_qca955x_eth_serdes_cal(unsigned int sgmii_value)
{
	unsigned int ethbase, pllbase, t;

	ethbase = KSEG1ADDR(QCA955X_GMAC_BASE);
	pllbase = KSEG1ADDR(AR71XX_PLL_BASE);

	/* To Check the locking of the SGMII PLL */
	t = READREG(ethbase + QCA955X_GMAC_REG_SGMII_SERDES);
	t &= ~(QCA955X_SGMII_SERDES_RES_CALIBRATION_MASK <<
	       QCA955X_SGMII_SERDES_RES_CALIBRATION_SHIFT);
	t |= (sgmii_value & QCA955X_SGMII_SERDES_RES_CALIBRATION_MASK) <<
	     QCA955X_SGMII_SERDES_RES_CALIBRATION_SHIFT;
	WRITEREG(ethbase + QCA955X_GMAC_REG_SGMII_SERDES, t);

	WRITEREG(pllbase + QCA955X_PLL_ETH_SGMII_SERDES_REG,
		 QCA955X_PLL_ETH_SGMII_SERDES_LOCK_DETECT |
		 QCA955X_PLL_ETH_SGMII_SERDES_PLL_REFCLK |
		 QCA955X_PLL_ETH_SGMII_SERDES_EN_PLL)
		;

	qca955x_device_reset_clear(QCA955X_RESET_SGMII_ANALOG);
	qca955x_device_reset_clear(QCA955X_RESET_SGMII);

	while (!(READREG(ethbase + QCA955X_GMAC_REG_SGMII_SERDES) &
		QCA955X_SGMII_SERDES_LOCK_DETECT_STATUS))
		;
}

static inline void mr18_init(void)
{
	int res;

	printf("Meraki MR18\n");

	res = mr18_extract_sgmii_res_cal();
	if (res >= 0)
		mr18_setup_qca955x_eth_serdes_cal(res);

}
#else
static inline void mr18_init(void) { }
#endif

#ifdef CONFIG_BOARD_HUAWEI_AP5030DN
static inline void ap5030dn_init(void)
{
	const unsigned int ap5030dn_watchdog_gpio = 15;
	unsigned int gpiobase, reg;

	gpiobase = KSEG1ADDR(AR71XX_GPIO_BASE);

	printf("Huawei AP5030DN\n");

	reg = READREG(gpiobase + AR71XX_GPIO_REG_OE);
	WRITEREG(gpiobase + AR71XX_GPIO_REG_OE,
			reg & ~(1 << ap5030dn_watchdog_gpio));

	/* Set GPIO15 MUX to output CLK_OBS5 (= CPU_CLK/4)
	 * to keep the watchdog happy until wdt-gpio takes over
	 */
	reg = READREG(gpiobase + AR934X_GPIO_REG_OUT_FUNC3);
	WRITEREG(gpiobase + AR934X_GPIO_REG_OUT_FUNC3,
			reg | (QCA955X_GPIO_OUTSEL_CLK_OBS5 << 24));
}
#else
static inline void ap5030dn_init(void) { }
#endif

void board_init(void)
{
	tlwr1043nd_init();
	mr18_init();
	ap5030dn_init();
}
