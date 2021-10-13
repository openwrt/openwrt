/*
 * Copyright (c) 2018-2019, 2021, The Linux Foundation. All rights reserved.
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

#include "qca808x.h"

#if defined(IN_PHY_I2C_MODE)
#include "sfp_phy.h"
#endif

#define PHY_INVALID_DATA            0xffff
#define QCA808X_INTR_INIT           0xec00

#define QCA808X_PHY_LINK_UP         1
#define QCA808X_PHY_LINK_DOWN       0

LIST_HEAD(g_qca808x_phy_list);

struct qca808x_phy_info* qca808x_phy_info_get(a_uint32_t phy_addr)
{
	struct qca808x_phy_info *pdata = NULL;
	list_for_each_entry(pdata, &g_qca808x_phy_list, list) {
		if (pdata->phydev_addr == phy_addr) {
			return pdata;
		}
	}

	SSDK_ERROR("%s can't get the data for phy addr: %d\n", __func__, phy_addr);
	return NULL;
}

static a_bool_t qca808x_sfp_present(struct phy_device *phydev)
{
	qca808x_priv *priv = phydev->priv;
	struct qca808x_phy_info *pdata = priv->phy_info;
	a_uint32_t phy_id = 0;
	sw_error_t rv = SW_OK;

	if (!pdata) {
		SSDK_ERROR("pdata is null\n");
		return A_FALSE;
	}
	rv = qca808x_phy_get_phy_id(pdata->dev_id, pdata->phy_addr, &phy_id);
	if(rv == SW_READ_ERROR) {
		return A_FALSE;
	}

	return A_TRUE;
}

static sw_error_t qca808x_phy_config_init(struct phy_device *phydev)
{
	a_uint16_t phy_data;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0))
	a_uint32_t features;
#else
	__ETHTOOL_DECLARE_LINK_MODE_MASK(mask) = { 0, };
#endif
	a_uint32_t dev_id = 0, phy_id = 0;
	qca808x_priv *priv = phydev->priv;
	struct qca808x_phy_info *pdata = priv->phy_info;

	if (!pdata) {
		return SW_NOT_FOUND;
	}

	dev_id = pdata->dev_id;
	phy_id = pdata->phy_addr;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0))
	features = SUPPORTED_TP | SUPPORTED_MII |
		SUPPORTED_AUI | SUPPORTED_BNC;
#else
	linkmode_set_bit(ETHTOOL_LINK_MODE_TP_BIT, mask);
	linkmode_set_bit(ETHTOOL_LINK_MODE_MII_BIT, mask);
	linkmode_set_bit(ETHTOOL_LINK_MODE_AUI_BIT, mask);
	linkmode_set_bit(ETHTOOL_LINK_MODE_BNC_BIT, mask);
#endif

	phy_data = qca808x_phy_reg_read(dev_id,
			phy_id, QCA808X_PHY_STATUS);

	if (phy_data == PHY_INVALID_DATA) {
		return SW_READ_ERROR;
	}

	if (phy_data & QCA808X_STATUS_AUTONEG_CAPS) {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0))
		features |= SUPPORTED_Autoneg;
#else
		linkmode_set_bit(ETHTOOL_LINK_MODE_Autoneg_BIT, mask);
#endif
	}
	if (phy_data & QCA808X_STATUS_100TX_FD_CAPS) {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0))
		features |= SUPPORTED_100baseT_Full;
#else
		linkmode_set_bit(ETHTOOL_LINK_MODE_100baseT_Full_BIT, mask);
#endif
	}
	if (phy_data & QCA808X_STATUS_100TX_HD_CAPS) {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0))
		features |= SUPPORTED_100baseT_Half;
#else
		linkmode_set_bit(ETHTOOL_LINK_MODE_100baseT_Half_BIT, mask);
#endif
	}
	if (phy_data & QCA808X_STATUS_10T_FD_CAPS) {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0))
		features |= SUPPORTED_10baseT_Full;
#else
		linkmode_set_bit(ETHTOOL_LINK_MODE_10baseT_Full_BIT, mask);
#endif
	}
	if (phy_data & QCA808X_STATUS_10T_HD_CAPS) {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0))
		features |= SUPPORTED_10baseT_Half;
#else
		linkmode_set_bit(ETHTOOL_LINK_MODE_10baseT_Half_BIT, mask);
#endif
	}

	if (phy_data & QCA808X_STATUS_EXTENDED_STATUS) {
		phy_data = qca808x_phy_reg_read(dev_id,
				phy_id, QCA808X_EXTENDED_STATUS);

		if (phy_data == PHY_INVALID_DATA) {
			return SW_READ_ERROR;
		}
		if (phy_data & QCA808X_STATUS_1000T_FD_CAPS) {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0))
			features |= SUPPORTED_1000baseT_Full;
#else
			linkmode_set_bit(ETHTOOL_LINK_MODE_1000baseT_Full_BIT,
					mask);
#endif
		}
		if (phy_data & QCA808X_STATUS_1000T_HD_CAPS) {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0))
			features |= SUPPORTED_1000baseT_Half;
#else
			linkmode_set_bit(ETHTOOL_LINK_MODE_1000baseT_Half_BIT,
					mask);
#endif
		}
	}

	phy_data = qca808x_phy_mmd_read(dev_id, phy_id, QCA808X_PHY_MMD1_NUM,
			QCA808X_MMD1_PMA_CAP_REG);

	if (phy_data & QCA808X_STATUS_2500T_FD_CAPS) {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0))
		features |= SUPPORTED_2500baseX_Full;
#else
		linkmode_set_bit(ETHTOOL_LINK_MODE_2500baseT_Full_BIT, mask);
#endif
	}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0))
	phydev->supported = features;
	phydev->advertising = features;
#else
	linkmode_copy(phydev->supported, mask);
	linkmode_copy(phydev->advertising, mask);
#endif

	return SW_OK;
}

static a_bool_t qca808x_config_init_done = A_FALSE;
static int _qca808x_config_init(struct phy_device *phydev)
{
	int ret = 0;
#if defined(IN_LINUX_STD_PTP)
	/* ptp function initialization */
	ret |= qca808x_ptp_config_init(phydev);
