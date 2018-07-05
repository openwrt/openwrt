#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=wmt
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/abbradar/wmt
PKG_SOURCE_VERSION:=2127e23dd94df960b12f3ffff806bcf41ebbf4b8
PKG_MAINTAINER:=Nikolay Amiantov <ab@fmap.me>

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=wmt utility for MT6625L
  DEPENDS:=kmod-mt6625l-wlan-gen2
  MAINTAINER:=Jinkai li <lijk@synertone.net>
endef

define Package/$(PKG_NAME)/description
  Utility for loading MT6625L firmware.
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/stp_uart_launcher $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wmt_loader $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wmt_loopback $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/system/etc/firmware
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/config/WMT_SOC.cfg $(1)/system/etc/firmware
	$(INSTALL_DIR) $(1)/etc/firmware
	$(CP) -r $(PKG_BUILD_DIR)/firmware/* $(1)/etc/firmware/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/wmt.init $(1)/etc/init.d/wmt
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) ./files/wmt.defaults $(1)/etc/uci-defaults/03wmt
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
