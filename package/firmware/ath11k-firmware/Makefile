#
# Copyright (C) 2022 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ath11k-firmware
PKG_SOURCE_DATE:=2024-10-14
PKG_SOURCE_VERSION:=15f050122da5ef5bef2cc8c7c19dfb7f98060a49
PKG_MIRROR_HASH:=e7a89d1570f32552ade539376181494724b5eafb31d00451ff2dcf5a2922a0cb
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://git.codelinaro.org/clo/ath-firmware/ath11k-firmware.git

PKG_LICENSE_FILES:=LICENSE.qca_firmware

PKG_MAINTAINER:=Robert Marko <robimarko@gmail.com>

include $(INCLUDE_DIR)/package.mk

RSTRIP:=:
STRIP:=:

define Package/ath11k-firmware-default
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=$(PKG_SOURCE_URL)
  DEPENDS:=
endef

define Package/ath11k-firmware-ipq6018
$(Package/ath11k-firmware-default)
  TITLE:=IPQ6018 ath11k firmware
endef

define Package/ath11k-firmware-ipq8074
$(Package/ath11k-firmware-default)
  TITLE:=IPQ8074 ath11k firmware
endef

define Package/ath11k-firmware-qcn9074
$(Package/ath11k-firmware-default)
  TITLE:=QCN9074 ath11k firmware
endef

define Build/Compile

endef

define Package/ath11k-firmware-ipq6018/install
	$(INSTALL_DIR) $(1)/lib/firmware/IPQ6018
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/IPQ6018/hw1.0/2.5.0.1/WLAN.HK.2.5.0.1-03982-QCAHKSWPL_SILICONZ-3/* \
		$(1)/lib/firmware/IPQ6018/
endef

define Package/ath11k-firmware-ipq8074/install
	$(INSTALL_DIR) $(1)/lib/firmware/IPQ8074
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/IPQ8074/hw2.0/2.9.0.1/WLAN.HK.2.9.0.1-02146-QCAHKSWPL_SILICONZ-1/* \
		$(1)/lib/firmware/IPQ8074/
endef

define Package/ath11k-firmware-qcn9074/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath11k/QCN9074/hw1.0
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/QCN9074/hw1.0/2.9.0.1/WLAN.HK.2.9.0.1-02146-QCAHKSWPL_SILICONZ-1/* \
		$(1)/lib/firmware/ath11k/QCN9074/hw1.0/
	$(INSTALL_BIN) \
		$(PKG_BUILD_DIR)/QCN9074/hw1.0/board-2.bin $(1)/lib/firmware/ath11k/QCN9074/hw1.0/board-2.bin
endef

$(eval $(call BuildPackage,ath11k-firmware-ipq6018))
$(eval $(call BuildPackage,ath11k-firmware-ipq8074))
$(eval $(call BuildPackage,ath11k-firmware-qcn9074))
