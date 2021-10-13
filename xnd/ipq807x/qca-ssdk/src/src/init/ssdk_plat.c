/*
 * Copyright (c) 2017-2019, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
/*qca808x_start*/
#include "sw.h"
#include "ssdk_init.h"
#include "fal_init.h"
#include "fal.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "ssdk_init.h"
/*qca808x_end*/
#include "ssdk_dts.h"
#if (defined(HPPE) || defined(MP))
#include "hppe_init.h"
#endif
#include <linux/kconfig.h>
/*qca808x_start*/
#include <linux/version.h>
/*qca808x_end*/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/types.h>
//#include <asm/mach-types.h>
#include <generated/autoconf.h>
#include <linux/if_arp.h>
#include <linux/inetdevice.h>
#include <linux/netdevice.h>
#include <linux/phy.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/string.h>

#if defined(ISIS) ||defined(ISISC) ||defined(GARUDA)
#include <f1_phy.h>
#endif
#if defined(ATHENA) ||defined(SHIVA) ||defined(HORUS)
#include <f2_phy.h>
#endif
#ifdef IN_MALIBU_PHY
#include <malibu_phy.h>
#endif
/*qca808x_start*/
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(4,1,0))
#include <linux/of.h>
#include <linux/of_mdio.h>
#include <linux/of_platform.h>
#elif defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
#include <linux/of.h>
#include <linux/of_mdio.h>
#include <drivers/leds/leds-ipq40xx.h>
#include <linux/of_platform.h>
#include <linux/reset.h>
#else
#include <linux/ar8216_platform.h>
#include <drivers/net/phy/ar8216.h>
#include <drivers/net/ethernet/atheros/ag71xx/ag71xx.h>
#endif
#include "ssdk_plat.h"
/*qca808x_end*/
#include "ssdk_clk.h"
#include "ref_vlan.h"
#include "ref_fdb.h"
#include "ref_mib.h"
#include "ref_port_ctrl.h"
#include "ref_misc.h"
#include "ref_uci.h"
#include "shell.h"
#ifdef BOARD_AR71XX
#include "ssdk_uci.h"
#endif

#include "hsl_phy.h"
#ifdef IN_LINUX_STD_PTP
#include "qca808x_ptp.h"
#endif

#ifdef IN_IP
#if defined (CONFIG_NF_FLOW_COOKIE)
#include "fal_flowcookie.h"
#ifdef IN_SFE
#include <shortcut-fe/sfe.h>
#endif
#endif
#endif

#ifdef IN_RFS
#if defined(CONFIG_VLAN_8021Q) || defined(CONFIG_VLAN_8021Q_MODULE)
#include <linux/if_vlan.h>
#endif
#include <qca-rfs/rfs_dev.h>
#ifdef IN_IP
#include "fal_rfs.h"
#endif
#endif

#include "adpt.h"
/*qca808x_start*/

extern struct qca_phy_priv **qca_phy_priv_global;
extern ssdk_chip_type SSDK_CURRENT_CHIP_TYPE;
/*qca808x_end*/
struct mutex switch_mdio_lock;

#ifdef BOARD_IPQ806X
#define PLATFORM_MDIO_BUS_NAME		"mdio-gpio"
#endif

#ifdef BOARD_AR71XX
#define PLATFORM_MDIO_BUS_NAME		"ag71xx-mdio"
#endif
/*qca808x_start*/
#define MDIO_BUS_0					0
#define MDIO_BUS_1					1
/*qca808x_end*/
#define PLATFORM_MDIO_BUS_NUM		MDIO_BUS_0

#define ISIS_CHIP_ID 0x18
#define ISIS_CHIP_REG 0
#define SHIVA_CHIP_ID 0x1f
#define SHIVA_CHIP_REG 0x10
#define HIGH_ADDR_DFLT	0x200

/*
 * Using ISIS's address as default
  */
static int switch_chip_id = ISIS_CHIP_ID;
static int switch_chip_reg = ISIS_CHIP_REG;

static int ssdk_dev_id = 0;
/*qca808x_start*/
a_uint32_t ssdk_log_level = SSDK_LOG_LEVEL_DEFAULT;
/*qca808x_end*/

#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
struct ag71xx_mdio {
	struct mii_bus		*mii_bus;
	int			mii_irq[PHY_MAX_ADDR];
	void __iomem		*mdio_base;
};
#endif

#ifdef BOARD_AR71XX
static uint32_t switch_chip_id_adjuest(a_uint32_t dev_id)
{
	uint32_t chip_version = 0;
	chip_version = (qca_ar8216_mii_read(dev_id, 0)&0xff00)>>8;
	if((chip_version !=0) && (chip_version !=0xff))
		return 0;

	switch_chip_id = SHIVA_CHIP_ID;
	switch_chip_reg = SHIVA_CHIP_REG;

	chip_version = (qca_ar8216_mii_read(dev_id, 0)&0xff00)>>8;
	printk("chip_version:0x%x\n", chip_version);
	return 1;
}
#endif

static inline void
split_addr(uint32_t regaddr, uint16_t *r1, uint16_t *r2, uint16_t *page)
{
	regaddr >>= 1;
	*r1 = regaddr & 0x1e;

	regaddr >>= 5;
	*r2 = regaddr & 0x7;

	regaddr >>= 3;
	*page = regaddr & 0x3ff;
}

a_uint32_t
qca_ar8216_mii_read(a_uint32_t dev_id, a_uint32_t reg)
{
	struct mii_bus *bus;
	uint16_t r1, r2, page;
	uint16_t lo, hi;

	bus = qca_phy_priv_global[dev_id]->miibus;

	split_addr((uint32_t) reg, &r1, &r2, &page);
	mutex_lock(&switch_mdio_lock);
	mdiobus_write(bus, switch_chip_id, switch_chip_reg, page);
	udelay(100);
	lo = mdiobus_read(bus, 0x10 | r2, r1);
	hi = mdiobus_read(bus, 0x10 | r2, r1 + 1);
	mdiobus_write(bus, switch_chip_id, switch_chip_reg, HIGH_ADDR_DFLT);
	mutex_unlock(&switch_mdio_lock);
	return (hi << 16) | lo;
}

