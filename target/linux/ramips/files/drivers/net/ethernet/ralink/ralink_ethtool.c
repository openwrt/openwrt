/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; version 2 of the License
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2009-2013 Michael Lee <igvtee@gmail.com>
 */

#include "ralink_soc_eth.h"

static const char fe_gdma_str[][ETH_GSTRING_LEN] = {
#define _FE(x...)	# x,
FE_STAT_REG_DECLARE
#undef _FE
};

static int fe_get_settings(struct net_device *dev,
		struct ethtool_cmd *cmd)
{
	struct fe_priv *priv = netdev_priv(dev);
	int err;

	if (!priv->phy_dev)
		goto out_gset;

	if (priv->phy_flags == FE_PHY_FLAG_ATTACH) {
		err = phy_read_status(priv->phy_dev);
		if (err)
			goto out_gset;
	}

	return phy_ethtool_gset(priv->phy_dev, cmd);

out_gset:
	return -ENODEV;
}

static int fe_set_settings(struct net_device *dev,
		struct ethtool_cmd *cmd)
{
	struct fe_priv *priv = netdev_priv(dev);

	if (!priv->phy_dev)
		goto out_sset;

	if (cmd->phy_address != priv->phy_dev->addr) {
		if (priv->phy->phy_node[cmd->phy_address]) {
			priv->phy_dev = priv->phy->phy[cmd->phy_address];
			priv->phy_flags = FE_PHY_FLAG_PORT;
		} else if (priv->mii_bus &&
				priv->mii_bus->phy_map[cmd->phy_address]) {
			priv->phy_dev = priv->mii_bus->phy_map[cmd->phy_address];
			priv->phy_flags = FE_PHY_FLAG_ATTACH;
		} else
			goto out_sset;
	}

	return phy_ethtool_sset(priv->phy_dev, cmd);

out_sset:
	return -ENODEV;
}

static void fe_get_drvinfo (struct net_device *dev,
		struct ethtool_drvinfo *info)
{
	struct fe_priv *priv = netdev_priv(dev);
	struct fe_soc_data *soc = priv->soc;

	strlcpy(info->driver, priv->device->driver->name, sizeof(info->driver));
	strlcpy(info->version, FE_DRV_VERSION, sizeof(info->version));
	strlcpy(info->bus_info, dev_name(priv->device), sizeof(info->bus_info));

	if (soc->reg_table[FE_REG_FE_COUNTER_BASE])
		info->n_stats = ARRAY_SIZE(fe_gdma_str);
}

static u32 fe_get_msglevel(struct net_device *dev)
{
	struct fe_priv *priv = netdev_priv(dev);

	return priv->msg_enable;
}

static void fe_set_msglevel(struct net_device *dev, u32 value)
{
	struct fe_priv *priv = netdev_priv(dev);

	priv->msg_enable = value;
}

static int fe_nway_reset(struct net_device *dev)
{
	struct fe_priv *priv = netdev_priv(dev);

	if (!priv->phy_dev)
		goto out_nway_reset;

	return genphy_restart_aneg(priv->phy_dev);

out_nway_reset:
	return -EOPNOTSUPP;
}

static u32 fe_get_link(struct net_device *dev)
{
	struct fe_priv *priv = netdev_priv(dev);
	int err;

	if (!priv->phy_dev)
		goto out_get_link;

	if (priv->phy_flags == FE_PHY_FLAG_ATTACH) {
		err = genphy_update_link(priv->phy_dev);
		if (err)
			goto out_get_link;
	}

	return priv->phy_dev->link;

out_get_link:
	return ethtool_op_get_link(dev);
}

static void fe_get_ringparam(struct net_device *dev,
		struct ethtool_ringparam *ring)
{
	ring->rx_max_pending = MAX_DMA_DESC;
	ring->tx_max_pending = MAX_DMA_DESC;
	ring->rx_pending = NUM_DMA_DESC;
	ring->tx_pending = NUM_DMA_DESC;
}

static int fe_get_coalesce(struct net_device *dev,
		struct ethtool_coalesce *coal)
{
        u32 delay_cfg = fe_reg_r32(FE_REG_DLY_INT_CFG);

        coal->rx_coalesce_usecs = (delay_cfg & 0xff) * FE_DELAY_TIME;
        coal->rx_max_coalesced_frames = ((delay_cfg >> 8) & 0x7f);
        coal->use_adaptive_rx_coalesce = (delay_cfg >> 15) & 0x1;