#endif

	ret |= qca808x_phy_config_init(phydev);

	return ret;
}

static int qca808x_config_init(struct phy_device *phydev)
{
	int ret = 0;

	if(!qca808x_sfp_present(phydev))
	{
		return 0;
	}
	ret = _qca808x_config_init(phydev);
	if(!ret)
	{
		qca808x_config_init_done = A_TRUE;
	}

	return ret;
}

static int qca808x_config_intr(struct phy_device *phydev)
{
	int err;
	a_uint16_t phy_data;
	a_uint32_t dev_id = 0, phy_id = 0;
	qca808x_priv *priv = phydev->priv;
	const struct qca808x_phy_info *pdata = priv->phy_info;

	if (!pdata) {
		return SW_FAIL;
	}

	dev_id = pdata->dev_id;
	phy_id = pdata->phy_addr;

	phy_data = qca808x_phy_reg_read(dev_id, phy_id,
			QCA808X_PHY_INTR_MASK);

	if (phydev->interrupts == PHY_INTERRUPT_ENABLED) {
		err = qca808x_phy_reg_write(dev_id, phy_id,
				QCA808X_PHY_INTR_MASK,
				phy_data | QCA808X_INTR_INIT);
	} else {
		err = qca808x_phy_reg_write(dev_id, phy_id,
				QCA808X_PHY_INTR_MASK, 0);
	}

	return err;
}

static int qca808x_ack_interrupt(struct phy_device *phydev)
{
	int err;
	a_uint32_t dev_id = 0, phy_id = 0;
	qca808x_priv *priv = phydev->priv;
	const struct qca808x_phy_info *pdata = priv->phy_info;

	if (!pdata) {
		return SW_FAIL;
	}

	dev_id = pdata->dev_id;
	phy_id = pdata->phy_addr;

	err = qca808x_phy_reg_read(dev_id, phy_id,
			QCA808X_PHY_INTR_STATUS);

	return (err < 0) ? err : 0;
}

