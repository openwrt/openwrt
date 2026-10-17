# SPDX-License-Identifier: GPL-2.0-or-later

include $(TOPDIR)/rules.mk

PKG_NAME:=qca-bt-firmware
PKG_RELEASE:=1

PKG_MAINTAINER:=George Moussalem <george.moussalem@outlook.com>

include $(INCLUDE_DIR)/package.mk

define Package/qca-bt-firmware-default
  SECTION:=firmware
  CATEGORY:=Firmware
  DEPENDS:=@TARGET_qualcommax_ipq50xx
  DEFAULTS:=n
endef

define Package/qca-bt-firmware-ipq5018
$(Package/qca-bt-firmware-default)
  TITLE:=Qualcomm IPQ5018 Bluetooth firmware
endef

define Package/qca-bt-firmware-ipq5018/install
	$(INSTALL_DIR) $(1)/lib/firmware/qca/
	$(INSTALL_DATA) files/* $(1)/lib/firmware/qca/
endef

define Build/Compile
endef

$(eval $(call BuildPackage,qca-bt-firmware-ipq5018))
