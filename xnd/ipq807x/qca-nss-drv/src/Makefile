# ###################################################
# # Makefile for the NSS driver
# ###################################################

obj-m += qca-nss-drv.o

#
# List the files that belong to the driver in alphabetical order.
#
qca-nss-drv-objs := \
			nss_cmn.o \
			nss_core.o \
			nss_coredump.o \
			nss_drv_stats.o \
			nss_drv_strings.o \
			nss_dynamic_interface.o \
			nss_dynamic_interface_log.o \
			nss_dynamic_interface_stats.o \
			nss_eth_rx.o \
			nss_eth_rx_stats.o \
			nss_eth_rx_strings.o \
			nss_gmac_stats.o \
			nss_if.o \
			nss_if_log.o \
			nss_init.o \
			nss_ipv4.o \
			nss_ipv4_stats.o \
			nss_ipv4_strings.o \
			nss_ipv4_log.o \
			nss_log.o \
			nss_lso_rx.o \
			nss_lso_rx_stats.o \
			nss_lso_rx_strings.o \
			nss_meminfo.o \
			nss_n2h.o \
			nss_n2h_stats.o \
			nss_n2h_strings.o \
			nss_phys_if.o \
			nss_pm.o \
			nss_profiler.o \
			nss_project.o \
			nss_pppoe.o \
			nss_pppoe_log.o \
			nss_pppoe_stats.o \
			nss_pppoe_strings.o \
			nss_rps.o \
			nss_stats.o \
			nss_strings.o \
			nss_tx_msg_sync.o \
			nss_unaligned.o \
			nss_unaligned_log.o \
			nss_unaligned_stats.o \
			nss_virt_if.o \
			nss_virt_if_stats.o \
			nss_vlan.o \
			nss_vlan_log.o \
			nss_wifi.o \
			nss_wifi_log.o \
			nss_wifi_stats.o \
			nss_wifi_vdev.o \
			nss_wifili.o \
			nss_wifili_log.o \
			nss_wifili_stats.o \
			nss_wifili_strings.o \
			nss_wifi_mac_db.o

# Base NSS data plane/HAL support
qca-nss-drv-objs += nss_data_plane/nss_data_plane_common.o
qca-nss-drv-objs += nss_hal/nss_hal.o

ifneq "$(NSS_DRV_BRIDGE_ENABLE)" "n"
ccflags-y += -DNSS_DRV_BRIDGE_ENABLE
qca-nss-drv-objs += \
		    nss_bridge.o \
		    nss_bridge_log.o
endif

ifneq "$(NSS_DRV_WIFI_EXT_VDEV_ENABLE)" "n"
ccflags-y += -DNSS_DRV_WIFI_EXT_VDEV_ENABLE
qca-nss-drv-objs += \
		    nss_wifi_ext_vdev.o \
		    nss_wifi_ext_vdev_stats.o \
		    nss_wifi_ext_vdev_log.o
endif

ifneq "$(NSS_DRV_WIFI_MESH_ENABLE)" "n"
ccflags-y += -DNSS_DRV_WIFI_MESH_ENABLE
qca-nss-drv-objs += \
		    nss_wifi_mesh.o \
		    nss_wifi_mesh_log.o \
		    nss_wifi_mesh_stats.o \
		    nss_wifi_mesh_strings.o
endif

ifneq "$(NSS_DRV_IPV4_REASM_ENABLE)" "n"
ccflags-y += -DNSS_DRV_IPV4_REASM_ENABLE
qca-nss-drv-objs += \
		    nss_ipv4_reasm.o \
		    nss_ipv4_reasm_stats.o \
		    nss_ipv4_reasm_strings.o
endif

ifneq "$(NSS_DRV_L2TP_ENABLE)" "n"
ccflags-y += -DNSS_DRV_L2TP_ENABLE
qca-nss-drv-objs += \
		   nss_l2tpv2.o \
		   nss_l2tpv2_log.o \
		   nss_l2tpv2_stats.o \
		   nss_l2tpv2_strings.o
endif

ifneq "$(NSS_DRV_LAG_ENABLE)" "n"
ccflags-y += -DNSS_DRV_LAG_ENABLE
qca-nss-drv-objs += \
		   nss_lag.o \
		   nss_lag_log.o
endif

ifneq "$(NSS_DRV_PVXLAN_ENABLE)" "n"
ccflags-y += -DNSS_DRV_PVXLAN_ENABLE
qca-nss-drv-objs += \
			 nss_pvxlan.o \
			 nss_pvxlan_log.o \
			 nss_pvxlan_stats.o
