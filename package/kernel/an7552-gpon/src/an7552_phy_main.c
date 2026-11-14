// SPDX-License-Identifier: GPL-2.0
/*
 * AN7552 xPON - PHY bring-up and teardown
 *
 * The PHY is half of the an7552-gpon platform device, not a device of its
 * own: an7552_mac_main.c owns the node, allocates both halves, maps the
 * GPON window and the WAN GDM, and calls in here for the three SerDes
 * windows, the two SCUs, the PHY reset line, the PHY interrupt and the
 * board properties.  The MAC then drives the PHY through an7552_phy.h.
 *
 * What a front end attaches to is not here either - the binding and the
 * transmitter regulator it needs are an7552_laser.c, the registry that calls
 * them.  This file is the platform device and nothing else: map, read the
 * board properties, take the interrupt, and undo it all again.
 */

#include <linux/device.h>
#include <linux/mfd/syscon.h>
#include <linux/of.h>
#include <linux/platform_device.h>

#include "an7552_mac.h"

int an7552_phy_probe(struct platform_device *pdev, struct an7552_phy *priv)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	unsigned long flags;
	u32 val;
	int ret;

	priv->csr = devm_platform_ioremap_resource_byname(pdev, "csr");
	if (IS_ERR(priv->csr))
		return PTR_ERR(priv->csr);
	priv->ana = devm_platform_ioremap_resource_byname(pdev, "ana");
	if (IS_ERR(priv->ana))
		return PTR_ERR(priv->ana);
	priv->pma = devm_platform_ioremap_resource_byname(pdev, "pma");
	if (IS_ERR(priv->pma))
		return PTR_ERR(priv->pma);

	priv->chip_scu = syscon_regmap_lookup_by_phandle(np, "airoha,chip-scu");
	if (IS_ERR(priv->chip_scu))
		return dev_err_probe(dev, PTR_ERR(priv->chip_scu), "no chip SCU\n");
	priv->np_scu = syscon_regmap_lookup_by_phandle(np, "airoha,scu");
	if (IS_ERR(priv->np_scu))
		return dev_err_probe(dev, PTR_ERR(priv->np_scu), "no SCU\n");

	/* RST_CTRL2 bit 0, the bit xPON_Phy_Reset pulses */
	priv->reset = devm_reset_control_get_exclusive(dev, "phy");
	if (IS_ERR(priv->reset))
		return dev_err_probe(dev, PTR_ERR(priv->reset), "no phy reset\n");

	spin_lock_init(&priv->lock);
	ret = devm_mutex_init(dev, &priv->op_lock);
	if (ret)
		return ret;
	INIT_WORK(&priv->event_work, an7552_phy_event_work);
	INIT_DELAYED_WORK(&priv->recover_work, an7552_phy_recover_work);
	INIT_DELAYED_WORK(&priv->poll_work, an7552_phy_poll_work);
	ret = devm_mutex_init(dev, &priv->tx_reg_lock);
	if (ret)
		return ret;

	INIT_WORK(&priv->laser_work, an7552_laser_work);

	/* pon_phy_init(): txPowerEnFlag = 1, tx mode; .data ncpo_online */
	priv->tx_power_en = true;
	priv->ncpo_online = AN7552_NCPO_ONLINE_INIT;
	priv->tx_mode = AN7552_TX_MODE_NORMAL;

	/* phy_tx_amp_setting(): 0..15 */
	if (!of_property_read_u32(np, "airoha,tx-amplitude", &val)) {
		if (val > FIELD_MAX(PHY_TX_AMP_LEVEL))
			return dev_err_probe(dev, -EINVAL,
					     "airoha,tx-amplitude %u > 15\n", val);
		priv->tx_amp = val;
		priv->tx_amp_valid = true;
	}

	/*
	 * phy_trans_model_setting(): with an EN7571 the vendor sets
	 * XPON_SETTING = 0x10f (en7571_trans_model_setting()).
	 *
	 * Read here rather than when a front end attaches.  The four bits are
	 * board wiring of this node, not something the front end owns, and two of
	 * them are needed before the SerDes settles: Mode_Config_7552() derives
	 * rg_tdc_los_inv (bit 6, RX_SD_INV) and rg_ben_inv (bit 7,
	 * BURST_EN_INV) from this word, and a link configured with the wrong
	 * sign there either never comes up or comes up with LOS inverted.  The
	 * vendor sets them with phy_trans_rx_setting()/phy_trans_tx_setting()
	 * (PON_SET_PHY 0x15); a stock XX530v V2 runs with 0x14f, i.e.
	 * RX SD inverted, which is what its DT says.
	 *
	 * Valid from here on unconditionally, so an7552_phy_scu_reset() always
	 * writes the word back and nothing has to read it out of the hardware
	 * to find out what it already held.
	 */
	priv->xpon_trans_val = 0x10f;
	if (of_property_read_bool(np, "airoha,rx-sd-inverted"))
		priv->xpon_trans_val |= XPON_SET_RX_SD_INV;
	if (of_property_read_bool(np, "airoha,tx-sd-inverted"))
		priv->xpon_trans_val |= XPON_SET_TX_SD_INV;
	if (of_property_read_bool(np, "airoha,tx-fault-inverted"))
		priv->xpon_trans_val |= XPON_SET_TX_FAULT_INV;
	if (of_property_read_bool(np, "airoha,burst-en-inverted"))
		priv->xpon_trans_val |= XPON_SET_BURST_EN_INV;
	priv->xpon_trans_val_valid = true;

	priv->mode = AN7552_XPON_GPON;
	if (!of_property_read_u32(np, "airoha,pon-mode", &val)) {
		/* GPON only: vendor mode 0 (EPON) is not supported.  -EOPNOTSUPP
		 * here, from an7552_phy_set_mode() and from an7552_mac_set_mode(),
		 * because the mode is one this driver cannot run and not one the
		 * property got wrong.  The comparison is against the two modes
		 * this driver has, so EPON is refused by being unnamed.
		 */
		if (val != AN7552_XPON_GPON && val != AN7552_XPON_GPON_SYM)
			return dev_err_probe(dev, -EOPNOTSUPP,
					     "unsupported airoha,pon-mode %u\n", val);
		priv->mode = val;
	}

	/* phy_dev_init writes 0xaaab5983; DT may override it */
	priv->guard_delimiter = GPON_DELIMITER_GUARD_DEFAULT;
	of_property_read_u32(np, "airoha,guard-bit-delimiter",
			     &priv->guard_delimiter);
	priv->preamble_valid = !of_property_read_u32(np, "airoha,gpon-preamble",
						     &priv->preamble);
	priv->ext_preamble_valid =
		!of_property_read_u32(np, "airoha,gpon-extend-preamble",
				      &priv->ext_preamble);
	if (!of_property_read_u32(np, "airoha,digital-bit-delay", &val)) {
		if (val > AN7552_PHY_BIT_DELAY_MAX)
			return dev_err_probe(dev, -EINVAL,
					     "airoha,digital-bit-delay %u > 7\n", val);
		priv->digital_bit_delay = val;
		/* 0 in the DT leaves the register at its reset value */
		priv->digital_bit_delay_valid = !!val;
	}

	/*
	 * The PBUS timeout that turns an access the window never answers into
	 * 0xdeadbeef instead of a stalled bus is armed at boot by the SCU
	 * clock driver (939-01, as ecnt_scu.c pbus_timeout_init()).
	 */

	/*
	 * pon_phy_init() runs xPON_Phy_Reset() - the reset pulse and the whole
	 * Mode_Config_7552(), PLL included - before phy_dev_init() touches
	 * anything past csr 0x1ff.  Without it only csr 0x100-0x1ff, ana and pma
	 * answer; +0x000, +0x200..+0x600 (int enable 0x5f0 among them) time out.
	 */
	ret = an7552_phy_reset(priv);
	if (ret)
		return dev_err_probe(dev, ret, "PHY reset failed\n");

	/*
	 * Open the PBUS gate and the transceiver pad before the first access
	 * to the window (phy_dev_init), then mask everything until
	 * power_on (phy_int_config(0, 0)).
	 */
	regmap_clear_bits(priv->np_scu, NP_SCU_RST_ACC_CHK_BMAP,
			  RST_ACC_CHK_PBUS_DISABLE);
	regmap_clear_bits(priv->np_scu, NP_SCU_GPIO_SSR, GPIO_SSR_TOP_LED1_MODE);
	spin_lock_irqsave(&priv->lock, flags);
	an7552_int_config_locked(priv, 0, 0);
	spin_unlock_irqrestore(&priv->lock, flags);

	priv->irq = platform_get_irq_byname(pdev, "phy");
	if (priv->irq < 0)
		return priv->irq;
	ret = devm_request_irq(dev, priv->irq, an7552_phy_irq_handler, 0,
			       AN7552_DRV_NAME, priv);
	if (ret)
		return dev_err_probe(dev, ret, "failed to request PHY IRQ\n");

	return 0;
}

void an7552_phy_remove(struct an7552_phy *priv)
{
	an7552_phy_stop(priv);
	/* The IRQ is devm-managed and freed after this; mask it first. */
	disable_irq(priv->irq);
	cancel_delayed_work_sync(&priv->recover_work);
	cancel_delayed_work_sync(&priv->poll_work);
	cancel_work_sync(&priv->event_work);
	cancel_work_sync(&priv->laser_work);
}
