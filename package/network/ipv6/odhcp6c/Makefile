#
# Copyright (C) 2012-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=odhcp6c
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(PROJECT_GIT)/project/odhcp6c.git
PKG_SOURCE_DATE:=2025-12-17
PKG_SOURCE_VERSION:=2582843959a7fdfd962a3f005cbd09475ed186bb
PKG_MIRROR_HASH:=fca98c3d3d02a5e97b99eb3ccb75671afd8774c8193d22ee1318587b6d9ee138

PKG_MAINTAINER:=Álvaro Fernández Rojas <noltari@gmail.com>
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_OPTIONS += -DUBUS=1

define Package/odhcp6c
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Embedded DHCPv6-client for OpenWrt
  DEPENDS:=@IPV6 +libubox +libubus
endef

define Package/odhcp6c/conffiles
/etc/odhcp6c.user
/etc/odhcp6c.user.d/
endef

define Package/odhcp6c/install
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/odhcp6c $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/lib/netifd/proto
	$(INSTALL_BIN) ./files/dhcpv6.sh $(1)/lib/netifd/proto/dhcpv6.sh
	$(INSTALL_BIN) ./files/dhcpv6.script $(1)/lib/netifd/
	$(INSTALL_DIR) $(1)/etc/odhcp6c.user.d/
	$(INSTALL_CONF) ./files/odhcp6c.user $(1)/etc/
endef

$(eval $(call BuildPackage,odhcp6c))
