// SPDX-License-Identifier: GPL-2.0-only

#include <linux/fwnode.h>
#include <linux/fwnode_mdio.h>
#include <linux/mfd/syscon.h>
#include <linux/of.h>
#include <linux/of_mdio.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/types.h>

#define RTMDIO_MAX_PORTS			57
#define RTMDIO_MAX_SMI_BUSSES			4

#define RTMDIO_838X_NUM_BUSSES			1
#define RTMDIO_838X_NUM_PAGES			4096
#define RTMDIO_839X_NUM_BUSSES			2
#define RTMDIO_839X_NUM_PAGES			8192
#define RTMDIO_930X_NUM_BUSSES			4
#define RTMDIO_930X_NUM_PAGES			4096
#define RTMDIO_931X_NUM_BUSSES			4
#define RTMDIO_931X_NUM_PAGES			8192
#define RTMDIO_PAGE_SELECT			0x1f
#define RTMDIO_RAW_PAGE(p)			((p) - 1)

#define RTMDIO_PHY_AQR113C_A			0x31c31c12
#define RTMDIO_PHY_AQR113C_B			0x31c31c13
#define RTMDIO_PHY_AQR813			0x31c31cb2
#define RTMDIO_PHY_RTL8218D			0x001cc983
#define RTMDIO_PHY_RTL8218E			0x001cc984
#define RTMDIO_PHY_RTL8221B_VB_CG		0x001cc849
#define RTMDIO_PHY_RTL8221B_VM_CG		0x001cc84a
#define RTMDIO_PHY_RTL8224			0x001ccad0
#define RTMDIO_PHY_RTL8226			0x001cc838
#define RTMDIO_PHY_RTL8261			0x001ccaf3

#define RTMDIO_PHY_MAC_1G			3
#define RTMDIO_PHY_MAC_2G_PLUS			1
#define RTMDIO_PHY_MAC_SDS			0

#define RTMDIO_PHY_POLL_MMD(dev, reg, bit)	((((bit) & GENMASK(3, 0)) << 21) | \
						 (((dev) & GENMASK(4, 0)) << 16) | \
						 ((reg) & GENMASK(15, 0)))

/* MDIO bus registers/fields */
#define RTMDIO_C45_DATA(devnum, regnum)		((((devnum) & GENMASK(4, 0)) << 16) | \
						  ((regnum) & GENMASK(15, 0)))
#define RTMDIO_DATA_MASK			GENMASK(15, 0)
#define RTMDIO_RUN				BIT(0)

#define RTMDIO_838X_C22_DATA(page, reg)		((reg) << 20 | RTMDIO_PAGE_SELECT << 15 | (page) << 3)
#define RTMDIO_838X_PHY_PATCH_DONE		BIT(15)
#define RTMDIO_838X_SMI_GLB_CTRL		(0xa100)
#define   RTMDIO_838X_SMI_GLB_PHY_MAN_24_27	BIT(7)
#define RTMDIO_838X_SMI_ACCESS_PHY_CTRL_0	(0xa1b8)
#define RTMDIO_838X_SMI_ACCESS_PHY_CTRL_1	(0xa1bc)
#define   RTMDIO_838X_CMD_FAIL			0 /* No hardware support */
#define   RTMDIO_838X_CMD_READ_C22		0
#define   RTMDIO_838X_CMD_READ_C45		BIT(1)
#define   RTMDIO_838X_CMD_WRITE_C22		BIT(2)
#define   RTMDIO_838X_CMD_WRITE_C45		(BIT(1) | BIT(2))
#define   RTMDIO_838X_CMD_MASK			GENMASK(2, 0)
#define RTMDIO_838X_SMI_ACCESS_PHY_CTRL_2	(0xa1c0)
#define RTMDIO_838X_SMI_POLL_CTRL		(0xa17c)
#define RTMDIO_838X_SMI_PORT0_5_ADDR_CTRL	(0xa1c8)

#define RTMDIO_839X_C22_DATA(page, reg)		((reg) << 5 | (page) << 10 | \
						 (((page) == RTMDIO_RAW_PAGE(RTMDIO_839X_NUM_PAGES)) ? \
						  RTMDIO_PAGE_SELECT : 0) << 23)
#define RTMDIO_839X_PHYREG_ACCESS_CTRL		(0x03DC)
#define   RTMDIO_839X_CMD_FAIL			BIT(1)
#define   RTMDIO_839X_CMD_READ_C22		0
#define   RTMDIO_839X_CMD_READ_C45		BIT(2)
#define   RTMDIO_839X_CMD_WRITE_C22		BIT(3)
#define   RTMDIO_839X_CMD_WRITE_C45		(BIT(2) | BIT(3))
#define   RTMDIO_839X_CMD_MASK			GENMASK(3, 0)
#define RTMDIO_839X_PHYREG_CTRL			(0x03e0)
#define   RTMDIO_839X_PHYREG_SKIP_EXT_PAGE	GENMASK(8, 0)
#define RTMDIO_839X_PHYREG_DATA_CTRL		(0x03F0)
#define RTMDIO_839X_SMI_PORT_POLLING_CTRL	(0x03fc)
#define RTMDIO_839X_SMI_GLB_CTRL		(0x03f8)

