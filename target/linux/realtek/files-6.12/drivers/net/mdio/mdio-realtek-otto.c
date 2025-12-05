// SPDX-License-Identifier: GPL-2.0-only

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

#define RTMDIO_838X_CPU_PORT			28
#define RTMDIO_838X_FAMILY_ID			0x8380

#define RTMDIO_839X_CPU_PORT			52
#define RTMDIO_839X_FAMILY_ID			0x8390

#define RTMDIO_930X_CPU_PORT			28
#define RTMDIO_930X_FAMILY_ID			0x9300

#define RTMDIO_931X_CPU_PORT			56
#define RTMDIO_931X_FAMILY_ID			0x9310

/* Register base */
#define RTMDIO_SW_BASE				((volatile void *) 0xBB000000)

/* MDIO bus registers */
#define RTMDIO_838X_SMI_GLB_CTRL		(0xa100)
#define RTMDIO_838X_SMI_ACCESS_PHY_CTRL_0	(0xa1b8)
#define RTMDIO_838X_SMI_ACCESS_PHY_CTRL_1	(0xa1bc)
#define RTMDIO_838X_SMI_ACCESS_PHY_CTRL_2	(0xa1c0)
#define RTMDIO_838X_SMI_ACCESS_PHY_CTRL_3	(0xa1c4)
#define RTMDIO_838X_SMI_POLL_CTRL		(0xa17c)

#define RTMDIO_839X_PHYREG_CTRL			(0x03E0)
#define RTMDIO_839X_PHYREG_PORT_CTRL		(0x03E4)
#define RTMDIO_839X_PHYREG_ACCESS_CTRL		(0x03DC)
#define RTMDIO_839X_PHYREG_DATA_CTRL		(0x03F0)
#define RTMDIO_839X_PHYREG_MMD_CTRL		(0x03F4)
#define RTMDIO_839X_SMI_PORT_POLLING_CTRL	(0x03fc)
#define RTMDIO_839X_SMI_GLB_CTRL		(0x03f8)

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

/* MDIO SerDes registers */
#define RTMDIO_838X_BASE			(0xe780)

#define RTMDIO_839X_BASE			(0xa000)

#define RTMDIO_930X_SDS_INDACS_CMD		(0x03B0)
#define RTMDIO_930X_SDS_INDACS_DATA		(0x03B4)

#define RTMDIO_931X_SERDES_INDRT_ACCESS_CTRL	(0x5638)
#define RTMDIO_931X_SERDES_INDRT_DATA_CTRL	(0x563C)

/* Other registers */
#define RTMDIO_839X_MODEL_NAME_INFO_REG		(0x0ff0)
#define RTMDIO_93XX_MODEL_NAME_INFO_REG		(0x0004)

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

DEFINE_MUTEX(rtmdio_lock);
DEFINE_MUTEX(rtmdio_lock_sds);

struct rtmdio_bus_priv {
	u16 id;
	u16 family_id;
	int rawpage;
	int cpu_port;
	int page[RTMDIO_MAX_PORT];
	bool raw[RTMDIO_MAX_PORT];
	int smi_bus[RTMDIO_MAX_PORT];
	u8 smi_addr[RTMDIO_MAX_PORT];
	int sds_id[RTMDIO_MAX_PORT];
	bool smi_bus_isc45[RTMDIO_MAX_SMI_BUS];
	bool phy_is_internal[RTMDIO_MAX_PORT];
	phy_interface_t interfaces[RTMDIO_MAX_PORT];
	int (*read_mmd_phy)(u32 port, u32 addr, u32 reg, u32 *val);
	int (*write_mmd_phy)(u32 port, u32 addr, u32 reg, u32 val);
	int (*read_phy)(u32 port, u32 page, u32 reg, u32 *val);
	int (*write_phy)(u32 port, u32 page, u32 reg, u32 val);
	int (*read_sds_phy)(int sds, int page, int regnum);
	int (*write_sds_phy)(int sds, int page, int regnum, u16 val);
};

/* SerDes reader/writer functions for the ports without external phy. */

/*
 * The RTL838x has 6 SerDes. The 16 bit registers start at 0xbb00e780 and are mapped directly into
 * 32 bit memory addresses. High 16 bits are always empty. A "lower" memory block serves pages 0/3
 * a "higher" memory block pages 1/2.
 */

static int rtmdio_838x_reg_offset(int sds, int page, int regnum)
{
	if (sds < 0 || sds > 5)
		return -EINVAL;

	if (page == 0 || page == 3)
		return (sds << 9) + (page << 7) + (regnum << 2);
	else if (page == 1 || page == 2)
		return 0xb80 + (sds << 8) + (page << 7) + (regnum << 2);

	return -EINVAL;
}

static int rtmdio_838x_read_sds_phy(int sds, int page, int regnum)
{
	int offset = rtmdio_838x_reg_offset(sds, page, regnum);

	if (offset < 0)
		return offset;

	return sw_r32(RTMDIO_838X_BASE + offset) & GENMASK(15, 0);
}

static int rtmdio_838x_write_sds_phy(int sds, int page, int regnum, u16 val)
{
	int offset = rtmdio_838x_reg_offset(sds, page, regnum);

	if (offset < 0)
		return offset;

	sw_w32(val, RTMDIO_838X_BASE + offset);

	return 0;
}

/* RTL838x specific MDIO functions */

static int rtmdio_838x_smi_wait_op(int timeout)
{
	int ret = 0;
	u32 val;

	ret = readx_poll_timeout(sw_r32, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_1,
				 val, !(val & 0x1), 20, timeout);
	if (ret)
		pr_err("%s: timeout\n", __func__);

	return ret;
}

/* Reads a register in a page from the PHY */
static int rtmdio_838x_read_phy(u32 port, u32 page, u32 reg, u32 *val)
{
	u32 v, park_page = 0x1f << 15;
	int err;

	if (port > 31) {
		*val = 0xffff;
		return 0;
	}

	if (page > 4095 || reg > 31)
		return -ENOTSUPP;

	mutex_lock(&rtmdio_lock);

	sw_w32_mask(0xffff0000, port << 16, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_2);
	v = reg << 20 | page << 3;
	sw_w32(v | park_page, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_1);
	sw_w32_mask(0, 1, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_1);

	err = rtmdio_838x_smi_wait_op(100000);
	if (err)
		goto errout;

	*val = sw_r32(RTMDIO_838X_SMI_ACCESS_PHY_CTRL_2) & 0xffff;
errout:
	mutex_unlock(&rtmdio_lock);

	return err;
}

/* Write to a register in a page of the PHY */
static int rtmdio_838x_write_phy(u32 port, u32 page, u32 reg, u32 val)
{
	u32 v, park_page = 0x1f << 15;
	int err;

	val &= 0xffff;
	if (port > 31 || page > 4095 || reg > 31)
		return -ENOTSUPP;

	mutex_lock(&rtmdio_lock);

	sw_w32(BIT(port), RTMDIO_838X_SMI_ACCESS_PHY_CTRL_0);
	sw_w32_mask(0xffff0000, val << 16, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_2);

	v = reg << 20 | page << 3 | 0x4;
	sw_w32(v | park_page, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_1);
	sw_w32_mask(0, 1, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_1);

	err = rtmdio_838x_smi_wait_op(100000);
	mutex_unlock(&rtmdio_lock);

	return err;
}

