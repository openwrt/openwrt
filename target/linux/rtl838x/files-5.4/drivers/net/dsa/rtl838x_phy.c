// SPDX-License-Identifier: GPL-2.0
/* Realtek RTL838X Ethernet MDIO interface driver
 *
 * Copyright (C) 2020 B. Koblitz
 */

#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/of_mdio.h>
#include <linux/phy.h>
#include <linux/platform_device.h>

#include <asm/mach-rtl838x/mach-rtl838x.h>
#include "rtl838x.h"

/* External RTL8218B and RTL8214FC IDs are identical */
#define PHY_ID_RTL8214FC	0x001cc981
#define PHY_ID_RTL8218B_E	0x001cc981
#define PHY_ID_RTL8218B_I	0x001cca40
#define PHY_ID_RTL_UNKNOWN	0x001ccab0

struct rtl838x_phy_priv {
	char *name;
};

extern struct rtl838x_soc_info soc_info;

extern int rtl838x_write_phy(u32 port, u32 page, u32 reg, u32 val);
extern int rtl839x_write_phy(u32 port, u32 page, u32 reg, u32 val);
extern int rtl838x_read_phy(u32 port, u32 page, u32 reg, u32 *val);
extern int rtl839x_read_phy(u32 port, u32 page, u32 reg, u32 *val);
extern int rtl838x_write_mmd_phy(u32 port, u32 addr, u32 reg, u32 val);
extern int rtl838x_read_mmd_phy(u32 port, u32 addr, u32 reg, u32 *val);


static int read_phy(u32 port, u32 page, u32 reg, u32 *val)
{
	if (soc_info.family == RTL8390_FAMILY_ID)
		return rtl839x_read_phy(port, page, reg, val);
	else
		return rtl838x_read_phy(port, page, reg, val);
}

static int write_phy(u32 port, u32 page, u32 reg, u32 val)
{
	if (soc_info.family == RTL8390_FAMILY_ID)
		return rtl839x_write_phy(port, page, reg, val);
	else
		return rtl838x_write_phy(port, page, reg, val);
}

static void rtl8380_int_phy_on_off(int mac, bool on)
{
	u32 val;
	read_phy(mac, 0, 0, &val);
	if (on)
		write_phy(mac, 0, 0, val & ~(1 << 11));
	else 
		write_phy(mac, 0, 0, val | (1 << 11));
}

static void rtl8380_rtl8214fc_on_off(int mac, bool on)
{
	u32 val;
	/* fiber ports */
	write_phy(mac, 4095, 30, 3);
	read_phy(mac, 0, 16, &val);
	if (on)
		write_phy(mac, 0, 16, val & ~(1 << 11));
	else 
		write_phy(mac, 0, 16, val | (1 << 11));
	
	/* copper ports */
	write_phy(mac, 4095, 30, 1);
	read_phy(mac, 0, 16, &val);
	if (on)
		write_phy(mac, 0xa40, 16, val & ~(1 << 11));
	else 
		write_phy(mac, 0xa40, 16, val | (1 << 11));
}

static void rtl8380_phy_reset(int mac)
{
	u32  val;
	read_phy(mac, 0, 0, &val);
	write_phy(mac, 0, 0, val | (0x1 << 15));
}

void rtl8380_sds_rst(int mac)
{
	u32 offset = (mac == 24)? 0: 0x100;
	sw_w32_mask(1 << 11, 0, (volatile void *) (0xbb00f800 + offset));
	sw_w32_mask(0x3, 0, RTL838X_SDS4_REG28 + offset);
	sw_w32_mask(0x3, 0x3, RTL838X_SDS4_REG28 + offset);
	sw_w32_mask(0, 0x1 << 6, RTL838X_SDS4_DUMMY0 + offset);
	sw_w32_mask(0x1 << 6, 0, RTL838X_SDS4_DUMMY0 + offset);
	printk("SERDES reset: %d\n", mac);
}

u32 rtl838x_6275B_intPhy_perport[][2] = {
	{0x1f, 0x0b82}, {0x10, 0x0000},
	{0x1f, 0x0a44}, {0x11, 0x0418}, {0x1f, 0x0bc0}, {0x16, 0x0c00},
	{0x1f, 0x0000}, {0x1b, 0x809a}, {0x1c, 0x8933}, {0x1b, 0x80a3},
	{0x1c, 0x9233}, {0x1b, 0x80ac}, {0x1c, 0xa444}, {0x1b, 0x809f},
	{0x1c, 0x6b20}, {0x1b, 0x80a8}, {0x1c, 0x6b22}, {0x1b, 0x80b1},
	{0x1c, 0x6b23}, {0, 0}
};

u32 rtl8218b_6276B_hwEsd_perport[][2] = {
	{0x1f, 0xbc4}, {0x17, 0xa200}, {0, 0}
};

static int rtl8390_configure_unknown(struct phy_device *phydev)
{
	u32 val, phy_id;
//	int i, p, ipd_flag;
	int mac = phydev->mdio.addr;

	read_phy(mac, 0, 2, &val);
	phy_id = val << 16;
	read_phy(mac, 0, 3, &val);
	phy_id |= val;
	pr_debug("Phy on MAC %d: %x\n", mac, phy_id);

	/* Read internal PHY ID */
	write_phy(mac, 31, 27, 0x0002);
	read_phy(mac, 31, 28, &val);

	/* Internal RTL8218B, version 2 */
	phydev_info(phydev, "Detected unknown %x\n", val);
	return 0;
}

static int rtl8380_configure_int_rtl8218b(struct phy_device *phydev)
{
	u32 val, phy_id;
	int i, p, ipd_flag;
	int mac = phydev->mdio.addr;

	read_phy(mac, 0, 2, &val);
	phy_id = val << 16;
	read_phy(mac, 0, 3, &val);
	phy_id |= val;
	pr_debug("Phy on MAC %d: %x\n", mac, phy_id);

	/* Read internal PHY ID */
	write_phy(mac, 31, 27, 0x0002);
	read_phy(mac, 31, 28, &val);
	if (val != 0x6275 ) {
		phydev_err(phydev,
			   "Expected internal RTL8218B, found PHY-ID %x\n", val);
		return -1;
	}
	
	/* Internal RTL8218B, version 2 */
	phydev_info(phydev, "Detected internal RTL8218B\n");
	if( sw_r32(RTL838X_DMY_REG31) == 0x1 )
		ipd_flag = 1;

	read_phy(mac, 0, 0, &val);
	if ( val & (1 << 11) )
		rtl8380_int_phy_on_off(mac, true);
	else
		rtl8380_phy_reset(mac);
	msleep(100);

	/* Ready PHY for patch */
	for (p = 0; p < 8; p++) {
		write_phy(mac + p, 0xfff, 0x1f, 0x0b82);
		write_phy(mac + p, 0xfff, 0x10, 0x0010);
	}
	msleep(500);
	for (p = 0; p < 8; p++) {
		for (i = 0; i < 100 ; i++) {
			read_phy(mac + p, 0x0b80, 0x10, &val);
			if (val & 0x40) break;
		}
		if (i >= 100) {
			phydev_err(phydev,
				   "ERROR: Port %d not ready for patch.\n",
				   mac + p);
			return -1;
		}
	}
	for (p = 0; p < 8; p++) {
		i = 0;
		while (rtl838x_6275B_intPhy_perport[i][0]) {
			write_phy(mac + p, 0xfff,
				rtl838x_6275B_intPhy_perport[i][0],
				rtl838x_6275B_intPhy_perport[i][1]);
			i++;
		}
		while (rtl8218b_6276B_hwEsd_perport[i][0]) {
			write_phy(mac + p, 0xfff,
				rtl8218b_6276B_hwEsd_perport[i][0],
				rtl8218b_6276B_hwEsd_perport[i][1]);
			i++;
		}
	}
	return 0;
}

