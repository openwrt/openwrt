// SPDX-License-Identifier: GPL-2.0-only

#include <linux/fwnode_mdio.h>
#include <linux/mutex.h>
#include <linux/of_mdio.h>
#include <linux/of_net.h>
#include <linux/of_platform.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/types.h>

#define RTMDIO_MAX_PORT				57
#define RTMDIO_MAX_SMI_BUS			4
#define RTMDIO_PAGE_SELECT			0x1f

#define RTMDIO_PHY_AQR113C_A			0x31c31c12
#define RTMDIO_PHY_AQR113C_B			0x31c31c13
#define RTMDIO_PHY_AQR813			0x31c31cb2
#define RTMDIO_PHY_RTL8221B_VB_CG		0x001cc849
#define RTMDIO_PHY_RTL8221B_VM_CG		0x001cc84a
#define RTMDIO_PHY_RTL8224			0x001ccad0
#define RTMDIO_PHY_RTL8226			0x001cc838
#define RTMDIO_PHY_RTL8218D			0x001cc983
#define RTMDIO_PHY_RTL8218E			0x001cc984

#define RTMDIO_PHY_MAC_1G			3
#define RTMDIO_PHY_MAC_2G_PLUS			1

#define RTMDIO_PHY_POLL_MMD(dev, reg, bit)	((bit << 21) | (dev << 16) | reg)

/* Register base */
#define RTMDIO_SW_BASE				((volatile void *) 0xBB000000)

/* MDIO bus registers */
#define RTMDIO_838X_CMD_FAIL			0
#define RTMDIO_838X_CMD_READ_C22		0
#define RTMDIO_838X_CMD_READ_C45		BIT(1)
#define RTMDIO_838X_CMD_WRITE_C22		BIT(2)
#define RTMDIO_838X_CMD_WRITE_C45		BIT(1) | BIT(2)
#define RTMDIO_838X_CMD_MASK			BIT(1) | BIT(2)
#define RTMDIO_838X_SMI_GLB_CTRL		(0xa100)
#define RTMDIO_838X_SMI_ACCESS_PHY_CTRL_0	(0xa1b8)
#define RTMDIO_838X_SMI_ACCESS_PHY_CTRL_1	(0xa1bc)
#define RTMDIO_838X_SMI_ACCESS_PHY_CTRL_2	(0xa1c0)
#define RTMDIO_838X_SMI_ACCESS_PHY_CTRL_3	(0xa1c4)
#define RTMDIO_838X_SMI_POLL_CTRL		(0xa17c)

#define RTMDIO_839X_CMD_FAIL			BIT(1)
#define RTMDIO_839X_CMD_READ_C22		0
#define RTMDIO_839X_CMD_READ_C45		BIT(2)
#define RTMDIO_839X_CMD_WRITE_C22		BIT(3)
#define RTMDIO_839X_CMD_WRITE_C45		BIT(2) | BIT(3)
#define RTMDIO_839X_CMD_MASK			BIT(1) | BIT(2) | BIT(3)
#define RTMDIO_839X_PHYREG_CTRL			(0x03E0)
#define RTMDIO_839X_PHYREG_PORT_CTRL		(0x03E4)
#define RTMDIO_839X_PHYREG_ACCESS_CTRL		(0x03DC)
#define RTMDIO_839X_PHYREG_DATA_CTRL		(0x03F0)
#define RTMDIO_839X_PHYREG_MMD_CTRL		(0x03F4)
#define RTMDIO_839X_SMI_PORT_POLLING_CTRL	(0x03fc)
#define RTMDIO_839X_SMI_GLB_CTRL		(0x03f8)

#define RTMDIO_930X_CMD_FAIL			BIT(25)
#define RTMDIO_930X_CMD_READ_C22		0
#define RTMDIO_930X_CMD_READ_C45		BIT(1)
#define RTMDIO_930X_CMD_WRITE_C22		BIT(2)
#define RTMDIO_930X_CMD_WRITE_C45		BIT(1) | BIT(2)
#define RTMDIO_930X_CMD_MASK			BIT(1) | BIT(2) | BIT(25)
#define RTMDIO_930X_SMI_GLB_CTRL		(0xCA00)
#define RTMDIO_930X_SMI_ACCESS_PHY_CTRL_0	(0xCB70)
#define RTMDIO_930X_SMI_ACCESS_PHY_CTRL_1	(0xCB74)
#define RTMDIO_930X_SMI_ACCESS_PHY_CTRL_2	(0xCB78)
#define RTMDIO_930X_SMI_ACCESS_PHY_CTRL_3	(0xCB7C)
#define RTMDIO_930X_SMI_PORT0_15_POLLING_SEL	(0xCA08)
#define RTMDIO_930X_SMI_PORT16_27_POLLING_SEL	(0xCA0C)
#define RTMDIO_930X_SMI_MAC_TYPE_CTRL		(0xCA04)
#define RTMDIO_930X_SMI_PRVTE_POLLING_CTRL	(0xCA10)
#define RTMDIO_930X_SMI_10G_POLLING_REG0_CFG	(0xCBB4)
#define RTMDIO_930X_SMI_10G_POLLING_REG9_CFG	(0xCBB8)
#define RTMDIO_930X_SMI_10G_POLLING_REG10_CFG	(0xCBBC)
#define RTMDIO_930X_SMI_PORT0_5_ADDR		(0xCB80)