endif

ifneq "$(NSS_DRV_IPV6_ENABLE)" "n"
ccflags-y += -DNSS_DRV_IPV6_ENABLE
qca-nss-drv-objs += \
			nss_ipv6.o \
			nss_ipv6_stats.o \
			nss_ipv6_strings.o \
			nss_ipv6_log.o
ifneq "$(NSS_DRV_IPV6_REASM_ENABLE)" "n"
ccflags-y += -DNSS_DRV_IPV6_REASM_ENABLE
qca-nss-drv-objs += \
		    nss_ipv6_reasm.o \
		    nss_ipv6_reasm_stats.o \
		    nss_ipv6_reasm_strings.o
endif
endif

ifneq "$(NSS_DRV_TSTAMP_ENABLE)" "n"
ccflags-y += -DNSS_DRV_TSTAMP_ENABLE
qca-nss-drv-objs += \
			 nss_tstamp.o \
			 nss_tstamp_stats.o
endif

ifneq "$(NSS_DRV_GRE_ENABLE)" "n"
ccflags-y += -DNSS_DRV_GRE_ENABLE
qca-nss-drv-objs += \
			 nss_gre.o \
			 nss_gre_log.o \
			 nss_gre_stats.o \
			 nss_gre_strings.o
endif

ifneq "$(NSS_DRV_GRE_REDIR_ENABLE)" "n"
ccflags-y += -DNSS_DRV_GRE_REDIR_ENABLE
qca-nss-drv-objs += \
			 nss_gre_redir.o \
			 nss_gre_redir_log.o \
			 nss_gre_redir_lag_ds.o \
			 nss_gre_redir_lag_ds_log.o \
			 nss_gre_redir_lag_ds_stats.o \
			 nss_gre_redir_lag_ds_strings.o \
			 nss_gre_redir_lag_us.o \
			 nss_gre_redir_lag_us_log.o \
			 nss_gre_redir_lag_us_stats.o \
			 nss_gre_redir_lag_us_strings.o \
			 nss_gre_redir_stats.o \
			 nss_gre_redir_strings.o \
			 nss_gre_redir_mark.o \
			 nss_gre_redir_mark_log.o \
			 nss_gre_redir_mark_stats.o \
			 nss_gre_redir_mark_strings.o
endif

ifneq "$(NSS_DRV_GRE_TUNNEL_ENABLE)" "n"
ccflags-y += -DNSS_DRV_GRE_TUNNEL_ENABLE
qca-nss-drv-objs += \
			 nss_gre_tunnel.o \
			 nss_gre_tunnel_log.o \
			 nss_gre_tunnel_stats.o \
			 nss_gre_tunnel_strings.o
endif

ifneq "$(NSS_DRV_CAPWAP_ENABLE)" "n"
ccflags-y += -DNSS_DRV_CAPWAP_ENABLE
qca-nss-drv-objs += \
			 nss_capwap.o \
			 nss_capwap_log.o \
			 nss_capwap_stats.o \
			 nss_capwap_strings.o
endif

ifneq "$(NSS_DRV_MAPT_ENABLE)" "n"
ccflags-y += -DNSS_DRV_MAPT_ENABLE
qca-nss-drv-objs += \
			 nss_map_t.o \
			 nss_map_t_log.o \
			 nss_map_t_stats.o \
			 nss_map_t_strings.o
endif

ifneq "$(NSS_DRV_PPTP_ENABLE)" "n"
ccflags-y += -DNSS_DRV_PPTP_ENABLE
qca-nss-drv-objs += \
			 nss_pptp.o \
			 nss_pptp_log.o \
			 nss_pptp_stats.o \
			 nss_pptp_strings.o
endif

ifneq "$(NSS_DRV_SHAPER_ENABLE)" "n"
ccflags-y += -DNSS_DRV_SHAPER_ENABLE
qca-nss-drv-objs += \
			 nss_shaper.o
endif

ifneq "$(NSS_DRV_SJACK_ENABLE)" "n"
ccflags-y += -DNSS_DRV_SJACK_ENABLE
qca-nss-drv-objs += \
			 nss_sjack.o \
			 nss_sjack_log.o \
			 nss_sjack_stats.o
endif

