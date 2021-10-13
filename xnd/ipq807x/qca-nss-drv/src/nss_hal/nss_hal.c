/*
 **************************************************************************
 * Copyright (c) 2016-2021, The Linux Foundation. All rights reserved.
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
 **************************************************************************
 */

/**
 * nss_hal.c
 *	NSS HAL general APIs.
 */

#include <linux/err.h>
#include <linux/version.h>
#include <linux/firmware.h>
#include <linux/of.h>
#include <linux/irq.h>

#include "nss_hal.h"
#include "nss_arch.h"
#include "nss_core.h"
#include "nss_tx_rx_common.h"
#include "nss_data_plane.h"
#if (NSS_PM_SUPPORT == 1)
#include "nss_pm.h"
#endif
#if (NSS_FABRIC_SCALING_SUPPORT == 1)
#include <linux/fab_scaling.h>
#endif

/*
 * Macros
 */
#define MIN_IMG_SIZE (64*1024)
#define NSS_AP0_IMAGE "qca-nss0.bin"
#define NSS_AP1_IMAGE "qca-nss1.bin"

/*
 * File local/Static variables/functions
 */
static const struct net_device_ops nss_netdev_ops;
static const struct ethtool_ops nss_ethtool_ops;

int nss_hal_firmware_load(struct nss_ctx_instance *nss_ctx, struct platform_device *nss_dev, struct nss_platform_data *npd)
{
	const struct firmware *nss_fw;
	void __iomem *load_mem;
	int rc;

	if (nss_ctx->id == 0) {
		rc = request_firmware(&nss_fw, NSS_AP0_IMAGE, &(nss_dev->dev));
	} else if (nss_ctx->id == 1) {
		rc = request_firmware(&nss_fw, NSS_AP1_IMAGE, &(nss_dev->dev));
	} else {
		nss_warning("%px: Invalid nss dev: %d\n", nss_ctx, nss_ctx->id);
		return -EINVAL;
	}

	/*
	 *  Check if the file read is successful
	 */
	if (rc) {
		nss_info_always("%px: request_firmware failed with err code: %d", nss_ctx, rc);
		return rc;
	}

	if (nss_fw->size < MIN_IMG_SIZE) {
		nss_info_always("%px: nss firmware is truncated, size:%d", nss_ctx, (int)nss_fw->size);
		return rc;
	}

	load_mem = ioremap_nocache(npd->load_addr, nss_fw->size);
	if (!load_mem) {
		nss_info_always("%px: ioremap_nocache failed: %x", nss_ctx, npd->load_addr);
		release_firmware(nss_fw);
		return rc;
	}

	nss_info_always("nss_driver - fw of size %d  bytes copied to load addr: %x, nss_id : %d\n", (int)nss_fw->size, npd->load_addr, nss_dev->id);
	memcpy_toio(load_mem, nss_fw->data, nss_fw->size);
	release_firmware(nss_fw);
	iounmap(load_mem);
	return 0;
}

/*
 * nss_hal_dt_parse_features()
 */