#define RTMDIO_931X_CMD_FAIL			BIT(1)
#define RTMDIO_931X_CMD_READ_C22		0
#define RTMDIO_931X_CMD_READ_C45		BIT(3)
#define RTMDIO_931X_CMD_WRITE_C22		BIT(4)
#define RTMDIO_931X_CMD_WRITE_C45		BIT(3) | BIT(4)
#define RTMDIO_931X_CMD_MASK			BIT(1) | BIT(2) | BIT(3) | BIT(4)
#define RTMDIO_931X_SMI_PORT_POLLING_CTRL	(0x0CCC)
#define RTMDIO_931X_SMI_INDRT_ACCESS_BC_CTRL	(0x0C14)
#define RTMDIO_931X_SMI_GLB_CTRL0		(0x0CC0)
#define RTMDIO_931X_SMI_GLB_CTRL1		(0x0CBC)
#define RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_0	(0x0C00)
#define RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_1	(0x0C04)
#define RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_2	(0x0C08)
#define RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_3	(0x0C10)
#define RTMDIO_931X_SMI_INDRT_ACCESS_MMD_CTRL	(0x0C18)
#define RTMDIO_931X_MAC_L2_GLOBAL_CTRL2		(0x1358)
#define RTMDIO_931X_SMI_PORT_POLLING_SEL	(0x0C9C)
#define RTMDIO_931X_SMI_PORT_ADDR		(0x0C74)
#define RTMDIO_931X_SMI_10GPHY_POLLING_SEL0	(0x0CF0)
#define RTMDIO_931X_SMI_10GPHY_POLLING_SEL1	(0x0CF4)
#define RTMDIO_931X_SMI_10GPHY_POLLING_SEL2	(0x0CF8)
#define RTMDIO_931X_SMI_10GPHY_POLLING_SEL3	(0x0CFC)
#define RTMDIO_931X_SMI_10GPHY_POLLING_SEL4	(0x0D00)

#define sw_r32(reg)				readl(RTMDIO_SW_BASE + reg)
#define sw_w32(val, reg)			writel(val, RTMDIO_SW_BASE + reg)
#define sw_w32_mask(clear, set, reg)		sw_w32((sw_r32(reg) & ~(clear)) | (set), reg)

/*
 * On all Realtek switch platforms the hardware periodically reads the link status of all
 * PHYs. This is to some degree programmable, so that one can tell the hardware to read
 * specific C22 registers from specific pages, or C45 registers, to determine the current
 * link speed, duplex, flow-control, ...
 *
 * This happens without any need for the driver to do anything at runtime, completely
 * invisible and in a parallel hardware thread, independent of the CPU running Linux.
 * All one needs to do is to set it up once. Having the MAC link settings automatically
 * follow the PHY link status also happens to be the only way to control MAC port status
 * in a meaningful way, or at least it's the only way we fully understand, as this is
 * what every vendor firmware is doing.
 *
 * The hardware PHY polling unit doesn't care about bus locking, it just assumes that all
 * paged PHY operations are also done via the same hardware unit offering this PHY access
 * abstractions.
 *
 * Additionally at least the RTL838x and RTL839x devices are known to have a so called
 * raw mode. Using the special MAX_PAGE-1 with the MDIO controller found in Realtek
 * SoCs allows to access the PHY in raw mode, ie. bypassing the cache and paging engine
 * of the MDIO controller. E.g. for RTL838x this is 0xfff.
 *
 * On the other hand Realtek PHYs usually make use of select register 0x1f to switch
 * pages. There is no problem to issue separate page and access bus calls to the PHYs
 * when they are not attached to an Realtek SoC. The paradigm should be to keep the PHY
 * implementation bus independent.
 *
 * As if this is not enough the PHY packages consist of 4 or 8 ports that all can be
 * programmed individually. Some registers are only available on port 0 and configure
 * the whole package.
 *
 * To bring all this together we need a tricky bus design that intercepts select page
 * calls but lets raw page accesses through. And especially knows how to handle raw
 * accesses to the select register. Additionally we need the possibility to write to
 * all 8 ports of the PHY individually.
 *
 * While the C45 clause stuff is pretty standard the legacy functions basically track
 * the accesses and the state of the bus with the attributes page[], raw[] and portaddr
 * of the bus_priv structure. The page selection works as follows:
 *
 * phy_write(phydev, RTMDIO_PAGE_SELECT, 12)	: store internal page 12 in driver
 * phy_write(phydev, 7, 33)			: write page=12, reg=7, val=33
 *
 * or simply
 *
 * phy_write_paged(phydev, 12, 7, 33)		: write page=12, reg=7, val=33
 *
 * Any Realtek PHY that will be connected to this bus must simply provide the standard
 * page functions:
 *
 * define RTL821X_PAGE_SELECT 0x1f
 *
 * static int rtl821x_read_page(struct phy_device *phydev)
 * {
 *   return __phy_read(phydev, RTL821X_PAGE_SELECT);
 * }
 *
 * static int rtl821x_write_page(struct phy_device *phydev, int page)
 * {
 *   return __phy_write(phydev, RTL821X_PAGE_SELECT, page);
 * }
 *
 * In case there are non Realtek PHYs attached to the bus the logic might need to be
 * reimplemented. For now it should be sufficient.
 */


struct rtmdio_ctrl {
	const struct rtmdio_config *cfg;
	int page[RTMDIO_MAX_PORT];
	bool raw[RTMDIO_MAX_PORT];
	int smi_bus[RTMDIO_MAX_PORT];
	int smi_addr[RTMDIO_MAX_PORT];
	struct device_node *dn[RTMDIO_MAX_PORT];
	bool smi_bus_isc45[RTMDIO_MAX_SMI_BUS];
};

struct rtmdio_config {
	int cpu_port;
	int raw_page;
	int (*read_mmd_phy)(u32 port, u32 addr, u32 reg, u32 *val);
	int (*read_phy)(u32 port, u32 page, u32 reg, u32 *val);
	int (*reset)(struct mii_bus *bus);
	int (*write_mmd_phy)(u32 port, u32 addr, u32 reg, u32 val);
	int (*write_phy)(u32 port, u32 page, u32 reg, u32 val);
};

struct rtmdio_phy_info {
	unsigned int phy_id;
	bool phy_unknown;
	int mac_type;
	bool has_giga_lite;
	bool has_res_reg;
	bool force_res;
	unsigned int poll_duplex;
	unsigned int poll_adv_1000;
	unsigned int poll_lpa_1000;
};

static int rtmdio_run_cmd(int cmd, int mask, int regnum, int fail)
{
	int ret, val;

	sw_w32_mask(mask, cmd | 1, regnum);
	ret = readx_poll_timeout(sw_r32, regnum, val, !(val & 1), 20, 500000);
	if (ret)
		WARN_ONCE(1, "mdio bus access timed out\n");
	else if (val & fail) {
		WARN_ONCE(1, "mdio bus access failed\n");
		ret = -EIO;
	}

	return ret;
}

/* RTL838x specific MDIO functions */