#define RTMDIO_930X_C22_DATA(page, reg)		((reg) << 20 | RTMDIO_PAGE_SELECT << 15 | (page) << 3)
#define RTMDIO_930X_SMI_GLB_CTRL		(0xCA00)
#define RTMDIO_930X_SMI_ACCESS_PHY_CTRL_0	(0xCB70)
#define RTMDIO_930X_SMI_ACCESS_PHY_CTRL_1	(0xCB74)
#define   RTMDIO_930X_CMD_FAIL			BIT(25)
#define   RTMDIO_930X_CMD_READ_C22		0
#define   RTMDIO_930X_CMD_READ_C45		BIT(1)
#define   RTMDIO_930X_CMD_WRITE_C22		BIT(2)
#define   RTMDIO_930X_CMD_WRITE_C45		(BIT(1) | BIT(2))
#define   RTMDIO_930X_CMD_MASK			(GENMASK(2, 0) | BIT(25))
#define RTMDIO_930X_SMI_ACCESS_PHY_CTRL_2	(0xCB78)
#define RTMDIO_930X_SMI_PORT0_15_POLLING_SEL	(0xCA08)
#define RTMDIO_930X_SMI_PORT16_27_POLLING_SEL	(0xCA0C)
#define RTMDIO_930X_SMI_MAC_TYPE_CTRL		(0xCA04)
#define RTMDIO_930X_SMI_PRVTE_POLLING_CTRL	(0xCA10)
#define RTMDIO_930X_SMI_10G_POLLING_REG0_CFG	(0xCBB4)
#define RTMDIO_930X_SMI_10G_POLLING_REG9_CFG	(0xCBB8)
#define RTMDIO_930X_SMI_10G_POLLING_REG10_CFG	(0xCBBC)
#define RTMDIO_930X_SMI_PORT0_5_ADDR_CTRL	(0xCB80)

#define RTMDIO_931X_C22_DATA(page, reg)		((reg) << 6 | (page) << 11)
#define RTMDIO_931X_SMI_PORT_POLLING_CTRL	(0x0CCC)
#define RTMDIO_931X_SMI_INDRT_ACCESS_BC_CTRL	(0x0C14)
#define RTMDIO_931X_SMI_GLB_CTRL0		(0x0CC0)
#define RTMDIO_931X_SMI_GLB_CTRL1		(0x0CBC)
#define RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_0	(0x0C00)
#define   RTMDIO_931X_CMD_FAIL			BIT(1)
#define   RTMDIO_931X_CMD_READ_C22		0
#define   RTMDIO_931X_CMD_READ_C45		BIT(3)
#define   RTMDIO_931X_CMD_WRITE_C22		BIT(4)
#define   RTMDIO_931X_CMD_WRITE_C45		(BIT(3) | BIT(4))
#define   RTMDIO_931X_CMD_MASK			GENMASK(4, 0)
#define RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_3	(0x0C10)
#define RTMDIO_931X_SMI_PHY_ABLTY_GET_SEL	(0x0CAC)
#define   RTMDIO_931X_SMI_PHY_ABLTY_MDIO	0x0
#define   RTMDIO_931X_SMI_PHY_ABLTY_SDS		0x2
#define RTMDIO_931X_SMI_PORT_POLLING_SEL	(0x0C9C)
#define RTMDIO_931X_SMI_PORT_ADDR_CTRL		(0x0C74)
#define RTMDIO_931X_SMI_10GPHY_POLLING_SEL0	(0x0CF0)
#define RTMDIO_931X_SMI_10GPHY_POLLING_SEL1	(0x0CF4)
#define RTMDIO_931X_SMI_10GPHY_POLLING_SEL2	(0x0CF8)
#define RTMDIO_931X_SMI_10GPHY_POLLING_SEL3	(0x0CFC)
#define RTMDIO_931X_SMI_10GPHY_POLLING_SEL4	(0x0D00)

#define for_each_port(ctrl, pn) \
	for_each_set_bit(pn, (ctrl)->valid_ports, RTMDIO_MAX_PORTS)

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

struct rtmdio_port {
	int page;
	bool raw;
	u8 smi_addr;
	u8 smi_bus;
};

struct rtmdio_bus {
	bool is_c45;
	struct mii_bus *mii_bus;
};

struct rtmdio_ctrl {
	struct mutex lock;
	struct regmap *map;
	const struct rtmdio_config *cfg;
	struct rtmdio_port port[RTMDIO_MAX_PORTS];
	struct rtmdio_bus bus[RTMDIO_MAX_SMI_BUSSES];
	DECLARE_BITMAP(valid_ports, RTMDIO_MAX_PORTS);
};

struct rtmdio_chan {
	struct rtmdio_ctrl *ctrl;
	u8 smi_bus;
};

struct rtmdio_config {
	u32 cmd_fail;
	u32 cmd_mask;
	u32 cmd_reg;
	int bus_map_base;
	u16 num_busses;
	u16 num_pages;
	u16 num_phys;
	int port_map_base;
	int (*read_c22)(struct mii_bus *bus, u32 pn, u32 page, u32 reg, u32 *val);
	int (*read_c45)(struct mii_bus *bus, u32 pn, u32 devnum, u32 regnum, u32 *val);
	u32 ret_mask;
	u32 ret_reg;
	int (*setup_ctrl)(struct rtmdio_ctrl *ctrl);
	void (*setup_polling)(struct rtmdio_ctrl *ctrl);
	u32 smi_base;
	u32 smi_size;
	int (*write_c22)(struct mii_bus *bus, u32 pn, u32 page, u32 reg, u32 val);
	int (*write_c45)(struct mii_bus *bus, u32 pn, u32 devnum, u32 regnum, u32 val);
};

struct rtmdio_phy_info {
	int mac_type;
	bool has_giga_lite;
	bool has_res_reg;
	bool force_res;
	unsigned int poll_duplex;
	unsigned int poll_adv_1000;
	unsigned int poll_lpa_1000;
};

struct rtmdio_838x_smi_access {
	u32 ctrl_0;
	u32 ctrl_1;
	u32 ctrl_2;
	u32 ctrl_3;
};

struct rtmdio_839x_smi_access {
	u32 accs_ctrl;
	u32 main_ctrl;
	u32 prt0_ctrl;
	u32 prt1_ctrl;
	u32 bcst_ctrl;
	u32 data_ctrl;
	u32 mmd0_ctrl;
};

struct rtmdio_930x_smi_access {
	u32 ctrl_0;
	u32 ctrl_1;
	u32 ctrl_2;
	u32 ctrl_3;
};

struct rtmdio_931x_smi_access {
	u32 ctrl_0;
	u32 ctrl_1;
	u32 ctrl_2;
	u32 ctrl_x;
	u32 ctrl_3;
	u32 b_ctrl;
	u32 m_ctrl;
};