u32 rtl8380_rtl8218b_perchip[][3] = {
	{0, 0x1f, 0x0000}, {0, 0x1e, 0x0008}, {0, 0x1f, 0x0405},
	{0, 0x14, 0x08ec}, {0, 0x1f, 0x0404}, {0, 0x17, 0x5359},
	{0, 0x1f, 0x0424}, {0, 0x17, 0x5359}, {0, 0x1f, 0x042c},
	{0, 0x11, 0x4000}, {0, 0x12, 0x2020}, {0, 0x17, 0x34ac},
	{0, 0x1f, 0x042d}, {0, 0x12, 0x6078}, {0, 0x1f, 0x042e},
	{0, 0x11, 0x2189}, {0, 0x1f, 0x0460}, {0, 0x10, 0x4800},
	{0, 0x1f, 0x0464}, {0, 0x12, 0x1fb0}, {0, 0x13, 0x3e0f},
	{0, 0x15, 0x202a}, {0, 0x16, 0xf072}, {0, 0x1f, 0x0465},
	{0, 0x10, 0x4208}, {0, 0x11, 0x3a08}, {0, 0x13, 0x8068},
	{0, 0x15, 0x29fb}, {0, 0x12, 0x4007}, {0, 0x14, 0x619f},
	{0, 0x1f, 0x0462}, {0, 0x10, 0xf206}, {0, 0x13, 0x530f},
	{0, 0x15, 0x2a58}, {0, 0x12, 0x97b3}, {0, 0x1f, 0x0464},
	{0, 0x17, 0x80f5}, {0, 0x17, 0x00f5}, {0, 0x1f, 0x042d},
	{0, 0x11, 0xc015}, {0, 0x11, 0xc014}, {0, 0x1f, 0x0467},
	{0, 0x14, 0x143d}, {0, 0x14, 0x3c15}, {0, 0x14, 0x3c17},
	{0, 0x14, 0x0000}, {0, 0x1f, 0x0404}, {0, 0x13, 0x7146},
	{0, 0x13, 0x7106}, {0, 0x1f, 0x0424}, {0, 0x13, 0x7146},
	{0, 0x13, 0x7106}, {0, 0x1f, 0x0261}, {0, 0x10, 0x6000},
	{0, 0x10, 0x0000}, {0, 0x1f, 0x0a42}, {0, 0x1e, 0x0000},
	{0, 0, 0}
};

u32 rtl8218B_6276B_rtl8380_perport[][2] = {
	{0x1f, 0x0b82}, {0x10, 0x0000}, {0x1f, 0x0a44}, {0x11, 0x0418},
	{0x1f, 0x0bc0}, {0x16, 0x0c00}, {0x1f, 0x0000}, {0x1b, 0x809a},
	{0x1c, 0x8933}, {0x1b, 0x80a3}, {0x1c, 0x9233}, {0x1b, 0x80ac},
	{0x1c, 0xa444}, {0x1b, 0x809f}, {0x1c, 0x6b20}, {0x1b, 0x80a8},
	{0x1c, 0x6b22}, {0x1b, 0x80b1}, {0x1c, 0x6b23}, {0, 0}
};

u32 rtl8380_rtl8218b_perport[][2] = {
	{0x1f, 0x0b82},{0x10, 0x0000}, {0x1f, 0x0a44},{0x11, 0x0418},
	{0x1f, 0x0bc0},{0x16, 0x0c00}, {0x1f, 0x0000},{0x1b, 0x809a},
	{0x1c, 0xa444},{0x1b, 0x80a3}, {0x1c, 0xa444},{0x1b, 0x80ac},
	{0x1c, 0xa444},{0x1b, 0x809f}, {0x1c, 0x6b20},{0x1b, 0x80a8},
	{0x1c, 0x6b24},{0x1b, 0x80b1}, {0x1c, 0x6b24},{0x1b, 0x8012},
	{0x1c, 0xffff},{0x1b, 0x81bd}, {0x1c, 0x2801},{0x1b, 0x8100},
	{0x1c, 0xe91e},{0x1b, 0x811f}, {0x1c, 0xe90e},{0x1b, 0x827b},
	{0x1c, 0x0000},{0x1f, 0x0bc4}, {0x17, 0xb200}, {0, 0}
};

