#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=omcproxy
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(PROJECT_GIT)/project/omcproxy.git
PKG_MIRROR_HASH:=4e62452e3b5a6df917e07db6e19b09e0fb67bac246b8fc3f9e6a35a12d90894b
PKG_SOURCE_DATE:=2025-10-04
PKG_SOURCE_VERSION:=fc2eac19ea031411ab589cd92d713961549d6a46
PKG_MAINTAINER:=Steven Barth <cyrus@openwrt.org>
PKG_LICENSE:=Apache-2.0

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/omcproxy
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libubox +libubus
  TITLE:=IGMPv3 and MLDv2 Multicast Proxy
endef

define Package/omcproxy/conffiles
/etc/config/omcproxy
endef

CMAKE_OPTIONS += -DWITH_LIBUBOX=1

define Package/omcproxy/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/omcproxy.config $(1)/etc/config/omcproxy
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/omcproxy.init $(1)/etc/init.d/omcproxy
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/omcproxy $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,omcproxy))
