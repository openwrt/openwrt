/*
* Description
*
* Copyright (C) 2016-2022 Qin.Xia <qin.xia@siflower.com.cn>
*
* Siflower software
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/bitops.h>
#include <linux/bitfield.h>
#include <linux/of_platform.h>
#include <linux/mfd/syscon.h>
#include <linux/seq_file.h>
#include "dpns_common.h"
#include "dpns.h"
#include "../dpns_common/io.h"


static int dpns_probe(struct platform_device *pdev)
{
	int err;
	COMMON_t* priv = NULL;

	err = dpns_common_probe(pdev);
	if (err)
		return err;

	// init other dpns module
	err = dpns_vlan_probe(pdev);
	if (err)
		goto err_vlan_probe;

	err = dpns_mac_probe(pdev);
	if (err)
		goto err_mac_probe;

	err = dpns_router_probe(pdev);
	if (err)
		goto err_router_probe;

	err = dpns_nat_probe(pdev);
	if (err)
		goto err_nat_probe;

	err = dpns_tmu_probe(pdev);
	if (err)
		goto err_tmu_probe;

	err = dpns_mcast_probe(pdev);
	if (err)
		goto err_mcast_probe;

	err = dpns_acl_probe(pdev);
	if (err)
		goto err_acl_probe;

	priv = platform_get_drvdata(pdev);

	err = register_netdevice_notifier(&priv->netdevice_nb);
	if (err) {
		COMMON_DBG(ERR_LV, "Failed to register netdevice notifier\n");
		goto err_register_netdevice_notifier;
	}

	/** enable mac mib table read_clear*/
	sf_writel(priv, CONFIG_COUNTER_ADDR, CONFIG_RD_CLR_EN | CONFIG_ROLL_OVER_EN | CONFIG_RAM_FLUSH_EN);

	/* avoid tmu packet loss due to modify back pressure */
	sf_writel(priv, MODIFY_MHDRSD_CFG0_3, 0x06060606);
	sf_writel(priv, MODIFY_MHDRSD_CFG4_7, 0x06060606);
	sf_writel(priv, MODIFY_MHDRSD_CFG8_11, 0x06060606);
	sf_writel(priv, MODIFY_MHDRSD_CFG12_15, 0x3d3d3d3d);

	sf_update(priv, EACL_MPP_CFG0_FIFO, EACL_FIFO_AFULL_THRESH,
			FIELD_PREP(EACL_FIFO_AFULL_THRESH, 0x6661));

	printk("End %s\n", __func__);
	return 0;

err_register_netdevice_notifier:
	dpns_acl_remove(pdev);
err_acl_probe:
	dpns_mcast_remove(pdev);
err_mcast_probe:
	dpns_tmu_remove(pdev);
err_tmu_probe:
	dpns_nat_remove(pdev);
err_nat_probe:
	dpns_router_remove(pdev);
err_router_probe:
	dpns_mac_remove(pdev);
err_mac_probe:
	dpns_vlan_remove(pdev);
err_vlan_probe:
	dpns_common_remove(pdev);
	return err;
}

static void dpns_remove(struct platform_device *pdev)
{
	COMMON_t* priv = NULL;

	priv = platform_get_drvdata(pdev);
	if (!priv)
		return;

	// deinit other dpns module
	dpns_vlan_remove(pdev);
	dpns_router_remove(pdev);
	dpns_mac_remove(pdev);
	dpns_nat_remove(pdev);
	dpns_tmu_remove(pdev);
	dpns_mcast_remove(pdev);
	dpns_acl_remove(pdev);

	unregister_netdevice_notifier(&priv->netdevice_nb);

	dpns_common_remove(pdev);

	printk("End %s\n", __func__);
	return;

}

static const struct of_device_id common_match[] = {
	{ .compatible = "siflower,sf21-dpns" },
	{ .compatible = "siflower,dpns" },
	{},
};
MODULE_DEVICE_TABLE(of, common_match);

static struct platform_driver common_driver = {
	.probe	= dpns_probe,
	.remove	= dpns_remove,
	.driver	= {
		.name		= "dpns",
		.of_match_table	= common_match,
	},
};

module_platform_driver(common_driver);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Qin Xia <qin.xia@siflower.com.cn>");
MODULE_DESCRIPTION("DPNS Driver");
