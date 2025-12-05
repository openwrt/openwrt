/* SPDX-License-Identifier: GPL-2.0-only */

struct __packed part {
	u16 start;
	u8 wordsize;
	u8 words;
};

struct __packed fw_header {
	u32 magic;
	u32 phy;
	u32 checksum;
	u32 version;
	struct part parts[10];
};

/* TODO: fixed path? */
#define FIRMWARE_838X_8380_1			"rtl838x_phy/rtl838x_8380.fw"
#define FIRMWARE_838X_8214FC_1			"rtl838x_phy/rtl838x_8214fc.fw"
#define FIRMWARE_838X_8218b_1			"rtl838x_phy/rtl838x_8218b.fw"

#define PHY_ID_RTL8214C				0x001cc942
#define PHY_ID_RTL8218B_E			0x001cc980
#define PHY_ID_RTL8214_OR_8218			0x001cc981
#define PHY_ID_RTL8218D				0x001cc983
#define PHY_ID_RTL8218E				0x001cc984
#define PHY_ID_RTL8218B_I			0x001cca40
#define PHY_ID_RTL8390_GENERIC			0x001ccab0
#define PHY_ID_RTL8393_I			0x001c8393
#define PHY_ID_RTL9300_I			0x338002a0

/* These PHYs share the same id (0x001cc981) */
#define PHY_IS_NOT_RTL821X			0
#define PHY_IS_RTL8214FC			1
#define PHY_IS_RTL8214FB			2
#define PHY_IS_RTL8218B_E			3

/* Registers of the internal Serdes of the 8380 */
#define RTL838X_SDS_MODE_SEL			(0x0028)
#define RTL838X_SDS_CFG_REG			(0x0034)
#define RTL838X_INT_MODE_CTRL			(0x005c)
#define RTL838X_DMY_REG31			(0x3b28)

#define RTL8380_SDS4_FIB_REG0			(0xF800)
#define RTL838X_SDS4_REG28			(0xef80)
#define RTL838X_SDS4_DUMMY0			(0xef8c)
#define RTL838X_SDS5_EXT_REG6			(0xf18c)
#define RTL838X_SDS4_FIB_REG0			(RTL838X_SDS4_REG28 + 0x880)
#define RTL838X_SDS5_FIB_REG0			(RTL838X_SDS4_REG28 + 0x980)

/* Registers of the internal SerDes of the RTL8390 */
#define RTL839X_SDS12_13_XSG0			(0xB800)

