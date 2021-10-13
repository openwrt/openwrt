/*
 * Copyright (c) 2018, 2020, The Linux Foundation. All rights reserved.
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

#include "sw.h"
#include "fal_port_ctrl.h"
#include "adpt.h"
#include "hsl_api.h"
#include "hsl.h"
#include "sfp_phy.h"
#include "aos_timer.h"
#include "hsl_phy.h"
#include <linux/kconfig.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/phy.h>
#include "ssdk_plat.h"
#include "ssdk_phy_i2c.h"

/******************************************************************************
*
* sfp_phy_init -
*
*/
#if (LINUX_VERSION_CODE < KERNEL_VERSION (5, 0, 0))
#define SFP_PHY_FEATURES        (SUPPORTED_FIBRE | \
                                SUPPORTED_1000baseT_Full | \
                                SUPPORTED_10000baseT_Full | \
                                SUPPORTED_Pause | \
                                SUPPORTED_Asym_Pause)
#else
__ETHTOOL_DECLARE_LINK_MODE_MASK(SFP_PHY_FEATURES) __ro_after_init;
#endif

static int
sfp_phy_probe(struct phy_device *pdev)
{
	SSDK_INFO("sfp phy is probed!\n");
	return 0;
}

static void
sfp_phy_remove(struct phy_device *pdev)
{
	return;
}

static int
sfp_phy_config_aneg(struct phy_device *pdev)
{

	return 0;
}

static int
sfp_phy_aneg_done(struct phy_device *pdev)
{

	return SFP_ANEG_DONE;
}

static int
sfp_read_status(struct phy_device *pdev)
{
	sw_error_t rv;
	fal_port_t port;
	a_uint32_t addr;
	struct qca_phy_priv *priv = pdev->priv;
	struct port_phy_status phy_status;
	adpt_api_t *p_api;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0))
	addr = pdev->mdio.addr;
#else
	addr = pdev->addr;
#endif
	port = qca_ssdk_phy_addr_to_port(priv->device_id, addr);
	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(priv->device_id));
	rv = p_api->adpt_port_phy_status_get(priv->device_id, port, &phy_status);
	pdev->link = phy_status.link_status;
	pdev->speed = phy_status.speed;
	pdev->duplex = phy_status.duplex;

	return rv;
}

static struct phy_driver sfp_phy_driver = {
	.name		= "QCA SFP",
	.phy_id		= SFP_PHY,
	.phy_id_mask = SFP_PHY_MASK,
	.probe		= sfp_phy_probe,
	.remove		= sfp_phy_remove,
	.config_aneg	= sfp_phy_config_aneg,
	.aneg_done	= sfp_phy_aneg_done,
	.read_status	= sfp_read_status,
	.features	= SFP_PHY_FEATURES,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0))
	.mdiodrv.driver	= { .owner = THIS_MODULE },
#else
	.driver		= { .owner = THIS_MODULE },
#endif
};

int sfp_phy_device_setup(a_uint32_t dev_id, a_uint32_t port, a_uint32_t phy_id)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
	struct phy_device *phydev;
	struct qca_phy_priv *priv;
	a_uint32_t addr = 0;
	struct mii_bus *bus;

	priv = ssdk_phy_priv_data_get(dev_id);
	/*create phy device*/
#if defined(IN_PHY_I2C_MODE)
	if (hsl_port_phy_access_type_get(dev_id, port) == PHY_I2C_ACCESS) {
		addr = qca_ssdk_port_to_phy_mdio_fake_addr(dev_id, port);
	} else
#endif
	{
		addr = qca_ssdk_port_to_phy_addr(dev_id, port);
	}
	bus = ssdk_miibus_get_by_device(dev_id);
	phydev = phy_device_create(bus, addr, phy_id, false, NULL);
	if (IS_ERR(phydev) || phydev == NULL) {
		SSDK_ERROR("Failed to create phy device!\n");
		return SW_NOT_SUPPORTED;
	}
	/*register phy device*/
	phy_device_register(phydev);

	phydev->priv = priv;