/* Read an mmd register of a PHY */
static int rtmdio_838x_read_mmd_phy(u32 port, u32 addr, u32 reg, u32 *val)
{
	int err;
	u32 v;

	mutex_lock(&rtmdio_lock);

	sw_w32(1 << port, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_0);
	sw_w32_mask(0xffff0000, port << 16, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_2);

	v = addr << 16 | reg;
	sw_w32(v, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_3);

	/* mmd-access | read | cmd-start */
	v = 1 << 1 | 0 << 2 | 1;
	sw_w32(v, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_1);

	err = rtmdio_838x_smi_wait_op(100000);
	if (err)
		goto errout;

	*val = sw_r32(RTMDIO_838X_SMI_ACCESS_PHY_CTRL_2) & 0xffff;
errout:
	mutex_unlock(&rtmdio_lock);

	return err;
}

/* Write to an mmd register of a PHY */
static int rtmdio_838x_write_mmd_phy(u32 port, u32 addr, u32 reg, u32 val)
{
	int err;
	u32 v;

	pr_debug("MMD write: port %d, dev %d, reg %d, val %x\n", port, addr, reg, val);
	val &= 0xffff;
	mutex_lock(&rtmdio_lock);

	sw_w32(1 << port, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_0);
	sw_w32_mask(0xffff0000, val << 16, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_2);
	sw_w32_mask(0x1f << 16, addr << 16, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_3);
	sw_w32_mask(0xffff, reg, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_3);
	/* mmd-access | write | cmd-start */
	v = 1 << 1 | 1 << 2 | 1;
	sw_w32(v, RTMDIO_838X_SMI_ACCESS_PHY_CTRL_1);

	err = rtmdio_838x_smi_wait_op(100000);
	mutex_unlock(&rtmdio_lock);

	return err;
}

/*
 * The RTL839x has 14 SerDes starting at 0xbb00a000. 0-7, 10, 11 are 5GBit, 8, 9, 12, 13 are
 * 10 GBit. Two adjacent SerDes are tightly coupled and share a 1024 bytes register area. Per 32
 * bit address two registers are stored. The first register is stored in the lower 2 bytes ("on
 * the right" due to big endian) and the second register in the upper 2 bytes. The following
 * register areas are known:
 *
 * - XSG0	(4 pages @ offset 0x000): for even SerDes
 * - XSG1	(4 pages @ offset 0x100): for odd SerDes
 * - TGRX	(4 pages @ offset 0x200): for even 10G SerDes
 * - ANA_RG	(2 pages @ offset 0x300): for even 5G SerDes
 * - ANA_RG	(2 pages @ offset 0x380): for odd 5G SerDes
 * - ANA_TG	(2 pages @ offset 0x300): for even 10G SerDes
 * - ANA_TG	(2 pages @ offset 0x380): for odd 10G SerDes
 *
 * The most consistent mapping that aligns to the RTL93xx devices is:
 *
 *		even 5G SerDes	odd 5G SerDes	even 10G SerDes	odd 10G SerDes
 * Page 0:	XSG0/0		XSG1/0		XSG0/0		XSG1/0
 * Page 1:	XSG0/1		XSG1/1		XSG0/1		XSG1/1
 * Page 2:	XSG0/2		XSG1/2		XSG0/2		XSG1/2
 * Page 3:	XSG0/3		XSG1/3		XSG0/3		XSG1/3
 * Page 4:	<zero>		<zero>		TGRX/0		<zero>
 * Page 5:	<zero>		<zero>		TGRX/1		<zero>
 * Page 6:	<zero>		<zero>		TGRX/2		<zero>
 * Page 7:	<zero>		<zero>		TGRX/3		<zero>
 * Page 8:	ANA_RG		ANA_RG		<zero>		<zero>
 * Page 9:	ANA_RG_EXT	ANA_RG_EXT	<zero>		<zero>
 * Page 10:	<zero>		<zero>		ANA_TG		ANA_TG
 * Page 11:	<zero>		<zero>		ANA_TG_EXT	ANA_TG_EXT
 */

static int rtmdio_839x_reg_offset(int sds, int page, int regnum)
{
	int offset = ((sds & 0xfe) << 9) + ((regnum & 0xfe) << 1) + (page << 6);
	int sds5g = (GENMASK(11, 10) | GENMASK(7, 0)) & BIT(sds);

	if (sds < 0 || sds > 13 || page < 0 || page > 11 || regnum < 0 || regnum > 31)
		return -EIO;

	if (page < 4)
		return offset + ((sds & 1) << 8);
	else if ((page & 4) && (sds == 8 || sds == 12))
		return offset + 0x100;
	else if (page >= 8 && page <= 9 && sds5g)
		return offset + 0x100 + ((sds & 1) << 7);
	else if (page >= 10 && !sds5g)
		return offset + 0x80 + ((sds & 1) << 7);

	return -EINVAL; /* hole */
}

static int rtmdio_839x_read_sds_phy(int sds, int page, int regnum)
{
	int bitpos = ((regnum << 4) & 0x10);
	int offset;
	u32 val;

	offset = rtmdio_839x_reg_offset(sds, page, regnum);
	if (offset == -EINVAL)
		return 0;

	if (offset < 0)
		return offset;

	/* phy id is empty so simulate one */
	if (page == 2 && regnum == 2)
		return 0x1c;
	if (page == 2 && regnum == 3)
		return 0x8393;

	val = sw_r32(RTMDIO_839X_BASE + offset);
	val = (val >> bitpos) & 0xffff;

	return val;
}

static int rtmdio_839x_write_sds_phy(int sds, int page, int regnum, u16 val)
{
	u32 neighbor;
	int offset;
	u32 set;

	offset = rtmdio_839x_reg_offset(sds, page, regnum);
	if (offset == -EINVAL)
		return 0;

	if (offset < 0)
		return 0;

	neighbor = rtmdio_839x_read_sds_phy(sds, page, regnum ^ 1);
	if (regnum & 1)
		set = (val << 16) + neighbor;
	else
		set = (neighbor << 16) + val;

	sw_w32(set, RTMDIO_839X_BASE + offset);

	return 0;
}

/* RTL839x specific MDIO functions */

static int rtmdio_839x_smi_wait_op(int timeout)
{
	int ret = 0;
	u32 val;

	ret = readx_poll_timeout(sw_r32, RTMDIO_839X_PHYREG_ACCESS_CTRL,
				 val, !(val & 0x1), 20, timeout);
	if (ret)
		pr_err("%s: timeout\n", __func__);

	return ret;
}