void nss_hal_dt_parse_features(struct device_node *np, struct nss_platform_data *npd)
{
	/*
	 * Read the features in
	 */
	npd->bridge_enabled = of_property_read_bool(np, "qcom,bridge-enabled");
	npd->capwap_enabled = of_property_read_bool(np, "qcom,capwap-enabled");
	npd->clmap_enabled = of_property_read_bool(np, "qcom,clmap-enabled");
	npd->crypto_enabled = of_property_read_bool(np, "qcom,crypto-enabled");
	npd->dtls_enabled = of_property_read_bool(np, "qcom,dtls-enabled");
	npd->gre_enabled = of_property_read_bool(np, "qcom,gre-enabled");
	npd->gre_redir_enabled = of_property_read_bool(np, "qcom,gre-redir-enabled");
	npd->gre_tunnel_enabled = of_property_read_bool(np, "qcom,gre_tunnel_enabled");
	npd->gre_redir_mark_enabled = of_property_read_bool(np, "qcom,gre-redir-mark-enabled");
	npd->igs_enabled = of_property_read_bool(np, "qcom,igs-enabled");
	npd->ipsec_enabled = of_property_read_bool(np, "qcom,ipsec-enabled");
	npd->ipv4_enabled = of_property_read_bool(np, "qcom,ipv4-enabled");
	npd->ipv4_reasm_enabled = of_property_read_bool(np, "qcom,ipv4-reasm-enabled");
	npd->ipv6_enabled = of_property_read_bool(np, "qcom,ipv6-enabled");
	npd->ipv6_reasm_enabled = of_property_read_bool(np, "qcom,ipv6-reasm-enabled");
	npd->l2tpv2_enabled = of_property_read_bool(np, "qcom,l2tpv2-enabled");
	npd->map_t_enabled = of_property_read_bool(np, "qcom,map-t-enabled");
	npd->oam_enabled = of_property_read_bool(np, "qcom,oam-enabled");
	npd->ppe_enabled = of_property_read_bool(np, "qcom,ppe-enabled");
	npd->pppoe_enabled = of_property_read_bool(np, "qcom,pppoe-enabled");
	npd->pptp_enabled = of_property_read_bool(np, "qcom,pptp-enabled");
	npd->portid_enabled = of_property_read_bool(np, "qcom,portid-enabled");
	npd->pvxlan_enabled = of_property_read_bool(np, "qcom,pvxlan-enabled");
	npd->qvpn_enabled = of_property_read_bool(np, "qcom,qvpn-enabled");
	npd->rmnet_rx_enabled = of_property_read_bool(np, "qcom,rmnet_rx-enabled");
	npd->shaping_enabled = of_property_read_bool(np, "qcom,shaping-enabled");
	npd->tls_enabled = of_property_read_bool(np, "qcom,tls-enabled");
	npd->tstamp_enabled = of_property_read_bool(np, "qcom,tstamp-enabled");
	npd->turbo_frequency = of_property_read_bool(np, "qcom,turbo-frequency");
	npd->tun6rd_enabled = of_property_read_bool(np, "qcom,tun6rd-enabled");
	npd->tunipip6_enabled = of_property_read_bool(np, "qcom,tunipip6-enabled");
	npd->vlan_enabled = of_property_read_bool(np, "qcom,vlan-enabled");
	npd->vxlan_enabled = of_property_read_bool(np, "qcom,vxlan-enabled");
	npd->wlanredirect_enabled = of_property_read_bool(np, "qcom,wlanredirect-enabled");
	npd->wifioffload_enabled = of_property_read_bool(np, "qcom,wlan-dataplane-offload-enabled");
	npd->match_enabled = of_property_read_bool(np, "qcom,match-enabled");
	npd->mirror_enabled = of_property_read_bool(np, "qcom,mirror-enabled");
	npd->udp_st_enabled = of_property_read_bool(np, "qcom,udp-st-enabled");
}
/*
 * nss_hal_clean_up_irq()
 */
static void nss_hal_clean_up_irq(struct int_ctx_instance *int_ctx)
{
	if (!int_ctx->irq) {
		return;
	}

	/*
	 * Wait here till the poll is complete.
	 */
	napi_disable(&int_ctx->napi);

	/*
	 * Interrupt can be raised here before free_irq() but as napi is
	 * already disabled, it will be never sheduled from hard_irq
	 * context.
	 */
	irq_clear_status_flags(int_ctx->irq, IRQ_DISABLE_UNLAZY);
	free_irq(int_ctx->irq, int_ctx);
	int_ctx->irq = 0;

	netif_napi_del(&int_ctx->napi);
}

/*
 * nss_hal_register_irq()
 */