static int rtl8380_configure_ext_rtl8218b(struct phy_device *phydev)
{
	u32 val, ipd, phy_id;
	int i, l;
	int mac = phydev->mdio.addr;

	if (soc_info.family == RTL8380_FAMILY_ID && mac != 0 && mac != 16) {
		phydev_err(phydev, "External RTL8218B must have PHY-IDs 0 or 16!\n");
		return -1;
	}
	read_phy(mac, 0, 2, &val);
	phy_id = val << 16;
	read_phy(mac, 0, 3, &val);
	phy_id |= val;
	pr_info("Phy on MAC %d: %x\n", mac, phy_id);

	/* Read internal PHY ID */
	write_phy(mac, 31, 27, 0x0002);
	read_phy(mac, 31, 28, &val);
	if (val != 0x6276) {
		phydev_err(phydev, "Expected external RTL8218B, found PHY-ID %x\n", val);
		return -1;
	}
	phydev_info(phydev, "Detected external RTL8218B\n");

	read_phy(mac, 0, 0, &val);
	if ( val & (1 << 11) )
		rtl8380_int_phy_on_off(mac, true);
	else
		rtl8380_phy_reset(mac);
	msleep(100);

	/* Get Chip revision */
	write_phy(mac, 0xfff, 0x1f, 0x0);
	write_phy(mac,  0xfff, 0x1b, 0x4);
	read_phy(mac, 0xfff, 0x1c, &val);

	i = 0;
	while (rtl8380_rtl8218b_perchip[i][0] 
		&& rtl8380_rtl8218b_perchip[i][1]) {
		write_phy(mac + rtl8380_rtl8218b_perchip[i][0],
					  0xfff, rtl8380_rtl8218b_perchip[i][1],
					  rtl8380_rtl8218b_perchip[i][2]);
		i++;
	}

	/* Enable PHY */
	for (i=0; i < 8; i++) {
		write_phy(mac + i, 0xfff, 0x1f, 0x0000);
		write_phy(mac + i, 0xfff, 0x00, 0x1140);
	}
	mdelay(100);

	/* Request patch */
	for (i = 0; i < 8; i++) {
		write_phy(mac + i,  0xfff, 0x1f, 0x0b82);
		write_phy(mac + i,  0xfff, 0x10, 0x0010);
	}
	mdelay(300);

	/* Verify patch readiness */
	for (i = 0; i < 8; i++) {
		for (l=0; l< 100; l++) {
			read_phy(mac + i, 0xb80, 0x10, &val);
			if (val & 0x40)
				break;
		}
		if (l >= 100) {
			phydev_err(phydev, "Could not patch PHY\n");
			return -1;
		}
	}

	/* Use Broadcast ID method for patching */
	write_phy(mac, 0xfff, 0x1f, 0x0000);
	write_phy(mac, 0xfff, 0x1d, 0x0008);
	write_phy(mac, 0xfff, 0x1f, 0x0266);
	write_phy(mac, 0xfff, 0x16, 0xff00 + mac);
	write_phy(mac, 0xfff, 0x1f, 0x0000);
	write_phy(mac, 0xfff, 0x1d, 0x0000);
	mdelay(1);

	write_phy(mac, 0xfff, 30, 8);
	write_phy(mac, 0x26e, 17, 0xb);
	write_phy(mac, 0x26e, 16, 0x2);
	mdelay(1);
	read_phy(mac, 0x26e, 19, &ipd);
	write_phy(mac, 0, 30, 0);
	ipd = (ipd >> 4) & 0xf;

	i = 0;
	while (rtl8218B_6276B_rtl8380_perport[i][0]) {
		write_phy(mac, 0xfff, rtl8218B_6276B_rtl8380_perport[i][0],
				  rtl8218B_6276B_rtl8380_perport[i][1]);
		i++;
	}

	/*Disable broadcast ID*/
	write_phy(mac, 0xfff, 0x1f, 0x0000);
	write_phy(mac, 0xfff, 0x1d, 0x0008);
	write_phy(mac, 0xfff, 0x1f, 0x0266);
	write_phy(mac, 0xfff, 0x16, 0x00 + mac);
	write_phy(mac, 0xfff, 0x1f, 0x0000);
	write_phy(mac, 0xfff, 0x1d, 0x0000);
	mdelay(1);

	return 0;
}

static int rtl8218b_ext_match_phy_device(struct phy_device *phydev)
{
	int addr = phydev->mdio.addr;
	return phydev->phy_id == PHY_ID_RTL8218B_E && addr < 8;
}

static u32 rtl8380_rtl8214fc_perchip[][3] = {
	/* Values are PHY, Register, Value */
	{0, 0x1e, 0x0008}, {0, 0x1f, 0x0405}, {0, 0x14, 0x08ec},
	{0, 0x1f, 0x0404}, {0, 0x17, 0x5359}, {0, 0x1f, 0x0424},
	{0, 0x17, 0x5359}, {0, 0x1f, 0x042c}, {0, 0x11, 0x4000},
	{0, 0x12, 0x2020}, {0, 0x17, 0x34ac}, {0, 0x1f, 0x042d},
	{0, 0x12, 0x6078}, {0, 0x1f, 0x042e}, {0, 0x11, 0x2189},
	{0, 0x1f, 0x0460}, {0, 0x10, 0x4800}, {0, 0x1f, 0x0464},
	{0, 0x12, 0x1fb0}, {0, 0x13, 0x3e0f}, {0, 0x15, 0x202a},
	{0, 0x16, 0xf072}, {0, 0x1f, 0x0465}, {0, 0x10, 0x4208},
	{0, 0x11, 0x3a08}, {0, 0x13, 0x8068}, {0, 0x15, 0x29fb},
	{0, 0x12, 0x4007}, {0, 0x14, 0x619f}, {0, 0x1f, 0x0462},
	{0, 0x10, 0xf206}, {0, 0x13, 0x530f}, {0, 0x15, 0x2a58},
	{0, 0x12, 0x97b3}, {0, 0x1f, 0x0464}, {0, 0x17, 0x80f5},
	{0, 0x17, 0x00f5}, {0, 0x1f, 0x042d}, {0, 0x11, 0xc015},
	{0, 0x11, 0xc014}, {0, 0x1f, 0x0467}, {0, 0x14, 0x143d},
	{0, 0x14, 0x3c15}, {0, 0x14, 0x3c17}, {0, 0x14, 0x0000},
	{0, 0x1f, 0x0404}, {0, 0x13, 0x7146}, {0, 0x13, 0x7106},
	{0, 0x1f, 0x0424}, {0, 0x13, 0x7146}, {0, 0x13, 0x7106},
	{0, 0x1f, 0x0261}, {0, 0x10, 0x6000}, {0, 0x10, 0x0000},
	{0, 0x1f, 0x0260}, {0, 0x13, 0x5820}, {0, 0x14, 0x032c},
	{0, 0x1f, 0x0280}, {0, 0x10, 0xf0bb}, {0, 0x1f, 0x0266},
	{0, 0x10, 0x0f95}, {0, 0x13, 0x0f95}, {0, 0x14, 0x0f95},
	{0, 0x15, 0x0f95}, {0, 0x1f, 0x0a42}, {0, 0x1e, 0x0000},
	{0, 0x1e, 0x0003}, {0, 0x1f, 0x0008}, {0, 0x17, 0x5359},
	{0, 0x14, 0x974d}, {0, 0x1e, 0x0000}, {1, 0x1e, 0x0003},
	{1, 0x1f, 0x0008}, {1, 0x17, 0x5359}, {1, 0x14, 0x974d},
	{1, 0x1e, 0x0000}, {2, 0x1e, 0x0003}, {2, 0x1f, 0x0008},
	{2, 0x17, 0x5359}, {2, 0x14, 0x974d}, {2, 0x1e, 0x0000},
	{3, 0x1e, 0x0003}, {3, 0x1f, 0x0008}, {3, 0x17, 0x5359},
	{3, 0x14, 0x974d}, {3, 0x1e, 0x0000}, {0, 0x1e, 0x0001},
	{1, 0x1e, 0x0001}, {2, 0x1e, 0x0001}, {3, 0x1e, 0x0001},
	{0, 0x00, 0x1340}, {1, 0x00, 0x1340}, {2, 0x00, 0x1340},
	{3, 0x00, 0x1340}, {0, 0x1e, 0x0003}, {1, 0x1e, 0x0003},
	{2, 0x1e, 0x0003}, {3, 0x1e, 0x0003}, {0, 0x1f, 0x0000},
	{1, 0x1f, 0x0000}, {2, 0x1f, 0x0000}, {3, 0x1f, 0x0000},
	{0, 0x10, 0x1340}, {1, 0x10, 0x1340}, {2, 0x10, 0x1340},
	{3, 0x10, 0x1340}, {0, 0x1e, 0x0000}, {1, 0x1e, 0x0000},
	{2, 0x1e, 0x0000}, {3, 0x1e, 0x0000}, {0, 0x1f, 0x0a42},
	{1, 0x1f, 0x0a42}, {2, 0x1f, 0x0a42}, {3, 0x1f, 0x0a42},
	{0, 0, 0}
};