/* switch linux negtiation capability to fal avariable */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0))
static a_uint32_t qca808x_negtiation_cap_get(struct phy_device *phydev)
{
	a_uint32_t autoneg = 0;
	a_uint32_t advertise = phydev->advertising & phydev->supported;

	if (advertise & ADVERTISED_Pause) {
		autoneg |= FAL_PHY_ADV_PAUSE;
	}
	if (advertise & ADVERTISED_Asym_Pause) {
		autoneg |= FAL_PHY_ADV_ASY_PAUSE;
	}
	if (advertise & ADVERTISED_10baseT_Half) {
		autoneg |= FAL_PHY_ADV_10T_HD;
	}
	if (advertise & ADVERTISED_10baseT_Full) {
		autoneg |= FAL_PHY_ADV_10T_FD;
	}
	if (advertise & ADVERTISED_100baseT_Half) {
		autoneg |= FAL_PHY_ADV_100TX_HD;
	}
	if (advertise & ADVERTISED_100baseT_Full) {
		autoneg |= FAL_PHY_ADV_100TX_FD;
	}
	if (advertise & ADVERTISED_1000baseT_Full) {
		autoneg |= FAL_PHY_ADV_1000T_FD;
	}
	if (advertise & ADVERTISED_2500baseX_Full) {
		autoneg |= FAL_PHY_ADV_2500T_FD;
	}

	return autoneg;
}
#else
static a_uint32_t qca808x_negtiation_cap_get(struct phy_device *phydev)
{
	a_uint32_t autoneg = 0;
	__ETHTOOL_DECLARE_LINK_MODE_MASK(advertising) = { 0, };

	linkmode_and(advertising, phydev->advertising, phydev->supported);

	if (linkmode_test_bit(ETHTOOL_LINK_MODE_Pause_BIT, advertising)) {
		autoneg |= FAL_PHY_ADV_PAUSE;
	}
	if (linkmode_test_bit(ETHTOOL_LINK_MODE_Asym_Pause_BIT, advertising)) {
		autoneg |= FAL_PHY_ADV_ASY_PAUSE;
	}
	if (linkmode_test_bit(ETHTOOL_LINK_MODE_10baseT_Half_BIT, advertising)) {
		autoneg |= FAL_PHY_ADV_10T_HD;
	}
	if (linkmode_test_bit(ETHTOOL_LINK_MODE_10baseT_Full_BIT, advertising)) {
		autoneg |= FAL_PHY_ADV_10T_FD;
	}
	if (linkmode_test_bit(ETHTOOL_LINK_MODE_100baseT_Half_BIT, advertising)) {
		autoneg |= FAL_PHY_ADV_100TX_HD;
	}
	if (linkmode_test_bit(ETHTOOL_LINK_MODE_100baseT_Full_BIT, advertising)) {
		autoneg |= FAL_PHY_ADV_100TX_FD;
	}
	if (linkmode_test_bit(ETHTOOL_LINK_MODE_1000baseT_Full_BIT, advertising)) {
		autoneg |= FAL_PHY_ADV_1000T_FD;
	}
	if (linkmode_test_bit(ETHTOOL_LINK_MODE_2500baseT_Full_BIT, advertising)) {
		autoneg |= FAL_PHY_ADV_2500T_FD;
	}

	return autoneg;
}
#endif

static int qca808x_config_aneg(struct phy_device *phydev)
{
	a_uint32_t advertise = 0;
	a_uint16_t phy_data = 0;
	int err = 0;
	a_uint32_t dev_id = 0, phy_id = 0;
	qca808x_priv *priv = phydev->priv;
	const struct qca808x_phy_info *pdata = priv->phy_info;

	if(!qca808x_sfp_present(phydev))
	{
		return 0;
	}

	if (!pdata) {
		return SW_FAIL;
	}

	dev_id = pdata->dev_id;
	phy_id = pdata->phy_addr;
	if (phydev->autoneg != AUTONEG_ENABLE)
	{
		/* force speed */
		phydev->pause = 0;
		phydev->asym_pause = 0;

		phy_data = qca808x_phy_reg_read(dev_id, phy_id, QCA808X_PHY_CONTROL);
		phy_data &= ~QCA808X_CTRL_AUTONEGOTIATION_ENABLE;
		if (phydev->duplex == FAL_FULL_DUPLEX) {
			phy_data |= QCA808X_CTRL_FULL_DUPLEX;
		} else {
			phy_data &= ~QCA808X_CTRL_FULL_DUPLEX;
		}
		qca808x_phy_reg_write(dev_id, phy_id, QCA808X_PHY_CONTROL, phy_data);
		err = qca808x_phy_set_force_speed(dev_id, phy_id, phydev->speed);
	} else {
		/* autoneg enabled */
		advertise = qca808x_negtiation_cap_get(phydev);
		err |= qca808x_phy_set_autoneg_adv(dev_id, phy_id, advertise);
		err |= qca808x_phy_restart_autoneg(dev_id, phy_id);
	}

	return err;
}