void
qca_ar8216_mii_write(a_uint32_t dev_id, a_uint32_t reg, a_uint32_t val)
{
	struct mii_bus *bus;
	uint16_t r1, r2, r3;
	uint16_t lo, hi;

	bus = qca_phy_priv_global[dev_id]->miibus;

	split_addr((a_uint32_t) reg, &r1, &r2, &r3);
	lo = val & 0xffff;
	hi = (a_uint16_t) (val >> 16);

	mutex_lock(&switch_mdio_lock);
	mdiobus_write(bus, switch_chip_id, switch_chip_reg, r3);
	udelay(100);
	if(SSDK_CURRENT_CHIP_TYPE != CHIP_SHIVA) {
		mdiobus_write(bus, 0x10 | r2, r1, lo);
		mdiobus_write(bus, 0x10 | r2, r1 + 1, hi);
	} else {
		mdiobus_write(bus, 0x10 | r2, r1 + 1, hi);
		mdiobus_write(bus, 0x10 | r2, r1, lo);
	}
	mdiobus_write(bus, switch_chip_id, switch_chip_reg, HIGH_ADDR_DFLT);
	mutex_unlock(&switch_mdio_lock);
}

/*qca808x_start*/
a_bool_t
phy_addr_validation_check(a_uint32_t phy_addr)
{

	if ((phy_addr > SSDK_PHY_BCAST_ID) || (phy_addr < SSDK_PHY_MIN_ID))
		return A_FALSE;
	else
		return A_TRUE;
}

static struct mii_bus *
ssdk_phy_miibus_get(a_uint32_t dev_id, a_uint32_t phy_addr)
{
	struct mii_bus *bus = NULL;
/*qca808x_end*/
#ifndef BOARD_AR71XX
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
	bus = ssdk_dts_miibus_get(dev_id, phy_addr);
#endif
#endif
/*qca808x_start*/
	if (!bus)
		bus = qca_phy_priv_global[dev_id]->miibus;

	return bus;
}

sw_error_t
qca_ar8327_phy_read(a_uint32_t dev_id, a_uint32_t phy_addr,
                           a_uint32_t reg, a_uint16_t* data)
{
	struct mii_bus *bus = NULL;

	if (A_TRUE != phy_addr_validation_check (phy_addr))
	{
		return SW_BAD_PARAM;
	}

	bus = ssdk_phy_miibus_get(dev_id, phy_addr);
	if (!bus)
		return SW_NOT_SUPPORTED;

	*data = mdiobus_read(bus, phy_addr, reg);
	return 0;
}

sw_error_t
qca_ar8327_phy_write(a_uint32_t dev_id, a_uint32_t phy_addr,
                            a_uint32_t reg, a_uint16_t data)
{
	struct mii_bus *bus = NULL;

	if (A_TRUE != phy_addr_validation_check (phy_addr))
	{
		return SW_BAD_PARAM;
	}

	bus = ssdk_phy_miibus_get(dev_id, phy_addr);
	if (!bus)
		return SW_NOT_SUPPORTED;

	mdiobus_write(bus, phy_addr, reg, data);
	return 0;
}

void
qca_ar8327_phy_dbg_write(a_uint32_t dev_id, a_uint32_t phy_addr,
                                a_uint16_t dbg_addr, a_uint16_t dbg_data)
{
	struct mii_bus *bus = NULL;

	if (A_TRUE != phy_addr_validation_check (phy_addr))
	{
		return;
	}

	bus = ssdk_phy_miibus_get(dev_id, phy_addr);
	if (!bus)
		return;

	mdiobus_write(bus, phy_addr, QCA_MII_DBG_ADDR, dbg_addr);
	mdiobus_write(bus, phy_addr, QCA_MII_DBG_DATA, dbg_data);
}

void
qca_ar8327_phy_dbg_read(a_uint32_t dev_id, a_uint32_t phy_addr,
		                a_uint16_t dbg_addr, a_uint16_t *dbg_data)
{
	struct mii_bus *bus = NULL;

	if (A_TRUE != phy_addr_validation_check (phy_addr))
	{
		return;
	}

	bus = ssdk_phy_miibus_get(dev_id, phy_addr);
	if (!bus)
		return;

	mdiobus_write(bus, phy_addr, QCA_MII_DBG_ADDR, dbg_addr);
	*dbg_data = mdiobus_read(bus, phy_addr, QCA_MII_DBG_DATA);
}


void
qca_ar8327_mmd_write(a_uint32_t dev_id, a_uint32_t phy_addr,
                          a_uint16_t addr, a_uint16_t data)
{
	struct mii_bus *bus = NULL;

	if (A_TRUE != phy_addr_validation_check (phy_addr))
	{
		return;
	}

	bus = ssdk_phy_miibus_get(dev_id, phy_addr);
	if (!bus)
		return;

	mdiobus_write(bus, phy_addr, QCA_MII_MMD_ADDR, addr);
	mdiobus_write(bus, phy_addr, QCA_MII_MMD_DATA, data);
}

void qca_phy_mmd_write(u32 dev_id, u32 phy_id,
                     u16 mmd_num, u16 reg_id, u16 reg_val)
{
	qca_ar8327_phy_write(dev_id, phy_id,
			QCA_MII_MMD_ADDR, mmd_num);
	qca_ar8327_phy_write(dev_id, phy_id,
			QCA_MII_MMD_DATA, reg_id);
	qca_ar8327_phy_write(dev_id, phy_id,
			QCA_MII_MMD_ADDR,
			0x4000 | mmd_num);
	qca_ar8327_phy_write(dev_id, phy_id,
		QCA_MII_MMD_DATA, reg_val);
}