static int rtmdio_838x_run_cmd(int cmd)
{
	return rtmdio_run_cmd(cmd, RTMDIO_838X_CMD_MASK,
			      RTMDIO_838X_SMI_ACCESS_PHY_CTRL_1, RTMDIO_838X_CMD_FAIL);
}

/* Reads a register in a page from the PHY */
static int rtmdio_838x_read_phy(u32 port, u32 page, u32 reg, u32 *val)
{
	u32 park_page = 0x1f;
	int err;

	sw_w32_mask(0xffff0000, port << 16, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_2);
	sw_w32(reg << 20 | page << 3 | park_page << 15, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_1);
	err = rtmdio_838x_run_cmd(RTMDIO_838X_CMD_READ_C22);
	if (!err)
		*val = sw_r32(RTMDIO_838X_SMI_ACCESS_PHY_CTRL_2) & 0xffff;

	return err;
}

/* Write to a register in a page of the PHY */
static int rtmdio_838x_write_phy(u32 port, u32 page, u32 reg, u32 val)
{
	u32 park_page = 0x1f;

	sw_w32(BIT(port), RTMDIO_838X_SMI_ACCESS_PHY_CTRL_0);
	sw_w32_mask(0xffff0000, val << 16, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_2);
	sw_w32(reg << 20 | page << 3 | park_page << 15, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_1);

	return rtmdio_838x_run_cmd(RTMDIO_838X_CMD_WRITE_C22);
}

/* Read an mmd register of a PHY */
static int rtmdio_838x_read_mmd_phy(u32 port, u32 addr, u32 reg, u32 *val)
{
	int err;

	sw_w32(1 << port, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_0);
	sw_w32_mask(0xffff0000, port << 16, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_2);
	sw_w32(addr << 16 | reg, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_3);
	err = rtmdio_838x_run_cmd(RTMDIO_838X_CMD_READ_C45);
	if (!err)
		*val = sw_r32(RTMDIO_838X_SMI_ACCESS_PHY_CTRL_2) & 0xffff;

	return err;
}

/* Write to an mmd register of a PHY */
static int rtmdio_838x_write_mmd_phy(u32 port, u32 addr, u32 reg, u32 val)
{
	sw_w32(1 << port, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_0);
	sw_w32_mask(0xffff0000, val << 16, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_2);
	sw_w32_mask(0x1f << 16, addr << 16, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_3);
	sw_w32_mask(0xffff, reg, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_3);

	return rtmdio_838x_run_cmd(RTMDIO_838X_CMD_WRITE_C45);
}

/* RTL839x specific MDIO functions */

static int rtmdio_839x_run_cmd(int cmd)
{
	return rtmdio_run_cmd(cmd, RTMDIO_839X_CMD_MASK,
			      RTMDIO_839X_PHYREG_ACCESS_CTRL, RTMDIO_839X_CMD_FAIL);
}

static int rtmdio_839x_read_phy(u32 port, u32 page, u32 reg, u32 *val)
{
	int err;
	u32 v;

	sw_w32_mask(0xffff0000, port << 16, RTMDIO_839X_PHYREG_DATA_CTRL);
	v = reg << 5 | page << 10 | ((page == 0x1fff) ? 0x1f : 0) << 23;
	sw_w32(v, RTMDIO_839X_PHYREG_ACCESS_CTRL);
	sw_w32(0x1ff, RTMDIO_839X_PHYREG_CTRL);
	err = rtmdio_839x_run_cmd(RTMDIO_839X_CMD_READ_C22);
	if (!err)
		*val = sw_r32(RTMDIO_839X_PHYREG_DATA_CTRL) & 0xffff;

	return err;
}

static int rtmdio_839x_write_phy(u32 port, u32 page, u32 reg, u32 val)
{
	u32 v;

	sw_w32(BIT_ULL(port), RTMDIO_839X_PHYREG_PORT_CTRL);
	sw_w32(BIT_ULL(port) >> 32, RTMDIO_839X_PHYREG_PORT_CTRL + 4);
	sw_w32_mask(0xffff0000, val << 16, RTMDIO_839X_PHYREG_DATA_CTRL);
	v = reg << 5 | page << 10 | ((page == 0x1fff) ? 0x1f : 0) << 23;
	sw_w32(v, RTMDIO_839X_PHYREG_ACCESS_CTRL);
	sw_w32(0x1ff, RTMDIO_839X_PHYREG_CTRL);

	return rtmdio_839x_run_cmd(RTMDIO_839X_CMD_WRITE_C22);
}

/* Read an mmd register of the PHY */
static int rtmdio_839x_read_mmd_phy(u32 port, u32 devnum, u32 regnum, u32 *val)
{
	int err;

	sw_w32_mask(0xffff << 16, port << 16, RTMDIO_839X_PHYREG_DATA_CTRL);
	sw_w32(devnum << 16 | (regnum & 0xffff), RTMDIO_839X_PHYREG_MMD_CTRL);
	err = rtmdio_839x_run_cmd(RTMDIO_839X_CMD_READ_C45);
	if (!err)
		*val = sw_r32(RTMDIO_839X_PHYREG_DATA_CTRL) & 0xffff;

	return err;
}

/* Write to an mmd register of the PHY */
static int rtmdio_839x_write_mmd_phy(u32 port, u32 devnum, u32 regnum, u32 val)
{
	sw_w32(BIT_ULL(port), RTMDIO_839X_PHYREG_PORT_CTRL);
	sw_w32(BIT_ULL(port) >> 32, RTMDIO_839X_PHYREG_PORT_CTRL + 4);
	sw_w32_mask(0xffff << 16, val << 16, RTMDIO_839X_PHYREG_DATA_CTRL);
	sw_w32(devnum << 16 | (regnum & 0xffff), RTMDIO_839X_PHYREG_MMD_CTRL);

	return rtmdio_839x_run_cmd(RTMDIO_839X_CMD_WRITE_C45);
}

/* RTL930x specific MDIO functions */

static int rtmdio_930x_run_cmd(int cmd)
{
	return rtmdio_run_cmd(cmd, RTMDIO_930X_CMD_MASK,
			      RTMDIO_930X_SMI_ACCESS_PHY_CTRL_1, RTMDIO_930X_CMD_FAIL);
}

