/*
 *  Realtek RTL865X-style built-in ethernet mac driver
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include "re865x.h"

static int re865x_ethtool_get_settings(struct net_device *dev,
				       struct ethtool_cmd *cmd)
{
	return -ENODEV;
}

static int re865x_ethtool_set_settings(struct net_device *dev,
				       struct ethtool_cmd *cmd)
{
	return -ENODEV;
}

static void re865x_ethtool_get_drvinfo(struct net_device *dev,
				       struct ethtool_drvinfo *info)
{
	struct re865x *re = netdev_priv(dev);

	strcpy(info->driver, re->pdev->dev.driver->name);
	strcpy(info->version, DRV_VERSION);
	strcpy(info->bus_info, dev_name(&re->pdev->dev));
}

static u32 re865x_ethtool_get_msglevel(struct net_device *dev)
{
	struct re865x *re = netdev_priv(dev);

	return re->msg_enable;
}

static void re865x_ethtool_set_msglevel(struct net_device *dev, u32 msg_level)
{
	struct re865x *re = netdev_priv(dev);

	re->msg_enable = msg_level;
}

static void re865x_ethtool_get_ringparam(struct net_device *dev,
					 struct ethtool_ringparam *er)
{
	struct re865x *re = netdev_priv(dev);

	er->tx_max_pending = RE865X_RX_RINGS_SIZE;
	er->rx_max_pending = RE865X_TX_RINGS_SIZE;
	er->rx_mini_max_pending = 0;
	er->rx_jumbo_max_pending = 0;

	er->tx_pending = re->tx_ring.num_descs;
	er->rx_pending = re->rx_ring.num_descs;
	er->rx_mini_pending = 0;
	er->rx_jumbo_pending = 0;
}

static int re865x_ethtool_set_ringparam(struct net_device *dev,
					struct ethtool_ringparam *er)
{
	return -EINVAL;
}

struct ethtool_ops re865x_ethtool_ops = {
	.set_settings	= re865x_ethtool_set_settings,
	.get_settings	= re865x_ethtool_get_settings,
	.get_drvinfo	= re865x_ethtool_get_drvinfo,
	.get_msglevel	= re865x_ethtool_get_msglevel,
	.set_msglevel	= re865x_ethtool_set_msglevel,
	.get_ringparam	= re865x_ethtool_get_ringparam,
	.set_ringparam	= re865x_ethtool_set_ringparam,
	.get_link	= ethtool_op_get_link,
	.get_ts_info	= ethtool_op_get_ts_info,
};