static u32 rtl8380_rtl8214fc_perport[][2] = {
	{0x1f, 0x0b82}, {0x10, 0x0000}, {0x1f, 0x0a44}, {0x11, 0x0418},
	{0x1f, 0x0bc0}, {0x16, 0x0c00}, {0x1f, 0x0a43}, {0x11, 0x0043},
	{0x1f, 0x0000}, {0x1b, 0x809a}, {0x1c, 0x8933}, {0x1b, 0x80a3},
	{0x1c, 0x9233}, {0x1b, 0x80ac}, {0x1c, 0xa444}, {0x1b, 0x809f},
	{0x1c, 0x6b20}, {0x1b, 0x80a8}, {0x1c, 0x6b22}, {0x1b, 0x80b1},
	{0x1c, 0x6b23}, {0x1f, 0x0000}, {0x1e, 0x0003}, {0x1f, 0x0003},
	{0x15, 0xe47f}, {0x1f, 0x0009}, {0x15, 0x46f4}, {0x1f, 0x0000}, 
	{0x1e, 0x0000}, {0, 0}
};

static int rtl8380_rtl8218b_write_mmd(struct phy_device *phydev,
				      int devnum, u16 regnum, u16 val)
{
	int addr = phydev->mdio.addr;
	
	return rtl838x_write_mmd_phy(addr, devnum, regnum, val);
}

static int rtl8380_rtl8218b_read_mmd(struct phy_device *phydev,
				     int devnum, u16 regnum)
{
	int ret;
	u32 val;
	int addr = phydev->mdio.addr;

	ret = rtl838x_read_mmd_phy(addr, devnum, regnum, &val);
	if (ret)
		return ret;
	return val;
}

static void rtl8380_rtl8214fc_media_set(int mac, bool set_fibre)
{
	int base = mac - (mac % 4);
	static int reg[] = {16, 19, 20, 21};
	int val, media, power;

	printk("In rtl8380_rtl8214fc_media_set, port %d, set_fibre: %d\n", 
	       mac, set_fibre);
	write_phy(base, 0xfff, 29, 8);
	read_phy(base, 0x266, reg[mac % 4], &val);

	media = (val >> 10) & 0x3;
	printk("Current media %x\n", media);
	if (media & 0x2) {
		printk("Powering off COPPER\n");
		write_phy(base, 0xfff, 29, 1);
		/* Ensure power is off */
		read_phy(base, 0xa40, 16, &power);
		if ( !(power & (1 << 11)) )
			write_phy(base, 0xa40, 16, power | (1 << 11));
	} else {
		printk("Powering off FIBRE");
		write_phy(base, 0xfff, 29, 3);
		/* Ensure power is off */
		read_phy(base, 0xa40, 16, &power);
		if ( !(power & (1 << 11)) )
			write_phy(base, 0xa40, 16, power | (1 << 11));
	}

	if (set_fibre) {
		val |= 1 << 10;
		val &= ~(1 << 11);
	} else {
		val |= 1 << 10;
		val |= 1 << 11;
	}
	write_phy(base, 0xfff, 29, 8);
	write_phy(base, 0x266, reg[mac %4], val);
	write_phy(base, 0xfff, 29, 0);
	
	if (set_fibre) {
		printk("Powering on FIBRE");
		write_phy(base, 0xfff, 29, 3);
		/* Ensure power is off */
		read_phy(base, 0xa40, 16, &power);
		if ( power & (1 << 11) )
			write_phy(base, 0xa40, 16, power & ~(1 << 11));
	} else {
		printk("Powering on COPPER\n");
		write_phy(base, 0xfff, 29, 1);
		/* Ensure power is off */
		read_phy(base, 0xa40, 16, &power);
		if ( power & (1 << 11) )
			write_phy(base, 0xa40, 16, power & ~(1 << 11));
	}
	
	write_phy(base, 0xfff, 29, 0);
}

static bool rtl8380_rtl8214fc_media_is_fibre(int mac)
{
	int base = mac - (mac % 4);
	static int reg[] = {16, 19, 20, 21};
	u32 val;
	
	write_phy(base, 0xfff, 29, 8);
	read_phy(base, 0x266, reg[mac % 4], &val);
	write_phy(base, 0xfff, 29, 0);
	if (val & (1 << 11))
		return false;
	return true;
}
static int rtl8380_rtl8214fc_set_port(struct phy_device *phydev, int port)
{
	bool is_fibre = (port == PORT_FIBRE? true : false);
	int addr = phydev->mdio.addr;
	printk("rtl8380_rtl8214fc_set_port %d to %d\n", addr, port);
	
	rtl8380_rtl8214fc_media_set(addr, is_fibre);
	
	return 0;
}

static int rtl8380_rtl8214fc_get_port(struct phy_device *phydev)
{
	int addr = phydev->mdio.addr;
	printk("rtl8380_rtl8214fc_get_port %d\n", addr);
	if (rtl8380_rtl8214fc_media_is_fibre(addr))
		return PORT_FIBRE;
	return PORT_MII;
}

void rtl8380_rtl8214fc_ldps_set(int mac, struct ethtool_eee *e)
{

}


static void rtl8380_rtl8218b_eee_set_u_boot(int port, bool enable)
{
	u32 val;
	bool an_enabled;
	
	/* Set GPHY page to copper */
	write_phy(port, 0, 30, 0x0001);
	read_phy(port, 0, 0, &val);
	an_enabled = val & (1 << 12);

	if (enable) {
		/* 100/1000M EEE Capability */
		write_phy(port, 0, 13, 0x0007);
		write_phy(port, 0, 14, 0x003C);
		write_phy(port, 0, 13, 0x4007);
		write_phy(port, 0, 14, 0x0006);

		read_phy(port, 0x0A43, 25, &val);
		val |= 1 << 4;
		write_phy(port,0x0A43, 25,val);
	} else {
		/* 100/1000M EEE Capability */
		write_phy(port, 0, 13, 0x0007);
		write_phy(port, 0, 14, 0x003C);
		write_phy(port, 0, 13, 0x0007);
		write_phy(port, 0, 14, 0x0000);

		read_phy(port, 0x0A43, 25, &val);
		val &= ~(1 << 4);
		write_phy(port,0x0A43, 25,val);	
	}
	
	/* Restart AN if enabled */
	if (an_enabled) {
		read_phy(port, 0, 0, &val);
		val |= (1 << 12) | (1 << 9);
		write_phy(port, 0, 0, val);
	}
	
	/* GPHY page back to auto*/
	write_phy(port, 0xa42, 29, 0);
}

static	int rtl8380_rtl8218b_get_eee_u_boot(struct phy_device *phydev,
				            struct ethtool_eee *e)
{
	u32 val;
	int addr = phydev->mdio.addr;
	printk("In rtl8380_rtl8218b_get_eee_u_boot %d\n", addr);

