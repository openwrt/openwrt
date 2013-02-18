#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=6relayd
PKG_VERSION:=2013-02-18
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=git://github.com/sbyx/6relayd.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=0b7f473690c669d300855a836585a98962518134

PKG_MAINTAINER:=Steven Barth <steven@midlink.org>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/6relayd
  SECTION:=ipv6
  CATEGORY:=IPv6
  TITLE:=IPv6-Relay and Server (RD-, DHCPv6- & NDP-Proxy)
  DEPENDS:=+kmod-ipv6
endef

define Package/6relayd/description
 6relayd is an IPv6-tool that relays IPv6-management protocols like router
 discovery, neighbor discovery and DHCPv6 so that clients on routed (non-
 bridged) interfaces can use the public address prefix, DHCPv6 and DNS-service
 of a master interface. This is useful to avoid NAT in chained IPv6-routers.

 It can also be used as a lightweight router advertisement daemon and provide
 stateless DHCPv6 service where size matters.
endef

define Package/6relayd/conffiles
/etc/config/6relayd
endef

define Package/6relayd/install
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/6relayd $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_DATA) ./files/6relayd.hotplug $(1)/etc/hotplug.d/iface/30-6relay
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/6relayd.config $(1)/etc/config/6relayd
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/6relayd.init $(1)/etc/init.d/6relayd
endef

$(eval $(call BuildPackage,6relayd))
