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

ATH11K_LEGACY_FW_NAME:=ath11k-legacy-firmware
ATH11K_LEGACY_FW_DATE:=2024-03-14
ATH11K_LEGACY_FW_RELEASE:=795809c7
ATH11K_LEGACY_FW_SUBDIR:=$(ATH11K_LEGACY_FW_NAME)-$(ATH11K_LEGACY_FW_DATE)~$(ATH11K_LEGACY_FW_RELEASE)
ATH11K_LEGACY_FW_SOURCE=$(ATH11K_LEGACY_FW_SUBDIR).tar.zst

ATH11K_IPQ9574_FW_NAME:=ath11k-firmware-ipq9574-board
ATH11K_IPQ9574_FW_DATE:=2026.03.24
ATH11K_IPQ9574_FW_RELEASE:=2044a5b7
ATH11K_IPQ9574_FW_SUBDIR:=$(PKG_NAME)-$(ATH11K_IPQ9574_FW_DATE)~$(ATH11K_IPQ9574_FW_RELEASE)
ATH11K_IPQ9574_FW_SOURCE:=$(ATH11K_IPQ9574_FW_SUBDIR).tar.zst

define Download/$(ATH11K_LEGACY_FW_NAME)
	FILE:=$(ATH11K_LEGACY_FW_SOURCE)
	PROTO:=git
	URL:=https://github.com/quic/upstream-wifi-fw.git
	SOURCE_VERSION:=795809c7041582bd51bdfaa1f548b916ae8d4382
	MIRROR_HASH:=b55bf7204543c7ba87876e84788239b0e47ad62434c5b0d1169526cde36a9daa
	SUBDIR:=$(ATH11K_LEGACY_FW_SUBDIR)
endef
$(eval $(call Download,$(ATH11K_LEGACY_FW_NAME)))

define Download/$(ATH11K_IPQ9574_FW_NAME)
	FILE:=$(ATH11K_IPQ9574_FW_SOURCE)
	PROTO:=git
	URL:=$(PKG_SOURCE_URL)
	SOURCE_VERSION:=2044a5b7e0da88a34c728a62e7f262b67c141739
	MIRROR_HASH:=281e3361ab20d3bcff78498dafaa3db9abce5440ea6bb20c83042812afd453e5
	SUBDIR:=$(ATH11K_IPQ9574_FW_SUBDIR)
endef
$(eval $(call Download,$(ATH11K_IPQ9574_FW_NAME)))

define Package/ath11k-firmware-default
  SECTION:=firmware
  CATEGORY:=Firmware
  URL:=$(PKG_SOURCE_URL)
  DEPENDS:=
endef

define Package/ath11k-firmware-ipq5018-qcn6122
$(Package/ath11k-firmware-default)
  TITLE:=IPQ5018/QCN6122 ath11k firmware
endef

define Package/ath11k-firmware-ipq6018
$(Package/ath11k-firmware-default)
  TITLE:=IPQ6018 ath11k firmware
endef

define Package/ath11k-firmware-ipq9574
$(Package/ath11k-firmware-default)
  TITLE:=IPQ9574 ath11k firmware
  VERSION:=$(ATH11K_IPQ9574_FW_DATE)~$(ATH11K_IPQ9574_FW_RELEASE)-r$(PKG_RELEASE)
endef

define Build/Clean
	$(call Build/Clean/Default,)

	rm -rf \
		$(BUILD_DIR)/$(ATH11K_LEGACY_FW_SUBDIR) \
		$(BUILD_DIR)/$(ATH11K_IPQ9574_FW_SUBDIR)
endef

define Build/Prepare
	$(call Build/Prepare/Default,)

	$(TAR) -C $(BUILD_DIR) -xf $(DL_DIR)/$(ATH11K_LEGACY_FW_SOURCE)
	$(TAR) -C $(BUILD_DIR) -xf $(DL_DIR)/$(ATH11K_IPQ9574_FW_SOURCE)
endef

define Build/Compile

endef

define Package/ath11k-firmware-ipq5018-qcn6122/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath11k/IPQ5018/hw1.0
	$(INSTALL_DATA) \
		$(BUILD_DIR)/$(ATH11K_LEGACY_FW_SUBDIR)/ath11k-firmware/IPQ5018_QCN6122_QCN6122/hw1.0/2.7.0.1/WLAN.HK.2.7.0.1-01744-QCAHKSWPL_SILICONZ-1/*.* \
		$(1)/lib/firmware/ath11k/IPQ5018/hw1.0/
	$(INSTALL_DIR) $(1)/lib/firmware/ath11k/QCN6122/hw1.0
	$(INSTALL_DATA) \
		$(BUILD_DIR)/$(ATH11K_LEGACY_FW_SUBDIR)/ath11k-firmware/IPQ5018_QCN6122_QCN6122/hw1.0/2.7.0.1/WLAN.HK.2.7.0.1-01744-QCAHKSWPL_SILICONZ-1/qcn6122/*.* \
		$(1)/lib/firmware/ath11k/QCN6122/hw1.0/
endef

define Package/ath11k-firmware-ipq6018/install
	$(INSTALL_DIR) $(1)/lib/firmware/IPQ6018
	$(INSTALL_DATA) \
		$(PKG_BUILD_DIR)/IPQ6018/hw1.0/2.5.0.1/WLAN.HK.2.5.0.1-03982-QCAHKSWPL_SILICONZ-3/* \
		$(1)/lib/firmware/IPQ6018/
endef

define Package/ath11k-firmware-ipq9574/install
	$(INSTALL_DIR) $(1)/lib/firmware/ath11k/IPQ9574/hw1.0
	$(INSTALL_DATA) \
		$(BUILD_DIR)/$(ATH11K_LEGACY_FW_SUBDIR)/ath11k-firmware/IPQ9574/hw1.0/2.11.0.1/WLAN.HK.2.11.0.1-01491-QCAHKSWPL_SILICONZ-1/*.* \
		$(1)/lib/firmware/ath11k/IPQ9574/hw1.0/
	$(INSTALL_DATA) \
		$(BUILD_DIR)/$(ATH11K_IPQ9574_FW_SUBDIR)/IPQ9574/hw1.0/board-2.bin \
		$(1)/lib/firmware/ath11k/IPQ9574/hw1.0/
endef

$(eval $(call BuildPackage,ath11k-firmware-ipq5018-qcn6122))
$(eval $(call BuildPackage,ath11k-firmware-ipq6018))
$(eval $(call BuildPackage,ath11k-firmware-ipq9574))