static int rtmdio_839x_read_phy(u32 port, u32 page, u32 reg, u32 *val)
{
	int err = 0;
	u32 v;

	if (port >= RTMDIO_839X_CPU_PORT || page > 8191 || reg > 31)
		return -ENOTSUPP;

	mutex_lock(&rtmdio_lock);

	sw_w32_mask(0xffff0000, port << 16, RTMDIO_839X_PHYREG_DATA_CTRL);
	v = reg << 5 | page << 10 | ((page == 0x1fff) ? 0x1f : 0) << 23;
	sw_w32(v, RTMDIO_839X_PHYREG_ACCESS_CTRL);

	sw_w32(0x1ff, RTMDIO_839X_PHYREG_CTRL);

	v |= 1;
	sw_w32(v, RTMDIO_839X_PHYREG_ACCESS_CTRL);

	err = rtmdio_839x_smi_wait_op(100000);
	if (err)
		goto errout;

	*val = sw_r32(RTMDIO_839X_PHYREG_DATA_CTRL) & 0xffff;

errout:
	mutex_unlock(&rtmdio_lock);

	return err;
}

static int rtmdio_839x_write_phy(u32 port, u32 page, u32 reg, u32 val)
{
	int err = 0;
	u32 v;

	val &= 0xffff;
	if (port >= RTMDIO_839X_CPU_PORT || page > 8191 || reg > 31)
		return -ENOTSUPP;

	mutex_lock(&rtmdio_lock);

	/* Set PHY to access */
	sw_w32(BIT_ULL(port), RTMDIO_839X_PHYREG_PORT_CTRL);
	sw_w32(BIT_ULL(port) >> 32, RTMDIO_839X_PHYREG_PORT_CTRL + 4);

	sw_w32_mask(0xffff0000, val << 16, RTMDIO_839X_PHYREG_DATA_CTRL);

	v = reg << 5 | page << 10 | ((page == 0x1fff) ? 0x1f : 0) << 23;
	sw_w32(v, RTMDIO_839X_PHYREG_ACCESS_CTRL);

	sw_w32(0x1ff, RTMDIO_839X_PHYREG_CTRL);

	v |= BIT(3) | 1; /* Write operation and execute */
	sw_w32(v, RTMDIO_839X_PHYREG_ACCESS_CTRL);

	err = rtmdio_839x_smi_wait_op(100000);
	if (err)
		goto errout;

	if (sw_r32(RTMDIO_839X_PHYREG_ACCESS_CTRL) & 0x2)
		err = -EIO;

errout:
	mutex_unlock(&rtmdio_lock);

	return err;
}

/* Read an mmd register of the PHY */
static int rtmdio_839x_read_mmd_phy(u32 port, u32 devnum, u32 regnum, u32 *val)
{
	int err = 0;
	u32 v;

	/* Take bug on RTL839x Rev <= C into account */
	if (port >= RTMDIO_839X_CPU_PORT)
		return -EIO;

	mutex_lock(&rtmdio_lock);

	/* Set PHY to access */
	sw_w32_mask(0xffff << 16, port << 16, RTMDIO_839X_PHYREG_DATA_CTRL);

	/* Set MMD device number and register to write to */
	sw_w32(devnum << 16 | (regnum & 0xffff), RTMDIO_839X_PHYREG_MMD_CTRL);

	v = BIT(2) | BIT(0); /* MMD-access | EXEC */
	sw_w32(v, RTMDIO_839X_PHYREG_ACCESS_CTRL);

	err = rtmdio_839x_smi_wait_op(100000);
	if (err)
		goto errout;

	/* There is no error-checking via BIT 1 of v, as it does not seem to be set correctly */
	*val = (sw_r32(RTMDIO_839X_PHYREG_DATA_CTRL) & 0xffff);
	pr_debug("%s: port %d, regnum: %x, val: %x (err %d)\n", __func__, port, regnum, *val, err);

errout:
	mutex_unlock(&rtmdio_lock);

	return err;
}

/* Write to an mmd register of the PHY */
static int rtmdio_839x_write_mmd_phy(u32 port, u32 devnum, u32 regnum, u32 val)
{
	int err = 0;
	u32 v;

	/* Take bug on RTL839x Rev <= C into account */
	if (port >= RTMDIO_839X_CPU_PORT)
		return -EIO;

	mutex_lock(&rtmdio_lock);

	/* Set PHY to access */
	sw_w32(BIT_ULL(port), RTMDIO_839X_PHYREG_PORT_CTRL);
	sw_w32(BIT_ULL(port) >> 32, RTMDIO_839X_PHYREG_PORT_CTRL + 4);

	/* Set data to write */
	sw_w32_mask(0xffff << 16, val << 16, RTMDIO_839X_PHYREG_DATA_CTRL);

	/* Set MMD device number and register to write to */
	sw_w32(devnum << 16 | (regnum & 0xffff), RTMDIO_839X_PHYREG_MMD_CTRL);

	v = BIT(3) | BIT(2) | BIT(0); /* WRITE | MMD-access | EXEC */
	sw_w32(v, RTMDIO_839X_PHYREG_ACCESS_CTRL);

	err = rtmdio_839x_smi_wait_op(100000);
	if (err)
		goto errout;

	pr_debug("%s: port %d, regnum: %x, val: %x (err %d)\n", __func__, port, regnum, val, err);

errout:
	mutex_unlock(&rtmdio_lock);

	return err;
}

/*
 * The RTL930x family has 12 SerDes of three types. They are accessed through two IO registers at
 * 0xbb0003b0 which simulate commands to an internal MDIO bus:
 *
 * - SerDes 0-1 exist on the RTL9301 and 9302B and are QSGMII capable
 * - SerDes 2-9 are USXGMII capabable with either quad or single configuration
 * - SerDes 10-11 are 10GBase-R capable
 */

static int rtmdio_930x_read_sds_phy(int sds, int page, int regnum)
{
	int i, ret = -EIO;
	u32 cmd;

	if (sds < 0 || sds > 11 || page < 0 || page > 63 || regnum < 0 || regnum > 31)
		return -EIO;

	mutex_lock(&rtmdio_lock_sds);

	cmd = sds << 2 | page << 7 | regnum << 13 | 1;
	sw_w32(cmd, RTMDIO_930X_SDS_INDACS_CMD);

	for (i = 0; i < 100; i++) {
		if (!(sw_r32(RTMDIO_930X_SDS_INDACS_CMD) & 0x1))
			break;
		mdelay(1);
	}

	if (i < 100)
		ret = sw_r32(RTMDIO_930X_SDS_INDACS_DATA) & 0xffff;

	mutex_unlock(&rtmdio_lock_sds);

	return ret;
}

static int rtmdio_930x_write_sds_phy(int sds, int page, int regnum, u16 val)
{
	int i, ret = -EIO;
	u32 cmd;

	if (sds < 0 || sds > 11 || page < 0 || page > 63 || regnum < 0 || regnum > 31)
		return -EIO;

	mutex_lock(&rtmdio_lock_sds);

	cmd = sds << 2 | page << 7 | regnum << 13 | 0x3;
	sw_w32(val, RTMDIO_930X_SDS_INDACS_DATA);
	sw_w32(cmd, RTMDIO_930X_SDS_INDACS_CMD);

	for (i = 0; i < 100; i++) {
		if (!(sw_r32(RTMDIO_930X_SDS_INDACS_CMD) & 0x1))
			break;
		mdelay(1);
	}

	mutex_unlock(&rtmdio_lock_sds);

	if (i < 100)
		ret = 0;

	return ret;
}

/* RTL930x specific MDIO functions */

