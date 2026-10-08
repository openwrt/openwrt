// SPDX-License-Identifier: GPL-2.0-only
/*
 * L2 table maintenance for the RTL960x switch core.
 *
 * The flush engine drops the dynamic entries of a port. It backs fast-age on
 * STP transitions and bridge leave, and the vlan_filtering toggle of a bridge,
 * which moves its VLANs between learning domains.
 */

#include <linux/bitfield.h>
#include <linux/regmap.h>

#include "rtl960x_l2.h"

/*
 * L2 flush command registers. Writing the control register with a port mode
 * flush starts it for the ports set in the enable register; completion is
 * signalled when BUSY clears.
 */
#define RTL960X_L2_FLUSH_CTRL_REG		0x17044
#define   RTL960X_L2_FLUSH_CTRL_BUSY_MSK	BIT(0)
#define   RTL960X_L2_FLUSH_CTRL_MODE_MSK	GENMASK(2, 1)
#define   RTL960X_L2_FLUSH_CTRL_MODE_PORT	0
#define   RTL960X_L2_FLUSH_CTRL_STATIC_MSK	BIT(3)
#define   RTL960X_L2_FLUSH_CTRL_DYNAMIC_MSK	BIT(4)
#define RTL960X_L2_FLUSH_EN_REG			0x17048
#define   RTL960X_L2_FLUSH_EN_PORT_MSK		GENMASK(10, 0)

/**
 * rtl960x_l2_flush() - flush the dynamic entries of a port
 * @priv: driver context
 * @port: port whose learned entries are dropped
 *
 * Context: Can sleep. Takes and releases &priv->l2_lock.
 * Return: 0 on success, or a negative error code on failure.
 */
int rtl960x_l2_flush(struct rtl960x_dsa *priv, int port)
{
	u32 val;
	int ret;

	mutex_lock(&priv->l2_lock);

	ret = regmap_write(priv->map, RTL960X_L2_FLUSH_CTRL_REG,
			   FIELD_PREP(RTL960X_L2_FLUSH_CTRL_MODE_MSK,
				      RTL960X_L2_FLUSH_CTRL_MODE_PORT) |
			   FIELD_PREP(RTL960X_L2_FLUSH_CTRL_DYNAMIC_MSK, 1));
	if (ret)
		goto out;

	ret = regmap_write(priv->map, RTL960X_L2_FLUSH_EN_REG,
			   FIELD_PREP(RTL960X_L2_FLUSH_EN_PORT_MSK, BIT(port)));
	if (ret)
		goto out;

	ret = regmap_read_poll_timeout(priv->map, RTL960X_L2_FLUSH_CTRL_REG,
				       val,
				       !FIELD_GET(RTL960X_L2_FLUSH_CTRL_BUSY_MSK, val),
				       10, 100000);

out:
	mutex_unlock(&priv->l2_lock);

	return ret;
}
