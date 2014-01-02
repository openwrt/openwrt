#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=6relayd
PKG_VERSION:=2013-10-21.1
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=git://github.com/sbyx/6relayd.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=ad00c3dd9ee42f172870708724858ab502b3a689

PKG_MAINTAINER:=Steven Barth <steven@midlink.org>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/6relayd
  SECTION:=net
  CATEGORY:=Network
  TITLE:=DHCPv6(-PD)/RA Server & Relay
  DEPENDS:=+kmod-ipv6 @!TARGET_avr32
endef

define Package/6relayd/description
 6relayd is a daemon for serving and relaying IPv6 management protocols to
 configure clients and downstream routers.

 6relayd provides server services for RA, stateless DHCPv6, DHCPv6-PD and can
 be used to relay RA, DHCPv6 and NDP between routed (non-bridged) interfaces
 in case no delegated prefixes are provided by the upstream router.
endef

define Package/6relayd/conffiles
/etc/config/6relayd
endef

define Package/6relayd/install
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/6relayd $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/6relayd-update $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_DATA) ./files/6relayd.hotplug $(1)/etc/hotplug.d/iface/30-6relay
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/6relayd.config $(1)/etc/config/6relayd
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/6relayd.init $(1)/etc/init.d/6relayd
endef

$(eval $(call BuildPackage,6relayd))