static int nss_hal_register_irq(struct nss_ctx_instance *nss_ctx, struct nss_platform_data *npd,
					struct net_device *netdev, int irq_num)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct int_ctx_instance *int_ctx = &nss_ctx->int_ctx[irq_num];
	int err = 0;

	/*
	 * request for IRQs
	 */
	int_ctx->nss_ctx = nss_ctx;
	err = nss_top->hal_ops->request_irq(nss_ctx, npd, irq_num);
	if (err) {
		nss_warning("%px: IRQ request for queue %d failed", nss_ctx, irq_num);
		return err;
	}

	/*
	 * Register NAPI for NSS core interrupt
	 */
	napi_enable(&int_ctx->napi);
	return 0;
}

/*
 * nss_hal_probe()
 *	HLOS device probe callback
 */
int nss_hal_probe(struct platform_device *nss_dev)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_ctx_instance *nss_ctx = NULL;
	struct nss_platform_data *npd = NULL;
	int i, err = 0;
#ifdef NSS_DRV_TSTAMP_ENABLE
	struct net_device *tstamp_ndev = NULL;
#endif

	if (nss_top_main.nss_hal_common_init_done == false) {
		err = nss_top->hal_ops->common_reset(nss_dev);
		if (err) {
			nss_info_always("NSS HAL common init failed\n");
			return -EFAULT;
		}
	}

#if (NSS_DT_SUPPORT == 1)
	if (!nss_dev->dev.of_node) {
		pr_err("nss-driver: Device tree not available\n");
		return -ENODEV;
	}

	npd = nss_top->hal_ops->of_get_pdata(nss_dev);
	if (!npd) {
		return -EFAULT;
	}

	nss_ctx = &nss_top->nss[npd->id];
	nss_ctx->id = npd->id;
	nss_dev->id = nss_ctx->id;
#else
	npd = (struct nss_platform_data *) nss_dev->dev.platform_data;
	nss_ctx = &nss_top->nss[nss_dev->id];
	nss_ctx->id = nss_dev->id;
#endif
	nss_ctx->num_irq = npd->num_irq;
	nss_ctx->nss_top = nss_top;

	/*
	 * dev is required for dma map/unmap
	 */
	nss_ctx->dev = &nss_dev->dev;

	nss_info("%px: NSS_DEV_ID %s\n", nss_ctx, dev_name(&nss_dev->dev));

	/*
	 * Do firmware load from nss-drv if required
	 */
	err = nss_top->hal_ops->firmware_load(nss_ctx, nss_dev, npd);
	if (err) {
		nss_info_always("%px: firmware load from driver failed\n", nss_ctx);
		goto err_init;
	}

	err = nss_top->hal_ops->clock_configure(nss_ctx, nss_dev, npd);
	if (err) {
		nss_info_always("%px: clock configure failed\n", nss_ctx);
		goto err_init;
	}

	/*
	 * Get load address of NSS firmware
	 */
	nss_info("%px: Setting NSS%d Firmware load address to %x\n", nss_ctx, nss_ctx->id, npd->load_addr);
	nss_top->nss[nss_ctx->id].load = npd->load_addr;

	/*
	 * Get virtual and physical memory addresses for nss logical/hardware address maps
	 */

	/*
	 * Virtual address of CSM space
	 */
	nss_ctx->nmap = npd->nmap;

	/*
	 * Physical address of CSM space
	 */
	nss_ctx->nphys = npd->nphys;
	nss_assert(nss_ctx->nphys);

	/*
	 * Virtual address of logical registers space
	 */
	nss_ctx->vmap = npd->vmap;

	/*
	 * Virtual address of QGIC interrupt space
	 */
	nss_ctx->qgic_map = npd->qgic_map;

	/*
	 * Physical address of logical registers space
	 */
	nss_ctx->vphys = npd->vphys;
	nss_info("%d:ctx=%px, vphys=%x, vmap=%px, nphys=%x, nmap=%px", nss_ctx->id,
			nss_ctx, nss_ctx->vphys, nss_ctx->vmap, nss_ctx->nphys, nss_ctx->nmap);

	if (!nss_meminfo_init(nss_ctx)) {
		nss_info_always("%px: meminfo init failed\n", nss_ctx);
		err = -EFAULT;
		goto err_init;
	}

	/*
	 * Initialize the dummy netdevice.
	 */
	init_dummy_netdev(&nss_ctx->napi_ndev);

	for (i = 0; i < npd->num_irq; i++) {
		err = nss_hal_register_irq(nss_ctx, npd, &nss_ctx->napi_ndev, i);
		if (err) {
			goto err_register_irq;
		}
	}

