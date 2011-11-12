#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=om2p-watchdog
PKG_RELEASE:=1
PKG_VERSION:=1

include $(INCLUDE_DIR)/package.mk

define Package/om2p-watchdog
  SECTION:=base
  CATEGORY:=Base system
  DEPENDS:=@TARGET_ar71xx
  TITLE:=om2p watchdog
  URL:=http://openwrt.org/
endef

define Package/om2p-watchdog/description
 This package contains the hw watchdog script for the OM2P device.
endef

define Build/Prepare
endef

define Build/Compile
endef

define Build/Compile
endef

define Package/om2p-watchdog/install
	$(INSTALL_DIR) $(1)/etc/init.d/
	$(INSTALL_DIR) $(1)/sbin/
	$(INSTALL_BIN) ./files/om2p-watchdog.init $(1)/etc/init.d/om2p-watchdog
	$(INSTALL_BIN) ./files/om2p-watchdog $(1)/sbin/om2p-watchdog
endef


$(eval $(call BuildPackage,om2p-watchdog))

