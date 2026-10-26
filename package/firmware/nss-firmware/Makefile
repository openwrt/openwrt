#
# Copyright (C) 2022 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=nss-firmware
PKG_VERSION:=2025.05.01
PKG_HASH:=10a4b1e69470db150915cb063525436494b8ae4eebb8b50ea6ad894082d7abb0
PKG_SOURCE_URL:=https://github.com/qosmio/qca-sdk-nss-fw/releases/download/v$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.zst
PKG_RELEASE:=1

PKG_LICENSE_FILES:=LICENSE.md

PKG_MAINTAINER:=Sean K <datapronix@protonmail.com>

PKG_CONFIG_DEPENDS:= \
	CONFIG_NSS_FIRMWARE_VERSION_11_4 \
	CONFIG_NSS_FIRMWARE_VERSION_12_2 \
	CONFIG_NSS_FIRMWARE_VERSION_12_5

include $(INCLUDE_DIR)/package.mk

RSTRIP:=:
STRIP:=:

NSS_PROFILE:=R

define Package/nss-firmware-common
	TITLE:=NSS firmware
	SECTION:=firmware
	CATEGORY:=Firmware
	URL:=$(PKG_SOURCE_URL)
	DEPENDS:=@TARGET_qualcommax
endef

define Package/nss-firmware
$(call Package/nss-firmware-common,$(1))
	DEPENDS+= +TARGET_qualcommax_ipq807x:nss-firmware-ipq807x \
		  +TARGET_qualcommax_ipq60xx:nss-firmware-ipq60xx \
		  +TARGET_qualcommax_ipq50xx:nss-firmware-ipq50xx
endef

define Package/nss-firmware-ipq807x
$(call Package/nss-firmware-common,$(1))
	IPQ_PLATFORM=IPQ8074
	NSS_SOC:=HK
endef

define Package/nss-firmware-ipq60xx
$(call Package/nss-firmware-common,$(1))
	IPQ_PLATFORM=IPQ6018
	NSS_SOC:=CP
endef

define Package/nss-firmware-ipq50xx
$(call Package/nss-firmware-common,$(1))
	IPQ_PLATFORM=IPQ5018
	CONFLICTS=nss-firmware-ipq807x nss-firmware-ipq60xx
	NSS_SOC:=MP
endef

define Package/nss-firmware/config
  menu "NSS Firmware Version"

  comment "Select NSS firmware version"

  choice
    prompt "Version"
    default NSS_FIRMWARE_VERSION_12_5

    config NSS_FIRMWARE_VERSION_12_5
          bool "NSS Firmware 12.5 Release 210"
          help
               The firmware line the qca-nss-drv source in this tree
               is pinned against, and the one the ipq50xx release
               images carry. On IPQ5018 it runs the ath11k NSS mesh
               offload (802.11s) as well as 12.2.

    config NSS_FIRMWARE_VERSION_12_2
          bool "NSS Firmware 12.2 Release 156"
          help
               The newest firmware published for IPQ5018 (2025-05-01,
               nine months after 12.5) and the one most ipq50xx
               measurements were taken on. 12.5 runs there as well;
               build for the version you run (the Wi-Fi peer
               statistics differ between the two).

    config NSS_FIRMWARE_VERSION_11_4
          bool "NSS Firmware 11.4.0.5 Release 6"
          help
               The firmware the ath11k NSS mesh offload needs on
               ipq807x/ipq60xx, where newer firmware rejects mesh
               interface allocation. On IPQ5018, 12.2 and 12.5 run
               the mesh offload.
  endchoice
  endmenu
endef

define nss-firmware-version
ifneq ($(CONFIG_NSS_FIRMWARE_VERSION_11_4),)
override NSS_MAJOR=11
override NSS_MINOR=4
override NSS_REL=6
override VERSION_PATH=$(PKG_BUILD_DIR)/QCA_Networking_2021.SPF_11.4/CS
else ifneq ($(CONFIG_NSS_FIRMWARE_VERSION_12_2),)
override NSS_MAJOR=12
override NSS_MINOR=2
override NSS_REL=156
override VERSION_PATH=$(PKG_BUILD_DIR)/QCA_Networking_2022.SPF_12.2/ED1
else
override NSS_MAJOR=12
override NSS_MINOR=5
override NSS_REL=210
override VERSION_PATH=$(PKG_BUILD_DIR)/QCA_Networking_2024.SPF_12.5/ED1
endif
endef

define Build/Compile
endef

define Package/nss-firmware-common/install
	$(eval $(call nss-firmware-version,$(IPQ_PLATFORM)))
	$(INSTALL_DIR) $(PKG_BUILD_DIR)/$(IPQ_PLATFORM)
	$(TAR) --strip-components=1 -C $(PKG_BUILD_DIR)/$(IPQ_PLATFORM) -xf \
		$(VERSION_PATH)/$(IPQ_PLATFORM).ATH.$(NSS_MAJOR).$(NSS_MINOR)/BIN-NSS*.$(NSS_MAJOR).$(NSS_MINOR)*-$(NSS_REL)*$(NSS_PROFILE).tar.bz2
	$(INSTALL_DIR) $(1)/lib/firmware/
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/$(IPQ_PLATFORM)/retail_router0.bin \
		$(1)/lib/firmware/qca-nss0-retail.bin
ifeq ($(NSS_SOC),HK)
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/$(IPQ_PLATFORM)/retail_router1.bin \
		$(1)/lib/firmware/qca-nss1-retail.bin
endif
endef

define Package/nss-firmware-ipq807x/install
	$(call Package/nss-firmware-common/install,$(1))
endef

define Package/nss-firmware-ipq60xx/install
	$(call Package/nss-firmware-common/install,$(1))
endef

define Package/nss-firmware-ipq50xx/install
	$(call Package/nss-firmware-common/install,$(1))
endef

define Package/nss-firmware/install
	true
endef

$(eval $(call BuildPackage,nss-firmware-ipq807x))
$(eval $(call BuildPackage,nss-firmware-ipq60xx))
$(eval $(call BuildPackage,nss-firmware-ipq50xx))
$(eval $(call BuildPackage,nss-firmware))
