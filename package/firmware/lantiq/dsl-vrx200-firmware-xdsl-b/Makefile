# Copyright (C) 2015 OpenWrt.org
# Copyright (C) 2015 Lantiq Beteiligungs GmbH & Co KG.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.

include $(TOPDIR)/rules.mk

PKG_NAME:=dsl_vr9_firmware_xdsl
PKG_VERSION:=05.07.03.03.00.06_05.07.01.0C.00.02_osc
PKG_RELEASE:=1
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://mirror2.openwrt.org/sources
PKG_MD5SUM:=af5581836cbe574574590c046fe45bab

include $(INCLUDE_DIR)/package.mk

define Package/dsl-vrx200-firmware-xdsl-b
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=VRX200 / VR9 CPE xDSL Annex B firmware
  URL:=http://www.lantiq.com/
  DEPENDS:=@TARGET_lantiq_xrx200
endef

define Package/dsl-vrx200-firmware-xdsl-b/description
	VRX200 / VR9 CPE VDSL and ADSL Annex B firmware
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
	$(TAR) -C $(PKG_BUILD_DIR) -xzf $(DL_DIR)/$(PKG_SOURCE)
endef

define Build/Compile
endef

define Package/dsl-vrx200-firmware-xdsl-b/install
	$(INSTALL_DIR) $(1)/lib/firmware/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/LICENSE $(1)/lib/firmware/xcpe_573306_571C02.LICENSE
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/xcpe_573306_571C02.bin $(1)/lib/firmware/
	ln -s xcpe_573306_571C02.bin $(1)/lib/firmware/vdsl.bin
	ln -s xcpe_573306_571C02.bin $(1)/lib/firmware/adsl-b.bin
endef

$(eval $(call BuildPackage,dsl-vrx200-firmware-xdsl-b))