static int rtmdio_930x_write_phy(u32 port, u32 page, u32 reg, u32 val)
{
	u32 v;

	sw_w32(BIT(port), RTMDIO_930X_SMI_ACCESS_PHY_CTRL_0);
	sw_w32_mask(0xffff << 16, val << 16, RTMDIO_930X_SMI_ACCESS_PHY_CTRL_2);
	v = reg << 20 | page << 3 | 0x1f << 15 | BIT(2);
	sw_w32(v, RTMDIO_930X_SMI_ACCESS_PHY_CTRL_1);

	return rtmdio_930x_run_cmd(RTMDIO_930X_CMD_WRITE_C22);
}

static int rtmdio_930x_read_phy(u32 port, u32 page, u32 reg, u32 *val)
{
	int err;
	u32 v;

	sw_w32_mask(0xffff << 16, port << 16, RTMDIO_930X_SMI_ACCESS_PHY_CTRL_2);
	v = reg << 20 | page << 3 | 0x1f << 15;
	sw_w32(v, RTMDIO_930X_SMI_ACCESS_PHY_CTRL_1);
	err = rtmdio_930x_run_cmd(RTMDIO_930X_CMD_READ_C22);
	if (!err)
		*val = (sw_r32(RTMDIO_930X_SMI_ACCESS_PHY_CTRL_2) & 0xffff);

	return err;
}

/* Write to an mmd register of the PHY */
static int rtmdio_930x_write_mmd_phy(u32 port, u32 devnum, u32 regnum, u32 val)
{
	sw_w32(BIT(port), RTMDIO_930X_SMI_ACCESS_PHY_CTRL_0);
	sw_w32_mask(0xffff << 16, val << 16, RTMDIO_930X_SMI_ACCESS_PHY_CTRL_2);
	sw_w32(devnum << 16 | (regnum & 0xffff), RTMDIO_930X_SMI_ACCESS_PHY_CTRL_3);

	return rtmdio_930x_run_cmd(RTMDIO_930X_CMD_WRITE_C45);
}

/* Read an mmd register of the PHY */
static int rtmdio_930x_read_mmd_phy(u32 port, u32 devnum, u32 regnum, u32 *val)
{
	int err ;

	sw_w32_mask(0xffff << 16, port << 16, RTMDIO_930X_SMI_ACCESS_PHY_CTRL_2);
	sw_w32(devnum << 16 | (regnum & 0xffff), RTMDIO_930X_SMI_ACCESS_PHY_CTRL_3);
	err = rtmdio_930x_run_cmd(RTMDIO_930X_CMD_READ_C45);
	if (!err)
		*val = (sw_r32(RTMDIO_930X_SMI_ACCESS_PHY_CTRL_2) & 0xffff);

	return err;
}

/* RTL931x specific MDIO functions */

static int rtmdio_931x_run_cmd(int cmd)
{
	return rtmdio_run_cmd(cmd, RTMDIO_931X_CMD_MASK,
			      RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_0, RTMDIO_931X_CMD_FAIL);
}

static int rtmdio_931x_write_phy(u32 port, u32 page, u32 reg, u32 val)
{
	sw_w32(0, RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_2);
	sw_w32(0, RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_2 + 4);
	sw_w32_mask(0, BIT(port % 32), RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_2 + (port / 32) * 4);
	sw_w32_mask(0xffff, val, RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_3);
	sw_w32(reg << 6 | page << 11, RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_0);
	sw_w32(0x1ff, RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_1);

	return rtmdio_931x_run_cmd(RTMDIO_931X_CMD_WRITE_C22);
}

static int rtmdio_931x_read_phy(u32 port, u32 page, u32 reg, u32 *val)
{
	int err;

	sw_w32(port << 5, RTMDIO_931X_SMI_INDRT_ACCESS_BC_CTRL);
	sw_w32(reg << 6 | page << 11, RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_0);
	err = rtmdio_931x_run_cmd(RTMDIO_931X_CMD_READ_C22);
	if (!err)
		*val = sw_r32(RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_3) >> 16;

	return err;
}

/* Read an mmd register of the PHY */
static int rtmdio_931x_read_mmd_phy(u32 port, u32 devnum, u32 regnum, u32 *val)
{
	int err;

	sw_w32(port << 5, RTMDIO_931X_SMI_INDRT_ACCESS_BC_CTRL);
	sw_w32(devnum << 16 | regnum, RTMDIO_931X_SMI_INDRT_ACCESS_MMD_CTRL);
	err = rtmdio_931x_run_cmd(RTMDIO_931X_CMD_READ_C45);
	if (!err)
		*val = sw_r32(RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_3) >> 16;

	return err;
}

/* Write to an mmd register of the PHY */
static int rtmdio_931x_write_mmd_phy(u32 port, u32 devnum, u32 regnum, u32 val)
{
	u64 mask = BIT_ULL(port);

	sw_w32((u32)mask, RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_2);
	sw_w32((u32)(mask >> 32), RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_2 + 4);
	sw_w32_mask(0xffff, val, RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_3);
	sw_w32(devnum << 16 | regnum, RTMDIO_931X_SMI_INDRT_ACCESS_MMD_CTRL);

	return rtmdio_931x_run_cmd(RTMDIO_931X_CMD_WRITE_C45);
}

/* These are the core functions of our new Realtek SoC MDIO bus. */

static int rtmdio_read_c45(struct mii_bus *bus, int addr, int devnum, int regnum)
{
	struct rtmdio_ctrl *ctrl = bus->priv;
	int err, val;

	if (addr >= ctrl->cfg->cpu_port)
		return -ENODEV;

	err = (*ctrl->cfg->read_mmd_phy)(addr, devnum, regnum, &val);
	pr_debug("rd_MMD(adr=%d, dev=%d, reg=%d) = %d, err = %d\n",
		 addr, devnum, regnum, val, err);
	return err ? err : val;
}