#ifdef NSS_DRV_TSTAMP_ENABLE
	/*
	 * Allocate tstamp net_device and register the net_device
	 */
	if (npd->tstamp_enabled == NSS_FEATURE_ENABLED) {
		tstamp_ndev = nss_tstamp_register_netdev();
		if (!tstamp_ndev) {
			nss_warning("%px: Unable to register the TSTAMP net_device", nss_ctx);
			npd->tstamp_enabled = NSS_FEATURE_NOT_ENABLED;
		}
		nss_top->tstamp_handler_id = nss_dev->id;
		nss_tstamp_register_handler(tstamp_ndev);
	}
#endif
	/*
	 * Initialize the handlers for all interfaces associated with core
	 */
	nss_core_init_handlers(nss_ctx);

	/*
	 * Features that will always be enabled on both cores
	 */
	nss_dynamic_interface_register_handler(nss_ctx);
	nss_n2h_register_handler(nss_ctx);
	nss_project_register_handler(nss_ctx);
#ifdef NSS_DRV_QRFS_ENABLE
	nss_qrfs_register_handler(nss_ctx);
#endif

#ifdef NSS_DRV_C2C_ENABLE
	nss_c2c_tx_register_handler(nss_ctx);
	nss_c2c_rx_register_handler(nss_ctx);
#endif
	nss_unaligned_register_handler(nss_ctx);

	/*
	 * Check functionalities are supported by this NSS core
	 */
#ifdef NSS_DRV_SHAPER_ENABLE
	if (npd->shaping_enabled == NSS_FEATURE_ENABLED) {
		nss_top->shaping_handler_id = nss_dev->id;
		nss_info("%d: NSS shaping is enabled", nss_dev->id);
	}
#endif

	if (npd->ipv4_enabled == NSS_FEATURE_ENABLED) {
		nss_top->ipv4_handler_id = nss_dev->id;
		nss_ipv4_register_handler();

#ifdef NSS_DRV_EDMA_ENABLE
		nss_top->edma_handler_id = nss_dev->id;
		nss_edma_register_handler();
#endif
		nss_eth_rx_register_handler(nss_ctx);
#ifdef NSS_DRV_LAG_ENABLE
		nss_lag_register_handler();
#endif
#ifdef NSS_DRV_TRUSTSEC_ENABLE
		nss_top->trustsec_tx_handler_id = nss_dev->id;
		nss_trustsec_tx_register_handler();
#endif

		nss_top->virt_if_handler_id = nss_dev->id;

		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_GENERIC_REDIR_N2H] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_GENERIC_REDIR_H2N] = nss_dev->id;
	}

#ifdef NSS_DRV_CAPWAP_ENABLE
	if (npd->capwap_enabled == NSS_FEATURE_ENABLED) {
		nss_top->capwap_handler_id = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_CAPWAP_OUTER] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_CAPWAP_HOST_INNER] = nss_dev->id;
	}
#endif

#ifdef NSS_DRV_IPV4_REASM_ENABLE
	if (npd->ipv4_reasm_enabled == NSS_FEATURE_ENABLED) {
		nss_top->ipv4_reasm_handler_id = nss_dev->id;
		nss_ipv4_reasm_register_handler();
	}
#endif

#ifdef NSS_DRV_IPV6_ENABLE
	if (npd->ipv6_enabled == NSS_FEATURE_ENABLED) {
		nss_top->ipv6_handler_id = nss_dev->id;
		nss_ipv6_register_handler();
	}

