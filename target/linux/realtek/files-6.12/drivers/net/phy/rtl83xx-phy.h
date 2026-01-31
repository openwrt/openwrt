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
/* NOTE: This firmware file contains both patch values for SerDes
 * configuration and for the internal RTL8218B PHY of RTL838x. Because
 * the SerDes setup has been moved to the PCS driver and the firmware
 * file isn't used there, this was only kept for the PHY. As soon as
 * this has been changed, this firmware file can be dropped completely.
 */
#define FIRMWARE_838X_8380_1			"rtl838x_phy/rtl838x_8380.fw"

#define PHY_ID_RTL8214C				0x001cc942
#define PHY_ID_RTL8218B_E			0x001cc980
#define PHY_ID_RTL8214_OR_8218			0x001cc981
#define PHY_ID_RTL8218D				0x001cc983
#define PHY_ID_RTL8218E				0x001cc984
#define PHY_ID_RTL8218B_I			0x001cca40

/* These PHYs share the same id (0x001cc981) */
#define PHY_IS_NOT_RTL821X			0
#define PHY_IS_RTL8214FC			1
#define PHY_IS_RTL8214FB			2
#define PHY_IS_RTL8218B_E			3