	/* Set GPHY page to copper */
	write_phy(addr, 0xa42, 29, 0x0001);

	read_phy(addr, 0xa43, 25, &val);
	printk("e1: %d e2: %x\n", e->eee_enabled, val);
	if(e->eee_enabled && (!!(val & (1 << 4))))
		e->eee_enabled = !!(val & (1 << 4));
	else
		e->eee_enabled = 0;

	/* GPHY page to auto */
	write_phy(addr, 0xa42, 29, 0x0000);

	return 0;
}

void rtl8380_rtl8218b_eee_set(int port, bool enable)
{
	u32 val;
	bool an_enabled;
	pr_info("In rtl8380_rtl8218b_eee_set %d, enable %d\n", port, enable);
	/* Set GPHY page to copper */
	write_phy(port, 0xa42, 29, 0x0001);

	read_phy(port, 0, 0, &val);
	an_enabled = val & (1 << 12);
	
	/* MAC based EEE */
	read_phy(port, 0xa43, 25, &val);
	val &= ~(1 << 5);
	write_phy(port, 0xa43, 25, val);

	/* 100M / 1000M EEE */
	if (enable)
		rtl838x_write_mmd_phy(port, 7, 60, 0x6);
	else
		rtl838x_write_mmd_phy(port, 7, 60, 0);

	/* 500M EEE ability */
	read_phy(port, 0xa42, 20, &val);
	if (enable)
		val |= 1 << 7;
	else
		val &= ~(1 << 7);
	write_phy(port, 0xa42, 20, val);

	/* Restart AN if enabled */
	if (an_enabled) {
		read_phy(port, 0, 0, &val);
		val |= (1 << 12) | (1 << 9);
		write_phy(port, 0, 0, val);
	}
	
	/* GPHY page back to auto*/
	write_phy(port, 0xa42, 29, 0);
}

int rtl8380_rtl8218b_get_eee(struct phy_device *phydev,
				     struct ethtool_eee *e)
{
	u32 val;
	int addr = phydev->mdio.addr;
	pr_info("In rtl8380_rtl8218b_get_eee %d\n", addr);

	/* Set GPHY page to copper */
	write_phy(addr, 0xa42, 29, 0x0001);

	rtl838x_read_mmd_phy(addr, 7, 60, &val);
	if(e->eee_enabled && (!!(val & (1 << 7))))
		e->eee_enabled = !!(val & (1 << 7));
	else
		e->eee_enabled = 0;

	/* GPHY page to auto */
	write_phy(addr, 0xa42, 29, 0x0000);

	return 0;
}

void rtl8380_rtl8218b_green_set(int mac, bool enable)
{
	u32 val;

	/* Set GPHY page to copper */
	write_phy(mac, 0xa42, 29, 0x0001);

	write_phy(mac, 0, 27, 0x8011);
	read_phy(mac, 0, 28, &val);
	if (enable) {
		val |= 1 << 9;
		write_phy(mac, 0, 27, 0x8011);
		write_phy(mac, 0, 28, val);
	} else {
		val &= ~(1 << 9);
		write_phy(mac, 0, 27, 0x8011);
		write_phy(mac, 0, 28, val);
	}

	/* GPHY page to auto */
	write_phy(mac, 0xa42, 29, 0x0000);
}

int rtl8380_rtl8214fc_get_green(struct phy_device *phydev,
				        struct ethtool_eee *e)
{
	u32 val;
	int addr = phydev->mdio.addr;
	pr_info("In rtl8380_rtl8214fc_get_green %d\n", addr);

	/* Set GPHY page to copper */
	write_phy(addr, 0xa42, 29, 0x0001);

	write_phy(addr, 0, 27, 0x8011);
	read_phy(addr, 0, 28, &val);
	if(e->eee_enabled && (!!(val & (1 << 9))))
		e->eee_enabled = !!(val & (1 << 9));
	else
		e->eee_enabled = 0;

	/* GPHY page to auto */
	write_phy(addr, 0xa42, 29, 0x0000);

	return 0;
}

static	int rtl8380_rtl8214fc_set_eee(struct phy_device *phydev,
				      struct ethtool_eee *e)
{
	u32 pollMask;
	int addr = phydev->mdio.addr;
	pr_info("In rtl8380_rtl8214fc_set_eee, port %d, enabled %d\n", addr,
	       e->eee_enabled);

	if (rtl8380_rtl8214fc_media_is_fibre(addr)) {
		netdev_err(phydev->attached_dev, "Port %d configured for FIBRE", addr);
		return -ENOTSUPP;
	}

	pollMask = sw_r32(RTL838X_SMI_POLL_CTRL);
	sw_w32(0, RTL838X_SMI_POLL_CTRL);
	rtl8380_rtl8218b_eee_set_u_boot(addr, (bool) e->eee_enabled);
	sw_w32(pollMask, RTL838X_SMI_POLL_CTRL);
	return 0;
}

static	int rtl8380_rtl8214fc_get_eee(struct phy_device *phydev,
				      struct ethtool_eee *e)
{
	int addr = phydev->mdio.addr;
	pr_info("In rtl8380_rtl8214fc_get_eee, port %d, enabled %d\n", addr,
	       e->eee_enabled);

	if (rtl8380_rtl8214fc_media_is_fibre(addr)) {
		netdev_err(phydev->attached_dev, "Port %d configured for FIBRE", addr);
		return -ENOTSUPP;
	}

	return rtl8380_rtl8218b_get_eee_u_boot(phydev, e);
}

static	int rtl8380_rtl8218b_set_eee(struct phy_device *phydev,
				     struct ethtool_eee *e)
{
	u32 pollMask;
	int addr = phydev->mdio.addr;
	printk("In rtl8380_rtl8218b_set_eee, port %d, enabled %d\n", addr,
	       e->eee_enabled);
	
	printk("Disabling POLLING, first\n");
	pollMask = sw_r32(RTL838X_SMI_POLL_CTRL);
	sw_w32(0, RTL838X_SMI_POLL_CTRL);
	rtl8380_rtl8218b_eee_set_u_boot(addr, (bool) e->eee_enabled);
	sw_w32(pollMask, RTL838X_SMI_POLL_CTRL);
	
	return 0;
}