u16 qca_phy_mmd_read(u32 dev_id, u32 phy_id,
		u16 mmd_num, u16 reg_id)
{
	u16 value = 0;
	qca_ar8327_phy_write(dev_id, phy_id,
			QCA_MII_MMD_ADDR, mmd_num);
	qca_ar8327_phy_write(dev_id, phy_id,
			QCA_MII_MMD_DATA, reg_id);
	qca_ar8327_phy_write(dev_id, phy_id,
			QCA_MII_MMD_ADDR,
			0x4000 | mmd_num);
	qca_ar8327_phy_read(dev_id, phy_id,
			QCA_MII_MMD_DATA, &value);
	return value;
}
/*qca808x_end*/
sw_error_t
qca_switch_reg_read(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t * reg_data, a_uint32_t len)
{
	uint32_t reg_val = 0;

	if (len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

	if ((reg_addr%4)!= 0)
	return SW_BAD_PARAM;

	reg_val = readl(qca_phy_priv_global[dev_id]->hw_addr + reg_addr);

	aos_mem_copy(reg_data, &reg_val, sizeof (a_uint32_t));
	return 0;
}

sw_error_t
qca_switch_reg_write(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t * reg_data, a_uint32_t len)
{
	uint32_t reg_val = 0;
	if (len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

	if ((reg_addr%4)!= 0)
	return SW_BAD_PARAM;

	aos_mem_copy(&reg_val, reg_data, sizeof (a_uint32_t));
	writel(reg_val, qca_phy_priv_global[dev_id]->hw_addr + reg_addr);
	return 0;
}

sw_error_t
qca_psgmii_reg_read(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t * reg_data, a_uint32_t len)
{
#ifdef DESS
	uint32_t reg_val = 0;

	if (len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

	if((reg_addr%4)!=0)
	return SW_BAD_PARAM;

	if (qca_phy_priv_global[dev_id]->psgmii_hw_addr == NULL)
		return SW_NOT_SUPPORTED;

	reg_val = readl(qca_phy_priv_global[dev_id]->psgmii_hw_addr + reg_addr);

	aos_mem_copy(reg_data, &reg_val, sizeof (a_uint32_t));
#endif
	return 0;
}

sw_error_t
qca_psgmii_reg_write(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t * reg_data, a_uint32_t len)
{
#ifdef DESS
	uint32_t reg_val = 0;
	if (len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

	if((reg_addr%4)!=0)
	return SW_BAD_PARAM;

	if (qca_phy_priv_global[dev_id]->psgmii_hw_addr == NULL)
		return SW_NOT_SUPPORTED;

	aos_mem_copy(&reg_val, reg_data, sizeof (a_uint32_t));
	writel(reg_val, qca_phy_priv_global[dev_id]->psgmii_hw_addr + reg_addr);
#endif
	return 0;
}

sw_error_t
qca_uniphy_reg_read(a_uint32_t dev_id, a_uint32_t uniphy_index,
				a_uint32_t reg_addr, a_uint8_t * reg_data, a_uint32_t len)
{
#if (defined(HPPE) || defined(MP))
	uint32_t reg_val = 0;
	void __iomem *hppe_uniphy_base = NULL;
	a_uint32_t reg_addr1, reg_addr2;

	if (len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

	if (SSDK_UNIPHY_INSTANCE0 == uniphy_index)
		hppe_uniphy_base = qca_phy_priv_global[dev_id]->uniphy_hw_addr;
	else if (SSDK_UNIPHY_INSTANCE1 == uniphy_index)
		hppe_uniphy_base = qca_phy_priv_global[dev_id]->uniphy_hw_addr + HPPE_UNIPHY_BASE1;

	else if (SSDK_UNIPHY_INSTANCE2 == uniphy_index)
		hppe_uniphy_base = qca_phy_priv_global[dev_id]->uniphy_hw_addr + HPPE_UNIPHY_BASE2;
	else
		return SW_BAD_PARAM;

	if ( reg_addr > HPPE_UNIPHY_MAX_DIRECT_ACCESS_REG)
	{
		// uniphy reg indireclty access
		reg_addr1 = (reg_addr & 0xffffff) >> 8;
		writel(reg_addr1, hppe_uniphy_base + HPPE_UNIPHY_INDIRECT_REG_ADDR);

		reg_addr2 = reg_addr & HPPE_UNIPHY_INDIRECT_LOW_ADDR;
		reg_addr = (HPPE_UNIPHY_INDIRECT_DATA << 10) | (reg_addr2 << 2);

		reg_val = readl(hppe_uniphy_base + reg_addr);
		aos_mem_copy(reg_data, &reg_val, sizeof (a_uint32_t));
	}
	else
	{	// uniphy reg directly access
		reg_val = readl(hppe_uniphy_base + reg_addr);
		aos_mem_copy(reg_data, &reg_val, sizeof (a_uint32_t));
	}
#endif
	return 0;
}

sw_error_t
qca_uniphy_reg_write(a_uint32_t dev_id, a_uint32_t uniphy_index,
				a_uint32_t reg_addr, a_uint8_t * reg_data, a_uint32_t len)
{
#if (defined(HPPE) || defined(MP))
	void __iomem *hppe_uniphy_base = NULL;
	a_uint32_t reg_addr1, reg_addr2;
	uint32_t reg_val = 0;

	if (len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

	if (SSDK_UNIPHY_INSTANCE0 == uniphy_index)
		hppe_uniphy_base = qca_phy_priv_global[dev_id]->uniphy_hw_addr;
	else if (SSDK_UNIPHY_INSTANCE1 == uniphy_index)
		hppe_uniphy_base = qca_phy_priv_global[dev_id]->uniphy_hw_addr + HPPE_UNIPHY_BASE1;

	else if (SSDK_UNIPHY_INSTANCE2 == uniphy_index)
		hppe_uniphy_base = qca_phy_priv_global[dev_id]->uniphy_hw_addr + HPPE_UNIPHY_BASE2;
	else
		return SW_BAD_PARAM;

	if ( reg_addr > HPPE_UNIPHY_MAX_DIRECT_ACCESS_REG)
	{
		// uniphy reg indireclty access
		reg_addr1 = (reg_addr & 0xffffff) >> 8;
		writel(reg_addr1, hppe_uniphy_base + HPPE_UNIPHY_INDIRECT_REG_ADDR);

		reg_addr2 = reg_addr & HPPE_UNIPHY_INDIRECT_LOW_ADDR;
		reg_addr = (HPPE_UNIPHY_INDIRECT_DATA << 10) | (reg_addr2 << 2);
		aos_mem_copy(&reg_val, reg_data, sizeof (a_uint32_t));
		writel(reg_val, hppe_uniphy_base + reg_addr);
	}
	else
	{	// uniphy reg directly access
		aos_mem_copy(&reg_val, reg_data, sizeof (a_uint32_t));
		writel(reg_val, hppe_uniphy_base + reg_addr);
	}
#endif
	return 0;
}
#ifndef BOARD_AR71XX
/*qca808x_start*/
static int miibus_get(a_uint32_t dev_id)
{
/*qca808x_end*/
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
/*qca808x_start*/
	struct device_node *mdio_node = NULL;
	struct device_node *switch_node = NULL;
	struct platform_device *mdio_plat = NULL;
	struct ipq40xx_mdio_data *mdio_data = NULL;
	struct qca_phy_priv *priv;
	hsl_reg_mode reg_mode = HSL_REG_LOCAL_BUS;
	priv = qca_phy_priv_global[dev_id];
	switch_node = qca_phy_priv_global[dev_id]->of_node;
	if (switch_node) {
		mdio_node = of_parse_phandle(switch_node, "mdio-bus", 0);
		if (mdio_node) {
			priv->miibus = of_mdio_find_bus(mdio_node);
			return 0;
		}
	}
/*qca808x_end*/
	reg_mode=ssdk_switch_reg_access_mode_get(dev_id);
/*qca808x_start*/
	if(reg_mode == HSL_REG_LOCAL_BUS)
		mdio_node = of_find_compatible_node(NULL, NULL, "qcom,ipq40xx-mdio");
	else
		mdio_node = of_find_compatible_node(NULL, NULL, "virtual,mdio-gpio");

	if (!mdio_node) {
		SSDK_ERROR("No MDIO node found in DTS!\n");
		return 1;
	}

	mdio_plat = of_find_device_by_node(mdio_node);
	if (!mdio_plat) {
		SSDK_ERROR("cannot find platform device from mdio node\n");
		return 1;
	}

	if(reg_mode == HSL_REG_LOCAL_BUS)
	{
		mdio_data = dev_get_drvdata(&mdio_plat->dev);
		if (!mdio_data) {
			SSDK_ERROR("cannot get mdio_data reference from device data\n");
			return 1;
		}
		priv->miibus = mdio_data->mii_bus;
	}
	else
		priv->miibus = dev_get_drvdata(&mdio_plat->dev);

	if (!priv->miibus) {
		SSDK_ERROR("cannot get mii bus reference from device data\n");
		return 1;
	}
/*qca808x_end*/
#else
	struct device *miidev;
	char busid[MII_BUS_ID_SIZE];
	struct qca_phy_priv *priv;

	priv = qca_phy_priv_global[dev_id];
	snprintf(busid, MII_BUS_ID_SIZE, "%s.%d",
		PLATFORM_MDIO_BUS_NAME, PLATFORM_MDIO_BUS_NUM);

	miidev = bus_find_device_by_name(&platform_bus_type, NULL, busid);
	if (!miidev) {
		SSDK_ERROR("Failed to get miidev\n");
		return 1;
	}

	priv->miibus = dev_get_drvdata(miidev);

	if(!priv->miibus){
		SSDK_ERROR("mdio bus '%s' get FAIL\n", busid);
		return 1;
	}
#endif
/*qca808x_start*/
	return 0;
}
/*qca808x_end*/
#else
static int miibus_get(a_uint32_t dev_id)
{
	struct ag71xx_mdio *am;
	struct qca_phy_priv *priv = qca_phy_priv_global[dev_id];
#if defined(CONFIG_OF) && (LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0))
	struct device_node *mdio_node = NULL;
	struct platform_device *mdio_plat = NULL;

	mdio_node = of_find_compatible_node(NULL, NULL, "qca,ag71xx-mdio");
	if (!mdio_node) {
		SSDK_ERROR("No MDIO node found in DTS!\n");
		return 1;
	}
	mdio_plat = of_find_device_by_node(mdio_node);
	if (!mdio_plat) {
		SSDK_ERROR("cannot find platform device from mdio node\n");
		return 1;
	}
	am = dev_get_drvdata(&mdio_plat->dev);
	if (!am) {
                	SSDK_ERROR("cannot get mdio_data reference from device data\n");
                	return 1;
	}
	priv->miibus = am->mii_bus;

	switch_chip_id_adjuest(dev_id);
#else
	struct device *miidev;
	char busid[MII_BUS_ID_SIZE];

	snprintf(busid, MII_BUS_ID_SIZE, "%s.%d",
		PLATFORM_MDIO_BUS_NAME, PLATFORM_MDIO_BUS_NUM);

	miidev = bus_find_device_by_name(&platform_bus_type, NULL, busid);
	if (!miidev) {
		SSDK_ERROR("Failed to get miidev!\n");
		return 1;
	}
	am = dev_get_drvdata(miidev);
	priv->miibus = am->mii_bus;

	if(switch_chip_id_adjuest(dev_id)) {
		snprintf(busid, MII_BUS_ID_SIZE, "%s.%d",
		PLATFORM_MDIO_BUS_NAME, MDIO_BUS_1);

		miidev = bus_find_device_by_name(&platform_bus_type, NULL, busid);
		if (!miidev) {
			SSDK_ERROR("Failed get mii bus\n");
			return 1;
		}

		am = dev_get_drvdata(miidev);
		priv->miibus = am->mii_bus;
		SSDK_INFO("chip_version:0x%x\n", (qca_ar8216_mii_read(dev_id, 0)&0xff00)>>8);
	}

	if(!miidev){
		SSDK_ERROR("mdio bus '%s' get FAIL\n", busid);
		return 1;
	}
#endif

	return 0;
}
#endif

struct mii_bus *ssdk_miibus_get_by_device(a_uint32_t dev_id)
{
	return qca_phy_priv_global[dev_id]->miibus;
}

static ssize_t ssdk_dev_id_get(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
	ssize_t count;
	a_uint32_t num;

	num = (a_uint32_t)ssdk_dev_id;

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "%u", num);
	return count;
}

static ssize_t ssdk_dev_id_set(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	char num_buf[12];
	a_uint32_t num;

	if (count >= sizeof(num_buf)) return 0;
	memcpy(num_buf, buf, count);
	num_buf[count] = '\0';
	sscanf(num_buf, "%u", &num);

	ssdk_dev_id = num;

	return count;
}

static ssize_t ssdk_log_level_get(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
	ssize_t count;
	a_uint32_t num;

	num = ssdk_log_level;

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "%u", num);
	return count;
}

static ssize_t ssdk_log_level_set(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	char num_buf[12];
	a_uint32_t num;

	if (count >= sizeof(num_buf))
		return 0;
	memcpy(num_buf, buf, count);
	num_buf[count] = '\0';
	sscanf(num_buf, "%u", &num);

	ssdk_log_level = (a_uint32_t)num;

	return count;
}

static ssize_t ssdk_packet_counter_get(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
	ssize_t count;
	adpt_api_t *p_api;

	p_api = adpt_api_ptr_get(ssdk_dev_id);
	if (p_api == NULL || p_api->adpt_debug_counter_get == NULL)
	{
		count = snprintf(buf, (ssize_t)PAGE_SIZE, "Unsupported\n");
		return count;
	}

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "\n");

	p_api->adpt_debug_counter_get(A_FALSE);

	return count;
}

static ssize_t ssdk_packet_counter_set(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	char num_buf[12];
	adpt_api_t *p_api;

	p_api = adpt_api_ptr_get(ssdk_dev_id);
	if (p_api == NULL || p_api->adpt_debug_counter_set == NULL) {
		SSDK_WARN("Unsupported\n");
		return count;
	}

	p_api->adpt_debug_counter_set();

	if (count >= sizeof(num_buf))
		return 0;
	memcpy(num_buf, buf, count);
	num_buf[count] = '\0';


	return count;
}

static ssize_t ssdk_byte_counter_get(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
	ssize_t count;
	adpt_api_t *p_api;

	p_api = adpt_api_ptr_get(ssdk_dev_id);
	if (p_api == NULL || p_api->adpt_debug_counter_get == NULL)
	{
		count = snprintf(buf, (ssize_t)PAGE_SIZE, "Unsupported\n");
		return count;
	}

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "\n");

	p_api->adpt_debug_counter_get(A_TRUE);

	return count;
}

static ssize_t ssdk_byte_counter_set(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	char num_buf[12];
	adpt_api_t *p_api;

	p_api = adpt_api_ptr_get(ssdk_dev_id);
	if (p_api == NULL || p_api->adpt_debug_counter_set == NULL) {
		SSDK_WARN("Unsupported\n");
		return count;
	}

	p_api->adpt_debug_counter_set();

	if (count >= sizeof(num_buf))
		return 0;
	memcpy(num_buf, buf, count);
	num_buf[count] = '\0';


	return count;
}

#ifdef HPPE
#ifdef IN_QOS
void ssdk_dts_port_scheduler_dump(a_uint32_t dev_id)
{
	a_uint32_t i;
	ssdk_dt_portscheduler_cfg *portscheduler_cfg;
	ssdk_dt_scheduler_cfg *scheduler_cfg;
	a_uint8_t srcmsg[7][16];

	scheduler_cfg = ssdk_bootup_shceduler_cfg_get(dev_id);

	printk("===============================port_scheduler_resource===========================\n");
	printk("portid     ucastq     mcastq     10sp     10cdrr     10edrr     11cdrr     11edrr\n");
	for (i = 0; i < SSDK_MAX_PORT_NUM; i++)
	{
		portscheduler_cfg = &scheduler_cfg->pool[i];
		snprintf(srcmsg[0], sizeof(srcmsg[0]), "<%d %d>", portscheduler_cfg->ucastq_start,
				portscheduler_cfg->ucastq_end);
		snprintf(srcmsg[1], sizeof(srcmsg[1]), "<%d %d>", portscheduler_cfg->mcastq_start,
				portscheduler_cfg->mcastq_end);
		snprintf(srcmsg[2], sizeof(srcmsg[2]), "<%d %d>", portscheduler_cfg->l0sp_start,
				portscheduler_cfg->l0sp_end);
		snprintf(srcmsg[3], sizeof(srcmsg[3]), "<%d %d>", portscheduler_cfg->l0cdrr_start,
				portscheduler_cfg->l0cdrr_end);
		snprintf(srcmsg[4], sizeof(srcmsg[4]), "<%d %d>", portscheduler_cfg->l0edrr_start,
				portscheduler_cfg->l0edrr_end);
		snprintf(srcmsg[5], sizeof(srcmsg[5]), "<%d %d>", portscheduler_cfg->l1cdrr_start,
				portscheduler_cfg->l1cdrr_end);
		snprintf(srcmsg[6], sizeof(srcmsg[6]), "<%d %d>", portscheduler_cfg->l1edrr_start,
				portscheduler_cfg->l1edrr_end);
		printk("%6d%11s%11s%9s%11s%11s%11s%11s\n", i, srcmsg[0], srcmsg[1], srcmsg[2], srcmsg[3],
				srcmsg[4], srcmsg[5], srcmsg[6]);
	}
}

void ssdk_dts_l0scheduler_dump(a_uint32_t dev_id)
{
	a_uint32_t i;
	ssdk_dt_l0scheduler_cfg *scheduler_cfg;
	ssdk_dt_scheduler_cfg *cfg;

	cfg = ssdk_bootup_shceduler_cfg_get(dev_id);
	printk("==========================l0scheduler_cfg===========================\n");
	printk("queue     portid     cpri     cdrr_id     epri     edrr_id     sp_id\n");
	for (i = 0; i < SSDK_L0SCHEDULER_CFG_MAX; i++)
	{
		scheduler_cfg = &cfg->l0cfg[i];
		if (scheduler_cfg->valid == 1)
			printk("%5d%11d%9d%12d%9d%12d%10d\n", i, scheduler_cfg->port_id,
				scheduler_cfg->cpri, scheduler_cfg->cdrr_id, scheduler_cfg->epri,
				scheduler_cfg->edrr_id, scheduler_cfg->sp_id);
	}
}

void ssdk_dts_l1scheduler_dump(a_uint32_t dev_id)
{
	a_uint32_t i;
	ssdk_dt_l1scheduler_cfg *scheduler_cfg;
	ssdk_dt_scheduler_cfg *cfg;

	cfg = ssdk_bootup_shceduler_cfg_get(dev_id);

	printk("=====================l1scheduler_cfg=====================\n");
	printk("flow     portid     cpri     cdrr_id     epri     edrr_id\n");
	for (i = 0; i < SSDK_L1SCHEDULER_CFG_MAX; i++)
	{
		scheduler_cfg = &cfg->l1cfg[i];
		if (scheduler_cfg->valid == 1)
			printk("%4d%11d%9d%12d%9d%12d\n", i, scheduler_cfg->port_id,
				scheduler_cfg->cpri, scheduler_cfg->cdrr_id,
				scheduler_cfg->epri, scheduler_cfg->edrr_id);
	}
}
#endif
#endif
static const a_int8_t *qca_phy_feature_str[QCA_PHY_FEATURE_MAX] = {
	"PHY_CLAUSE45",
	"PHY_COMBO",
	"PHY_QGMAC",
	"PHY_XGMAC",
	"PHY_I2C",
	"PHY_INIT",
	"PHY_FORCE"
};

void ssdk_dts_phyinfo_dump(a_uint32_t dev_id)
{
	a_uint32_t i, j;
	ssdk_port_phyinfo *port_phyinfo;

	printk("=====================port phyinfo========================\n");
	printk("portid     phy_addr     features\n");

	for (i = 0; i <= SSDK_MAX_PORT_NUM; i++) {
		port_phyinfo = ssdk_port_phyinfo_get(dev_id, i);
		if (port_phyinfo) {
			printk("%6d%13d%*s", port_phyinfo->port_id,
					port_phyinfo->phy_addr, 5, "");
			for (j = 0; j < QCA_PHY_FEATURE_MAX; j++) {
				if (port_phyinfo->phy_features & BIT(j) && BIT(j) != PHY_F_INIT) {
					printk(KERN_CONT "%s ", qca_phy_feature_str[j]);
					if (BIT(j) == PHY_F_FORCE) {
						printk(KERN_CONT "(speed: %d, duplex: %s) ",
								port_phyinfo->port_speed,
								port_phyinfo->port_duplex > 0 ?
								"full" : "half");
					}
				}
			}
			printk(KERN_CONT "\n");
		}
	}
}

static ssize_t ssdk_dts_dump(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
	ssize_t count;
	a_uint32_t dev_id, dev_num;
	ssdk_reg_map_info map;
	hsl_reg_mode mode;

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "\n");

	dev_num = ssdk_switch_device_num_get();
	for (dev_id = 0; dev_id < dev_num; dev_id ++)
	{
		ssdk_switch_reg_map_info_get(dev_id, &map);
		mode = ssdk_switch_reg_access_mode_get(dev_id);
		printk("=======================================================\n");
		printk("ess-switch\n");
		printk("        reg = <0x%x 0x%x>\n", map.base_addr, map.size);
		if (mode == HSL_REG_LOCAL_BUS)
			printk("        switch_access_mode = <local bus>\n");
		else if (mode == HSL_REG_MDIO)
			printk("        switch_access_mode = <mdio bus>\n");
		else
			printk("        switch_access_mode = <(null)>\n");
		printk("        switch_cpu_bmp = <0x%x>\n", ssdk_cpu_bmp_get(dev_id));
		printk("        switch_lan_bmp = <0x%x>\n", ssdk_lan_bmp_get(dev_id));
		printk("        switch_wan_bmp = <0x%x>\n", ssdk_wan_bmp_get(dev_id));
		printk("        switch_inner_bmp = <0x%x>\n", ssdk_inner_bmp_get(dev_id));
		printk("        switch_mac_mode = <0x%x>\n", ssdk_dt_global_get_mac_mode(dev_id, 0));
		printk("        switch_mac_mode1 = <0x%x>\n", ssdk_dt_global_get_mac_mode(dev_id, 1));
		printk("        switch_mac_mode2 = <0x%x>\n", ssdk_dt_global_get_mac_mode(dev_id, 2));
#ifdef IN_BM
		printk("        bm_tick_mode = <0x%x>\n", ssdk_bm_tick_mode_get(dev_id));
#endif
#ifdef HPPE
#ifdef IN_QOS
		printk("        tm_tick_mode = <0x%x>\n", ssdk_tm_tick_mode_get(dev_id));
#endif
#endif
#ifdef DESS
		printk("ess-psgmii\n");
		ssdk_psgmii_reg_map_info_get(dev_id, &map);
		mode = ssdk_psgmii_reg_access_mode_get(dev_id);
		printk("        reg = <0x%x 0x%x>\n", map.base_addr, map.size);
		if (mode == HSL_REG_LOCAL_BUS)
			printk("        psgmii_access_mode = <local bus>\n");
		else if (mode == HSL_REG_MDIO)
			printk("        psgmii_access_mode = <mdio bus>\n");
		else
			printk("        psgmii_access_mode = <(null)>\n");
#endif
#ifdef IN_UNIPHY
		printk("ess-uniphy\n");
		ssdk_uniphy_reg_map_info_get(dev_id, &map);
		mode = ssdk_uniphy_reg_access_mode_get(dev_id);
		printk("        reg = <0x%x 0x%x>\n", map.base_addr, map.size);
		if (mode == HSL_REG_LOCAL_BUS)
			printk("        uniphy_access_mode = <local bus>\n");
		else if (mode == HSL_REG_MDIO)
			printk("        uniphy_access_mode = <mdio bus>\n");
		else
			printk("        uniphy_access_mode = <(null)>\n");
#endif
#ifdef HPPE
#ifdef IN_QOS
		printk("\n");
		ssdk_dts_port_scheduler_dump(dev_id);
		printk("\n");
		ssdk_dts_l0scheduler_dump(dev_id);
		printk("\n");
		ssdk_dts_l1scheduler_dump(dev_id);
#endif
#endif
		printk("\n");
		ssdk_dts_phyinfo_dump(dev_id);
	}

	return count;
}

static a_uint16_t phy_reg_val = 0;
static ssize_t ssdk_phy_write_reg_set(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	char phy_buf[32];
	char *this_opt;
	char *options = phy_buf;
	unsigned int phy_addr, reg_addr, reg_value;

	if (count >= sizeof(phy_buf))
		return 0;
	memcpy(phy_buf, buf, count);
	phy_buf[count] = '\0';

	this_opt = strsep(&options, " ");
	if (!this_opt)
		goto fail;

	kstrtouint(this_opt, 0, &phy_addr);
	if ((options - phy_buf) >= (count - 1))
		goto fail;

	this_opt = strsep(&options, " ");
	if (!this_opt)
		goto fail;

	kstrtouint(this_opt, 0, &reg_addr);
	if ((options - phy_buf) >= (count - 1))
		goto fail;

	this_opt = strsep(&options, " ");
	if (!this_opt)
		goto fail;

	kstrtouint(this_opt, 0, &reg_value);

	qca_ar8327_phy_write(0, phy_addr, reg_addr, reg_value);

	return count;

fail:
	printk("Format: phy_addr reg_addr reg_value\n");
	return -EINVAL;
}

static ssize_t ssdk_phy_read_reg_get(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
	ssize_t count;

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "reg_val = 0x%x\n", phy_reg_val);
	return count;
}