        coal->tx_coalesce_usecs = ((delay_cfg >> 16 )& 0xff) * FE_DELAY_TIME;
        coal->tx_max_coalesced_frames = ((delay_cfg >> 24) & 0x7f);
        coal->use_adaptive_tx_coalesce = (delay_cfg >> 31) & 0x1;

        return 0;
}

static int fe_set_coalesce(struct net_device *dev,
		struct ethtool_coalesce *coal)
{
	u32 delay_cfg;
	u32 rx_usecs, tx_usecs;
	u32 rx_frames, tx_frames;

	if (!coal->use_adaptive_rx_coalesce || !coal->use_adaptive_tx_coalesce)
		return -EINVAL;

	rx_usecs = DIV_ROUND_UP(coal->rx_coalesce_usecs, FE_DELAY_TIME);
	rx_frames = coal->rx_max_coalesced_frames;
	tx_usecs = DIV_ROUND_UP(coal->tx_coalesce_usecs, FE_DELAY_TIME);
	tx_frames = coal->tx_max_coalesced_frames;

	if (((tx_usecs == 0) && (tx_frames ==0)) ||
			((rx_usecs == 0) && (rx_frames ==0)))
		return -EINVAL;

	if (rx_usecs > 0xff) rx_usecs = 0xff;
	if (rx_frames > 0x7f) rx_frames = 0x7f;
	if (tx_usecs > 0xff) tx_usecs = 0xff;
	if (tx_frames > 0x7f) tx_frames = 0x7f;

	delay_cfg = ((((FE_DELAY_EN_INT | tx_frames) << 8) | tx_usecs) << 16) |
		(((FE_DELAY_EN_INT | rx_frames) << 8) | rx_usecs);

	fe_reg_w32(delay_cfg, FE_REG_DLY_INT_CFG);

	return 0;
}

static void fe_get_strings(struct net_device *dev, u32 stringset, u8 *data)
{
	switch (stringset) {
	case ETH_SS_STATS:
		memcpy(data, *fe_gdma_str, sizeof(fe_gdma_str));
		break;
	}
}

static int fe_get_sset_count(struct net_device *dev, int sset)
{
	switch (sset) {
	case ETH_SS_STATS:
		return ARRAY_SIZE(fe_gdma_str);
	default:
		return -EOPNOTSUPP;
	}
}

static void fe_get_ethtool_stats(struct net_device *dev,
		struct ethtool_stats *stats, u64 *data)
{
	struct fe_priv *priv = netdev_priv(dev);
	struct fe_hw_stats *hwstats = priv->hw_stats;
	u64 *data_src, *data_dst;
	unsigned int start;
	int i;

	if (netif_running(dev) && netif_device_present(dev)) {
		if (spin_trylock(&hwstats->stats_lock)) {
			fe_stats_update(priv);
			spin_unlock(&hwstats->stats_lock);
		}
	}

	do {
		data_src = &hwstats->tx_bytes;
		data_dst = data;
		start = u64_stats_fetch_begin_bh(&hwstats->syncp);

		for (i = 0; i < ARRAY_SIZE(fe_gdma_str); i++)
			*data_dst++ = *data_src++;

	} while (u64_stats_fetch_retry_bh(&hwstats->syncp, start));
}

static struct ethtool_ops fe_ethtool_ops = {
	.get_settings		= fe_get_settings,
	.set_settings		= fe_set_settings,
	.get_drvinfo		= fe_get_drvinfo,
	.get_msglevel		= fe_get_msglevel,
	.set_msglevel		= fe_set_msglevel,
	.nway_reset		= fe_nway_reset,
	.get_link		= fe_get_link,
	.get_ringparam		= fe_get_ringparam,
	.get_coalesce		= fe_get_coalesce,
	.set_coalesce		= fe_set_coalesce,
};

void fe_set_ethtool_ops(struct net_device *netdev)
{
	struct fe_priv *priv = netdev_priv(netdev);
	struct fe_soc_data *soc = priv->soc;

	if (soc->reg_table[FE_REG_FE_COUNTER_BASE]) {
		fe_ethtool_ops.get_strings = fe_get_strings;
		fe_ethtool_ops.get_sset_count = fe_get_sset_count;
		fe_ethtool_ops.get_ethtool_stats = fe_get_ethtool_stats;
	}

	SET_ETHTOOL_OPS(netdev, &fe_ethtool_ops);
}