static inline struct rtmdio_ctrl *rtmdio_ctrl_from_bus(struct mii_bus *bus)
{
	return ((struct rtmdio_chan *)bus->priv)->ctrl;
}

static int rtmdio_phy_to_port(struct mii_bus *bus, int phy)
{
	struct rtmdio_chan *chan = bus->priv;
	struct rtmdio_ctrl *ctrl = chan->ctrl;
	int pn;

	for_each_port(ctrl, pn)
		if (ctrl->port[pn].smi_bus == chan->smi_bus && ctrl->port[pn].smi_addr == phy)
			return pn;

	return -ENOENT;
}

static int rtmdio_run_cmd(struct mii_bus *bus, int cmd, void *smi_access, u32 *val)
{
	struct rtmdio_ctrl *ctrl = rtmdio_ctrl_from_bus(bus);
	int ret, cmdstate;

	ret = regmap_bulk_write(ctrl->map, ctrl->cfg->smi_base,
				smi_access, ctrl->cfg->smi_size / sizeof(u32));
	if (ret)
		return ret;

	/* Use masks as C22 data and command bits share the same register. */
	ret = regmap_update_bits(ctrl->map, ctrl->cfg->cmd_reg,
				 ctrl->cfg->cmd_mask, cmd | RTMDIO_RUN);
	if (ret)
		return ret;

	ret = regmap_read_poll_timeout(ctrl->map, ctrl->cfg->cmd_reg,
				       cmdstate, !(cmdstate & RTMDIO_RUN), 20, 500000);
	if (ret) {
		dev_warn_once(&bus->dev, "access timed out\n");
		return ret;
	}

	if (cmdstate & ctrl->cfg->cmd_fail) {
		dev_warn_once(&bus->dev, "access failed\n");
		return -EIO;
	}

	if (!val)
		return 0;

	ret = regmap_read(ctrl->map, ctrl->cfg->ret_reg, val);
	if (!ret)
		*val = (*val & ctrl->cfg->ret_mask) >> __ffs(ctrl->cfg->ret_mask);

	return ret;
}

static int rtmdio_838x_read_c22(struct mii_bus *bus, u32 pn, u32 page, u32 reg, u32 *val)
{
	struct rtmdio_838x_smi_access smi_access = {
		.ctrl_0 = BIT(pn),
		.ctrl_1 = RTMDIO_838X_C22_DATA(page, reg),
		.ctrl_2 = pn << 16,
	};

	return rtmdio_run_cmd(bus, RTMDIO_838X_CMD_READ_C22, &smi_access, val);
}

static int rtmdio_838x_write_c22(struct mii_bus *bus, u32 pn, u32 page, u32 reg, u32 val)
{
	struct rtmdio_838x_smi_access smi_access = {
		.ctrl_0 = BIT(pn),
		.ctrl_1 = RTMDIO_838X_C22_DATA(page, reg),
		.ctrl_2 = val << 16,
	};

	return rtmdio_run_cmd(bus, RTMDIO_838X_CMD_WRITE_C22, &smi_access, NULL);
}

static int rtmdio_838x_read_c45(struct mii_bus *bus, u32 pn, u32 devnum, u32 regnum, u32 *val)
{
	struct rtmdio_838x_smi_access smi_access = {
		.ctrl_0 = BIT(pn),
		.ctrl_2 = pn << 16,
		.ctrl_3 = RTMDIO_C45_DATA(devnum, regnum),
	};

	return rtmdio_run_cmd(bus, RTMDIO_838X_CMD_READ_C45, &smi_access, val);
}

static int rtmdio_838x_write_c45(struct mii_bus *bus, u32 pn, u32 devnum, u32 regnum, u32 val)
{
	struct rtmdio_838x_smi_access smi_access = {
		.ctrl_0 = BIT(pn),
		.ctrl_2 = val << 16,
		.ctrl_3 = RTMDIO_C45_DATA(devnum, regnum),
	};

	return rtmdio_run_cmd(bus, RTMDIO_838X_CMD_WRITE_C45, &smi_access, NULL);
}

static int rtmdio_839x_read_c22(struct mii_bus *bus, u32 pn, u32 page, u32 reg, u32 *val)
{
	struct rtmdio_839x_smi_access smi_access = {
		.main_ctrl = RTMDIO_839X_PHYREG_SKIP_EXT_PAGE,
		.data_ctrl = pn << 16,
		.accs_ctrl = RTMDIO_839X_C22_DATA(page, reg),
	};

	return rtmdio_run_cmd(bus, RTMDIO_839X_CMD_READ_C22, &smi_access, val);
}

static int rtmdio_839x_write_c22(struct mii_bus *bus, u32 pn, u32 page, u32 reg, u32 val)
{
	struct rtmdio_839x_smi_access smi_access = {
		.main_ctrl = RTMDIO_839X_PHYREG_SKIP_EXT_PAGE,
		.data_ctrl = val << 16,
		.prt0_ctrl = (u32)(BIT_ULL(pn)),
		.prt1_ctrl = (u32)(BIT_ULL(pn) >> 32),
		.accs_ctrl = RTMDIO_839X_C22_DATA(page, reg),
	};

	return rtmdio_run_cmd(bus, RTMDIO_839X_CMD_WRITE_C22, &smi_access, NULL);
}

static int rtmdio_839x_read_c45(struct mii_bus *bus, u32 pn, u32 devnum, u32 regnum, u32 *val)
{
	struct rtmdio_839x_smi_access smi_access = {
		.data_ctrl = pn << 16,
		.mmd0_ctrl = RTMDIO_C45_DATA(devnum, regnum),
	};

	return rtmdio_run_cmd(bus, RTMDIO_839X_CMD_READ_C45, &smi_access, val);
}