static ssize_t ssdk_phy_read_reg_set(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	char phy_buf[32];
	char *this_opt;
	char *options = phy_buf;
	unsigned int phy_addr, reg_addr;

	if (count >= sizeof(phy_buf))
		return 0;
	memcpy(phy_buf, buf, count);
	phy_buf[count] = '\0';

	this_opt = strsep(&options, " ");
	if (!this_opt)
		goto fail;

	kstrtouint(this_opt, 0, &phy_addr);
	if ((options - phy_buf) >= (count - 1))
		goto fail;

	this_opt = strsep(&options, " ");
	if (!this_opt)
		goto fail;

	kstrtouint(this_opt, 0, &reg_addr);

	qca_ar8327_phy_read(0, phy_addr, reg_addr, &phy_reg_val);

	return count;

fail:
	printk("Format: phy_addr reg_addr\n");
	return -EINVAL;
}

#ifdef IN_LINUX_STD_PTP
static ssize_t ssdk_ptp_counter_get(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
	ssize_t count;

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "\n");
	qca808x_ptp_stat_get();
	return count;
}

static ssize_t ssdk_ptp_counter_set(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	qca808x_ptp_stat_set();
	return count;
}
#endif

static const struct device_attribute ssdk_dev_id_attr =
	__ATTR(dev_id, 0660, ssdk_dev_id_get, ssdk_dev_id_set);