static int rtmdio_read(struct mii_bus *bus, int addr, int regnum)
{
	struct rtmdio_ctrl *ctrl = bus->priv;
	int err, val;

	if (addr >= ctrl->cfg->cpu_port)
		return -ENODEV;

	if (regnum == RTMDIO_PAGE_SELECT && ctrl->page[addr] != ctrl->cfg->raw_page)
		return ctrl->page[addr];

	ctrl->raw[addr] = (ctrl->page[addr] == ctrl->cfg->raw_page);

	err = (*ctrl->cfg->read_phy)(addr, ctrl->page[addr], regnum, &val);
	pr_debug("rd_PHY(adr=%d, pag=%d, reg=%d) = %d, err = %d\n",
		 addr, ctrl->page[addr], regnum, val, err);
	return err ? err : val;
}

static int rtmdio_write_c45(struct mii_bus *bus, int addr, int devnum, int regnum, u16 val)
{
	struct rtmdio_ctrl *ctrl = bus->priv;
	int err;

	if (addr >= ctrl->cfg->cpu_port)
		return -ENODEV;

	err = (*ctrl->cfg->write_mmd_phy)(addr, devnum, regnum, val);
	pr_debug("wr_MMD(adr=%d, dev=%d, reg=%d, val=%d) err = %d\n",
		 addr, devnum, regnum, val, err);
	return err;
}

static int rtmdio_write(struct mii_bus *bus, int addr, int regnum, u16 val)
{
	struct rtmdio_ctrl *ctrl = bus->priv;
	int err, page;

	if (addr >= ctrl->cfg->cpu_port)
		return -ENODEV;

	page = ctrl->page[addr];

	if (regnum == RTMDIO_PAGE_SELECT)
		ctrl->page[addr] = val;

	if (!ctrl->raw[addr] && (regnum != RTMDIO_PAGE_SELECT || page == ctrl->cfg->raw_page)) {
		ctrl->raw[addr] = (page == ctrl->cfg->raw_page);

		err = (*ctrl->cfg->write_phy)(addr, page, regnum, val);
		pr_debug("wr_PHY(adr=%d, pag=%d, reg=%d, val=%d) err = %d\n",
			 addr, page, regnum, val, err);
		return err;
	}

	ctrl->raw[addr] = false;
	return 0;
}

static int rtmdio_read_phy_id(struct mii_bus *bus, u8 addr, unsigned int *phy_id)
{
	static const int common_mmds[] = {
		MDIO_MMD_PMAPMD, MDIO_MMD_PCS, MDIO_MMD_AN,
		MDIO_MMD_VEND1, MDIO_MMD_VEND2
	};
	struct rtmdio_ctrl *ctrl = bus->priv;
	int devid1 = 0, devid2 = 0;
	unsigned int id = 0;

	/* Clause 22 */
	if (!ctrl->smi_bus_isc45[ctrl->smi_bus[addr]]) {
		devid1 = rtmdio_read(bus, addr, MDIO_DEVID1);
		devid2 = rtmdio_read(bus, addr, MDIO_DEVID2);
		if (devid1 < 0 || devid2 < 0)
			return -EIO;

		id = (devid1 << 16) | devid2;
		if (!id || (id & 0x1fffffff) == 0x1fffffff)
			return -ENODEV;

		*phy_id = id;
		return 0;
	}


	/* Clause 45
	 * only scan some MMDs which can be considered as common i.e.
	 * implemented by most PHYs.
	 */
	for (int i = 0; i < ARRAY_SIZE(common_mmds); i++) {
		devid1 = rtmdio_read_c45(bus, addr, common_mmds[i], MDIO_DEVID1);
		devid2 = rtmdio_read_c45(bus, addr, common_mmds[i], MDIO_DEVID2);
		if (devid1 < 0 || devid2 < 0)
			continue;

		id = (devid1 << 16) | devid2;
		if (id && id != 0xffffffff) {
			*phy_id = id;
			return 0;
		}
	}

	return -ENODEV;
}

static void rtmdio_get_phy_info(struct mii_bus *bus, int addr, struct rtmdio_phy_info *phyinfo)
{
	struct rtmdio_ctrl *ctrl = bus->priv;

	/*
	 * Depending on the attached PHY the polling mechanism must be fine tuned. Basically
	 * this boils down to which registers must be read and if there are any special
	 * features.
	 */
	memset(phyinfo, 0, sizeof(*phyinfo));
	if (ctrl->smi_bus[addr] < 0) {
		phyinfo->phy_unknown = true;
		return;
	}

	if (rtmdio_read_phy_id(bus, addr, &phyinfo->phy_id) < 0) {
		phyinfo->phy_unknown = true;
		return;
	}

	switch(phyinfo->phy_id) {
	case RTMDIO_PHY_AQR113C_A:
	case RTMDIO_PHY_AQR113C_B:
	case RTMDIO_PHY_AQR813:
		phyinfo->mac_type = RTMDIO_PHY_MAC_2G_PLUS;
		phyinfo->poll_duplex = RTMDIO_PHY_POLL_MMD(1, 0x0000, 8);
		phyinfo->poll_adv_1000 = RTMDIO_PHY_POLL_MMD(7, 0xc400, 15);
		phyinfo->poll_lpa_1000 = RTMDIO_PHY_POLL_MMD(7, 0xe820, 15);
		break;
	case RTMDIO_PHY_RTL8218D:
	case RTMDIO_PHY_RTL8218E:
		phyinfo->mac_type = RTMDIO_PHY_MAC_1G;
		phyinfo->has_giga_lite = true;
		break;
	case RTMDIO_PHY_RTL8226:
	case RTMDIO_PHY_RTL8221B_VB_CG:
	case RTMDIO_PHY_RTL8221B_VM_CG:
	case RTMDIO_PHY_RTL8224:
		phyinfo->mac_type = RTMDIO_PHY_MAC_2G_PLUS;
		phyinfo->has_giga_lite = true;
		phyinfo->poll_duplex = RTMDIO_PHY_POLL_MMD(31, 0xa400, 8);
		phyinfo->poll_adv_1000 = RTMDIO_PHY_POLL_MMD(31, 0xa412, 9);
		phyinfo->poll_lpa_1000 = RTMDIO_PHY_POLL_MMD(31, 0xa414, 11);
		break;
	default:
		phyinfo->phy_unknown = true;
		break;
	}
}

