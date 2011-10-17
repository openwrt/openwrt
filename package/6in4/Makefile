#
# Copyright (C) 2010-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=6in4
PKG_VERSION:=10
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/6in4
  SECTION:=ipv6
  CATEGORY:=IPv6
  DEPENDS:=+ip +kmod-ipv6 +kmod-sit
  TITLE:=IPv6-in-IPv4 configuration support
  MAINTAINER:=Jo-Philipp Wich <xm@subsignal.org>
  PKGARCH:=all
endef

define Package/6in4/description
Provides support for 6in4 tunnels in /etc/config/network.
Refer to http://wiki.openwrt.org/doc/uci/network for
configuration details.
endef

define Build/Compile
endef

define Build/Configure
endef

define Package/6in4/install
	$(INSTALL_DIR) $(1)/lib/network
	$(INSTALL_DATA) ./files/6in4.sh $(1)/lib/network/6in4.sh
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_DATA) ./files/6in4.hotplug $(1)/etc/hotplug.d/iface/90-6in4
endef

$(eval $(call BuildPackage,6in4))