static int qca808x_aneg_done(struct phy_device *phydev)
{

	a_uint16_t phy_data;
	a_uint32_t dev_id = 0, phy_id = 0;
	qca808x_priv *priv = phydev->priv;
	const struct qca808x_phy_info *pdata = priv->phy_info;

	if (!pdata) {
		return SW_FAIL;
	}

	dev_id = pdata->dev_id;
	phy_id = pdata->phy_addr;

	phy_data = qca808x_phy_reg_read(dev_id, phy_id,
			QCA808X_PHY_STATUS);

	return (phy_data < 0) ? phy_data : (phy_data & QCA808X_STATUS_AUTO_NEG_DONE);
}

static int qca808x_read_status(struct phy_device *phydev)
{
	struct port_phy_status phy_status = {0};
	a_uint32_t dev_id = 0, phy_id = 0;
	qca808x_priv *priv = phydev->priv;
	const struct qca808x_phy_info *pdata = priv->phy_info;

	if(!qca808x_config_init_done)
	{
		if(!_qca808x_config_init(phydev))
		{
			qca808x_config_init_done = A_TRUE;
		}
	}

	if (!pdata) {
		return SW_FAIL;
	}

	dev_id = pdata->dev_id;
	phy_id = pdata->phy_addr;

	qca808x_phy_get_status(dev_id, phy_id, &phy_status);

	if (phy_status.link_status) {
		phydev->link = QCA808X_PHY_LINK_UP;
	} else {
		phydev->link = QCA808X_PHY_LINK_DOWN;
	}

	switch (phy_status.speed) {
		case FAL_SPEED_2500:
			phydev->speed = SPEED_2500;
			break;
		case FAL_SPEED_1000:
			phydev->speed = SPEED_1000;
			break;
		case FAL_SPEED_100:
			phydev->speed = SPEED_100;
			break;
		default:
			phydev->speed = SPEED_10;
			break;
	}

	if (phy_status.duplex == FAL_FULL_DUPLEX) {
		phydev->duplex = DUPLEX_FULL;
	} else {
		phydev->duplex = DUPLEX_HALF;
	}

	return 0;
}

static int qca808x_suspend(struct phy_device *phydev)
{
	a_uint32_t dev_id = 0, phy_id = 0;
	qca808x_priv *priv = phydev->priv;
	const struct qca808x_phy_info *pdata = priv->phy_info;

	if (!pdata) {
		return SW_FAIL;
	}

	dev_id = pdata->dev_id;
	phy_id = pdata->phy_addr;

	return qca808x_phy_poweroff(dev_id, phy_id);
}

static int qca808x_resume(struct phy_device *phydev)
{
	a_uint32_t dev_id = 0, phy_id = 0;
	qca808x_priv *priv = phydev->priv;
	const struct qca808x_phy_info *pdata = priv->phy_info;

	if (!pdata) {
		return SW_FAIL;
	}

	dev_id = pdata->dev_id;
	phy_id = pdata->phy_addr;

	return qca808x_phy_poweron(dev_id, phy_id);
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
static int qca808x_soft_reset(struct phy_device *phydev)
{
	a_uint32_t dev_id = 0, phy_id = 0;
	qca808x_priv *priv = phydev->priv;
	const struct qca808x_phy_info *pdata = priv->phy_info;

	if(!qca808x_sfp_present(phydev)) {
		return 0;
	}

	if (!pdata) {
		return SW_FAIL;
	}

	dev_id = pdata->dev_id;
	phy_id = pdata->phy_addr;

	return qca808x_phy_reset(dev_id, phy_id);
}

static void qca808x_link_change_notify(struct phy_device *phydev)
{
#if defined(IN_LINUX_STD_PTP)
	qca808x_ptp_change_notify(phydev);
#endif
}
#endif

static int qca808x_phy_probe(struct phy_device *phydev)
{
	qca808x_priv *priv;
	int err = 0;

	priv = kzalloc(sizeof(qca808x_priv), GFP_KERNEL);
	if (!priv) {
		return -ENOMEM;
	}

	priv->phydev = phydev;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0))
	priv->phy_info = qca808x_phy_info_get(phydev->addr);