static int rtmdio_930x_write_phy(u32 port, u32 page, u32 reg, u32 val)
{
	u32 v;
	int err = 0;

	pr_debug("%s: port %d, page: %d, reg: %x, val: %x\n", __func__, port, page, reg, val);

	if (port > 63 || page > 4095 || reg > 31)
		return -ENOTSUPP;

	val &= 0xffff;
	mutex_lock(&rtmdio_lock);

	sw_w32(BIT(port), RTMDIO_930X_SMI_ACCESS_PHY_CTRL_0);
	sw_w32_mask(0xffff << 16, val << 16, RTMDIO_930X_SMI_ACCESS_PHY_CTRL_2);
	v = reg << 20 | page << 3 | 0x1f << 15 | BIT(2) | BIT(0);
	sw_w32(v, RTMDIO_930X_SMI_ACCESS_PHY_CTRL_1);

	do {
		v = sw_r32(RTMDIO_930X_SMI_ACCESS_PHY_CTRL_1);
	} while (v & 0x1);

	if (v & 0x2)
		err = -EIO;

	mutex_unlock(&rtmdio_lock);

	return err;
}

static int rtmdio_930x_read_phy(u32 port, u32 page, u32 reg, u32 *val)
{
	u32 v;
	int err = 0;

	if (port > 63 || page > 4095 || reg > 31)
		return -ENOTSUPP;

	mutex_lock(&rtmdio_lock);

	sw_w32_mask(0xffff << 16, port << 16, RTMDIO_930X_SMI_ACCESS_PHY_CTRL_2);
	v = reg << 20 | page << 3 | 0x1f << 15 | 1;
	sw_w32(v, RTMDIO_930X_SMI_ACCESS_PHY_CTRL_1);

	do {
		v = sw_r32(RTMDIO_930X_SMI_ACCESS_PHY_CTRL_1);
	} while (v & 0x1);

	if (v & BIT(25)) {
		pr_debug("Error reading phy %d, register %d\n", port, reg);
		err = -EIO;
	}
	*val = (sw_r32(RTMDIO_930X_SMI_ACCESS_PHY_CTRL_2) & 0xffff);

	pr_debug("%s: port %d, page: %d, reg: %x, val: %x\n", __func__, port, page, reg, *val);

	mutex_unlock(&rtmdio_lock);

	return err;
}

/* Write to an mmd register of the PHY */
static int rtmdio_930x_write_mmd_phy(u32 port, u32 devnum, u32 regnum, u32 val)
{
	int err = 0;
	u32 v;

	mutex_lock(&rtmdio_lock);

	/* Set PHY to access */
	sw_w32(BIT(port), RTMDIO_930X_SMI_ACCESS_PHY_CTRL_0);

	/* Set data to write */
	sw_w32_mask(0xffff << 16, val << 16, RTMDIO_930X_SMI_ACCESS_PHY_CTRL_2);

	/* Set MMD device number and register to write to */
	sw_w32(devnum << 16 | (regnum & 0xffff), RTMDIO_930X_SMI_ACCESS_PHY_CTRL_3);

	v = BIT(2) | BIT(1) | BIT(0); /* WRITE | MMD-access | EXEC */
	sw_w32(v, RTMDIO_930X_SMI_ACCESS_PHY_CTRL_1);

	do {
		v = sw_r32(RTMDIO_930X_SMI_ACCESS_PHY_CTRL_1);
	} while (v & BIT(0));

	pr_debug("%s: port %d, regnum: %x, val: %x (err %d)\n", __func__, port, regnum, val, err);
	mutex_unlock(&rtmdio_lock);
	return err;
}

/* Read an mmd register of the PHY */
static int rtmdio_930x_read_mmd_phy(u32 port, u32 devnum, u32 regnum, u32 *val)
{
	int err = 0;
	u32 v;

	mutex_lock(&rtmdio_lock);

	/* Set PHY to access */
	sw_w32_mask(0xffff << 16, port << 16, RTMDIO_930X_SMI_ACCESS_PHY_CTRL_2);

	/* Set MMD device number and register to write to */
	sw_w32(devnum << 16 | (regnum & 0xffff), RTMDIO_930X_SMI_ACCESS_PHY_CTRL_3);

	v = BIT(1) | BIT(0); /* MMD-access | EXEC */
	sw_w32(v, RTMDIO_930X_SMI_ACCESS_PHY_CTRL_1);

	do {
		v = sw_r32(RTMDIO_930X_SMI_ACCESS_PHY_CTRL_1);
	} while (v & BIT(0));
	/* There is no error-checking via BIT 25 of v, as it does not seem to be set correctly */
	*val = (sw_r32(RTMDIO_930X_SMI_ACCESS_PHY_CTRL_2) & 0xffff);
	pr_debug("%s: port %d, regnum: %x, val: %x (err %d)\n", __func__, port, regnum, *val, err);

	mutex_unlock(&rtmdio_lock);

	return err;
}

/*
 * The RTL931x family has 14 "frontend" SerDes that are cascaded. All operations (e.g. reset) work
 * on this frontend view while their registers are distributed over a total of least 26 background
 * SerDes with 64 pages and 32 registers. Three types of SerDes exist:
 *
 * - Serdes 0,1 are "simple" and work on one background serdes.
 * - "Even" SerDes with numbers 2, 4, 6, 8, 10, 12 work on two background SerDes. One analog and
 *   one digital.
 * - "Odd" SerDes with numbers 3, 5, 7, 9, 11, 13 work on a total of 3 background SerDes (one analog
 *   and two digital)
 *
 * This maps to:
 *
 * Frontend SerDes  |  0  1  2  3  4  5  6  7  8  9 10 11 12 13
 * -----------------+------------------------------------------
 * Backend SerDes 1 |  0  1  2  3  6  7 10 11 14 15 18 19 22 23
 * Backend SerDes 2 |  0  1  2  4  6  8 10 12 14 16 18 20 22 24
 * Backend SerDes 3 |  0  1  3  5  7  9 11 13 15 17 19 21 23 25
 *
 * Note: In Realtek proprietary XSGMII mode (10G pumped SGMII) the frontend SerDes works on the
 * two digital SerDes while in all other modes it works on the analog and the first digital SerDes.
 * Overlapping (e.g. backend SerDes 7 can be analog or digital 2) is avoided by the existing
 * hardware designs.
 *
 * Align this for readability by simulating a total of 576 pages and mix them as follows.
 *
 * frontend page		"even" frontend SerDes		"odd" frontend SerDes
 * page 0x000-0x03f (analog):	page 0x000-0x03f back SDS	page 0x000-0x03f back SDS
 * page 0x100-0x13f (digi 1):	page 0x000-0x03f back SDS	page 0x000-0x03f back SDS+1
 * page 0x200-0x23f (digi 2):	page 0x000-0x03f back SDS+1	page 0x000-0x03f back SDS+2
 */

static int rtsds_931x_get_backing_sds(int sds, int page)
{
	int map[] = {0, 1, 2, 3, 6, 7, 10, 11, 14, 15, 18, 19, 22, 23};
	int back = map[sds];

	if (page & 0xc0)
		return -EINVAL; /* hole */

	if (sds >= 2) {
		if (sds & 1)
			back += (page >> 8); /* distribute "odd" to 3 background SerDes */
		else
			back += (page >> 9); /* distribute "even" to 2 background SerDes */
	}

	return back;
}