#ifdef NSS_DRV_IPV6_REASM_ENABLE
	if (npd->ipv6_reasm_enabled == NSS_FEATURE_ENABLED) {
		nss_top->ipv6_reasm_handler_id = nss_dev->id;
		nss_ipv6_reasm_register_handler();
	}
#endif
#endif

#ifdef NSS_DRV_CRYPTO_ENABLE
	/*
	 * TODO: when Crypto is moved to Core-1 it needs to
	 * flush based on nss_top->crypto_enabled
	 */
	if (npd->crypto_enabled == NSS_FEATURE_ENABLED) {
		nss_top->crypto_handler_id = nss_dev->id;
#if defined(NSS_HAL_IPQ807x_SUPPORT) || defined(NSS_HAL_IPQ60XX_SUPPORT) || defined(NSS_HAL_IPQ50XX_SUPPORT)
		nss_crypto_cmn_register_handler();
#else
		nss_top->crypto_enabled = 1;
		nss_crypto_register_handler();
#endif

#if defined(NSS_HAL_IPQ807x_SUPPORT) || defined(NSS_HAL_IPQ60XX_SUPPORT)
		nss_top->dma_handler_id = nss_dev->id;
		nss_dma_register_handler();
#endif
	}
#endif

#ifdef NSS_DRV_IPSEC_ENABLE
	if (npd->ipsec_enabled == NSS_FEATURE_ENABLED) {
		nss_top->ipsec_handler_id = nss_dev->id;
#if defined(NSS_HAL_IPQ807x_SUPPORT) || defined(NSS_HAL_IPQ60XX_SUPPORT) || defined(NSS_HAL_IPQ50XX_SUPPORT)
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_INNER] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_OUTER] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_MDATA_INNER] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_MDATA_OUTER] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_REDIRECT] = nss_dev->id;
		nss_ipsec_cmn_register_handler();
#else
		nss_ipsec_register_handler();
#endif
	}
#endif

	if (npd->wlanredirect_enabled == NSS_FEATURE_ENABLED) {
		nss_top->wlan_handler_id = nss_dev->id;
	}

#ifdef NSS_DRV_TUN6RD_ENABLE
	if (npd->tun6rd_enabled == NSS_FEATURE_ENABLED) {
		nss_top->tun6rd_handler_id = nss_dev->id;
	}
#endif

#ifdef NSS_DRV_PPTP_ENABLE
	if (npd->pptp_enabled == NSS_FEATURE_ENABLED) {
		nss_top->pptp_handler_id = nss_dev->id;
		nss_pptp_register_handler();
	}
#endif

	if (npd->pppoe_enabled == NSS_FEATURE_ENABLED) {
		nss_top->pppoe_handler_id = nss_dev->id;
		nss_pppoe_register_handler();
	}

#ifdef NSS_DRV_PPE_ENABLE
	if (npd->ppe_enabled == NSS_FEATURE_ENABLED) {
		nss_top->ppe_handler_id = nss_dev->id;
		nss_ppe_register_handler();
		nss_ppe_vp_register_handler();
	}
#endif

#ifdef NSS_DRV_L2TP_ENABLE
	if (npd->l2tpv2_enabled == NSS_FEATURE_ENABLED) {
		nss_top->l2tpv2_handler_id = nss_dev->id;
		nss_l2tpv2_register_handler();
	}
#endif

#ifdef NSS_DRV_DTLS_ENABLE
	if (npd->dtls_enabled == NSS_FEATURE_ENABLED) {
		nss_top->dtls_handler_id = nss_dev->id;
#if defined(NSS_HAL_IPQ807x_SUPPORT) || defined(NSS_HAL_IPQ60XX_SUPPORT) || defined(NSS_HAL_IPQ50XX_SUPPORT)
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_DTLS_CMN_INNER] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_DTLS_CMN_OUTER] = nss_dev->id;
		nss_dtls_cmn_register_handler();
#else
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_DTLS] = nss_dev->id;
		nss_dtls_register_handler();
#endif
	}
#endif

