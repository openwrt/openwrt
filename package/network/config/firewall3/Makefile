#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=firewall3
PKG_VERSION:=2013-03-21
PKG_RELEASE:=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://nbd.name/firewall3.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=cb80885eeb7d45b56651c22a7bec2bfe6a646add
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_MAINTAINER:=Jo-Philipp Wich <jow@openwrt.org>


include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/firewall3
  SECTION:=net
  CATEGORY:=Network
  TITLE:=UCI C Firewall
  DEPENDS:=+libubox +libubus +libuci
endef

define Package/firewall3/description
 This package provides a config-compatible C implementation of the UCI firewall.
endef

define Package/firewall3/conffiles
/etc/config/firewall
/etc/firewall.user
endef

define Package/firewall3/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/firewall3 $(1)/sbin/fw3
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/firewall.init $(1)/etc/init.d/firewall
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_DATA) ./files/firewall.hotplug $(1)/etc/hotplug.d/iface/20-firewall
	$(INSTALL_DIR) $(1)/etc/config/
	$(INSTALL_DATA) ./files/firewall.config $(1)/etc/config/firewall
	$(INSTALL_DIR) $(1)/etc/
	$(INSTALL_DATA) ./files/firewall.user $(1)/etc/firewall.user
endef

$(eval $(call BuildPackage,firewall3))
