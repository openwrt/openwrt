/*
 *  ADM5120 ethernet switch definitions
 *
 *  This header file defines the hardware registers of the ADM5120 SoC
 *  built-in Ethernet switch.
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Gabor Juhos <juhosg@freemail.hu>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 */

#ifndef _ADM5120_SWITCH_H
#define _ADM5120_SWITCH_H

#define BITMASK(len)	((1 << (len))-1)
#define ONEBIT(at)	(1 << (at))

/* Switch register offsets */
#define SWITCH_REG_CODE		0x0000
#define SWITCH_REG_SOFT_RESET	0x0004
#define SWITCH_REG_MEMCTRL	0x001C
#define SWITCH_REG_CPUP_CONF	0x0024
#define SWITCH_REG_PORT_CONF0	0x0028
#define SWITCH_REG_PORT_CONF1	0x002C
#define SWITCH_REG_PORT_CONF2	0x0030
#define SWITCH_REG_VLAN_G1	0x0040
#define SWITCH_REG_VLAN_G2	0x0044
#define SWITCH_REG_SEND_TRIG	0x0048
#define SWITCH_REG_MAC_WT0	0x0058
#define SWITCH_REG_MAC_WT1	0x005C
#define SWITCH_REG_PHY_CNTL0	0x0068
#define SWITCH_REG_PHY_CNTL1	0x006C
#define SWITCH_REG_PHY_CNTL2	0x007C
#define SWITCH_REG_PHY_CNTL3	0x0080
#define SWITCH_REG_PRI_CNTL	0x0084
#define SWITCH_REG_INT_STATUS	0x00B0
#define SWITCH_REG_INT_MASK	0x00B4
#define SWITCH_REG_GPIO_CONF0	0x00B8
#define SWITCH_REG_GPIO_CONF2	0x00BC
#define SWITCH_REG_WDOG0	0x00C0
#define SWITCH_REG_WDOG1	0x00C4
#define SWITCH_REG_PHY_CNTL4	0x00A0

#define SWITCH_REG_SEND_HBADDR	0x00D0
#define SWITCH_REG_SEND_LBADDR	0x00D4
#define SWITCH_REG_RECV_HBADDR	0x00D8
#define SWITCH_REG_RECV_LBADDR	0x00DC

#define SWITCH_REG_TIMER_INT	0x00F0
#define SWITCH_REG_TIMER	0x00F4

#define SWITCH_REG_PORT0_LED	0x0100
#define SWITCH_REG_PORT1_LED	0x0104
#define SWITCH_REG_PORT2_LED	0x0108
#define SWITCH_REG_PORT3_LED	0x010C
#define SWITCH_REG_PORT4_LED	0x0110

/* CODE register bits */
#define CODE_PC_MASK		BITMASK(16)	/* Product Code */
#define CODE_REV_SHIFT		16
#define CODE_REV_MASK		BITMASK(4)	/* Product Revision */
#define CODE_CLKS_SHIFT		20
#define CODE_CLKS_MASK		BITMASK(2)	/* Clock Speed */
#define CODE_CLKS_175		0		/* 175 MHz */
#define CODE_CLKS_200		1		/* 200 MHz */
#define CODE_CLKS_225		2		/* 225 MHz */
#define CODE_CLKS_250		3		/* 250 MHz */
#define CODE_NAB		ONEBIT(24)	/* NAND boot */
#define CODE_PK_MASK		BITMASK(1)	/* Package type */
#define CODE_PK_SHIFT		29
#define CODE_PK_BGA		0		/* BGA package */
#define CODE_PK_PQFP		1		/* PQFP package */

/* MEMCTRL register bits */
#define MEMCTRL_SDRS_MASK	BITMASK(3)	/* SDRAM bank size */
#define MEMCTRL_SDRS_4M		0x01
#define MEMCTRL_SDRS_8M		0x02
#define MEMCTRL_SDRS_16M	0x03
#define MEMCTRL_SDRS_64M	0x04
#define MEMCTRL_SDRS_128M	0x05
#define MEMCTRL_SDR1_ENABLE	ONEBIT(5)	/* enable SDRAM bank 1 */

#define MEMCTRL_SRS0_SHIFT	8		/* shift for SRAM0 size */
#define MEMCTRL_SRS1_SHIFT	16		/* shift for SRAM1 size */
#define MEMCTRL_SRS_MASK	BITMASK(3)	/* SRAM size mask */
#define MEMCTRL_SRS_DISABLED	0x00		/* Disabled */
#define MEMCTRL_SRS_512K	0x01		/* 512KB*/
#define MEMCTRL_SRS_1M		0x02		/* 1MB */
#define MEMCTRL_SRS_2M		0x03		/* 2MB */
#define MEMCTRL_SRS_4M		0x04		/* 4MB */

/* GPIO_CONF0 register bits */
#define GPIO_CONF0_MASK		BITMASK(8)
#define GPIO_CONF0_IM_SHIFT	0
#define GPIO_CONF0_IV_SHIFT	8
#define GPIO_CONF0_OE_SHIFT	16
#define GPIO_CONF0_OV_SHIFT	24
#define GPIO_CONF0_IM_MASK	(0xFF << GPIO_CONF0_IM_SHIFT)
#define GPIO_CONF0_IV_MASK	(0xFF << GPIO_CONF0_IV_SHIFT)
#define GPIO_CONF0_OE_MASK	(0xFF << GPIO_CONF0_OE_SHIFT)
#define GPIO_CONF0_OV_MASK	(0xFF << GPIO_CONF0_OV_SHIFT)

/* TIMER_INT register bits */
#define TIMER_INT_TOS		ONEBIT(1)	/* time-out status */
#define TIMER_INT_TOM		ONEBIT(16)	/* mask time-out interrupt */

/* TIMER register bits */
#define TIMER_PERIOD_MASK	BITMASK(16)	/* mask for timer period */
#define TIMER_PERIOD_DEFAULT	0xFFFF		/* default timer period */
#define TIMER_TE		ONEBIT(16)	/* timer enable bit */

/* PORTx_LED register bits */
#define LED_MODE_MASK		BITMASK(4)
#define LED_MODE_INPUT		0
#define LED_MODE_FLASH		1
#define LED_MODE_OUT_HIGH	2
#define LED_MODE_OUT_LOW	3
#define LED_MODE_LINK		4
#define LED_MODE_SPEED		5
#define LED_MODE_DUPLEX		6
#define LED_MODE_ACT		7
#define LED_MODE_COLL		8
#define LED_MODE_LINK_ACT	9
#define LED_MODE_DUPLEX_COLL	10
#define LED_MODE_10M_ACT	11
#define LED_MODE_100M_ACT	12
#define LED0_MODE_SHIFT		0	/* LED0 mode shift */
#define LED1_MODE_SHIFT		4	/* LED1 mode shift */
#define LED2_MODE_SHIFT		8	/* LED2 mode shift */
#define LED0_IV_SHIFT		12	/* LED0 input value shift */
#define LED1_IV_SHIFT		13	/* LED1 input value shift */
#define LED2_IV_SHIFT		14	/* LED2 input value shift */

#endif /* _ADM5120_SWITCH_H */