static int rtsds_931x_read(int sds, int page, int regnum)
{
	int backsds, i, cmd, ret = -EIO;
	int backpage = page & 0x3f;

	if (sds < 0 || sds > 13 || page < 0 || page > 575 || regnum < 0 || regnum > 31)
		return -EIO;

	backsds = rtsds_931x_get_backing_sds(sds, page);
	if (backsds == -EINVAL)
		return 0;

	mutex_lock(&rtmdio_lock_sds);

	cmd = backsds << 2 | backpage << 7 | regnum << 13 | 0x1;
	sw_w32(cmd, RTMDIO_931X_SERDES_INDRT_ACCESS_CTRL);

	for (i = 0; i < 100; i++) {
		if (!(sw_r32(RTMDIO_931X_SERDES_INDRT_ACCESS_CTRL) & 0x1)) {
			ret = sw_r32(RTMDIO_931X_SERDES_INDRT_DATA_CTRL) & 0xffff;
			break;
		}
		mdelay(1);
	}

	mutex_unlock(&rtmdio_lock_sds);

	return ret;
}

static int rtsds_931x_write(int sds, int page, int regnum, u16 val)
{
	int backsds, i, cmd, ret = -EIO;
	int backpage = page & 0x3f;

	if (sds < 0 || sds > 13 || page < 0 || page > 575 || regnum < 0 || regnum > 31)
		return -EIO;

	backsds = rtsds_931x_get_backing_sds(sds, page);
	if (backsds == -EINVAL)
		return 0;

	mutex_lock(&rtmdio_lock_sds);

	cmd = backsds << 2 | backpage << 7 | regnum << 13 | 0x3;
	sw_w32(val, RTMDIO_931X_SERDES_INDRT_DATA_CTRL);
	sw_w32(cmd, RTMDIO_931X_SERDES_INDRT_ACCESS_CTRL);

	for (i = 0; i < 100; i++) {
		if (!(sw_r32(RTMDIO_931X_SERDES_INDRT_ACCESS_CTRL) & 0x1)) {
			ret = 0;
			break;
		}
		mdelay(1);
	}

	mutex_unlock(&rtmdio_lock_sds);

	return ret;
}

__always_unused
static int rtsds_931x_write_field(int sds, int page, int reg, int end_bit, int start_bit, u16 val)
{
	int l = end_bit - start_bit + 1;
	u32 data = val;

	if (l < 32) {
		u32 mask = BIT(l) - 1;

		data = rtsds_931x_read(sds, page, reg);
		data &= ~(mask << start_bit);
		data |= (val & mask) << start_bit;
	}

	return rtsds_931x_write(sds, page, reg, data);
}

__always_unused
static int rtsds_931x_read_field(int sds, int page, int reg, int end_bit, int start_bit)
{
	int l = end_bit - start_bit + 1;
	u32 v = rtsds_931x_read(sds, page, reg);

	if (l >= 32)
		return v;

	return (v >> start_bit) & (BIT(l) - 1);
}

/* RTL931x specific MDIO functions */

static int rtmdio_931x_write_phy(u32 port, u32 page, u32 reg, u32 val)
{
	u32 v;
	int err = 0;

	val &= 0xffff;
	if (port > 63 || page > 4095 || reg > 31)
		return -ENOTSUPP;

	mutex_lock(&rtmdio_lock);
	pr_debug("%s: writing to phy %d %d %d %d\n", __func__, port, page, reg, val);
	/* Clear both port registers */
	sw_w32(0, RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_2);
	sw_w32(0, RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_2 + 4);
	sw_w32_mask(0, BIT(port % 32), RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_2 + (port / 32) * 4);

	sw_w32_mask(0xffff, val, RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_3);

	v = reg << 6 | page << 11;
	sw_w32(v, RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_0);

	sw_w32(0x1ff, RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_1);

	v |= BIT(4) | 1; /* Write operation and execute */
	sw_w32(v, RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_0);

	do {
	} while (sw_r32(RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_0) & 0x1);

	if (sw_r32(RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_0) & 0x2)
		err = -EIO;

	mutex_unlock(&rtmdio_lock);

	return err;
}

static int rtmdio_931x_read_phy(u32 port, u32 page, u32 reg, u32 *val)
{
	u32 v;

	if (port > 63 || page > 4095 || reg > 31)
		return -ENOTSUPP;

	mutex_lock(&rtmdio_lock);

	sw_w32(port << 5, RTMDIO_931X_SMI_INDRT_ACCESS_BC_CTRL);

	v = reg << 6 | page << 11 | 1;
	sw_w32(v, RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_0);

	do {
	} while (sw_r32(RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_0) & 0x1);

	v = sw_r32(RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_0);
	*val = sw_r32(RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_3);
	*val = (*val & 0xffff0000) >> 16;

	pr_debug("%s: port %d, page: %d, reg: %x, val: %x, v: %08x\n",
		 __func__, port, page, reg, *val, v);

	mutex_unlock(&rtmdio_lock);

	return 0;
}

/* Read an mmd register of the PHY */
static int rtmdio_931x_read_mmd_phy(u32 port, u32 devnum, u32 regnum, u32 *val)
{
	int err = 0;
	u32 v;
	/* Select PHY register type
	 * If select 1G/10G MMD register type, registers EXT_PAGE, MAIN_PAGE and REG settings are don’t care.
	 * 0x0  Normal register (Clause 22)
	 * 0x1: 1G MMD register (MMD via Clause 22 registers 13 and 14)
	 * 0x2: 10G MMD register (MMD via Clause 45)
	 */
	int type = 2;

	mutex_lock(&rtmdio_lock);

	/* Set PHY to access via port-number */
	sw_w32(port << 5, RTMDIO_931X_SMI_INDRT_ACCESS_BC_CTRL);

	/* Set MMD device number and register to write to */
	sw_w32(devnum << 16 | regnum, RTMDIO_931X_SMI_INDRT_ACCESS_MMD_CTRL);

	v = type << 2 | BIT(0); /* MMD-access-type | EXEC */
	sw_w32(v, RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_0);

	do {
		v = sw_r32(RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_0);
	} while (v & BIT(0));

	/* Check for error condition */
	if (v & BIT(1))
		err = -EIO;

	*val = sw_r32(RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_3) >> 16;

	pr_debug("%s: port %d, dev: %x, regnum: %x, val: %x (err %d)\n", __func__,
		 port, devnum, regnum, *val, err);

	mutex_unlock(&rtmdio_lock);

	return err;
}

/* Write to an mmd register of the PHY */
static int rtmdio_931x_write_mmd_phy(u32 port, u32 devnum, u32 regnum, u32 val)
{
	int err = 0;
	u32 v;
	int type = 2;
	u64 pm;

	mutex_lock(&rtmdio_lock);

	/* Set PHY to access via port-mask */
	pm = (u64)1 << port;
	sw_w32((u32)pm, RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_2);
	sw_w32((u32)(pm >> 32), RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_2 + 4);

	/* Set data to write */
	sw_w32_mask(0xffff, val, RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_3);

	/* Set MMD device number and register to write to */
	sw_w32(devnum << 16 | regnum, RTMDIO_931X_SMI_INDRT_ACCESS_MMD_CTRL);

	v = BIT(4) | type << 2 | BIT(0); /* WRITE | MMD-access-type | EXEC */
	sw_w32(v, RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_0);

	do {
		v = sw_r32(RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_0);
	} while (v & BIT(0));

	pr_debug("%s: port %d, dev: %x, regnum: %x, val: %x (err %d)\n", __func__,
		 port, devnum, regnum, val, err);
	mutex_unlock(&rtmdio_lock);

	return err;
}