static int rtmdio_838x_reset(struct mii_bus *bus)
{
	struct rtmdio_ctrl *ctrl = bus->priv;
	int combo_phy;

	/* Disable MAC polling for PHY config. It will be activated later in the DSA driver */
	sw_w32(0, RTMDIO_838X_SMI_POLL_CTRL);

	/*
	 * Control bits EX_PHY_MAN_xxx have an important effect on the detection of the media
	 * status (fibre/copper) of a PHY. Once activated, register MAC_LINK_MEDIA_STS can
	 * give the real media status (0=copper, 1=fibre). For now assume that if port 24 is
	 * PHY driven, it must be a combo PHY and media detection is needed.
	 */
	combo_phy = ctrl->smi_bus[24] < 0 ? 0 : BIT(7);
	sw_w32_mask(BIT(7), combo_phy, RTMDIO_838X_SMI_GLB_CTRL);

	return 0;
}

static int rtmdio_839x_reset(struct mii_bus *bus)
{
	return 0;

	pr_debug("%s called\n", __func__);
	/* BUG: The following does not work, but should! */
	/* Disable MAC polling the PHY so that we can start configuration */
	sw_w32(0x00000000, RTMDIO_839X_SMI_PORT_POLLING_CTRL);
	sw_w32(0x00000000, RTMDIO_839X_SMI_PORT_POLLING_CTRL + 4);
	/* Disable PHY polling via SoC */
	sw_w32_mask(1 << 7, 0, RTMDIO_839X_SMI_GLB_CTRL);

	/* Probably should reset all PHYs here... */
	return 0;
}

static int rtmdio_930x_reset(struct mii_bus *bus)
{
	struct rtmdio_ctrl *ctrl = bus->priv;
	struct rtmdio_phy_info phyinfo;
	unsigned int reg, mask, val;

	/* Define bus topology */
	for (int addr = 0; addr < ctrl->cfg->cpu_port; addr++) {
		if (ctrl->smi_bus[addr] < 0)
			continue;

		reg = (addr / 6) * 4;
		mask = 0x1f << ((addr % 6) * 5);
		val = ctrl->smi_addr[addr] << (ffs(mask) - 1);
		sw_w32_mask(mask, val, RTMDIO_930X_SMI_PORT0_5_ADDR + reg);

		reg = (addr / 16) * 4;
		mask = 0x3 << ((addr % 16) * 2);
		val = ctrl->smi_bus[addr] << (ffs(mask) - 1);
		sw_w32_mask(mask, val, RTMDIO_930X_SMI_PORT0_15_POLLING_SEL + reg);
	}

	/* Define c22/c45 bus polling */
	for (int addr = 0; addr < RTMDIO_MAX_SMI_BUS; addr++) {
		mask = BIT(16 + addr);
		val = ctrl->smi_bus_isc45[addr] ? mask : 0;
		sw_w32_mask(mask, val, RTMDIO_930X_SMI_GLB_CTRL);
	}

	/* Define PHY specific polling parameters */
	for (int addr = 0; addr < ctrl->cfg->cpu_port; addr++) {
		if (ctrl->smi_bus[addr] < 0)
			continue;

		rtmdio_get_phy_info(bus, addr, &phyinfo);
		if (phyinfo.phy_unknown) {
			pr_warn("skip polling setup for unknown PHY %08x on port %d\n",
				phyinfo.phy_id, addr);
			continue;
		}

		/* port MAC type */
		mask = addr > 23 ? 0x7 << ((addr - 24) * 3 + 12): 0x3 << ((addr / 4) * 2);
		val = phyinfo.mac_type << (ffs(mask) - 1);
		sw_w32_mask(mask, val, RTMDIO_930X_SMI_MAC_TYPE_CTRL);

		/* polling via standard or resolution register */
		mask = BIT(20 + ctrl->smi_bus[addr]);
		val = phyinfo.has_res_reg ? mask : 0;
		sw_w32_mask(mask, val, RTMDIO_930X_SMI_GLB_CTRL);

		/* proprietary Realtek 1G/2.5 lite polling */
		mask = BIT(addr);
		val = phyinfo.has_giga_lite ? mask : 0;
		sw_w32_mask(mask, val, RTMDIO_930X_SMI_PRVTE_POLLING_CTRL);

		/* special duplex/advertisement polling registers */
		if (phyinfo.poll_duplex || phyinfo.poll_adv_1000 || phyinfo.poll_lpa_1000) {
			sw_w32(phyinfo.poll_duplex, RTMDIO_930X_SMI_10G_POLLING_REG0_CFG);
			sw_w32(phyinfo.poll_adv_1000, RTMDIO_930X_SMI_10G_POLLING_REG9_CFG);
			sw_w32(phyinfo.poll_lpa_1000, RTMDIO_930X_SMI_10G_POLLING_REG10_CFG);
		}
	}

	pr_debug("%s: RTMDIO_930X_SMI_GLB_CTRL %08x\n", __func__,
		 sw_r32(RTMDIO_930X_SMI_GLB_CTRL));
	pr_debug("%s: RTMDIO_930X_SMI_PORT0_15_POLLING_SEL %08x\n", __func__,
		 sw_r32(RTMDIO_930X_SMI_PORT0_15_POLLING_SEL));
	pr_debug("%s: RTMDIO_930X_SMI_PORT16_27_POLLING_SEL %08x\n", __func__,
		 sw_r32(RTMDIO_930X_SMI_PORT16_27_POLLING_SEL));
	pr_debug("%s: RTMDIO_930X_SMI_MAC_TYPE_CTRL %08x\n", __func__,
		 sw_r32(RTMDIO_930X_SMI_MAC_TYPE_CTRL));
	pr_debug("%s: RTMDIO_930X_SMI_10G_POLLING_REG0_CFG %08x\n", __func__,
		 sw_r32(RTMDIO_930X_SMI_10G_POLLING_REG0_CFG));
	pr_debug("%s: RTMDIO_930X_SMI_10G_POLLING_REG9_CFG %08x\n", __func__,
		 sw_r32(RTMDIO_930X_SMI_10G_POLLING_REG9_CFG));
	pr_debug("%s: RTMDIO_930X_SMI_10G_POLLING_REG10_CFG %08x\n", __func__,
		 sw_r32(RTMDIO_930X_SMI_10G_POLLING_REG10_CFG));
	pr_debug("%s: RTMDIO_930X_SMI_PRVTE_POLLING_CTRL %08x\n", __func__,
		 sw_r32(RTMDIO_930X_SMI_PRVTE_POLLING_CTRL));

	return 0;
}

