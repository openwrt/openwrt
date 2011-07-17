#
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=6to4
PKG_VERSION:=6
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/6to4
  SECTION:=ipv6
  CATEGORY:=IPv6
  DEPENDS:=+ip +kmod-ipv6 +kmod-sit
  TITLE:=IPv6-to-IPv4 configuration support
  MAINTAINER:=Jo-Philipp Wich <xm@subsignal.org>
  PKGARCH:=all
endef

define Package/6to4/description
Provides support for 6to4 tunnels in /etc/config/network.
Refer to http://wiki.openwrt.org/doc/uci/network for
configuration details.
endef

define Build/Compile
endef

define Build/Configure
endef

define Package/6to4/install
	$(INSTALL_DIR) $(1)/lib/network
	$(INSTALL_DATA) ./files/6to4.sh $(1)/lib/network/6to4.sh
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_DATA) ./files/6to4.hotplug $(1)/etc/hotplug.d/iface/91-6to4
endef

$(eval $(call BuildPackage,6to4))