static int rtmdio_839x_write_c45(struct mii_bus *bus, u32 pn, u32 devnum, u32 regnum, u32 val)
{
	struct rtmdio_839x_smi_access smi_access = {
		.data_ctrl = val << 16,
		.prt0_ctrl = (u32)(BIT_ULL(pn)),
		.prt1_ctrl = (u32)(BIT_ULL(pn) >> 32),
		.mmd0_ctrl = RTMDIO_C45_DATA(devnum, regnum),
	};

	return rtmdio_run_cmd(bus, RTMDIO_839X_CMD_WRITE_C45, &smi_access, NULL);
}

static int rtmdio_930x_read_c22(struct mii_bus *bus, u32 pn, u32 page, u32 reg, u32 *val)
{
	struct rtmdio_930x_smi_access smi_access = {
		.ctrl_1 = RTMDIO_930X_C22_DATA(page, reg),
		.ctrl_2 = pn << 16,
	};

	return rtmdio_run_cmd(bus, RTMDIO_930X_CMD_READ_C22, &smi_access, val);
}

static int rtmdio_930x_write_c22(struct mii_bus *bus, u32 pn, u32 page, u32 reg, u32 val)
{
	struct rtmdio_930x_smi_access smi_access = {
		.ctrl_0 = BIT(pn),
		.ctrl_1 = RTMDIO_930X_C22_DATA(page, reg),
		.ctrl_2 = val << 16,
	};

	return rtmdio_run_cmd(bus, RTMDIO_930X_CMD_WRITE_C22, &smi_access, NULL);
}

static int rtmdio_930x_read_c45(struct mii_bus *bus, u32 pn, u32 devnum, u32 regnum, u32 *val)
{
	struct rtmdio_930x_smi_access smi_access = {
		.ctrl_2 = pn << 16,
		.ctrl_3 = RTMDIO_C45_DATA(devnum, regnum),
	};

	return rtmdio_run_cmd(bus, RTMDIO_930X_CMD_READ_C45, &smi_access, val);
}

static int rtmdio_930x_write_c45(struct mii_bus *bus, u32 pn, u32 devnum, u32 regnum, u32 val)
{
	struct rtmdio_930x_smi_access smi_access = {
		.ctrl_0 = BIT(pn),
		.ctrl_2 = val << 16,
		.ctrl_3 = RTMDIO_C45_DATA(devnum, regnum),
	};

	return rtmdio_run_cmd(bus, RTMDIO_930X_CMD_WRITE_C45, &smi_access, NULL);
}

static int rtmdio_931x_read_c22(struct mii_bus *bus, u32 pn, u32 page, u32 reg, u32 *val)
{
	struct rtmdio_931x_smi_access smi_access = {
		.ctrl_0 = RTMDIO_931X_C22_DATA(page, reg),
		.b_ctrl = pn << 5,
	};

	return rtmdio_run_cmd(bus, RTMDIO_931X_CMD_READ_C22, &smi_access, val);
}

static int rtmdio_931x_write_c22(struct mii_bus *bus, u32 pn, u32 page, u32 reg, u32 val)
{
	struct rtmdio_931x_smi_access smi_access = {
		.ctrl_0 = RTMDIO_931X_C22_DATA(page, reg),
		.ctrl_2 = (u32)(BIT_ULL(pn)),
		.ctrl_x = (u32)(BIT_ULL(pn) >> 32),
		.ctrl_3 = val,
	};

	return rtmdio_run_cmd(bus, RTMDIO_931X_CMD_WRITE_C22, &smi_access, NULL);
}

static int rtmdio_931x_read_c45(struct mii_bus *bus, u32 pn, u32 devnum, u32 regnum, u32 *val)
{
	struct rtmdio_931x_smi_access smi_access = {
		.b_ctrl = pn << 5,
		.m_ctrl = RTMDIO_C45_DATA(devnum, regnum),
	};

	return rtmdio_run_cmd(bus, RTMDIO_931X_CMD_READ_C45, &smi_access, val);
}

static int rtmdio_931x_write_c45(struct mii_bus *bus, u32 pn, u32 devnum, u32 regnum, u32 val)
{
	struct rtmdio_931x_smi_access smi_access = {
		.ctrl_2 = (u32)(BIT_ULL(pn)),
		.ctrl_x = (u32)(BIT_ULL(pn) >> 32),
		.ctrl_3 = val,
		.m_ctrl = RTMDIO_C45_DATA(devnum, regnum),
	};

	return rtmdio_run_cmd(bus, RTMDIO_931X_CMD_WRITE_C45, &smi_access, NULL);
}

static int rtmdio_read_c45(struct mii_bus *bus, int phy, int devnum, int regnum)
{
	struct rtmdio_ctrl *ctrl = rtmdio_ctrl_from_bus(bus);
	int ret, pn, val = 0;

	pn = rtmdio_phy_to_port(bus, phy);
	if (pn < 0)
		return pn;

	guard(mutex)(&ctrl->lock);
	ret = (*ctrl->cfg->read_c45)(bus, pn, devnum, regnum, &val);
	dev_dbg(&bus->dev, "rd_MMD(phy=0x%02x, dev=0x%04x, reg=0x%04x) = 0x%04x, ret = %d\n",
		phy, devnum, regnum, val, ret);

	return ret ? ret : val;
}

static int rtmdio_read_c22(struct mii_bus *bus, int phy, int regnum)
{
	struct rtmdio_ctrl *ctrl = rtmdio_ctrl_from_bus(bus);
	int ret, pn, val = 0;

	pn = rtmdio_phy_to_port(bus, phy);
	if (pn < 0)
		return pn;

	guard(mutex)(&ctrl->lock);
	if (regnum == RTMDIO_PAGE_SELECT &&
	    ctrl->port[pn].page != RTMDIO_RAW_PAGE(ctrl->cfg->num_pages))
		return ctrl->port[pn].page;

	ctrl->port[pn].raw = (ctrl->port[pn].page == RTMDIO_RAW_PAGE(ctrl->cfg->num_pages));

	ret = (*ctrl->cfg->read_c22)(bus, pn, ctrl->port[pn].page, regnum, &val);
	dev_dbg(&bus->dev, "rd_PHY(phy=0x%02x, pag=0x%04x, reg=0x%04x) = 0x%04x, ret = %d\n",
		phy, ctrl->port[pn].page, regnum, val, ret);

	return ret ? ret : val;
}