static const struct device_attribute ssdk_log_level_attr =
	__ATTR(log_level, 0660, ssdk_log_level_get, ssdk_log_level_set);
static const struct device_attribute ssdk_packet_counter_attr =
	__ATTR(packet_counter, 0660, ssdk_packet_counter_get, ssdk_packet_counter_set);
static const struct device_attribute ssdk_byte_counter_attr =
	__ATTR(byte_counter, 0660, ssdk_byte_counter_get, ssdk_byte_counter_set);
static const struct device_attribute ssdk_dts_dump_attr =
	__ATTR(dts_dump, 0660, ssdk_dts_dump, NULL);
static const struct device_attribute ssdk_phy_write_reg_attr =
	__ATTR(phy_write_reg, 0660, NULL, ssdk_phy_write_reg_set);
static const struct device_attribute ssdk_phy_read_reg_attr =
	__ATTR(phy_read_reg, 0660, ssdk_phy_read_reg_get, ssdk_phy_read_reg_set);
#ifdef IN_LINUX_STD_PTP
static const struct device_attribute ssdk_ptp_counter_attr =
	__ATTR(ptp_packet_counter, 0660, ssdk_ptp_counter_get, ssdk_ptp_counter_set);
#endif
struct kobject *ssdk_sys = NULL;