ifneq "$(NSS_DRV_TUN6RD_ENABLE)" "n"
ccflags-y += -DNSS_DRV_TUN6RD_ENABLE
qca-nss-drv-objs += \
			 nss_tun6rd.o \
			 nss_tun6rd_log.o
endif

ifneq "$(NSS_DRV_TRUSTSEC_ENABLE)" "n"
ccflags-y += -DNSS_DRV_TRUSTSEC_ENABLE
qca-nss-drv-objs += \
			 nss_trustsec_tx.o \
			 nss_trustsec_tx_log.o \
			 nss_trustsec_tx_stats.o
endif

ifneq "$(NSS_DRV_TUNIPIP6_ENABLE)" "n"
ccflags-y += -DNSS_DRV_TUNIPIP6_ENABLE
qca-nss-drv-objs += \
			 nss_tunipip6.o \
			 nss_tunipip6_log.o \
			 nss_tunipip6_stats.o
endif

ifneq "$(NSS_DRV_QRFS_ENABLE)" "n"
ccflags-y += -DNSS_DRV_QRFS_ENABLE
qca-nss-drv-objs += \
			 nss_qrfs.o \
			 nss_qrfs_log.o \
			 nss_qrfs_stats.o
endif

ifneq "$(NSS_DRV_RMNET_ENABLE)" "n"
ccflags-y += -DNSS_DRV_RMNET_ENABLE
qca-nss-drv-objs += \
			 nss_rmnet_rx.o \
			 nss_rmnet_rx_stats.o
endif

ifneq "$(NSS_DRV_PORTID_ENABLE)" "n"
ccflags-y += -DNSS_DRV_PORTID_ENABLE
qca-nss-drv-objs += \
			 nss_portid.o \
			 nss_portid_log.o \
			 nss_portid_stats.o
endif

ifneq "$(NSS_DRV_IGS_ENABLE)" "n"
ccflags-y += -DNSS_DRV_IGS_ENABLE
qca-nss-drv-objs += \
			 nss_igs.o \
			 nss_igs_stats.o
endif

ifneq "$(NSS_DRV_OAM_ENABLE)" "n"
ccflags-y += -DNSS_DRV_OAM_ENABLE
qca-nss-drv-objs += \
			 nss_oam.o \
			 nss_oam_log.o
endif

ifneq "$(NSS_DRV_CLMAP_ENABLE)" "n"
ccflags-y += -DNSS_DRV_CLMAP_ENABLE
qca-nss-drv-objs += \
			 nss_clmap.o \
			 nss_clmap_log.o \
			 nss_clmap_stats.o \
			 nss_clmap_strings.o
endif


ifneq "$(NSS_DRV_VXLAN_ENABLE)" "n"
ccflags-y += -DNSS_DRV_VXLAN_ENABLE
qca-nss-drv-objs += \
			 nss_vxlan.o \
			 nss_vxlan_log.o \
			 nss_vxlan_stats.o
endif

ifneq "$(NSS_DRV_MATCH_ENABLE)" "n"
ccflags-y += -DNSS_DRV_MATCH_ENABLE
qca-nss-drv-objs += \
			nss_match.o \
			nss_match_log.o \
			nss_match_stats.o \
			nss_match_strings.o
endif

ifneq "$(NSS_DRV_MIRROR_ENABLE)" "n"
ccflags-y += -DNSS_DRV_MIRROR_ENABLE
qca-nss-drv-objs += \
			nss_mirror.o \
			nss_mirror_log.o \
			nss_mirror_stats.o \
			nss_mirror_strings.o
endif

ifneq "$(NSS_DRV_UDP_ST_ENABLE)" "n"
ccflags-y += -DNSS_DRV_UDP_ST_ENABLE
qca-nss-drv-objs += \
			nss_udp_st.o \
			nss_udp_st_log.o \
			nss_udp_st_stats.o \
			nss_udp_st_strings.o
endif

ifeq ($(SoC),$(filter $(SoC),ipq806x))
qca-nss-drv-objs += nss_data_plane/nss_data_plane_gmac.o \
		    nss_hal/ipq806x/nss_hal_pvt.o

ifneq "$(NSS_DRV_C2C_ENABLE)" "n"
ccflags-y += -DNSS_DRV_C2C_ENABLE
qca-nss-drv-objs += \
			 nss_c2c_tx.o \
			 nss_c2c_tx_log.o \
			 nss_c2c_tx_stats.o \
			 nss_c2c_tx_strings.o \
			 nss_c2c_rx.o \
			 nss_c2c_rx_stats.o \
			 nss_c2c_rx_strings.o