/* These are the core functions of our new Realtek SoC MDIO bus. */

static int rtmdio_read_c45(struct mii_bus *bus, int addr, int devnum, int regnum)
{
	struct rtmdio_bus_priv *priv = bus->priv;
	int err, val;

	if (addr >= priv->cpu_port)
		return -ENODEV;

	err = (*priv->read_mmd_phy)(addr, devnum, regnum, &val);
	pr_debug("rd_MMD(adr=%d, dev=%d, reg=%d) = %d, err = %d\n",
		 addr, devnum, regnum, val, err);
	return err ? err : val;
}

static int rtmdio_map_sds_register(int page, int regnum, int *sds_page, int *sds_regnum)
{
	/*
	 * For the SerDes PHY simulate a register mapping like common RealTek PHYs do. Always
	 * keep the common registers 0x00-0x0f in place and map the SerDes registers into the
	 * upper vendor specific registers 0x10-0x17 according to the page select register
	 * (0x1f). That gives a register mapping as follows:
	 *
	 * +-----------------------+-----------------------+---------------+-----------------+
	 * | reg 0x00-0x0f         | reg 0x10-0x17         | reg 0x18-0x1e | reg 0x1f        |
	 * +-----------------------+-----------------------+---------------+-----------------+
	 * | SerDes fiber page (2) | real SerDes registers | zero          | SerDes page     |
	 * | registers 0x00-0x0f   | in packages of 8      |               | select register |
	 * +-----------------------+-----------------------+---------------+-----------------+
	 */

	if (regnum < 16) {
		*sds_page = 2;
		*sds_regnum = regnum;
	} else if (regnum < 24) {
		*sds_page = page / 4;
		*sds_regnum = 8 * (page % 4) + (regnum - 16);
	} else
		return 0;

	return 1;
}

static int rtmdio_read_sds_phy(struct rtmdio_bus_priv *priv, int sds, int page, int regnum)
{
	int ret, sds_page, sds_regnum;

	ret = rtmdio_map_sds_register(page, regnum, &sds_page, &sds_regnum);
	if (ret)
		ret = priv->read_sds_phy(sds, sds_page, sds_regnum);
	pr_debug("rd_SDS(sds=%d, pag=%d, reg=%d) = %d\n", sds, page, regnum, ret);

	return ret;
}

static int rtmdio_write_sds_phy(struct rtmdio_bus_priv *priv, int sds, int page, int regnum, u16 val)
{
	int ret, sds_page, sds_regnum;

	ret = rtmdio_map_sds_register(page, regnum, &sds_page, &sds_regnum);
	if (ret)
		ret = priv->write_sds_phy(sds, sds_page, sds_regnum, val);
	pr_debug("wr_SDS(sds=%d, pag=%d, reg=%d, val=%d) err = %d\n", sds, page, regnum, val, ret);

	return ret;
}

static int rtmdio_read(struct mii_bus *bus, int addr, int regnum)
{
	struct rtmdio_bus_priv *priv = bus->priv;
	int err, val;

	if (addr >= priv->cpu_port)
		return -ENODEV;

	if (regnum == RTMDIO_PAGE_SELECT && priv->page[addr] != priv->rawpage)
		return priv->page[addr];

	priv->raw[addr] = (priv->page[addr] == priv->rawpage);
	if ((priv->phy_is_internal[addr]) && (priv->sds_id[addr] >= 0))
		return rtmdio_read_sds_phy(priv, priv->sds_id[addr],
					   priv->page[addr], regnum);

	err = (*priv->read_phy)(addr, priv->page[addr], regnum, &val);
	pr_debug("rd_PHY(adr=%d, pag=%d, reg=%d) = %d, err = %d\n",
		 addr, priv->page[addr], regnum, val, err);
	return err ? err : val;
}

static int rtmdio_write_c45(struct mii_bus *bus, int addr, int devnum, int regnum, u16 val)
{
	struct rtmdio_bus_priv *priv = bus->priv;
	int err;

	if (addr >= priv->cpu_port)
		return -ENODEV;

	err = (*priv->write_mmd_phy)(addr, devnum, regnum, val);
	pr_debug("wr_MMD(adr=%d, dev=%d, reg=%d, val=%d) err = %d\n",
		 addr, devnum, regnum, val, err);
	return err;
}

static int rtmdio_write(struct mii_bus *bus, int addr, int regnum, u16 val)
{
	struct rtmdio_bus_priv *priv = bus->priv;
	int err, page;

	if (addr >= priv->cpu_port)
		return -ENODEV;

	page = priv->page[addr];

	if (regnum == RTMDIO_PAGE_SELECT)
		priv->page[addr] = val;

	if (!priv->raw[addr] && (regnum != RTMDIO_PAGE_SELECT || page == priv->rawpage)) {
		priv->raw[addr] = (page == priv->rawpage);
		if (priv->phy_is_internal[addr] && priv->sds_id[addr] >= 0)
			return rtmdio_write_sds_phy(priv, priv->sds_id[addr],
						    priv->page[addr], regnum, val);

		err = (*priv->write_phy)(addr, page, regnum, val);
		pr_debug("wr_PHY(adr=%d, pag=%d, reg=%d, val=%d) err = %d\n",
			 addr, page, regnum, val, err);
		return err;
	}

	priv->raw[addr] = false;
	return 0;
}