static int rtmdio_write_c45(struct mii_bus *bus, int phy, int devnum, int regnum, u16 val)
{
	struct rtmdio_ctrl *ctrl = rtmdio_ctrl_from_bus(bus);
	int ret, pn;

	pn = rtmdio_phy_to_port(bus, phy);
	if (pn < 0)
		return pn;

	guard(mutex)(&ctrl->lock);
	ret = (*ctrl->cfg->write_c45)(bus, pn, devnum, regnum, val);
	dev_dbg(&bus->dev, "wr_MMD(phy=0x%02x, dev=0x%04x, reg=0x%04x, val=0x%04x), ret = %d\n",
		phy, devnum, regnum, val, ret);

	return ret;
}

static int rtmdio_write_c22(struct mii_bus *bus, int phy, int regnum, u16 val)
{
	struct rtmdio_ctrl *ctrl = rtmdio_ctrl_from_bus(bus);
	int ret, page, pn;

	pn = rtmdio_phy_to_port(bus, phy);
	if (pn < 0)
		return pn;

	guard(mutex)(&ctrl->lock);
	page = ctrl->port[pn].page;

	if (regnum == RTMDIO_PAGE_SELECT)
		ctrl->port[pn].page = val;

	if (!ctrl->port[pn].raw &&
	    (regnum != RTMDIO_PAGE_SELECT || page == RTMDIO_RAW_PAGE(ctrl->cfg->num_pages))) {
		ctrl->port[pn].raw = (page == RTMDIO_RAW_PAGE(ctrl->cfg->num_pages));

		ret = (*ctrl->cfg->write_c22)(bus, pn, page, regnum, val);
		dev_dbg(&bus->dev,
			"wr_PHY(phy=0x%02x, pag=0x%04x, reg=0x%04x, val=0x%04x), ret = %d\n",
			phy, page, regnum, val, ret);
		return ret;
	}

	ctrl->port[pn].raw = false;

	return 0;
}

static int rtmdio_setup_smi_topology(struct rtmdio_ctrl *ctrl)
{
	u32 reg, mask, val, pn;
	int ret;

	for_each_port(ctrl, pn) {
		if (ctrl->cfg->bus_map_base) {
			reg = ctrl->cfg->bus_map_base + (pn / 16) * 4;
			mask = GENMASK(1, 0) << ((pn % 16) * 2);
			val = (u32)ctrl->port[pn].smi_bus << __ffs(mask);
			ret = regmap_update_bits(ctrl->map, reg, mask, val);
			if (ret)
				return ret;
		}

		if (ctrl->cfg->port_map_base) {
			reg = ctrl->cfg->port_map_base + (pn / 6) * 4;
			mask = GENMASK(4, 0) << ((pn % 6) * 5);
			val = (u32)ctrl->port[pn].smi_addr << __ffs(mask);
			ret = regmap_update_bits(ctrl->map, reg, mask, val);
			if (ret)
				return ret;
		}
	}

	return 0;
}

static u32 rtmdio_get_phy_id(struct phy_device *phydev)
{
	if (!phydev)
		return 0;

	if (phydev->is_c45) {
		for (int devad = 0; devad < MDIO_MMD_NUM; devad++) {
			u32 phyid = phydev->c45_ids.device_ids[devad];

			if (phyid && phyid != 0xffffffff)
				return phyid;
		}
	}

	return phydev->phy_id;
}

static int rtmdio_get_phy_info(struct rtmdio_ctrl *ctrl, int pn, struct rtmdio_phy_info *phyinfo)
{
	struct mii_bus *bus = ctrl->bus[ctrl->port[pn].smi_bus].mii_bus;
	int addr = ctrl->port[pn].smi_addr;
	int ret = 0;
	u32 phyid;

	/*
	 * Depending on the attached PHY the polling mechanism must be fine tuned. Basically
	 * this boils down to which registers must be read and if there are any special
	 * features.
	 */
	memset(phyinfo, 0, sizeof(*phyinfo));
	phyid = rtmdio_get_phy_id(mdiobus_get_phy(bus, addr));

	switch (phyid) {
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
	case RTMDIO_PHY_RTL8261:
		phyinfo->mac_type = RTMDIO_PHY_MAC_2G_PLUS;
		phyinfo->has_giga_lite = true;
		phyinfo->has_res_reg = true;
		break;
	default:
		dev_warn(&bus->dev, "skip polling setup for phy 0x%08x on port %d\n", phyid, pn);
		ret = -EINVAL;
		break;
	}

	return ret;
}

static int rtmdio_838x_setup_ctrl(struct rtmdio_ctrl *ctrl)
{
	/*
	 * PHY_PATCH_DONE enables phy control via SoC. This is required for phy access,
	 * including patching. Must always be set before the phys are probed.
	 */
	return regmap_set_bits(ctrl->map, RTMDIO_838X_SMI_GLB_CTRL, RTMDIO_838X_PHY_PATCH_DONE);
}

static void rtmdio_838x_setup_polling(struct rtmdio_ctrl *ctrl)
{
	/* Disable MAC polling for PHY config. It will be activated later in the DSA driver */
	regmap_write(ctrl->map, RTMDIO_838X_SMI_POLL_CTRL, 0);

	/*
	 * Control bits EX_PHY_MAN_xxx have an important effect on the detection of the media
	 * status (fibre/copper) of a PHY. Once activated, register MAC_LINK_MEDIA_STS can
	 * give the real media status (0=copper, 1=fibre). For now assume that if address 24 is
	 * PHY driven, it must be a combo PHY and media detection is needed.
	 */
	regmap_assign_bits(ctrl->map, RTMDIO_838X_SMI_GLB_CTRL,
			   RTMDIO_838X_SMI_GLB_PHY_MAN_24_27,
			   test_bit(24, ctrl->valid_ports));
}