endif
ifneq "$(NSS_DRV_IPSEC_ENABLE)" "n"
ccflags-y += -DNSS_DRV_IPSEC_ENABLE
qca-nss-drv-objs += \
		    nss_ipsec_log.o \
		    nss_ipsec.o
endif

ifneq "$(NSS_DRV_CRYPTO_ENABLE)" "n"
ccflags-y += -DNSS_DRV_CRYPTO_ENABLE
qca-nss-drv-objs += \
		    nss_crypto.o \
		    nss_crypto_log.o
endif

ifneq "$(NSS_DRV_DTLS_ENABLE)" "n"
ccflags-y += -DNSS_DRV_DTLS_ENABLE
qca-nss-drv-objs += \
		    nss_dtls.o \
		    nss_dtls_log.o \
		    nss_dtls_stats.o
endif
ccflags-y += -I$(obj)/nss_hal/ipq806x -DNSS_HAL_IPQ806X_SUPPORT
endif

ifeq ($(SoC),$(filter $(SoC),ipq60xx ipq60xx_64 ipq807x ipq807x_64))
qca-nss-drv-objs += nss_data_plane/nss_data_plane.o \
					nss_edma.o \
					nss_edma_stats.o \
					nss_edma_strings.o \
					nss_ppe.o \
					nss_ppe_log.o \
					nss_ppe_stats.o \
					nss_ppe_strings.o \
					nss_ppe_vp.o \
					nss_ppe_vp_log.o \
					nss_ppe_vp_stats.o

ccflags-y += -DNSS_DRV_PPE_ENABLE
ccflags-y += -DNSS_DRV_EDMA_ENABLE

ifneq "$(NSS_DRV_IPSEC_ENABLE)" "n"
ccflags-y += -DNSS_DRV_IPSEC_ENABLE
qca-nss-drv-objs += \
		    nss_ipsec_cmn_log.o \
		    nss_ipsec_cmn.o \
		    nss_ipsec_cmn_stats.o \
		    nss_ipsec_cmn_strings.o
endif

ifneq "$(NSS_DRV_CRYPTO_ENABLE)" "n"
ccflags-y += -DNSS_DRV_CRYPTO_ENABLE
ccflags-y += -DNSS_DRV_DMA_ENABLE

qca-nss-drv-objs += \
		    nss_crypto_cmn.o \
		    nss_crypto_cmn_log.o \
		    nss_crypto_cmn_stats.o \
		    nss_crypto_cmn_strings.o \
		    nss_dma.o \
		    nss_dma_log.o \
		    nss_dma_stats.o \
		    nss_dma_strings.o
endif

ifneq "$(NSS_DRV_DTLS_ENABLE)" "n"
ccflags-y += -DNSS_DRV_DTLS_ENABLE
qca-nss-drv-objs += \
		    nss_dtls_cmn.o \
		    nss_dtls_cmn_log.o \
		    nss_dtls_cmn_stats.o \
		    nss_dtls_cmn_strings.o
endif

ifneq "$(NSS_DRV_QVPN_ENABLE)" "n"
ccflags-y += -DNSS_DRV_QVPN_ENABLE
qca-nss-drv-objs += \
	     nss_qvpn.o \
	     nss_qvpn_log.o \
	     nss_qvpn_stats.o \
	     nss_qvpn_strings.o
endif
ifneq "$(NSS_DRV_TLS_ENABLE)" "n"
ccflags-y += -DNSS_DRV_TLS_ENABLE
qca-nss-drv-objs += \
		     nss_tls.o \
		     nss_tls_log.o \
		     nss_tls_stats.o \
		     nss_tls_strings.o
endif
endif

ifeq ($(SoC),$(filter $(SoC),ipq807x ipq807x_64))
qca-nss-drv-objs += nss_hal/ipq807x/nss_hal_pvt.o \
		    nss_data_plane/hal/nss_ipq807x.o
ifneq "$(NSS_DRV_C2C_ENABLE)" "n"
ccflags-y += -DNSS_DRV_C2C_ENABLE
qca-nss-drv-objs += \
			 nss_c2c_tx.o \
			 nss_c2c_tx_log.o \
			 nss_c2c_tx_stats.o \
			 nss_c2c_tx_strings.o \
			 nss_c2c_rx.o \
			 nss_c2c_rx_stats.o \
			 nss_c2c_rx_strings.o
