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
#include "io.h"
#include "ops.h"

static irqreturn_t dpns_irq_handler(int irq, void *dev_id)
{
	//struct COMMON_t *priv = dev_id;

	COMMON_DBG(DBG_LV, "trigger dpns abnormal irq:%d\n", irq);
	disable_irq_nosync(irq);
	return IRQ_NONE;
}

static int dpns_setup_ports(COMMON_t *priv)
{
	COMMON_DBG(INFO_LV, "probe vport count: %d\n", priv->port_count);
	priv->ports = devm_kzalloc(&priv->pdev->dev,
				   priv->port_count * sizeof(dpns_port_t *),
				   GFP_KERNEL);

	if (!priv->ports)
		return -ENOMEM;

	return 0;
}

static void sf_se_init(COMMON_t *priv)
{
	int i;

	sf_writel(priv, CLR_CTRL_RAM_ADDR, 0x1FFFFF);
	sf_writel(priv, CLR_CTRL_TCAM_ADDR, 0x1F);

	/* enable arp up*/
	sf_update(priv, PKT_ERR_STG_CFG2, ARP_REQ_ERR_DROP_EN, ARP_REQ_ERR_UP_EN);

	/* enable intf */
	sf_update(priv, CONFIG2_RGT_ADDR, 0, FIELD_PREP(CONFIG2_INTF_VID_EN, 1));

	sf_writel(priv, SE_TCAM_BLK_CONFIG0,
			FIELD_PREP(TCAM_BLK_CONFIG0_BLK0_CFG, TCAM_VFP) |
			FIELD_PREP(TCAM_BLK_CONFIG0_BLK1_CFG, TCAM_L3UCMCG) |
			FIELD_PREP(TCAM_BLK_CONFIG0_BLK2_CFG, TCAM_L3MCSG) |
			FIELD_PREP(TCAM_BLK_CONFIG0_BLK3_CFG, TCAM_IACL) |
			FIELD_PREP(TCAM_BLK_CONFIG0_BLK4_CFG, TCAM_EACL)
			);

	for (i = 0; i < 5; i++) {
		sf_update(priv, NPU_HW_PROC_PORT01_ADDR + i*4,
				FIELD_PREP(PORT_ERR_PKT_PROC_ACT_L, 3) |
				FIELD_PREP(PORT_ERR_PKT_PROC_ACT_H, 3),
				FIELD_PREP(PORT_ERR_PKT_PROC_ACT_L, ERR_PKT_DROP) |
				FIELD_PREP(PORT_ERR_PKT_PROC_ACT_H, ERR_PKT_DROP)
				);
	}

	sf_writel(priv, NPU_MODEIFY_IPP_CORE_CFG, 0x17666);
}

static void sf_se_fini(COMMON_t *priv)
{
	/** VLAN, ingress, egress, L2, L3 */
	se_reg_set_wait(priv, CLR_CTRL_RAM_ADDR, 0xffff, 0x0, 1);

	/** cleanup 5blocks of tcam */
	se_reg_set_wait(priv, CLR_CTRL_TCAM_ADDR, 0x1f, 0x0, 1);
}