static int rtmdio_839x_setup_ctrl(struct rtmdio_ctrl *ctrl)
{
	return 0;

	pr_debug("%s called\n", __func__);
	/* BUG: The following does not work, but should! */
	/* Disable MAC polling the PHY so that we can start configuration */
	regmap_write(ctrl->map, RTMDIO_839X_SMI_PORT_POLLING_CTRL, 0);
	regmap_write(ctrl->map, RTMDIO_839X_SMI_PORT_POLLING_CTRL + 4, 0);
	/* Disable PHY polling via SoC */
	regmap_clear_bits(ctrl->map, RTMDIO_839X_SMI_GLB_CTRL, BIT(7));

	/* Probably should reset all PHYs here... */
	return 0;
}

static int rtmdio_930x_setup_ctrl(struct rtmdio_ctrl *ctrl)
{
	int ret;

	/* Define C22/C45 bus feature set */
	for (int smi_bus = 0; smi_bus < ctrl->cfg->num_busses; smi_bus++) {
		ret = regmap_assign_bits(ctrl->map, RTMDIO_930X_SMI_GLB_CTRL,
					 BIT(16 + smi_bus), ctrl->bus[smi_bus].is_c45);
		if (ret)
			return ret;
	}

	return 0;
}

static int rtmdio_930x_set_port_ability(struct rtmdio_ctrl *ctrl, u32 pn, u32 ability)
{
	u32 mask, val;

	if (pn >= ctrl->cfg->num_phys)
		return -EINVAL;
	/*
	 * Hardware accepts only register values 0-3 but uses 2 types of fields. Ports 0-23 are
	 * grouped by 4 with 2 field bits. Ports 24-27 are handled individually with 3 field bits.
	 */
	mask = pn < 24 ? GENMASK(1, 0) << ((pn / 4) * 2) : GENMASK(2, 0) << ((pn - 24) * 3 + 12);
	val = ability << __ffs(mask);

	return regmap_update_bits(ctrl->map, RTMDIO_930X_SMI_MAC_TYPE_CTRL, mask, val);
}

static void rtmdio_930x_setup_polling(struct rtmdio_ctrl *ctrl)
{
	struct rtmdio_phy_info phyinfo;
	unsigned int pn;

	/* set all ports to "SerDes driven" */
	for (pn = 0; pn < ctrl->cfg->num_phys; pn++)
		rtmdio_930x_set_port_ability(ctrl, pn, RTMDIO_PHY_MAC_SDS);

	/* Define PHY specific polling parameters */
	for_each_port(ctrl, pn) {
		if (rtmdio_get_phy_info(ctrl, pn, &phyinfo))
			continue;

		/* set port to "PHY driven" */
		rtmdio_930x_set_port_ability(ctrl, pn, phyinfo.mac_type);

		/* polling via standard or resolution register */
		regmap_assign_bits(ctrl->map, RTMDIO_930X_SMI_GLB_CTRL,
				   BIT(20 + ctrl->port[pn].smi_bus), phyinfo.has_res_reg);

		/* proprietary Realtek 1G/2.5 lite polling */
		regmap_assign_bits(ctrl->map, RTMDIO_930X_SMI_PRVTE_POLLING_CTRL,
				   BIT(pn), phyinfo.has_giga_lite);

		/* Unique 10G polling setup enforced by hardware design. Always same 10G PHYs. */
		if (phyinfo.poll_duplex || phyinfo.poll_adv_1000 || phyinfo.poll_lpa_1000) {
			regmap_write(ctrl->map, RTMDIO_930X_SMI_10G_POLLING_REG0_CFG, phyinfo.poll_duplex);
			regmap_write(ctrl->map, RTMDIO_930X_SMI_10G_POLLING_REG9_CFG, phyinfo.poll_adv_1000);
			regmap_write(ctrl->map, RTMDIO_930X_SMI_10G_POLLING_REG10_CFG, phyinfo.poll_lpa_1000);
		}
	}
}

static int rtmdio_931x_setup_ctrl(struct rtmdio_ctrl *ctrl)
{
	int ret;

	/* Disable polling for configuration purposes */
	regmap_write(ctrl->map, RTMDIO_931X_SMI_PORT_POLLING_CTRL, 0);
	regmap_write(ctrl->map, RTMDIO_931X_SMI_PORT_POLLING_CTRL + 4, 0);
	msleep(100);

	/* Define C22/C45 bus feature set (bit 1 of SMI_SETx_FMT_SEL) */
	for (int smi_bus = 0; smi_bus < ctrl->cfg->num_busses; smi_bus++) {
		ret = regmap_assign_bits(ctrl->map, RTMDIO_931X_SMI_GLB_CTRL1,
					 BIT(smi_bus * 2 + 1), ctrl->bus[smi_bus].is_c45);
		if (ret)
			return ret;
	}

	return 0;
}

static int rtmdio_931x_set_port_ability(struct rtmdio_ctrl *ctrl, u32 pn, u32 ability)
{
	u32 mask, val, reg;

	if (pn >= ctrl->cfg->num_phys)
		return -EINVAL;

	reg = RTMDIO_931X_SMI_PHY_ABLTY_GET_SEL + (pn / 16) * 4;
	mask = GENMASK(1, 0) << ((pn % 16) * 2);
	val = ability << __ffs(mask);

	return regmap_update_bits(ctrl->map, reg, mask, val);
}