static int rtmdio_931x_reset(struct mii_bus *bus)
{
	struct rtmdio_ctrl *ctrl = bus->priv;
	struct rtmdio_phy_info phyinfo;
	u32 poll_sel[4] = { 0 };
	u32 poll_ctrl = 0;
	u32 c45_mask = 0;

	pr_info("%s called\n", __func__);
	/* Disable port polling for configuration purposes */
	sw_w32(0, RTMDIO_931X_SMI_PORT_POLLING_CTRL);
	sw_w32(0, RTMDIO_931X_SMI_PORT_POLLING_CTRL + 4);
	msleep(100);

	/* Mapping of port to phy-addresses on an SMI bus */
	for (int addr = 0; addr < ctrl->cfg->cpu_port; addr++) {
		u32 pos;

		if (ctrl->smi_bus[addr] < 0)
			continue;

		pos = (addr % 6) * 5;
		sw_w32_mask(0x1f << pos, ctrl->smi_addr[addr] << pos, RTMDIO_931X_SMI_PORT_ADDR + (addr / 6) * 4);
		pos = (addr * 2) % 32;
		poll_sel[addr / 16] |= ctrl->smi_bus[addr] << pos;
		poll_ctrl |= BIT(20 + ctrl->smi_bus[addr]);
	}

	/* Configure which SMI bus is behind which port number */
	for (int i = 0; i < RTMDIO_MAX_SMI_BUS; i++) {
		pr_info("poll sel %d, %08x\n", i, poll_sel[i]);
		sw_w32(poll_sel[i], RTMDIO_931X_SMI_PORT_POLLING_SEL + (i * 4));
	}

	/* Configure c22/c45 polling (bit 1 of SMI_SETX_FMT_SEL)
	 *
	 * NOTE: this seems to be needed before accessing the bus though
	 * it should only apply to the SMI polling. Not setting c22/c45 here
	 * apparently causes garbage being read below.
	 */
	for (int i = 0; i < RTMDIO_MAX_SMI_BUS; i++) {
		/* bus is polled in c45 */
		if (ctrl->smi_bus_isc45[i])
			c45_mask |= 0x2 << (i * 2);  /* Std. C45, non-standard is 0x3 */
	}
	pr_info("%s: c45_mask: %08x", __func__, c45_mask);
	sw_w32_mask(GENMASK(7, 0), c45_mask, RTMDIO_931X_SMI_GLB_CTRL1);

	/* Define PHY specific polling parameters
	 *
	 * Those are applied per port here but the SoC only supports them
	 * per SMI bus or for all GPHY/10GPHY. This should be guarded by
	 * the existing hardware designs (i.e. only equally polled PHYs on
	 * the same SMI bus or kind of PHYs).
	 */
	for (int addr = 0; addr < ctrl->cfg->cpu_port; addr++) {
		unsigned int mask, val;
		int smi = ctrl->smi_bus[addr];
		
		if (smi < 0)
			continue;

		rtmdio_get_phy_info(bus, addr, &phyinfo);
		if (phyinfo.phy_unknown) {
			pr_warn("skip polling setup for unknown PHY %08x on port %d\n",
				phyinfo.phy_id, addr);
			continue;
		}

		mask = val = 0;

		/* PRVTE0 polling */
		mask |= BIT(20 + smi);
		if (phyinfo.has_res_reg)
			val |= BIT(20 + smi);

		/* PRVTE1 polling */
		mask |= BIT(24 + smi);
		if (phyinfo.force_res)
			val |= BIT(24 + smi);

		sw_w32_mask(mask, val, RTMDIO_931X_SMI_GLB_CTRL0);

		/* polling std. or proprietary format (bit 0 of SMI_SETX_FMT_SEL) */
		mask = BIT(smi * 2);
		val = phyinfo.force_res ? mask : 0;
		sw_w32_mask(mask, val, RTMDIO_931X_SMI_GLB_CTRL1);

		/* special polling registers */
		if (phyinfo.poll_duplex || phyinfo.poll_adv_1000 || phyinfo.poll_lpa_1000) {
			sw_w32(phyinfo.poll_duplex, RTMDIO_931X_SMI_10GPHY_POLLING_SEL2);
			sw_w32(phyinfo.poll_adv_1000, RTMDIO_931X_SMI_10GPHY_POLLING_SEL3);
			sw_w32(phyinfo.poll_lpa_1000, RTMDIO_931X_SMI_10GPHY_POLLING_SEL4);
		}
	}

	pr_debug("%s: RTMDIO_931X_SMI_GLB_CTRL0 %08x\n", __func__,
		 sw_r32(RTMDIO_931X_SMI_GLB_CTRL0));
	pr_debug("%s: RTMDIO_931X_SMI_GLB_CTRL1 %08x\n", __func__,
		 sw_r32(RTMDIO_931X_SMI_GLB_CTRL1));
	pr_debug("%s: RTMDIO_931X_SMI_PORT_POLLING_SEL_0_15 %08x\n", __func__,
		 sw_r32(RTMDIO_931X_SMI_PORT_POLLING_SEL));
	pr_debug("%s: RTMDIO_931X_SMI_PORT_POLLING_SEL_16_27 %08x\n", __func__,
		 sw_r32(RTMDIO_931X_SMI_PORT_POLLING_SEL + 4));
	pr_debug("%s: RTMDIO_931X_SMI_PORT_POLLING_SEL_28_43 %08x\n", __func__,
		 sw_r32(RTMDIO_931X_SMI_PORT_POLLING_SEL + 8));
	pr_debug("%s: RTMDIO_931X_SMI_PORT_POLLING_SEL_44_55 %08x\n", __func__,
		 sw_r32(RTMDIO_931X_SMI_PORT_POLLING_SEL + 12));
	pr_debug("%s: RTMDIO_931X_SMI_10GPHY_POLLING_SEL0 %08x\n", __func__,
		 sw_r32(RTMDIO_931X_SMI_10GPHY_POLLING_SEL0));
	pr_debug("%s: RTMDIO_931X_SMI_10GPHY_POLLING_SEL1 %08x\n", __func__,
		 sw_r32(RTMDIO_931X_SMI_10GPHY_POLLING_SEL1));
	pr_debug("%s: RTMDIO_931X_SMI_10GPHY_POLLING_SEL2 %08x\n", __func__,
		 sw_r32(RTMDIO_931X_SMI_10GPHY_POLLING_SEL2));
	pr_debug("%s: RTMDIO_931X_SMI_10GPHY_POLLING_SEL3 %08x\n", __func__,
		 sw_r32(RTMDIO_931X_SMI_10GPHY_POLLING_SEL3));
	pr_debug("%s: RTMDIO_931X_SMI_10GPHY_POLLING_SEL4 %08x\n", __func__,
		 sw_r32(RTMDIO_931X_SMI_10GPHY_POLLING_SEL4));

	return 0;
}