#ifdef NSS_DRV_MAPT_ENABLE
	if (npd->map_t_enabled == NSS_FEATURE_ENABLED) {
		nss_top->map_t_handler_id = nss_dev->id;
		nss_map_t_register_handler();
	}
#endif

#ifdef NSS_DRV_TUNIPIP6_ENABLE
	if (npd->tunipip6_enabled == NSS_FEATURE_ENABLED) {
		nss_top->tunipip6_handler_id = nss_dev->id;
		nss_tunipip6_register_handler();
	}
#endif

#ifdef NSS_DRV_GRE_ENABLE
	if (npd->gre_enabled == NSS_FEATURE_ENABLED) {
		nss_top->gre_handler_id = nss_dev->id;
		nss_gre_register_handler();
	}
#endif

#ifdef NSS_DRV_GRE_REDIR_ENABLE
	if (npd->gre_redir_enabled == NSS_FEATURE_ENABLED) {
		nss_top->gre_redir_handler_id = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_WIFI_HOST_INNER] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_WIFI_OFFL_INNER] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_SJACK_INNER] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_OUTER] = nss_dev->id;
		nss_gre_redir_register_handler();
		nss_gre_redir_lag_us_register_handler();
		nss_gre_redir_lag_ds_register_handler();
#ifdef NSS_DRV_SJACK_ENABLE
		nss_top->sjack_handler_id = nss_dev->id;
		nss_sjack_register_handler();
#endif

	}

	if (npd->gre_redir_mark_enabled == NSS_FEATURE_ENABLED) {
		nss_top->gre_redir_mark_handler_id = nss_dev->id;
		nss_gre_redir_mark_register_handler();
	}
#endif

#ifdef NSS_DRV_GRE_TUNNEL_ENABLE
	if (npd->gre_tunnel_enabled == NSS_FEATURE_ENABLED) {
		nss_top->gre_tunnel_handler_id = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_GRE_TUNNEL_INNER] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_GRE_TUNNEL_OUTER] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_GRE_TUNNEL_INLINE_INNER] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_GRE_TUNNEL_INLINE_OUTER] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_GRE_TUNNEL_INNER_EXCEPTION] = nss_dev->id;
	}
#endif

#ifdef NSS_DRV_PORTID_ENABLE
	if (npd->portid_enabled == NSS_FEATURE_ENABLED) {
		nss_top->portid_handler_id = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_PORTID] = nss_dev->id;
		nss_portid_register_handler();
	}
#endif

	if (npd->wifioffload_enabled == NSS_FEATURE_ENABLED) {
		nss_top->wifi_handler_id = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_VAP] = nss_dev->id;
		nss_wifi_register_handler();
		nss_wifili_register_handler();
#ifdef NSS_DRV_WIFI_EXT_VDEV_ENABLE
		nss_wifi_ext_vdev_register_handler();
#endif
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_WIFILI_INTERNAL] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_WIFILI_EXTERNAL0] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_WIFILI_EXTERNAL1] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_WIFI_EXT_VDEV_WDS] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_WIFI_EXT_VDEV_VLAN] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_WIFI_MESH_INNER] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_WIFI_MESH_OUTER] = nss_dev->id;

		/*
		 * Register wifi mac database when offload enabled
		 */
		nss_top->wmdb_handler_id = nss_dev->id;
		nss_wifi_mac_db_register_handler();

		/*
		 * Initialize wifili thread scheme database
		 */
		nss_wifili_thread_scheme_db_init(nss_dev->id);
	}

#ifdef NSS_DRV_OAM_ENABLE
	if (npd->oam_enabled == NSS_FEATURE_ENABLED) {
		nss_top->oam_handler_id = nss_dev->id;
		nss_oam_register_handler();
	}
#endif

#ifdef NSS_DRV_BRIDGE_ENABLE
	if (npd->bridge_enabled == NSS_FEATURE_ENABLED) {
		nss_top->bridge_handler_id = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_BRIDGE] = nss_dev->id;
		nss_bridge_init();
	}