static void rtmdio_931x_setup_polling(struct rtmdio_ctrl *ctrl)
{
	struct rtmdio_phy_info phyinfo;
	u32 pn;

	/* set all ports to "SerDes driven" */
	for (pn = 0; pn < ctrl->cfg->num_phys; pn++)
		rtmdio_931x_set_port_ability(ctrl, pn, RTMDIO_931X_SMI_PHY_ABLTY_SDS);

	/* Define PHY specific polling parameters */
	for_each_port(ctrl, pn) {
		u8 smi_bus = ctrl->port[pn].smi_bus;

		if (rtmdio_get_phy_info(ctrl, pn, &phyinfo))
			continue;

		/* set port to "PHY driven" */
		rtmdio_931x_set_port_ability(ctrl, pn, RTMDIO_931X_SMI_PHY_ABLTY_MDIO);

		/* PRVTE0 polling */
		regmap_assign_bits(ctrl->map, RTMDIO_931X_SMI_GLB_CTRL0,
				   BIT(20 + smi_bus), phyinfo.has_res_reg);
		/* PRVTE1 polling */
		regmap_assign_bits(ctrl->map, RTMDIO_931X_SMI_GLB_CTRL0,
				   BIT(24 + smi_bus), phyinfo.force_res);

		/* polling std. or proprietary format (bit 0 of SMI_SETx_FMT_SEL) */
		regmap_assign_bits(ctrl->map, RTMDIO_931X_SMI_GLB_CTRL1,
				   BIT(smi_bus * 2), phyinfo.force_res);

		/* Unique 10G polling setup enforced by hardware design. Always same 10G PHYs. */
		if (phyinfo.poll_duplex || phyinfo.poll_adv_1000 || phyinfo.poll_lpa_1000) {
			regmap_write(ctrl->map, RTMDIO_931X_SMI_10GPHY_POLLING_SEL2, phyinfo.poll_duplex);
			regmap_write(ctrl->map, RTMDIO_931X_SMI_10GPHY_POLLING_SEL3, phyinfo.poll_adv_1000);
			regmap_write(ctrl->map, RTMDIO_931X_SMI_10GPHY_POLLING_SEL4, phyinfo.poll_lpa_1000);
		}
	}
}

static int rtmdio_map_ports(struct device *dev)
{
	struct rtmdio_ctrl *ctrl = dev_get_drvdata(dev);
	int smi_bus, smi_addr, pn;

	struct device_node *switch_node __free(device_node) =
		of_get_child_by_name(dev->of_node->parent, "ethernet-switch");
	if (!switch_node)
		return dev_err_probe(dev, -ENODEV, "%pfwP missing ethernet-switch\n",
				     of_fwnode_handle(dev->of_node->parent));

	struct device_node *ports __free(device_node) =
		of_get_child_by_name(switch_node, "ethernet-ports");
	if (!ports)
		return dev_err_probe(dev, -ENODEV, "%pfwP missing ethernet-ports\n",
				     of_fwnode_handle(switch_node));

	for_each_child_of_node_scoped(ports, port) {
		if (of_property_read_u32(port, "reg", &pn))
			continue;

		struct device_node *phy __free(device_node) =
			of_parse_phandle(port, "phy-handle", 0);
		if (!phy)
			continue;

		if (pn >= ctrl->cfg->num_phys)
			return dev_err_probe(dev, -EINVAL, "%pfwP illegal port number\n",
					     of_fwnode_handle(port));

		if (test_bit(pn, ctrl->valid_ports))
			return dev_err_probe(dev, -EINVAL, "%pfwP duplicated port number\n",
					     of_fwnode_handle(port));

		if (of_property_read_u32(phy, "reg", &smi_addr))
			return dev_err_probe(dev, -EINVAL, "%pfwP no phy address\n",
					     of_fwnode_handle(phy));

		if (smi_addr >= PHY_MAX_ADDR)
			return dev_err_probe(dev, -EINVAL, "%pfwP illegal phy address\n",
					     of_fwnode_handle(phy));

		if (of_property_read_u32(phy->parent, "reg", &smi_bus))
			return dev_err_probe(dev, -EINVAL, "%pfwP no bus address\n",
					     of_fwnode_handle(phy->parent));

		if (smi_bus >= ctrl->cfg->num_busses)
			return dev_err_probe(dev, -EINVAL, "%pfwP illegal bus number\n",
					     of_fwnode_handle(phy->parent));

		if (of_device_is_compatible(phy, "ethernet-phy-ieee802.3-c45"))
			ctrl->bus[smi_bus].is_c45 = true;

		ctrl->port[pn].smi_bus = smi_bus;
		ctrl->port[pn].smi_addr = smi_addr;
		__set_bit(pn, ctrl->valid_ports);
	}

	return 0;
}

static int rtmdio_probe_one(struct device *dev, struct rtmdio_ctrl *ctrl,
			    struct fwnode_handle *node)
{
	struct rtmdio_chan *chan;
	struct mii_bus *bus;
	int smi_bus, ret;

	ret = fwnode_property_read_u32(node, "reg", &smi_bus);
	if (ret)
		return dev_err_probe(dev, ret, "%pfwP missing reg property for MDIO bus\n", node);
	if (smi_bus >= ctrl->cfg->num_busses)
		return dev_err_probe(dev, -EINVAL, "%pfwP wrong bus index %d\n", node, smi_bus);

	bus = devm_mdiobus_alloc_size(dev, sizeof(*chan));
	if (!bus)
		return -ENOMEM;

	ctrl->bus[smi_bus].mii_bus = bus;

	chan = bus->priv;
	chan->ctrl = ctrl;
	chan->smi_bus = smi_bus;

	bus->name = "Realtek MDIO bus";
	bus->read = rtmdio_read_c22;
	bus->write = rtmdio_write_c22;
	bus->read_c45 = rtmdio_read_c45;
	bus->write_c45 = rtmdio_write_c45;
	bus->parent = dev;
	snprintf(bus->id, MII_BUS_ID_SIZE, "realtek-mdio-%d", smi_bus);

	ret = devm_of_mdiobus_register(dev, bus, to_of_node(node));
	if (ret)
		return dev_err_probe(dev, ret, "cannot register MDIO %d bus\n", smi_bus);

	return 0;
}

