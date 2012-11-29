#
# Copyright (C) 2010-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ipv6-support
PKG_VERSION:=2012-11-29
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/ipv6-support
  SECTION:=ipv6
  CATEGORY:=IPv6
  DEPENDS:=+kmod-ipv6 +6relayd +odhcp6c +6distributed +ip6tables +ubus
  TITLE:=Basic IPv6-support for Customer Edge Routers
  MAINTAINER:=Steven Barth <steven@midlink.org>
  PKGARCH:=all
endef

define Package/ipv6-support/description
This package provides basic IPv6 support including Router Discovery,
DHCPv6 (client & server), prefix delegation and distribution.
endef

define Build/Compile
endef

define Build/Configure
endef

define Package/ipv6-support/install
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_DATA) ./files/ipv6.hotplug $(1)/etc/hotplug.d/iface/20-ipv6
	$(INSTALL_DIR) $(1)/lib/ipv6
	$(INSTALL_DATA) ./files/support.sh $(1)/lib/ipv6/support.sh
	$(INSTALL_BIN) ./files/dhcpv6.sh $(1)/lib/ipv6/dhcpv6.sh
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/network6.config $(1)/etc/config/network6
endef

$(eval $(call BuildPackage,ipv6-support))