int ssdk_sysfs_init (void)
{
	int ret = 0;

	/* create /sys/ssdk/ dir */
	ssdk_sys = kobject_create_and_add("ssdk", NULL);
	if (!ssdk_sys) {
		printk("Failed to register SSDK sysfs\n");
		return ret;
	}

	/* create /sys/ssdk/dev_id file */
	ret = sysfs_create_file(ssdk_sys, &ssdk_dev_id_attr.attr);
	if (ret) {
		printk("Failed to register SSDK dev id SysFS file\n");
		goto CLEANUP_1;
	}

	/* create /sys/ssdk/log_level file */
	ret = sysfs_create_file(ssdk_sys, &ssdk_log_level_attr.attr);
	if (ret) {
		printk("Failed to register SSDK log level SysFS file\n");
		goto CLEANUP_2;
	}

	/* create /sys/ssdk/packet_counter file */
	ret = sysfs_create_file(ssdk_sys, &ssdk_packet_counter_attr.attr);
	if (ret) {
		printk("Failed to register SSDK switch counter SysFS file\n");
		goto CLEANUP_3;
	}

	/* create /sys/ssdk/byte_counter file */
	ret = sysfs_create_file(ssdk_sys, &ssdk_byte_counter_attr.attr);
	if (ret) {
		printk("Failed to register SSDK switch counter bytes SysFS file\n");
		goto CLEANUP_4;
	}

	/* create /sys/ssdk/dts_dump file */
	ret = sysfs_create_file(ssdk_sys, &ssdk_dts_dump_attr.attr);
	if (ret) {
		printk("Failed to register SSDK switch show dts SysFS file\n");
		goto CLEANUP_5;
	}

	/* create /sys/ssdk/phy_write_reg file */
	ret = sysfs_create_file(ssdk_sys, &ssdk_phy_write_reg_attr.attr);
	if (ret) {
		printk("Failed to register SSDK phy write reg file\n");
		goto CLEANUP_6;
	}

	/* create /sys/ssdk/phy_read_reg file */
	ret = sysfs_create_file(ssdk_sys, &ssdk_phy_read_reg_attr.attr);
	if (ret) {
		printk("Failed to register SSDK phy read reg file\n");
		goto CLEANUP_7;
	}

#ifdef IN_LINUX_STD_PTP
	/* create /sys/ssdk/ptp_packet_counter file */
	ret = sysfs_create_file(ssdk_sys, &ssdk_ptp_counter_attr.attr);
	if (ret) {
		printk("Failed to register SSDK ptp counter file\n");
		goto CLEANUP_8;
	}
#endif

	return 0;

#ifdef IN_LINUX_STD_PTP
CLEANUP_8:
	sysfs_remove_file(ssdk_sys, &ssdk_phy_read_reg_attr.attr);
#endif
CLEANUP_7:
	sysfs_remove_file(ssdk_sys, &ssdk_phy_write_reg_attr.attr);
CLEANUP_6:
	sysfs_remove_file(ssdk_sys, &ssdk_dts_dump_attr.attr);
CLEANUP_5:
	sysfs_remove_file(ssdk_sys, &ssdk_byte_counter_attr.attr);
CLEANUP_4:
	sysfs_remove_file(ssdk_sys, &ssdk_packet_counter_attr.attr);
CLEANUP_3:
	sysfs_remove_file(ssdk_sys, &ssdk_log_level_attr.attr);
CLEANUP_2:
	sysfs_remove_file(ssdk_sys, &ssdk_dev_id_attr.attr);
CLEANUP_1:
	kobject_put(ssdk_sys);

	return ret;
}