static int rtmdio_reset(struct mii_bus *bus)
{
	struct rtmdio_ctrl *ctrl = bus->priv;

	return ctrl->cfg->reset(bus);
}

static int rtmdio_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct rtmdio_ctrl *ctrl;
	struct device_node *dn;
	struct mii_bus *bus;
	int ret, addr;

	bus = devm_mdiobus_alloc_size(dev, sizeof(*ctrl));
	if (!bus)
		return -ENOMEM;

	ctrl = bus->priv;
	ctrl->cfg = (const struct rtmdio_config *)device_get_match_data(dev);
	for (addr = 0; addr < RTMDIO_MAX_PORT; addr++)
		ctrl->smi_bus[addr] = -1;

	for_each_node_by_name(dn, "ethernet-phy") {
		if (of_property_read_u32(dn, "reg", &addr))
			continue;

		if (addr >= ctrl->cfg->cpu_port) {
			pr_err("%s: illegal port number %d\n", __func__, addr);
			return -ENODEV;
		}

		of_property_read_u32(dn->parent, "reg", &ctrl->smi_bus[addr]);
		if (of_property_read_u32(dn, "realtek,smi-address", &ctrl->smi_addr[addr]))
			ctrl->smi_addr[addr] = addr;
		
		if (ctrl->smi_bus[addr] >= RTMDIO_MAX_SMI_BUS) {
			pr_err("%s: illegal SMI bus number %d\n", __func__, ctrl->smi_bus[addr]);
			return -ENODEV;
		}

		if (of_device_is_compatible(dn, "ethernet-phy-ieee802.3-c45"))
			ctrl->smi_bus_isc45[ctrl->smi_bus[addr]] = true;

		ctrl->dn[addr] = dn;
	}

	bus->name = "Realtek MDIO bus";
	bus->reset = rtmdio_reset;
	bus->read = rtmdio_read;
	bus->write = rtmdio_write;
	bus->read_c45 = rtmdio_read_c45;
	bus->write_c45 = rtmdio_write_c45;
	bus->parent = dev;
	bus->phy_mask = ~0;
	snprintf(bus->id, MII_BUS_ID_SIZE, "%s-mii", dev_name(dev));

	device_set_node(&bus->dev, of_fwnode_handle(dev->of_node));
	ret = devm_mdiobus_register(dev, bus);
	if (ret)
		return ret;

	for (addr = 0; addr < ctrl->cfg->cpu_port; addr++) {
		if (ctrl->dn[addr]) {
			ret = fwnode_mdiobus_register_phy(bus, of_fwnode_handle(ctrl->dn[addr]), addr);
			if (ret)
				return ret;
		}
	}

	return 0;
}

static const struct rtmdio_config rtmdio_838x_cfg = {
	.cpu_port	= 28,
	.raw_page	= 4095,
	.read_mmd_phy	= rtmdio_838x_read_mmd_phy,
	.read_phy	= rtmdio_838x_read_phy,
	.reset		= rtmdio_838x_reset,
	.write_mmd_phy	= rtmdio_838x_write_mmd_phy,
	.write_phy	= rtmdio_838x_write_phy,
};

static const struct rtmdio_config rtmdio_839x_cfg = {
	.cpu_port	= 52,
	.raw_page	= 8191,
	.read_mmd_phy	= rtmdio_839x_read_mmd_phy,
	.read_phy	= rtmdio_839x_read_phy,
	.reset		= rtmdio_839x_reset,
	.write_mmd_phy	= rtmdio_839x_write_mmd_phy,
	.write_phy	= rtmdio_839x_write_phy,
};

static const struct rtmdio_config rtmdio_930x_cfg = {
	.cpu_port	= 28,
	.raw_page	= 4095,
	.read_mmd_phy	= rtmdio_930x_read_mmd_phy,
	.read_phy	= rtmdio_930x_read_phy,
	.reset		= rtmdio_930x_reset,
	.write_mmd_phy	= rtmdio_930x_write_mmd_phy,
	.write_phy	= rtmdio_930x_write_phy,
};

static const struct rtmdio_config rtmdio_931x_cfg = {
	.cpu_port	= 56,
	.raw_page	= 8191,
	.read_mmd_phy	= rtmdio_931x_read_mmd_phy,
	.read_phy	= rtmdio_931x_read_phy,
	.reset		= rtmdio_931x_reset,
	.write_mmd_phy	= rtmdio_931x_write_mmd_phy,
	.write_phy	= rtmdio_931x_write_phy,
};

static const struct of_device_id rtmdio_ids[] = {
	{
		.compatible = "realtek,rtl8380-mdio",
		.data = &rtmdio_838x_cfg,
	},
	{
		.compatible = "realtek,rtl8392-mdio",
		.data = &rtmdio_839x_cfg,
	},
	{
		.compatible = "realtek,rtl9301-mdio",
		.data = &rtmdio_930x_cfg,
	},
	{
		.compatible = "realtek,rtl9311-mdio",
		.data = &rtmdio_931x_cfg,
	},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, rtmdio_ids);

static struct platform_driver rtmdio_driver = {
	.probe = rtmdio_probe,
	.driver = {
		.name = "mdio-rtl-otto",
		.of_match_table = rtmdio_ids,
	},
};

module_platform_driver(rtmdio_driver);

MODULE_DESCRIPTION("RTL83xx/RTL93xx MDIO driver");
MODULE_LICENSE("GPL");