static int rtl8380_configure_rtl8214fc(struct phy_device *phydev)
{
	u32 phy_id, val, page = 0;
	int i, l;
	int mac = phydev->mdio.addr;

	read_phy(mac, 0, 2, &val);
	phy_id = val << 16;
	read_phy(mac, 0, 3, &val);
	phy_id |= val;
	pr_debug("Phy on MAC %d: %x\n", mac, phy_id);

	/* Read internal PHY id */
	write_phy(mac, 0, 30, 0x0001);
	write_phy(mac, 0, 31, 0x0a42);
	write_phy(mac, 31, 27, 0x0002);
	read_phy(mac, 31, 28, &val);
	if (val != 0x6276 ) {
		phydev_err(phydev,
			   "Expected external RTL8214FC, found PHY-ID %x\n", 
			   val);
		return -1;
	}
	phydev_info(phydev, "Detected external RTL8214FC\n");

	/* detect phy version */
	write_phy(mac, 0xfff, 27, 0x0004);
	read_phy(mac, 0xfff, 28, &val);
	
	read_phy(mac, 0, 16, &val);
	if ( val & (1 << 11) )
		rtl8380_rtl8214fc_on_off(mac, true);
	else
		rtl8380_phy_reset(mac);

	msleep(100);
	write_phy(mac, 0, 30, 0x0001);

	i = 0;
	while (rtl8380_rtl8214fc_perchip[i][0] 
		&& rtl8380_rtl8214fc_perchip[i][1]) {
		
		if (rtl8380_rtl8214fc_perchip[i][1] == 0x1f)
			page = rtl8380_rtl8214fc_perchip[i][2];
		if (rtl8380_rtl8214fc_perchip[i][1] == 0x13 && page == 0x260) {
			read_phy(mac + rtl8380_rtl8214fc_perchip[i][0],
					 0x260, 13, &val);
			val = (val & 0x1f00) | (rtl8380_rtl8214fc_perchip[i][2]
				& 0xe0ff);
			write_phy(mac + rtl8380_rtl8214fc_perchip[i][0],
					  0xfff, rtl8380_rtl8214fc_perchip[i][1],
					  val);
		} else {
			write_phy(mac + rtl8380_rtl8214fc_perchip[i][0],
					  0xfff, rtl8380_rtl8214fc_perchip[i][1],
					  rtl8380_rtl8214fc_perchip[i][2]);
		}
		i++;
	}

	/* Force copper medium */
	for (i=0; i < 4; i++) {
		write_phy(mac + i, 0xfff, 0x1f, 0x0000);
		write_phy(mac + i, 0xfff, 0x1e, 0x0001);
	}

	/* Enable PHY */
	for (i=0; i < 4; i++) {
		write_phy(mac + i, 0xfff, 0x1f, 0x0000);
		write_phy(mac + i, 0xfff, 0x00, 0x1140);
	}
	mdelay(100);

	/* Disable Autosensing */
	for (i = 0; i < 4; i++) {
		for (l = 0; l< 100; l++) {
			read_phy(mac + i, 0x0a42, 0x10, &val);
			if ((val & 0x7) >= 3)
				break;
		}
		if (l >= 100) {
			phydev_err(phydev, "Could not disable autosensing\n");
			return -1;
		}
	}

	/* Request patch */
	for (i = 0; i < 4; i++) {
		write_phy(mac + i,  0xfff, 0x1f, 0x0b82);
		write_phy(mac + i,  0xfff, 0x10, 0x0010);
	}
	mdelay(300);

	/* Verify patch readiness */
	for (i = 0; i < 4; i++) {
		for (l=0; l< 100; l++) {
			read_phy(mac + i, 0xb80, 0x10, &val);
			if (val & 0x40)
				break;
		}
		if (l >= 100) {
			phydev_err(phydev, "Could not patch PHY\n");
			return -1;
		}
	}

	/* Use Broadcast ID method for patching */
	write_phy(mac, 0xfff, 0x1f, 0x0000);
	write_phy(mac, 0xfff, 0x1d, 0x0008);
	write_phy(mac, 0xfff, 0x1f, 0x0266);
	write_phy(mac, 0xfff, 0x16, 0xff00 + mac);
	write_phy(mac, 0xfff, 0x1f, 0x0000);
	write_phy(mac, 0xfff, 0x1d, 0x0000);
	mdelay(1);

	i = 0;
	while (rtl8380_rtl8214fc_perport[i][0]) {
		write_phy(mac, 0xfff,rtl8380_rtl8214fc_perport[i][0],
				  rtl8380_rtl8214fc_perport[i][1]);
		i++;
	}

	/*Disable broadcast ID*/
	write_phy(mac, 0xfff, 0x1f, 0x0000);
	write_phy(mac, 0xfff, 0x1d, 0x0008);
	write_phy(mac, 0xfff, 0x1f, 0x0266);
	write_phy(mac, 0xfff, 0x16, 0x00 + mac);
	write_phy(mac, 0xfff, 0x1f, 0x0000);
	write_phy(mac, 0xfff, 0x1d, 0x0000);
	mdelay(1);

	/* Auto medium selection */
	for (i=0; i < 4; i++) {
		write_phy(mac + i, 0xfff, 0x1f, 0x0000);
		write_phy(mac + i, 0xfff, 0x1e, 0x0000);
	}

	return 0;
}

static int rtl8214fc_match_phy_device(struct phy_device *phydev)
{
	int addr = phydev->mdio.addr;
	return phydev->phy_id == PHY_ID_RTL8214FC && addr >= 24;
}

static u32 rtl8380_sds_take_reset[][2] = {
	{0xbb000034, 0x0000003f}, {0xbb00003c, 0x00000010}, {0xbb00e78c, 0x00007146},
	{0xbb00e98c, 0x00007146}, {0xbb00eb8c, 0x00007146}, {0xbb00ed8c, 0x00007146},
	{0xbb00ef8c, 0x00007146}, {0xbb00f18c, 0x00007146}, {0, 0}
};

static u32 rtl8380_sds_common[][2] = {
	{0xbb00f878, 0x0000071e}, {0xbb00f978, 0x0000071e}, {0xbb00e784, 0x00000F00},
	{0xbb00e984, 0x00000F00}, {0xbb00eb84, 0x00000F00}, {0xbb00ed84, 0x00000F00},
	{0xbb00ef84, 0x00000F00}, {0xbb00f184, 0x00000F00}, {0xbb00e788, 0x00007060},
	{0xbb00e988, 0x00007060}, {0xbb00eb88, 0x00007060}, {0xbb00ed88, 0x00007060},
	{0xbb00ef88, 0x00007060}, {0xbb00f188, 0x00007060}, {0xbb00ef90, 0x0000074d},
	{0xbb00f190, 0x0000074d}, {0, 0}
};

static u32 rtl8380_sds01_qsgmii_6275b[][2] = {
	{0xbb00f38c, 0x0000f46f}, {0xbb00f388, 0x000085fa}, {0xbb00f488, 0x000085fa},
	{0xbb00f398, 0x000020d8}, {0xbb00f498, 0x000020d8}, {0xbb00f3c4, 0x0000B7C9},
	{0xbb00f4ac, 0x00000482}, {0xbb00f4a8, 0x000080c7}, {0xbb00f3c8, 0x0000ab8e},
	{0xbb00f3ac, 0x00000482}, {0xbb00f3cc, 
		/* External RTL8214FC */0x000024ab}, {0xbb00f4c4, 0x00004208},
	{0xbb00f4c8, 0x0000c208}, {0xbb00f464, 0x00000303}, {0xbb00f564, 0x00000303},
	{0xbb00f3b8, 0x0000FCC2}, {0xbb00f4b8, 0x0000FCC2}, {0xbb00f3a4, 0x00008e64},
	{0xbb00f3a4, 0x00008c64}, {0xbb00f4a4, 0x00008e64}, {0xbb00f4a4, 0x00008c64},
	{0, 0}
  };