void ssdk_sysfs_exit (void)
{
#ifdef IN_LINUX_STD_PTP
	sysfs_remove_file(ssdk_sys, &ssdk_ptp_counter_attr.attr);
#endif
	sysfs_remove_file(ssdk_sys, &ssdk_phy_read_reg_attr.attr);
	sysfs_remove_file(ssdk_sys, &ssdk_phy_write_reg_attr.attr);
	sysfs_remove_file(ssdk_sys, &ssdk_dts_dump_attr.attr);
	sysfs_remove_file(ssdk_sys, &ssdk_byte_counter_attr.attr);
	sysfs_remove_file(ssdk_sys, &ssdk_packet_counter_attr.attr);
	sysfs_remove_file(ssdk_sys, &ssdk_log_level_attr.attr);
	sysfs_remove_file(ssdk_sys, &ssdk_dev_id_attr.attr);
	kobject_put(ssdk_sys);
}

/*qca808x_start*/
int
ssdk_plat_init(ssdk_init_cfg *cfg, a_uint32_t dev_id)
{
/*qca808x_end*/
	hsl_reg_mode reg_mode;
	ssdk_reg_map_info map;
	struct clk *  ess_clk;
	struct clk *  cmn_clk;
	#ifdef BOARD_AR71XX
	int rv = 0;
	#endif
/*qca808x_start*/
	printk("ssdk_plat_init start\n");
/*qca808x_end*/
	mutex_init(&switch_mdio_lock);

	if(!ssdk_is_emulation(dev_id)){
/*qca808x_start*/
		if(miibus_get(dev_id))
			return -ENODEV;
/*qca808x_end*/
	}
#ifdef IN_UNIPHY
	reg_mode = ssdk_uniphy_reg_access_mode_get(dev_id);
	if(reg_mode == HSL_REG_LOCAL_BUS) {
		ssdk_uniphy_reg_map_info_get(dev_id, &map);
		qca_phy_priv_global[dev_id]->uniphy_hw_addr = ioremap_nocache(map.base_addr,
									map.size);
		if (!qca_phy_priv_global[dev_id]->uniphy_hw_addr) {
			SSDK_ERROR("%s ioremap fail.", __func__);
			cfg->reg_func.uniphy_reg_set = NULL;
			cfg->reg_func.uniphy_reg_get = NULL;
			return -1;
		}
		cfg->reg_func.uniphy_reg_set = qca_uniphy_reg_write;
		cfg->reg_func.uniphy_reg_get = qca_uniphy_reg_read;
	}
#endif
	reg_mode = ssdk_switch_reg_access_mode_get(dev_id);
	if(reg_mode == HSL_REG_LOCAL_BUS) {
		ssdk_switch_reg_map_info_get(dev_id, &map);
		qca_phy_priv_global[dev_id]->hw_addr = ioremap_nocache(map.base_addr,
								map.size);
		if (!qca_phy_priv_global[dev_id]->hw_addr) {
			SSDK_ERROR("%s ioremap fail.", __func__);
			return -1;
		}
		ess_clk = ssdk_dts_essclk_get(dev_id);
		cmn_clk = ssdk_dts_cmnclk_get(dev_id);
		if (!IS_ERR(ess_clk)) {
			/* Enable ess clock here */
			SSDK_INFO("Enable ess clk\n");
			clk_prepare_enable(ess_clk);
		} else if (!IS_ERR(cmn_clk)) {
#if defined(HPPE) || defined(MP)
			ssdk_gcc_clock_init();
#endif
			return 0;
		}

		cfg->reg_mode = HSL_HEADER;
	}
#ifdef DESS
	reg_mode = ssdk_psgmii_reg_access_mode_get(dev_id);
	if(reg_mode == HSL_REG_LOCAL_BUS) {
		ssdk_psgmii_reg_map_info_get(dev_id, &map);
		if (!request_mem_region(map.base_addr,
					map.size, "psgmii_mem")) {
			SSDK_ERROR("%s Unable to request psgmii resource.", __func__);
			return -1;
		}

		qca_phy_priv_global[dev_id]->psgmii_hw_addr = ioremap_nocache(map.base_addr,
								map.size);
		if (!qca_phy_priv_global[dev_id]->psgmii_hw_addr) {
			SSDK_ERROR("%s ioremap fail.", __func__);
			cfg->reg_func.psgmii_reg_set = NULL;
			cfg->reg_func.psgmii_reg_get = NULL;
			return -1;
		}

		cfg->reg_func.psgmii_reg_set = qca_psgmii_reg_write;
		cfg->reg_func.psgmii_reg_get = qca_psgmii_reg_read;
	}
#endif
	reg_mode = ssdk_switch_reg_access_mode_get(dev_id);
	if(reg_mode == HSL_REG_MDIO) {
		cfg->reg_mode = HSL_MDIO;
	} else
		return 0;
/*qca808x_start*/

	return 0;
}

