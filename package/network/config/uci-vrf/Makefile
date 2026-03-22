#
# Copyright (C) 2026 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=uci-vrf
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/uci-vrf
  SECTION:=net
  CATEGORY:=Network
  TITLE:=VRF config support in /etc/config/network
  DEPENDS:=+kmod-vrf +ip
  PKGARCH:=all
endef

define Package/uci-vrf/description
 Virtual Routing and Forwarding (VRF) config support in /etc/config/network
 using custom UCI sections and a lightweight helper outside of netifd.
endef

define Build/Compile
endef

define Build/Configure
endef

define Package/uci-vrf/install
	$(INSTALL_DIR) \
		$(1)/etc/init.d \
		$(1)/etc/hotplug.d/iface \
		$(1)/etc/hotplug.d/net \
		$(1)/usr/sbin
	$(INSTALL_BIN) ./files/uci-vrf $(1)/usr/sbin/uci-vrf
	$(INSTALL_BIN) ./files/uci-vrf.init $(1)/etc/init.d/uci-vrf
	$(INSTALL_DATA) ./files/uci-vrf.hotplug $(1)/etc/hotplug.d/iface/95-uci-vrf
	$(INSTALL_DATA) ./files/uci-vrf.hotplug $(1)/etc/hotplug.d/net/95-uci-vrf
endef

$(eval $(call BuildPackage,uci-vrf))