#endif

	if (npd->vlan_enabled == NSS_FEATURE_ENABLED) {
		nss_top->vlan_handler_id = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_VLAN] = nss_dev->id;
		nss_vlan_register_handler();
	}

#ifdef NSS_DRV_QVPN_ENABLE
#if defined(NSS_HAL_IPQ807x_SUPPORT) || defined(NSS_HAL_IPQ60XX_SUPPORT)
	if (npd->qvpn_enabled == NSS_FEATURE_ENABLED) {
		nss_top->qvpn_handler_id = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_QVPN_OUTER] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_QVPN_INNER] = nss_dev->id;
		nss_qvpn_register_handler();
	}
#endif
#endif

#ifdef NSS_DRV_PVXLAN_ENABLE
	if (npd->pvxlan_enabled == NSS_FEATURE_ENABLED) {
		nss_top->pvxlan_handler_id = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_PVXLAN_HOST_INNER] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_PVXLAN_OUTER] = nss_dev->id;
	}
#endif

#ifdef NSS_DRV_RMNET_ENABLE
	if (npd->rmnet_rx_enabled == NSS_FEATURE_ENABLED) {
		nss_top->rmnet_rx_handler_id = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_RMNET_RX_N2H] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_RMNET_RX_H2N] = nss_dev->id;
	}
#endif

#ifdef NSS_DRV_IGS_ENABLE
	if (npd->igs_enabled == NSS_FEATURE_ENABLED) {
		nss_top->igs_handler_id = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_IGS] = nss_dev->id;
		nss_info("%d: NSS IGS is enabled", nss_dev->id);
	}
#endif

#ifdef NSS_DRV_CLMAP_ENABLE
	if (npd->clmap_enabled == NSS_FEATURE_ENABLED) {
		nss_top->clmap_handler_id = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_CLMAP_US] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_CLMAP_DS] = nss_dev->id;
	}
#endif

#ifdef NSS_DRV_VXLAN_ENABLE
	if (npd->vxlan_enabled == NSS_FEATURE_ENABLED) {
		nss_top->vxlan_handler_id = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_VXLAN_INNER] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_VXLAN_OUTER] = nss_dev->id;
		nss_vxlan_init();
	}
#endif

#ifdef NSS_DRV_MATCH_ENABLE
	if (npd->match_enabled == NSS_FEATURE_ENABLED) {
		nss_top->match_handler_id = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_MATCH] = nss_dev->id;
		nss_match_init();
	}
#endif

#ifdef NSS_DRV_TLS_ENABLE
#if defined(NSS_HAL_IPQ807x_SUPPORT) || defined(NSS_HAL_IPQ60XX_SUPPORT)
	if (npd->tls_enabled == NSS_FEATURE_ENABLED) {
		nss_top->tls_handler_id = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_TLS_INNER] = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_TLS_OUTER] = nss_dev->id;
		nss_tls_register_handler();
	}
#endif
#endif

#ifdef NSS_DRV_MIRROR_ENABLE
	if (npd->mirror_enabled == NSS_FEATURE_ENABLED) {
		nss_top->mirror_handler_id = nss_dev->id;
		nss_top->dynamic_interface_table[NSS_DYNAMIC_INTERFACE_TYPE_MIRROR] = nss_dev->id;
		nss_mirror_register_handler();
		nss_info("%d: NSS mirror is enabled", nss_dev->id);
	}

#endif

#ifdef NSS_DRV_UDP_ST_ENABLE
	if (npd->udp_st_enabled == NSS_FEATURE_ENABLED) {
		nss_top->udp_st_handler_id = nss_dev->id;
		nss_udp_st_register_handler(nss_ctx);
	}