#endif
	return 0;
}

void sfp_phy_device_remove(a_uint32_t dev_id, a_uint32_t port)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
	struct phy_device *phydev = NULL;
	a_uint32_t addr = 0;
	struct mii_bus *bus;

	bus = ssdk_miibus_get_by_device(dev_id);
#if defined(IN_PHY_I2C_MODE)
	if (hsl_port_phy_access_type_get(dev_id, port) == PHY_I2C_ACCESS) {
		addr = qca_ssdk_port_to_phy_mdio_fake_addr(dev_id, port);
	} else
#endif
	{
		addr = qca_ssdk_port_to_phy_addr(dev_id, port);
	}

	if (addr < PHY_MAX_ADDR)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0))
		if (bus->mdio_map[addr])
			phydev = to_phy_device(&bus->mdio_map[addr]->dev);
#else
		phydev = bus->phy_map[addr];
#endif
	if (phydev)
		phy_device_remove(phydev);
#endif
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 0, 0))
static void sfp_features_init(void)
{
	const int features[] = {
		ETHTOOL_LINK_MODE_FIBRE_BIT,
		ETHTOOL_LINK_MODE_1000baseT_Full_BIT,
		ETHTOOL_LINK_MODE_10000baseT_Full_BIT,
		ETHTOOL_LINK_MODE_Pause_BIT,
		ETHTOOL_LINK_MODE_Asym_Pause_BIT,
	};

	linkmode_set_bit_array(features,
		ARRAY_SIZE(features),
		SFP_PHY_FEATURES);
}
#endif

int sfp_phy_init(a_uint32_t dev_id, a_uint32_t port_bmp)
{
	a_uint32_t port_id = 0;

	SSDK_INFO("sfp phy init for port 0x%x!\n", port_bmp);

	for (port_id = 0; port_id < SW_MAX_NR_PORT; port_id ++) {
		if (port_bmp & (0x1 << port_id)) {
			sfp_phy_device_setup(dev_id, port_id, SFP_PHY);
		}
	}
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 0, 0))
	sfp_features_init();
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0))
	phy_driver_register(&sfp_phy_driver, THIS_MODULE);
#else
	phy_driver_register(&sfp_phy_driver);
#endif
	return 0;
}

void sfp_phy_exit(a_uint32_t dev_id, a_uint32_t port_bmp)
{
	a_uint32_t port_id = 0;

	phy_driver_unregister(&sfp_phy_driver);

	for (port_id = 0; port_id < SW_MAX_NR_PORT; port_id ++) {
		if (port_bmp & (0x1 << port_id)) {
			sfp_phy_device_remove(dev_id, port_id);
		}
	}

}
sw_error_t sfp_phy_interface_get_mode_status(a_uint32_t dev_id,
	a_uint32_t phy_id, fal_port_interface_mode_t *interface_mode_status)
{
	sw_error_t rv = SW_OK;
	a_uint16_t reg_data = 0, sfp_speed = 0;

	rv = qca_phy_i2c_mii_read(dev_id, SFP_E2PROM_ADDR, SFP_SPEED_ADDR,
		&reg_data);
	SW_RTN_ON_ERROR(rv);
	sfp_speed = SFP_TO_SFP_SPEED(reg_data);
	SSDK_DEBUG("sfp_speed:%d\n", sfp_speed);
	if(sfp_speed >= SFP_SPEED_1000M &&
		sfp_speed < SFP_SPEED_2500M)
	{
		*interface_mode_status =  PORT_SGMII_FIBER;
	}
	else if(sfp_speed >= SFP_SPEED_10000M)
	{
		*interface_mode_status =  PORT_10GBASE_R;
	}
	else
	{
		return SW_NOT_SUPPORTED;
	}

	return rv;
}
