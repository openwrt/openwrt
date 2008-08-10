/*
 *  Atheros AR71xx built-in ethernet mac driver
 *
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  Based on Atheros' AG7100 driver
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include "ag71xx.h"

static int ag71xx_ethtool_get_settings(struct net_device *dev,
				       struct ethtool_cmd *cmd)
{
	struct ag71xx *ag = netdev_priv(dev);
	struct phy_device *phydev = ag->phy_dev;

	if (!phydev)
		return -ENODEV;

	return phy_ethtool_gset(phydev, cmd);
}

static int ag71xx_ethtool_set_settings(struct net_device *dev,
				       struct ethtool_cmd *cmd)
{
	struct ag71xx *ag = netdev_priv(dev);
	struct phy_device *phydev = ag->phy_dev;

	if (!phydev)
		return -ENODEV;

	return phy_ethtool_sset(phydev, cmd);
}

static void ag71xx_ethtool_get_drvinfo(struct net_device *dev,
				       struct ethtool_drvinfo *info)
{
	struct ag71xx *ag = netdev_priv(dev);

	strcpy(info->driver, ag->pdev->dev.driver->name);
	strcpy(info->version, AG71XX_DRV_VERSION);
	strcpy(info->bus_info, ag->pdev->dev.bus_id);
}

static u32 ag71xx_ethtool_get_msglevel(struct net_device *dev)
{
	struct ag71xx *ag = netdev_priv(dev);

	return ag->msg_enable;
}

static void ag71xx_ethtool_set_msglevel(struct net_device *dev, u32 msg_level)
{
	struct ag71xx *ag = netdev_priv(dev);

	ag->msg_enable = msg_level;
}

struct ethtool_ops ag71xx_ethtool_ops = {
	.set_settings	= ag71xx_ethtool_set_settings,
	.get_settings	= ag71xx_ethtool_get_settings,
	.get_drvinfo	= ag71xx_ethtool_get_drvinfo,
	.get_msglevel	= ag71xx_ethtool_get_msglevel,
	.set_msglevel	= ag71xx_ethtool_set_msglevel,
	.get_link	= ethtool_op_get_link,
};
