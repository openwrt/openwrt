#
# Copyright (C) 2021 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ath11k-firmware
PKG_SOURCE_DATE:=2021-10-13
PKG_SOURCE_VERSION:=305714081ec958431a0948f0bd2defc1ed445fba
PKG_MIRROR_HASH:=79b42bff5bccebcc8032d3a80132b946444adf73234c2051b247fbd585af2c3f
PKG_RELEASE:=$(AUTORELEASE)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/quic/upstream-wifi-fw.git

PKG_LICENSE_FILES:=ath11k-firmware/LICENSE.md

PKG_MAINTAINER:=Robert Marko <robimarko@gmail.com>

include $(INCLUDE_DIR)/package.mk

define Package/ath11k-firmware-default
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=$(PKG_SOURCE_URL)
  DEPENDS:=
endef

define Package/ath11k-firmware-ipq6018
$(Package/ath11k-firmware-default)
  TITLE:=ath11k firmware for IPQ6018 devices
endef

define Package/ath11k-firmware-ipq8074
$(Package/ath11k-firmware-default)
  TITLE:=ath11k firmware for IPQ8074 devices
endef

define Package/ath11k-firmware-qcn9074
$(Package/ath11k-firmware-default)
  TITLE:=ath11k firmware for QCN9074 devices
endef

define Build/Compile

endef

define Package/ath11k-firmware-ipq6018/install
	$(INSTALL_DIR) $(1)/lib/firmware/IPQ6018
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/ath11k-firmware/IPQ6018/hw1.0/2.5.0.1/WLAN.HK.2.5.0.1-01192-QCAHKSWPL_SILICONZ-1/* \
		$(1)/lib/firmware/IPQ6018/
endef

define Package/ath11k-firmware-ipq8074/install
	$(INSTALL_DIR) $(1)/lib/firmware/IPQ8074
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/ath11k-firmware/IPQ8074/hw2.0/2.5.0.1/WLAN.HK.2.5.0.1-01192-QCAHKSWPL_SILICONZ-1/* \
		$(1)/lib/firmware/IPQ8074/
endef

define Package/ath11k-firmware-qcn9074/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath11k/QCN9074/hw1.0
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/ath11k-firmware/QCN9074/hw1.0/2.5.0.1/WLAN.HK.2.5.0.1-01192-QCAHKSWPL_SILICONZ-1/* \
		$(1)/lib/firmware/ath11k/QCN9074/hw1.0/
endef

$(eval $(call BuildPackage,ath11k-firmware-ipq6018))
$(eval $(call BuildPackage,ath11k-firmware-ipq8074))
$(eval $(call BuildPackage,ath11k-firmware-qcn9074))
