###################################################
# Makefile for the NSS data plane driver
###################################################

obj ?= .

obj-m += qca-nss-dp.o

qca-nss-dp-objs += nss_dp_attach.o \
		   nss_dp_ethtools.o \
		   nss_dp_main.o

ifneq ($(CONFIG_NET_SWITCHDEV),)
qca-nss-dp-objs += nss_dp_switchdev.o
endif

ifeq ($(SoC),$(filter $(SoC),ipq807x ipq807x_64 ipq60xx ipq60xx_64))
qca-nss-dp-objs += hal/edma/edma_cfg.o \
		   hal/edma/edma_data_plane.o \
		   hal/edma/edma_tx_rx.o \
		   hal/gmac_hal_ops/qcom/qcom_if.o \
		   hal/gmac_hal_ops/syn/xgmac/syn_if.o
endif

NSS_DP_INCLUDE = -I$(obj)/include -I$(obj)/exports -I$(obj)/gmac_hal_ops/include \
		 -I$(obj)/hal/include

ifeq ($(SoC),$(filter $(SoC),ipq50xx ipq50xx_64))
NSS_DP_INCLUDE += -I$(obj)/hal/gmac_hal_ops/syn/gmac
endif

ccflags-y += $(NSS_DP_INCLUDE)
ccflags-y += -Wall -Werror

ifeq ($(SoC),$(filter $(SoC),ipq807x ipq807x_64 ipq60xx ipq60xx_64))
ccflags-y += -DNSS_DP_PPE_SUPPORT
endif

ifeq ($(SoC),$(filter $(SoC),ipq60xx ipq60xx_64))
qca-nss-dp-objs += hal/arch/ipq60xx/nss_ipq60xx.o
ccflags-y += -DNSS_DP_IPQ60XX
endif

ifeq ($(SoC),$(filter $(SoC),ipq807x ipq807x_64))
qca-nss-dp-objs += hal/arch/ipq807x/nss_ipq807x.o
ccflags-y += -DNSS_DP_IPQ807X -DNSS_DP_EDMA_TX_SMALL_PKT_WAR
endif

ifeq ($(SoC),$(filter $(SoC),ipq50xx ipq50xx_64))
qca-nss-dp-objs += hal/arch/ipq50xx/nss_ipq50xx.o \
		   hal/gmac_hal_ops/syn/gmac/syn_if.o \
		   hal/syn_gmac_dp/syn_data_plane.o \
		   hal/syn_gmac_dp/syn_dp_tx_rx.o \
		   hal/syn_gmac_dp/syn_dp_cfg.o
ccflags-y += -DNSS_DP_IPQ50XX
endif