endif
ccflags-y += -I$(obj)/nss_hal/ipq807x -DNSS_HAL_IPQ807x_SUPPORT -DNSS_MULTI_H2N_DATA_RING_SUPPORT
endif

ifeq ($(SoC),$(filter $(SoC),ipq60xx ipq60xx_64))
qca-nss-drv-objs += nss_hal/ipq60xx/nss_hal_pvt.o \
		    nss_data_plane/hal/nss_ipq60xx.o
ccflags-y += -I$(obj)/nss_hal/ipq60xx -DNSS_HAL_IPQ60XX_SUPPORT -DNSS_MULTI_H2N_DATA_RING_SUPPORT
endif

ifeq ($(SoC),$(filter $(SoC),ipq50xx ipq50xx_64))
qca-nss-drv-objs += nss_data_plane/nss_data_plane.o \
		    nss_hal/ipq50xx/nss_hal_pvt.o \
		    nss_data_plane/hal/nss_ipq50xx.o

ifneq "$(NSS_DRV_IPSEC_ENABLE)" "n"
ccflags-y += -DNSS_DRV_IPSEC_ENABLE
qca-nss-drv-objs += \
		    nss_ipsec_cmn_log.o \
		    nss_ipsec_cmn.o \
		    nss_ipsec_cmn_stats.o \
		    nss_ipsec_cmn_strings.o
endif

ifneq "$(NSS_DRV_CRYPTO_ENABLE)" "n"
ccflags-y += -DNSS_DRV_CRYPTO_ENABLE
qca-nss-drv-objs += \
		    nss_crypto_cmn.o \
		    nss_crypto_cmn_log.o \
		    nss_crypto_cmn_stats.o \
		    nss_crypto_cmn_strings.o
endif

ifneq "$(NSS_DRV_DTLS_ENABLE)" "n"
ccflags-y += -DNSS_DRV_DTLS_ENABLE
qca-nss-drv-objs += \
		    nss_dtls_cmn.o \
		    nss_dtls_cmn_log.o \
		    nss_dtls_cmn_stats.o \
		    nss_dtls_cmn_strings.o
endif
ccflags-y += -I$(obj)/nss_hal/ipq50xx -DNSS_HAL_IPQ50XX_SUPPORT -DNSS_MULTI_H2N_DATA_RING_SUPPORT
endif

ccflags-y += -I$(obj)/nss_hal/include -I$(obj)/nss_data_plane/include -I$(obj)/exports -DNSS_DEBUG_LEVEL=0 -DNSS_PKT_STATS_ENABLED=1
ccflags-y += -I$(obj)/nss_data_plane/hal/include
ccflags-y += -DNSS_PM_DEBUG_LEVEL=0 -DNSS_SKB_REUSE_SUPPORT=1
ccflags-y += -Wall -Werror

KERNELVERSION := $(word 1, $(subst ., ,$(KERNELVERSION))).$(word 2, $(subst ., ,$(KERNELVERSION)))

ifneq ($(findstring 3.4, $(KERNELVERSION)),)
NSS_CCFLAGS = -DNSS_DT_SUPPORT=0 -DNSS_FW_DBG_SUPPORT=1 -DNSS_PM_SUPPORT=1
else
NSS_CCFLAGS = -DNSS_DT_SUPPORT=1 -DNSS_FW_DBG_SUPPORT=0 -DNSS_PM_SUPPORT=0

ccflags-y += -I$(obj)
endif

# Fabric scaling is supported in 3.14 and 4.4 only
ifneq ($(findstring 3.14, $(KERNELVERSION)),)
NSS_CCFLAGS += -DNSS_FABRIC_SCALING_SUPPORT=1
else ifneq ($(findstring 4.4, $(KERNELVERSION)),)
NSS_CCFLAGS += -DNSS_FABRIC_SCALING_SUPPORT=1
else
NSS_CCFLAGS += -DNSS_FABRIC_SCALING_SUPPORT=0
endif

# Disable Frequency scaling
ifeq "$(NSS_FREQ_SCALE_DISABLE)" "y"
ccflags-y += -DNSS_FREQ_SCALE_SUPPORT=0
else
qca-nss-drv-objs += \
			nss_freq.o \
			nss_freq_log.o \
			nss_freq_stats.o
ccflags-y += -DNSS_FREQ_SCALE_SUPPORT=1
endif

ccflags-y += $(NSS_CCFLAGS)

export NSS_CCFLAGS

obj ?= .