static u32 rtl8380_sds23_qsgmii_6275b[][2] = {
	{0xbb00f58c, 0x0000f46d}, {0xbb00f588, 0x000085fa}, {0xbb00f688, 0x000085fa},
	{0xbb00f788, 0x000085fa}, {0xbb00f598, 0x000020d8}, {0xbb00f698, 0x000020d8},
	{0xbb00f5c4, 0x0000B7C9}, {0xbb00f5c8, 0x0000ab8e}, {0xbb00f5ac, 0x00000482},
	{0xbb00f6ac, 0x00000482}, {0xbb00f5cc, 0x000024ab}, {0xbb00f6c4, 0x00004208},
	{0xbb00f6c8, 0x0000c208}, {0xbb00f664, 0x00000303}, {0xbb00f764, 0x00000303},
	{0xbb00f5b8, 0x0000FCC2}, {0xbb00f6b8, 0x0000FCC2}, {0xbb00f5a4, 0x00008e64},
	{0xbb00f5a4, 0x00008c64}, {0xbb00f6a4, 0x00008e64}, {0xbb00f6a4, 0x00008c64},
	{0, 0}
 };

static u32 rtl8380_sds4_fiber_6275b[][2] = {
	{0xbb00f788, 0x000085fa}, {0xbb00f7ac, 0x00001482}, {0xbb00f798, 0x000020d8},
	{0xbb00f7a8, 0x000000c3}, {0xbb00f7c4, 0x0000B7C9}, {0xbb00f7c8, 0x0000ab8e},
	{0xbb00f864, 0x00000303}, {0xbb00f7b8, 0x0000FCC2}, {0xbb00f7a4, 0x00008e64},
	{0xbb00f7a4, 0x00008c64}, {0, 0}
};

static u32 rtl8380_sds5_fiber_6275b[][2] = {
	{0xbb00f888, 0x000085fa}, {0xbb00f88c, 0x00000000}, {0xbb00f890, 0x0000dccc},
	{0xbb00f894, 0x00000000}, {0xbb00f898, 0x00003600}, {0xbb00f89c, 0x00000003},
	{0xbb00f8a0, 0x000079aa}, {0xbb00f8a4, 0x00008c64}, {0xbb00f8a8, 0x000000c3},
	{0xbb00f8ac, 0x00001482}, {0xbb00f960, 0x000014aa}, {0xbb00f964, 0x00000303},
	{0xbb00f8b8, 0x0000f002}, {0xbb00f96c, 0x000004bf}, {0xbb00f8a4, 0x00008e64},
	{0xbb00f8a4, 0x00008c64}, {0, 0}
};

static u32 rtl8380_sds_reset[][2] = {
	{0xbb00e780, 0x00000c00}, {0xbb00e980, 0x00000c00}, {0xbb00eb80, 0x00000c00},
	{0xbb00ed80, 0x00000c00}, {0xbb00ef80, 0x00000c00}, {0xbb00f180, 0x00000c00},
	{0xbb00e780, 0x00000c03}, {0xbb00e980, 0x00000c03}, {0xbb00eb80, 0x00000c03},
	{0xbb00ed80, 0x00000c03}, {0xbb00ef80, 0x00000c03}, {0xbb00f180, 0x00000c03},
	{0, 0}
};

static u32 rtl8380_sds_release_reset[][2] = {
	{0xbb00e78c, 0x00007106}, {0xbb00e98c, 0x00007106}, {0xbb00eb8c, 0x00007106},
	{0xbb00ed8c, 0x00007106}, {0xbb00ef8c, 0x00007106}, {0xbb00f18c, 0x00007106},
	{0, 0}
};

static int rtl8380_configure_serdes(struct phy_device *phydev)
{
	u32 v;
	u32 sds_conf_value;
	int i;

	phydev_info(phydev, "Detected internal RTL8380 SERDES\n");
	/* Back up serdes power down value */
	sds_conf_value = sw_r32(RTL838X_SDS_CFG_REG);
	pr_info("SDS power down value: %x\n", sds_conf_value);

	/* take serdes into reset */
	i = 0;
	while (rtl8380_sds_take_reset[i][0]) {
		sw_w32(rtl8380_sds_take_reset[i][1], 
			(volatile void *)rtl8380_sds_take_reset[i][0]);
		i++;
		udelay(1000);
	}

	/* apply common serdes patch */
	i = 0;
	while (rtl8380_sds_common[i][0]) {
		sw_w32(rtl8380_sds_common[i][1],
			(volatile void *)rtl8380_sds_common[i][0]);
		i++;
		udelay(1000);
	}

	/* internal R/W enable */
	sw_w32(3, RTL838X_INT_RW_CTRL);
	
	/* SerDes ports 4 and 5 are FIBRE ports */
	sw_w32_mask(0x7 | 0x38, 1 | (1 << 3), RTL838X_INT_MODE_CTRL);

	/* SerDes module settings, SerDes 0-3 are QSGMII */
	v = 0x6 << 25 | 0x6 << 20 | 0x6 << 15 | 0x6 << 10;
	/* SerDes 4 and 5 are 1000BX FIBRE */
	v |= 0x4 << 5 | 0x4 ;
	sw_w32(v, RTL838X_SDS_MODE_SEL);

	pr_info("PLL control register: %x\n", sw_r32(RTL838X_PLL_CML_CTRL));
	sw_w32_mask(0xfffffff0, 0xaaaaaaaf & 0xf, RTL838X_PLL_CML_CTRL);
	i = 0;
	while (rtl8380_sds01_qsgmii_6275b[i][0]) {
		sw_w32(rtl8380_sds01_qsgmii_6275b[i][1],
			(volatile void *) rtl8380_sds01_qsgmii_6275b[i][0]);
		i++;
	}

	i = 0;
	while (rtl8380_sds23_qsgmii_6275b[i][0]) {
		sw_w32(rtl8380_sds23_qsgmii_6275b[i][1],
			(volatile void *) rtl8380_sds23_qsgmii_6275b[i][0]);
		i++;
	}

	i = 0;
	while (rtl8380_sds4_fiber_6275b[i][0]) {
		sw_w32(rtl8380_sds4_fiber_6275b[i][1],
			(volatile void *) rtl8380_sds4_fiber_6275b[i][0]);
		i++;
	}

	i = 0;
	while (rtl8380_sds5_fiber_6275b[i][0]) {
		sw_w32(rtl8380_sds5_fiber_6275b[i][1],
			(volatile void *) rtl8380_sds5_fiber_6275b[i][0]);
		i++;
	}

	i = 0;
	while (rtl8380_sds_reset[i][0]) {
		sw_w32(rtl8380_sds_reset[i][1],
			(volatile void *) rtl8380_sds_reset[i][0]);
		i++;
	}

	i = 0;
	while (rtl8380_sds_release_reset[i][0]) {
		sw_w32(rtl8380_sds_release_reset[i][1],
			(volatile void *) rtl8380_sds_release_reset[i][0]);
		i++;
	}

	pr_info("SDS power down value now: %x\n", sw_r32(RTL838X_SDS_CFG_REG));
	sw_w32(sds_conf_value, RTL838X_SDS_CFG_REG);

	/* Fibre port power off, in order to disable LEDs */
	/* sw_w32_mask( 0, 1 << 11, (volatile void *)0xbb00f800); 
	sw_w32_mask( 0, 1 << 11, (volatile void *)0xbb00f900); */
	printk("Configuration of SERDES done\n");
	return 0;
}