static int rtmdio_838x_reset(struct mii_bus *bus)
{
	pr_debug("%s called\n", __func__);
	/* Disable MAC polling the PHY so that we can start configuration */
	sw_w32(0x00000000, RTMDIO_838X_SMI_POLL_CTRL);

	/* Enable PHY control via SoC */
	sw_w32_mask(0, 1 << 15, RTMDIO_838X_SMI_GLB_CTRL);

	/* Probably should reset all PHYs here... */
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

u8 mac_type_bit[RTMDIO_930X_CPU_PORT] = {0, 0, 0, 0, 2, 2, 2, 2, 4, 4, 4, 4, 6, 6, 6, 6,
					 8, 8, 8, 8, 10, 10, 10, 10, 12, 15, 18, 21};

static int rtmdio_930x_reset(struct mii_bus *bus)
{
	struct rtmdio_bus_priv *priv = bus->priv;
	bool uses_usxgmii = false; /* For the Aquantia PHYs */
	bool uses_hisgmii = false; /* For the RTL8221/8226 */
	u32 private_poll_mask = 0;
	u32 poll_sel[2] = { 0 };
	u32 poll_ctrl = 0;
	u32 c45_mask = 0;
	u32 v;

	/* Mapping of port to phy-addresses on an SMI bus */
	for (int i = 0; i < RTMDIO_930X_CPU_PORT; i++) {
		int pos;

		if (priv->smi_bus[i] < 0)
			continue;

		pos = (i % 6) * 5;
		sw_w32_mask(0x1f << pos, priv->smi_addr[i] << pos,
			    RTMDIO_930X_SMI_PORT0_5_ADDR + (i / 6) * 4);

		pos = (i * 2) % 32;
		poll_sel[i / 16] |= priv->smi_bus[i] << pos;
		poll_ctrl |= BIT(20 + priv->smi_bus[i]);
	}

	/* Configure which SMI bus is behind which port number */
	sw_w32(poll_sel[0], RTMDIO_930X_SMI_PORT0_15_POLLING_SEL);
	sw_w32(poll_sel[1], RTMDIO_930X_SMI_PORT16_27_POLLING_SEL);

	/* Disable POLL_SEL for any SMI bus with a normal PHY (not RTL8295R for SFP+) */
	sw_w32_mask(poll_ctrl, 0, RTMDIO_930X_SMI_GLB_CTRL);

	/* Configure which SMI busses are polled in c45 based on a c45 PHY being on that bus */
	for (int i = 0; i < RTMDIO_MAX_SMI_BUS; i++)
		if (priv->smi_bus_isc45[i])
			c45_mask |= BIT(i + 16);

	pr_info("c45_mask: %08x\n", c45_mask);
	sw_w32_mask(GENMASK(19, 16), c45_mask, RTMDIO_930X_SMI_GLB_CTRL);

	/* Set the MAC type of each port according to the PHY-interface */
	/* Values are FE: 2, GE: 3, XGE/2.5G: 0(SERDES) or 1(otherwise), SXGE: 0 */
	v = 0;
	for (int i = 0; i < RTMDIO_930X_CPU_PORT; i++) {
		switch (priv->interfaces[i]) {
		case PHY_INTERFACE_MODE_10GBASER:
			break;			/* Serdes: Value = 0 */
		case PHY_INTERFACE_MODE_USXGMII:
			v |= BIT(mac_type_bit[i]);
			uses_usxgmii = true;
			break;
		case PHY_INTERFACE_MODE_QSGMII:
			private_poll_mask |= BIT(i);
			v |= 3 << mac_type_bit[i];
			break;
		default:
			break;
		}
	}
	sw_w32(v, RTMDIO_930X_SMI_MAC_TYPE_CTRL);

	/* Set the private polling mask for all Realtek PHYs (i.e. not the 10GBit Aquantia ones) */
	sw_w32(private_poll_mask, RTMDIO_930X_SMI_PRVTE_POLLING_CTRL);

	/* The following magic values are found in the port configuration, they seem to
	 * define different ways of polling a PHY. The below is for the Aquantia PHYs of
	 * the XGS1250 and the RTL8226 of the XGS1210
	 */
	if (uses_usxgmii) {
		sw_w32(0x01010000, RTMDIO_930X_SMI_10G_POLLING_REG0_CFG);
		sw_w32(0x01E7C400, RTMDIO_930X_SMI_10G_POLLING_REG9_CFG);
		sw_w32(0x01E7E820, RTMDIO_930X_SMI_10G_POLLING_REG10_CFG);
	}
	if (uses_hisgmii) {
		sw_w32(0x011FA400, RTMDIO_930X_SMI_10G_POLLING_REG0_CFG);
		sw_w32(0x013FA412, RTMDIO_930X_SMI_10G_POLLING_REG9_CFG);
		sw_w32(0x017FA414, RTMDIO_930X_SMI_10G_POLLING_REG10_CFG);
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
	struct rtmdio_bus_priv *priv = bus->priv;
	u32 poll_sel[4] = { 0 };
	u32 poll_ctrl = 0;
	u32 c45_mask = 0;

	pr_info("%s called\n", __func__);
	/* Disable port polling for configuration purposes */
	sw_w32(0, RTMDIO_931X_SMI_PORT_POLLING_CTRL);
	sw_w32(0, RTMDIO_931X_SMI_PORT_POLLING_CTRL + 4);
	msleep(100);

	/* Mapping of port to phy-addresses on an SMI bus */
	for (int i = 0; i < RTMDIO_931X_CPU_PORT; i++) {
		u32 pos;

		if (priv->smi_bus[i] < 0)
			continue;

		pos = (i % 6) * 5;
		sw_w32_mask(0x1f << pos, priv->smi_addr[i] << pos, RTMDIO_931X_SMI_PORT_ADDR + (i / 6) * 4);
		pos = (i * 2) % 32;
		poll_sel[i / 16] |= priv->smi_bus[i] << pos;
		poll_ctrl |= BIT(20 + priv->smi_bus[i]);
	}

	/* Configure which SMI bus is behind which port number */
	for (int i = 0; i < RTMDIO_MAX_SMI_BUS; i++) {
		pr_info("poll sel %d, %08x\n", i, poll_sel[i]);
		sw_w32(poll_sel[i], RTMDIO_931X_SMI_PORT_POLLING_SEL + (i * 4));
	}

	/* Configure which SMI busses */
	pr_info("c45_mask: %08x, RTMDIO_931X_SMI_GLB_CTRL0 was %X", c45_mask, sw_r32(RTMDIO_931X_SMI_GLB_CTRL0));
	for (int i = 0; i < RTMDIO_MAX_SMI_BUS; i++) {
		/* bus is polled in c45 */
		if (priv->smi_bus_isc45[i])
			c45_mask |= 0x2 << (i * 2);  /* Std. C45, non-standard is 0x3 */
	}

	pr_info("c45_mask: %08x, RTL931X_SMI_GLB_CTRL0 was %X", c45_mask, sw_r32(RTMDIO_931X_SMI_GLB_CTRL0));

	/* We have a 10G PHY enable polling
	 * sw_w32(0x01010000, RTL931X_SMI_10GPHY_POLLING_SEL2);
	 * sw_w32(0x01E7C400, RTL931X_SMI_10GPHY_POLLING_SEL3);
	 * sw_w32(0x01E7E820, RTL931X_SMI_10GPHY_POLLING_SEL4);
	 */
	sw_w32_mask(GENMASK(7, 0), c45_mask, RTMDIO_931X_SMI_GLB_CTRL1);

	return 0;
}

/*
 * TODO: This is a tiny leftover from the central SoC include. For now try to detect the
 * Realtek SoC automatically. This needs to be changed to a proper DTS compatible in a
 * future driver version.
 */
static int rtmdio_get_family(void)
{
	unsigned int val;

	val = sw_r32(RTMDIO_93XX_MODEL_NAME_INFO_REG);
	if ((val & 0xfffc0000) == 0x93000000)
		return RTMDIO_930X_FAMILY_ID;
	if ((val & 0xfffc0000) == 0x93100000)
		return RTMDIO_931X_FAMILY_ID;

	val = sw_r32(RTMDIO_839X_MODEL_NAME_INFO_REG);
	if ((val & 0xfff80000) == 0x83900000)
		return RTMDIO_839X_FAMILY_ID;

	return RTMDIO_838X_FAMILY_ID;
}

static int rtmdio_probe(struct platform_device *pdev)
{
	struct device_node *dn, *mii_np, *pcs_node;
	struct device *dev = &pdev->dev;
	struct rtmdio_bus_priv *priv;
	struct mii_bus *bus;
	int i, family;
	u32 pn;

	family = rtmdio_get_family();
	dev_info(dev, "probing RTL%04x family mdio bus\n", family);

	mii_np = of_get_child_by_name(dev->of_node, "mdio-bus");
	if (!mii_np)
		return -ENODEV;

	if (!of_device_is_available(mii_np)) {
		of_node_put(mii_np);
		return -ENODEV;
	}

	bus = devm_mdiobus_alloc_size(dev, sizeof(*priv));
	if (!bus)
		return -ENOMEM;

	priv = bus->priv;
	for (i = 0; i < RTMDIO_MAX_PORT; i++) {
		priv->page[i] = 0;
		priv->raw[i] = false;
	}

	switch (family) {
	case RTMDIO_838X_FAMILY_ID:
		bus->name = "rtl838x-eth-mdio";
		bus->read = rtmdio_read;
		bus->write = rtmdio_write;
		bus->reset = rtmdio_838x_reset;
		priv->read_sds_phy = rtmdio_838x_read_sds_phy;
		priv->write_sds_phy = rtmdio_838x_write_sds_phy;
		priv->read_mmd_phy = rtmdio_838x_read_mmd_phy;
		priv->write_mmd_phy = rtmdio_838x_write_mmd_phy;
		priv->read_phy = rtmdio_838x_read_phy;
		priv->write_phy = rtmdio_838x_write_phy;
		priv->cpu_port = RTMDIO_838X_CPU_PORT;
		priv->rawpage = 0xfff;
		break;
	case RTMDIO_839X_FAMILY_ID:
		bus->name = "rtl839x-eth-mdio";
		bus->read = rtmdio_read;
		bus->write = rtmdio_write;
		bus->reset = rtmdio_839x_reset;
		priv->read_sds_phy = rtmdio_839x_read_sds_phy;
		priv->write_sds_phy = rtmdio_839x_write_sds_phy;
		priv->read_mmd_phy = rtmdio_839x_read_mmd_phy;
		priv->write_mmd_phy = rtmdio_839x_write_mmd_phy;
		priv->read_phy = rtmdio_839x_read_phy;
		priv->write_phy = rtmdio_839x_write_phy;
		priv->cpu_port = RTMDIO_839X_CPU_PORT;
		priv->rawpage = 0x1fff;
		break;
	case RTMDIO_930X_FAMILY_ID:
		bus->name = "rtl930x-eth-mdio";
		bus->read = rtmdio_read;
		bus->write = rtmdio_write;
		bus->reset = rtmdio_930x_reset;
		priv->read_sds_phy = rtmdio_930x_read_sds_phy;
		priv->write_sds_phy = rtmdio_930x_write_sds_phy;
		priv->read_mmd_phy = rtmdio_930x_read_mmd_phy;
		priv->write_mmd_phy = rtmdio_930x_write_mmd_phy;
		priv->read_phy = rtmdio_930x_read_phy;
		priv->write_phy = rtmdio_930x_write_phy;
		priv->cpu_port = RTMDIO_930X_CPU_PORT;
		priv->rawpage = 0xfff;
		break;
	case RTMDIO_931X_FAMILY_ID:
		bus->name = "rtl931x-eth-mdio";
		bus->read = rtmdio_read;
		bus->write = rtmdio_write;
		bus->reset = rtmdio_931x_reset;
		priv->read_sds_phy = rtsds_931x_read;
		priv->write_sds_phy = rtsds_931x_write;
		priv->read_mmd_phy = rtmdio_931x_read_mmd_phy;
		priv->write_mmd_phy = rtmdio_931x_write_mmd_phy;
		priv->read_phy = rtmdio_931x_read_phy;
		priv->write_phy = rtmdio_931x_write_phy;
		priv->cpu_port = RTMDIO_931X_CPU_PORT;
		priv->rawpage = 0x1fff;
		break;
	}
	bus->read_c45 = rtmdio_read_c45;
	bus->write_c45 = rtmdio_write_c45;
	bus->parent = dev;
	bus->phy_mask = ~(BIT_ULL(priv->cpu_port) - 1ULL);

	for_each_node_by_name(dn, "ethernet-phy") {
		u32 smi_addr[2];

		if (of_property_read_u32(dn, "reg", &pn))
			continue;

		if (pn >= RTMDIO_MAX_PORT) {
			pr_err("%s: illegal port number %d\n", __func__, pn);
			return -ENODEV;
		}

		if (of_property_read_u32_array(dn, "rtl9300,smi-address", &smi_addr[0], 2)) {
			priv->smi_bus[pn] = 0;
			priv->smi_addr[pn] = pn;
		} else {
			priv->smi_bus[pn] = smi_addr[0];
			priv->smi_addr[pn] = smi_addr[1];
		}

		if (priv->smi_bus[pn] >= RTMDIO_MAX_SMI_BUS) {
			pr_err("%s: illegal SMI bus number %d\n", __func__, priv->smi_bus[pn]);
			return -ENODEV;
		}

		priv->phy_is_internal[pn] = of_property_read_bool(dn, "phy-is-integrated");

		if (of_device_is_compatible(dn, "ethernet-phy-ieee802.3-c45"))
			priv->smi_bus_isc45[priv->smi_bus[pn]] = true;
	}

	dn = of_find_compatible_node(NULL, NULL, "realtek,rtl83xx-switch");
	if (!dn) {
		dev_err(dev, "No RTL switch node in DTS\n");
		return -ENODEV;
	}

	for_each_node_by_name(dn, "port") {
		if (of_property_read_u32(dn, "reg", &pn))
			continue;
		dev_dbg(dev, "Looking at port %d\n", pn);
		if (pn > priv->cpu_port)
			continue;
		if (of_get_phy_mode(dn, &priv->interfaces[pn]))
			priv->interfaces[pn] = PHY_INTERFACE_MODE_NA;
		dev_dbg(dev, "phy mode of port %d is %s\n", pn, phy_modes(priv->interfaces[pn]));

		/*
		 * TODO: The MDIO driver does not need any info about the SerDes. As long as
		 * the PCS driver cannot completely control the SerDes, look up the information
		 * via the pcs-handle of the switch port node.
		 */

		priv->sds_id[pn] = -1;
		pcs_node = of_parse_phandle(dn, "pcs-handle", 0);
		if (pcs_node)
			of_property_read_u32(pcs_node, "reg", &priv->sds_id[pn]);
		if (priv->phy_is_internal[pn] && priv->sds_id[pn] >= 0)
			priv->smi_bus[pn] = -1;
		if (priv->sds_id[pn] >= 0)
			dev_dbg(dev, "PHY %d has SDS %d\n", pn, priv->sds_id[pn]);
	}

	snprintf(bus->id, MII_BUS_ID_SIZE, "%s-mii", dev_name(dev));

	return devm_of_mdiobus_register(dev, bus, mii_np);
}

static const struct of_device_id rtmdio_ids[] = {
	{ .compatible = "realtek,rtl8380-mdio" },
	{ .compatible = "realtek,rtl8392-mdio" },
	{ .compatible = "realtek,rtl9301-mdio" },
	{ .compatible = "realtek,rtl9311-mdio" },
	{}
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