#endif

	if (nss_ctx->id == 0) {
#if (NSS_FREQ_SCALE_SUPPORT == 1)
		nss_freq_register_handler();

		/*
		 * Init CPU usage detail
		 * Note: As of now, ubi cpu usage is supported only for core0
		 */
		nss_freq_init_cpu_usage();
#endif

		nss_lso_rx_register_handler(nss_ctx);
	}

	nss_top->frequency_handler_id = nss_dev->id;

	/*
	 * Initialize decongestion callbacks to NULL
	 */
	for (i = 0; i < NSS_MAX_CLIENTS; i++) {
		nss_ctx->queue_decongestion_callback[i] = 0;
		nss_ctx->queue_decongestion_ctx[i] = 0;
	}

	spin_lock_init(&(nss_ctx->decongest_cb_lock));
	nss_ctx->magic = NSS_CTX_MAGIC;

	nss_info("%px: Reseting NSS core %d now", nss_ctx, nss_ctx->id);

	/*
	 * Enable clocks and bring NSS core out of reset
	 */
	err = nss_top->hal_ops->core_reset(nss_dev, nss_ctx->nmap, nss_ctx->load, nss_top->clk_src);
	if (err) {
		goto err_register_irq;
	}

	/*
	 * Initialize max buffer size for NSS core
	 */
	nss_ctx->max_buf_size = NSS_NBUF_PAYLOAD_SIZE;

	/*
	 * Initialize S/G status pointers to NULL
	 */
	for (i = 0; i < NSS_N2H_DESC_RING_NUM; i++) {
		nss_ctx->n2h_desc_ring[i].head = NULL;
		nss_ctx->n2h_desc_ring[i].tail = NULL;
		nss_ctx->n2h_desc_ring[i].jumbo_start = NULL;
	}

	/*
	 * Enable interrupts for NSS core.
	 */
	for (i = 0; i < npd->num_irq; i++) {
		nss_hal_enable_interrupt(nss_ctx, nss_ctx->int_ctx[i].shift_factor, NSS_HAL_SUPPORTED_INTERRUPTS);
	}

	nss_info("%px: All resources initialized and nss core%d has been brought out of reset", nss_ctx, nss_dev->id);
	goto out;

err_register_irq:
	for (i = 0; i < npd->num_irq; i++) {
		nss_hal_clean_up_irq(&nss_ctx->int_ctx[i]);
	}

err_init:
	if (nss_dev->dev.of_node) {
		if (npd->nmap) {
			iounmap(npd->nmap);
		}

		if (npd->vmap) {
			iounmap(npd->vmap);
		}
	}

out:
	if (nss_dev->dev.of_node) {
		devm_kfree(&nss_dev->dev, npd);
	}
	return err;
}

/*
 * nss_hal_remove()
 *	HLOS device remove callback
 */
int nss_hal_remove(struct platform_device *nss_dev)
{
	struct nss_top_instance *nss_top = &nss_top_main;
	struct nss_ctx_instance *nss_ctx = &nss_top->nss[nss_dev->id];
	int i;

	/*
	 * Clean up debugfs
	 */
	nss_stats_clean();

	/*
	 * Clear up the resources associated with the interrupt
	 */
	for (i = 0; i < nss_ctx->num_irq; i++) {
		nss_hal_disable_interrupt(nss_ctx, nss_ctx->int_ctx[i].shift_factor,
					  NSS_HAL_SUPPORTED_INTERRUPTS);
		nss_hal_clean_up_irq(&nss_ctx->int_ctx[i]);
	}

	/*
	 * nss-drv is exiting, unregister and restore host data plane
	 */
	nss_top->data_plane_ops->data_plane_unregister();

#if (NSS_FABRIC_SCALING_SUPPORT == 1)
	fab_scaling_unregister(nss_core0_clk);
#endif

	if (nss_dev->dev.of_node) {
		if (nss_ctx->nmap) {
			iounmap(nss_ctx->nmap);
			nss_ctx->nmap = 0;
		}

		if (nss_ctx->vmap) {
			iounmap(nss_ctx->vmap);
			nss_ctx->vmap = 0;
		}
	}

	nss_info("%px: All resources freed for nss core%d", nss_ctx, nss_dev->id);
	return 0;
}