int dpns_common_probe(struct platform_device *pdev)
{
	struct platform_device *dma_pdev;
	struct device_node *dma_node;
	COMMON_t* priv = NULL;
	const char *irq_name;
	int i, err;

	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->iobase = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(priv->iobase)) {
		dev_err(&pdev->dev, "iobase ioremap error!\n");
		return PTR_ERR(priv->iobase);
	}

	priv->ethsys = syscon_regmap_lookup_by_phandle(pdev->dev.of_node,
						       "ethsys");
	if (IS_ERR(priv->ethsys)) {
		dev_err(&pdev->dev, "ethsys lookup error!\n");
		return PTR_ERR(priv->ethsys);
	}

	priv->npu_rstc = devm_reset_control_get(&pdev->dev, "npu");
	if (IS_ERR(priv->npu_rstc)) {
		return dev_err_probe(&pdev->dev, PTR_ERR(priv->npu_rstc),
				     "npu reset lookup error!\n");
	}

	priv->npu2ddr_rstc = devm_reset_control_get(&pdev->dev, "npu2ddr");
	if (IS_ERR(priv->npu2ddr_rstc)) {
		return dev_err_probe(&pdev->dev, PTR_ERR(priv->npu2ddr_rstc),
				     "npu2ddr reset lookup error!\n");
	}

	priv->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(priv->clk)) {
		dev_err(&pdev->dev, "cannot request npu clock!\n");
		return PTR_ERR(priv->clk);
	}

	dma_node = of_parse_phandle(pdev->dev.of_node, "siflower,edma", 0);
	if (!dma_node)
		return -ENODEV;

	dma_pdev = of_find_device_by_node(dma_node);
	of_node_put(dma_node);
	if (!dma_pdev)
		return -ENODEV;

	priv->edma_priv = platform_get_drvdata(dma_pdev);
	if (!priv->edma_priv)
		return -EPROBE_DEFER;

	for (i = 0; i < DPNS_MAX_IRQ_CNT; i++) {
		err = platform_get_irq(pdev, i);
		if (err < 0) {
			printk("dpns get irq:%d faild!\n", i);
			return err;
		}

		priv->dpns_irq[i] = err;
		irq_name = devm_kasprintf(&pdev->dev, GFP_KERNEL,
					  "dpns_irq%d", err);
		if (!irq_name)
			return err;

		err = devm_request_irq(&pdev->dev, priv->dpns_irq[i],
				dpns_irq_handler, 0, irq_name, priv);
		if (err) {
			printk("dpns request irq:%d faild!\n", priv->dpns_irq[i]);
			return err;
		}
	}

	spin_lock_init(&priv->hw_lock);
	platform_set_drvdata(pdev, priv);
	priv->pdev = pdev;
	priv->table_read = dpns_table_read;
	priv->table_write = dpns_table_write;
	priv->intf_add = dpns_intf_table_add;
	priv->intf_del = dpns_intf_table_del;
	priv->tcam_access = dpns_tcam_access;
	priv->tcam_clean = dpns_tcam_clean;
	priv->tcam_update = dpns_tcam_update;
	priv->port_dev_check = dpns_port_dev_check;
	priv->port_by_netdev = dpns_port_by_netdev;
	priv->port_id_by_netdev = dpns_port_id_by_netdev;
	priv->se_wait = se_wait_busy;
	priv->port_count = DPNS_MAX_PORT;
	priv->netdevice_nb.notifier_call = dpns_common_netdevice_event;

	// NPU clock gate
	err = clk_prepare_enable(priv->clk);
	if (err)
		return err;

	// NPU reset and release reset
	err = reset_control_assert(priv->npu_rstc);
	if (err)
		goto err_clk;

	err = reset_control_deassert(priv->npu_rstc);
	if (err)
		goto err_clk;

	err = reset_control_deassert(priv->npu2ddr_rstc);
	if (err)
		goto err_clk;

	for (i = 0; i < DPNS_CNT_M; i++) {
		err = dpns_mem_alloc_init(i);
		if (err < 0)
			goto err_clk;
	}

	sf_se_init(priv);

	err = dpns_setup_ports(priv);
	if (err) {
		COMMON_DBG(ERR_LV, "failed to probe ports\n");
		goto err_probe_ports;
	}

	dpns_common_genl_init(priv);
	printk("End %s\n", __func__);
	return 0;

err_probe_ports:
err_clk:
	clk_disable_unprepare(priv->clk);

	return err;

}

EXPORT_SYMBOL(dpns_common_probe);

int dpns_common_remove(struct platform_device *pdev)
{
	int i;
	COMMON_t* priv = NULL;

	priv = platform_get_drvdata(pdev);
	if (!priv)
		return -ENODATA;

	dpns_destroy_portsarray(priv);
	sf_se_fini(priv);

	dpns_common_genl_exit();
	//remove dpns mem alloc framework
	for (i = 0; i < DPNS_CNT_M; i++)
		dpns_mem_alloc_deinit(i);

	reset_control_assert(priv->npu2ddr_rstc);
	reset_control_assert(priv->npu_rstc);
	clk_disable_unprepare(priv->clk);
	printk("End %s\n", __func__);
	return 0;
}

EXPORT_SYMBOL(dpns_common_remove);


MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Qin Xia <qin.xia@siflower.com.cn>");
MODULE_DESCRIPTION("DPNS Common Driver");
