// SPDX-License-Identifier: GPL-2.0-only

// TODO: not really used
struct rtl838x_phy_priv {
	char *name;
};

struct __attribute__ ((__packed__)) part {
	uint16_t start;
	uint8_t wordsize;
	uint8_t words;
};

struct __attribute__ ((__packed__)) fw_header {
	uint32_t magic;
	uint32_t phy;
	uint32_t checksum;
	uint32_t version;
	struct part parts[10];
};

// TODO: fixed path?
#define FIRMWARE_838X_8380_1	"rtl838x_phy/rtl838x_8380.fw"
#define FIRMWARE_838X_8214FC_1	"rtl838x_phy/rtl838x_8214fc.fw"
#define FIRMWARE_838X_8218b_1	"rtl838x_phy/rtl838x_8218b.fw"

/* External RTL8218B and RTL8214FC IDs are identical */
#define PHY_ID_RTL8214C		0x001cc942
#define PHY_ID_RTL8214FC	0x001cc981
#define PHY_ID_RTL8218B_E	0x001cc981
#define PHY_ID_RTL8218B_I	0x001cca40
#define PHY_ID_RTL8390_GENERIC	0x001ccab0
#define PHY_ID_RTL8393_I	0x001c8393

#define RTL839X_SDS12_13_XSG0			(0xB800)

#define RTL838X_SDS_MODE_SEL			(0x0028)
#define RTL838X_SDS_CFG_REG			(0x0034)
#define RTL838X_INT_MODE_CTRL			(0x005c)
#define RTL838X_DMY_REG31			(0x3b28)