#else
	priv->phy_info = qca808x_phy_info_get(phydev->mdio.addr);
#endif
	phydev->priv = priv;

#if defined(IN_LINUX_STD_PTP)
	err = qca808x_ptp_init(priv);
#endif

	return err;
}

static void qca808x_phy_remove(struct phy_device *phydev)
{
	qca808x_priv *priv = phydev->priv;

#if defined(IN_LINUX_STD_PTP)
	qca808x_ptp_deinit(priv);
#endif
	kfree(priv);
}

struct phy_driver qca808x_phy_driver = {
	.phy_id		= QCA8081_PHY_V1_1,
	.phy_id_mask    = 0xffffffff,
	.name		= "QCA808X ethernet",
	.features	= PHY_GBIT_FEATURES,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0))
	.flags		= PHY_HAS_INTERRUPT,
#endif
	.probe		= qca808x_phy_probe,
	.remove		= qca808x_phy_remove,
	.config_init	= qca808x_config_init,
	.config_intr	= qca808x_config_intr,
	.config_aneg	= qca808x_config_aneg,
	.aneg_done	= qca808x_aneg_done,
	.ack_interrupt	= qca808x_ack_interrupt,
	.read_status	= qca808x_read_status,
	.suspend	= qca808x_suspend,
	.resume		= qca808x_resume,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
	.soft_reset	= qca808x_soft_reset,
	.link_change_notify     = qca808x_link_change_notify,
#endif
#if defined(IN_LINUX_STD_PTP)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0))
	.ts_info	= qca808x_ts_info,
#endif
	.hwtstamp	= qca808x_hwtstamp,
	.rxtstamp	= qca808x_rxtstamp,
	.txtstamp	= qca808x_txtstamp,
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0))
	.mdiodrv.driver		= { .owner = THIS_MODULE },
#else
	.driver		= { .owner = THIS_MODULE },
#endif
};

a_int32_t qca808x_phy_driver_register(void)
{
	a_int32_t ret;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0))
	ret = phy_driver_register(&qca808x_phy_driver, THIS_MODULE);
#else
	ret = phy_driver_register(&qca808x_phy_driver);
#endif
	return ret;
}

void qca808x_phy_driver_unregister(void)
{
	phy_driver_unregister(&qca808x_phy_driver);
}

void qca808x_phydev_init(a_uint32_t dev_id, a_uint32_t port_id)
{
	struct qca808x_phy_info *pdata;
	pdata = kzalloc(sizeof(struct qca808x_phy_info), GFP_KERNEL);

	if (!pdata) {
		return;
	}
	list_add_tail(&pdata->list, &g_qca808x_phy_list);
	pdata->dev_id = dev_id;
	/* the phy address may be the i2c slave addr or mdio addr */
	pdata->phy_addr = qca_ssdk_port_to_phy_addr(dev_id, port_id);
	pdata->phydev_addr = pdata->phy_addr;
#if defined(IN_PHY_I2C_MODE)
	/* in i2c mode, need to register a fake phy device
	 * before the phy driver register */
	if (hsl_port_phy_access_type_get(dev_id, port_id) == PHY_I2C_ACCESS) {
		a_uint32_t phy_id = QCA8081_PHY_V1_1;
		qca808x_phy_get_phy_id(dev_id, pdata->phy_addr, &phy_id);
		if(phy_id != QCA8081_PHY_V1_1 && phy_id != INVALID_PHY_ID) {
			SSDK_ERROR("phy id 0x%x is not supported\n", phy_id);
			return;
		}
		pdata->phydev_addr = qca_ssdk_port_to_phy_mdio_fake_addr(dev_id, port_id);
		sfp_phy_device_setup(dev_id, port_id, phy_id);
	}
#endif
}

void qca808x_phydev_deinit(a_uint32_t dev_id, a_uint32_t port_id)
{
	struct qca808x_phy_info *pdata, *pnext;

#if defined(IN_PHY_I2C_MODE)
	/* in i2c mode, need to remove the fake phy device
	 * after the phy driver unregistered */
	if (hsl_port_phy_access_type_get(dev_id, port_id) == PHY_I2C_ACCESS) {
		sfp_phy_device_remove(dev_id, port_id);
	}
#endif
	list_for_each_entry_safe(pdata, pnext, &g_qca808x_phy_list, list) {
		list_del(&pdata->list);
		kfree(pdata);
	}
}