void
ssdk_plat_exit(a_uint32_t dev_id)
{
/*qca808x_end*/
	hsl_reg_mode reg_mode;
#ifdef DESS
	ssdk_reg_map_info map;
#endif
/*qca808x_start*/
	printk("ssdk_plat_exit\n");
/*qca808x_end*/
	reg_mode = ssdk_switch_reg_access_mode_get(dev_id);
	if (reg_mode == HSL_REG_LOCAL_BUS) {
		iounmap(qca_phy_priv_global[dev_id]->hw_addr);
	}
#ifdef DESS
	reg_mode = ssdk_psgmii_reg_access_mode_get(dev_id);
	if (reg_mode == HSL_REG_LOCAL_BUS) {
		ssdk_psgmii_reg_map_info_get(dev_id, &map);
		iounmap(qca_phy_priv_global[dev_id]->psgmii_hw_addr);
		release_mem_region(map.base_addr,
                                        map.size);
	}
#endif
#ifdef IN_UNIPHY
	reg_mode = ssdk_uniphy_reg_access_mode_get(dev_id);
	if (reg_mode == HSL_REG_LOCAL_BUS) {
		iounmap(qca_phy_priv_global[dev_id]->uniphy_hw_addr);
	}
#endif
/*qca808x_start*/
}
/*qca808x_end*/