static int rtmdio_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct rtmdio_ctrl *ctrl;
	int ret;

	ctrl = devm_kzalloc(dev, sizeof(*ctrl), GFP_KERNEL);
	if (!ctrl)
		return -ENOMEM;

	ret = devm_mutex_init(dev, &ctrl->lock);
	if (ret)
		return ret;

	platform_set_drvdata(pdev, ctrl);
	ctrl->cfg = (const struct rtmdio_config *)device_get_match_data(dev);
	ctrl->map = syscon_node_to_regmap(pdev->dev.of_node->parent);
	if (IS_ERR(ctrl->map))
		return PTR_ERR(ctrl->map);

	ret = rtmdio_map_ports(dev);
	if (ret)
		return ret;

	ret = rtmdio_setup_smi_topology(ctrl);
	if (ret)
		return ret;

	ret = ctrl->cfg->setup_ctrl(ctrl);
	if (ret)
		return ret;

	device_for_each_child_node_scoped(dev, child) {
		ret = rtmdio_probe_one(dev, ctrl, child);
		if (ret)
			return ret;
	}

	if (ctrl->cfg->setup_polling)
		ctrl->cfg->setup_polling(ctrl);

	return 0;
}

static const struct rtmdio_config rtmdio_838x_cfg = {
	.cmd_fail	= RTMDIO_838X_CMD_FAIL,
	.cmd_mask	= RTMDIO_838X_CMD_MASK,
	.cmd_reg	= RTMDIO_838X_SMI_ACCESS_PHY_CTRL_1,
	.num_busses	= RTMDIO_838X_NUM_BUSSES,
	.num_pages	= RTMDIO_838X_NUM_PAGES,
	.num_phys	= 28,
	.port_map_base	= RTMDIO_838X_SMI_PORT0_5_ADDR_CTRL,
	.read_c22	= rtmdio_838x_read_c22,
	.read_c45	= rtmdio_838x_read_c45,
	.ret_mask	= GENMASK(15, 0),
	.ret_reg	= RTMDIO_838X_SMI_ACCESS_PHY_CTRL_2,
	.setup_ctrl	= rtmdio_838x_setup_ctrl,
	.setup_polling	= rtmdio_838x_setup_polling,
	.smi_base	= RTMDIO_838X_SMI_ACCESS_PHY_CTRL_0,
	.smi_size	= sizeof(struct rtmdio_838x_smi_access),
	.write_c22	= rtmdio_838x_write_c22,
	.write_c45	= rtmdio_838x_write_c45,
};

static const struct rtmdio_config rtmdio_839x_cfg = {
	.cmd_fail	= RTMDIO_839X_CMD_FAIL,
	.cmd_mask	= RTMDIO_839X_CMD_MASK,
	.cmd_reg	= RTMDIO_839X_PHYREG_ACCESS_CTRL,
	.num_busses	= RTMDIO_839X_NUM_BUSSES,
	.num_pages	= RTMDIO_839X_NUM_PAGES,
	.num_phys	= 52,
	.read_c22	= rtmdio_839x_read_c22,
	.read_c45	= rtmdio_839x_read_c45,
	.ret_mask	= GENMASK(15, 0),
	.ret_reg	= RTMDIO_839X_PHYREG_DATA_CTRL,
	.setup_ctrl	= rtmdio_839x_setup_ctrl,
	.smi_base	= RTMDIO_839X_PHYREG_ACCESS_CTRL,
	.smi_size	= sizeof(struct rtmdio_839x_smi_access),
	.write_c22	= rtmdio_839x_write_c22,
	.write_c45	= rtmdio_839x_write_c45,
};

static const struct rtmdio_config rtmdio_930x_cfg = {
	.cmd_fail	= RTMDIO_930X_CMD_FAIL,
	.cmd_mask	= RTMDIO_930X_CMD_MASK,
	.cmd_reg	= RTMDIO_930X_SMI_ACCESS_PHY_CTRL_1,
	.bus_map_base	= RTMDIO_930X_SMI_PORT0_15_POLLING_SEL,
	.num_busses	= RTMDIO_930X_NUM_BUSSES,
	.num_pages	= RTMDIO_930X_NUM_PAGES,
	.num_phys	= 28,
	.port_map_base	= RTMDIO_930X_SMI_PORT0_5_ADDR_CTRL,
	.read_c22	= rtmdio_930x_read_c22,
	.read_c45	= rtmdio_930x_read_c45,
	.ret_mask	= GENMASK(15, 0),
	.ret_reg	= RTMDIO_930X_SMI_ACCESS_PHY_CTRL_2,
	.setup_ctrl	= rtmdio_930x_setup_ctrl,
	.setup_polling	= rtmdio_930x_setup_polling,
	.smi_base	= RTMDIO_930X_SMI_ACCESS_PHY_CTRL_0,
	.smi_size	= sizeof(struct rtmdio_930x_smi_access),
	.write_c22	= rtmdio_930x_write_c22,
	.write_c45	= rtmdio_930x_write_c45,
};

static const struct rtmdio_config rtmdio_931x_cfg = {
	.cmd_fail	= RTMDIO_931X_CMD_FAIL,
	.cmd_mask	= RTMDIO_931X_CMD_MASK,
	.cmd_reg	= RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_0,
	.bus_map_base	= RTMDIO_931X_SMI_PORT_POLLING_SEL,
	.num_busses	= RTMDIO_931X_NUM_BUSSES,
	.num_pages	= RTMDIO_931X_NUM_PAGES,
	.num_phys	= 56,
	.port_map_base	= RTMDIO_931X_SMI_PORT_ADDR_CTRL,
	.read_c22	= rtmdio_931x_read_c22,
	.read_c45	= rtmdio_931x_read_c45,
	.ret_mask	= GENMASK(31, 16),
	.ret_reg	= RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_3,
	.setup_ctrl	= rtmdio_931x_setup_ctrl,
	.setup_polling	= rtmdio_931x_setup_polling,
	.smi_base	= RTMDIO_931X_SMI_INDRT_ACCESS_CTRL_0,
	.smi_size	= sizeof(struct rtmdio_931x_smi_access),
	.write_c22	= rtmdio_931x_write_c22,
	.write_c45	= rtmdio_931x_write_c45,
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

MODULE_AUTHOR("Markus Stockhausen <markus.stockhausen@gmx.de>");
MODULE_DESCRIPTION("Realtek Otto MDIO driver");
MODULE_LICENSE("GPL v2");