static int rtl8214fc_phy_probe(struct phy_device *phydev)
{
	struct device *dev = &phydev->mdio.dev;
	struct rtl838x_phy_priv *priv;
	int addr = phydev->mdio.addr;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->name = "RTL8214FC";

	/* All base addresses of the PHYs start at multiples of 8 */
	if(!(addr % 8)) {
		/* Configuration must be done whil patching still possible */
		return rtl8380_configure_rtl8214fc(phydev);
	}
	return 0;
}

static int rtl8218b_ext_phy_probe(struct phy_device *phydev)
{
	struct device *dev = &phydev->mdio.dev;
	struct rtl838x_phy_priv *priv;
	int addr = phydev->mdio.addr;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->name = "RTL8218B (external)";

	/* All base addresses of the PHYs start at multiples of 8 */
	if(!(addr % 8)) {
		/* Configuration must be done while patching still possible */
		return rtl8380_configure_ext_rtl8218b(phydev);
	}
	return 0;
}

static int rtl8218b_int_phy_probe(struct phy_device *phydev)
{
	struct device *dev = &phydev->mdio.dev;
	struct rtl838x_phy_priv *priv;
	int addr = phydev->mdio.addr;

	if (soc_info.family != RTL8380_FAMILY_ID)
		return -ENODEV;
	if (addr >= 24)
		return -ENODEV;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->name = "RTL8218B (internal)";

	/* All base addresses of the PHYs start at multiples of 8 */
	if(!(addr % 8)) {
		/* Configuration must be done while patching still possible */
		return rtl8380_configure_int_rtl8218b(phydev);
	}
	return 0;
}

static int rtl838x_serdes_probe(struct phy_device *phydev)
{
	struct device *dev = &phydev->mdio.dev;
	struct rtl838x_phy_priv *priv;
	int addr = phydev->mdio.addr;

	if (soc_info.family != RTL8380_FAMILY_ID)
		return -ENODEV;
	if (addr < 24)
		return -ENODEV;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->name = "RTL8380 Serdes";

	/* On the RTL8380M, PHYs 24-27 connect to the internal SerDes */
	if (soc_info.id == 0x8380) {
		if (addr == 24)
			return rtl8380_configure_serdes(phydev);
		return 0;
	}
	return -ENODEV;
}

static int rtl8390_serdes_probe(struct phy_device *phydev)
{
	struct device *dev = &phydev->mdio.dev;
	struct rtl838x_phy_priv *priv;
	int addr = phydev->mdio.addr;

	if (soc_info.family != RTL8390_FAMILY_ID)
		return -ENODEV;

	if (addr < 24)
		return -ENODEV;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->name = "RTL8390 Serdes";
	return rtl8390_configure_unknown(phydev);
}

static struct phy_driver rtl838x_phy_driver[] = {
	{
		PHY_ID_MATCH_MODEL(PHY_ID_RTL8214FC),
		.name		= "REATLTEK RTL8214FC",
		.features	= PHY_GBIT_FIBRE_FEATURES,
		.match_phy_device = rtl8214fc_match_phy_device,
		.probe		= rtl8214fc_phy_probe,
		.suspend	= genphy_suspend,
		.resume		= genphy_resume,
		.set_loopback	= genphy_loopback,
		.read_mmd	= rtl8380_rtl8218b_read_mmd,
		.write_mmd	= rtl8380_rtl8218b_write_mmd,
		.set_port	= rtl8380_rtl8214fc_set_port,
		.get_port	= rtl8380_rtl8214fc_get_port,
		.set_eee	= rtl8380_rtl8214fc_set_eee,
		.get_eee	= rtl8380_rtl8214fc_get_eee,
	},
	{
		PHY_ID_MATCH_MODEL(PHY_ID_RTL8218B_E),
		.name		= "REATLTEK RTL8218B (external)",
		.features	= PHY_GBIT_FEATURES,
		.match_phy_device = rtl8218b_ext_match_phy_device,
		.probe		= rtl8218b_ext_phy_probe,
		.suspend	= genphy_suspend,
		.resume		= genphy_resume,
		.set_loopback	= genphy_loopback,
		.read_mmd	= rtl8380_rtl8218b_read_mmd,
		.write_mmd	= rtl8380_rtl8218b_write_mmd,
		.set_eee	= rtl8380_rtl8218b_set_eee,
		.get_eee	= rtl8380_rtl8218b_get_eee_u_boot,
	},
	{
		PHY_ID_MATCH_MODEL(PHY_ID_RTL8218B_I),
		.name		= "REATLTEK RTL8218B (internal)",
		.features	= PHY_GBIT_FEATURES,
		.probe		= rtl8218b_int_phy_probe,
		.suspend	= genphy_suspend,
		.resume		= genphy_resume,
		.set_loopback	= genphy_loopback,
		.read_mmd	= rtl8380_rtl8218b_read_mmd,
		.write_mmd	= rtl8380_rtl8218b_write_mmd,
		.set_eee	= rtl8380_rtl8218b_set_eee,
		.get_eee	= rtl8380_rtl8218b_get_eee_u_boot,
	},
	{
		PHY_ID_MATCH_MODEL(PHY_ID_RTL8218B_I),
		.name		= "REATLTEK RTL8380 SERDES",
		.features	= PHY_GBIT_FIBRE_FEATURES,
		.probe		= rtl838x_serdes_probe,
		.suspend	= genphy_suspend,
		.resume		= genphy_resume,
		.set_loopback	= genphy_loopback,
		.read_mmd	= rtl8380_rtl8218b_read_mmd,
		.write_mmd	= rtl8380_rtl8218b_write_mmd,
	},
	{
		PHY_ID_MATCH_MODEL(PHY_ID_RTL_UNKNOWN),
		.name		= "REATLTEK RTL8390 Unknown",
		.features	= PHY_GBIT_FIBRE_FEATURES,
		.probe		= rtl8390_serdes_probe,
		.suspend	= genphy_suspend,
		.resume		= genphy_resume,
		.set_loopback	= genphy_loopback,
	}
};

module_phy_driver(rtl838x_phy_driver);

static struct mdio_device_id __maybe_unused rtl838x_tbl[] = {
	{ PHY_ID_MATCH_MODEL(PHY_ID_RTL8214FC) },
	{ }
};

MODULE_DEVICE_TABLE(mdio, rtl838x_tbl);

MODULE_AUTHOR("B. Koblitz");
MODULE_DESCRIPTION("RTL838x PHY driver");
MODULE_LICENSE("GPL");
