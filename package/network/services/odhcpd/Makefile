#
# Copyright (C) 2013-2025 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=odhcpd
PKG_RELEASE:=2

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(PROJECT_GIT)/project/odhcpd.git
PKG_MIRROR_HASH:=52c861067b98cc389694a929e003b067d269b5d3eca1145315fef236efbea113
PKG_SOURCE_DATE:=2026-10-28
PKG_SOURCE_VERSION:=5f5f900a7177b23588f290685609cec1b9ddd7c5

PKG_MAINTAINER:=Álvaro Fernández Rojas <noltari@gmail.com>
PKG_LICENSE:=GPL-2.0

PKG_ASLR_PIE_REGULAR:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/odhcpd/default
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libjson-c +libubox +libuci +libubus +libnl-tiny
endef

define Package/odhcpd/default/description
 odhcpd is a daemon for serving and relaying IP management protocols to
 configure clients and downstream routers. It tries to follow the RFC 6204
 requirements for IPv6 home routers.
endef

define Package/odhcpd
  $(Package/odhcpd/default)
  TITLE:=OpenWrt DHCPv4/DHCPv6/NDP/RA server
  VARIANT:=full
  PROVIDES:=odhcpd-ipv6only
endef

define Package/odhcpd/description
 $(Package/odhcpd/default/description)

 This is a variant with support for RA, DHCPv4 and DHCPv6. It can also be used
 to relay RA, DHCPv6 and NDP messages between routed (non-bridged) interfaces
 in case no delegated prefixes are available.
endef

define Package/odhcpd-ipv6only
  $(Package/odhcpd/default)
  TITLE:=OpenWrt DHCPv6/NDP/RA server (without DHCPv4)
  VARIANT:=ipv6only
  DEFAULT_VARIANT:=1
  CONFLICTS:=odhcpd
  DEPENDS+=@IPV6
endef

define Package/odhcpd-ipv6only/description
 $(Package/odhcpd/default/description)

 This is a variant with support for RA and DHCPv6. It can also be used to
 relay RA, DHCPv6 and NDP messages between routed (non-bridged) interfaces
 in case no delegated prefixes are available.
endef

CMAKE_OPTIONS += -DUBUS=1

ifeq ($(BUILD_VARIANT),full)
  CMAKE_OPTIONS += -DDHCPV4_SUPPORT=1
endif

define Package/odhcpd/install
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/odhcpd $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/odhcpd-update $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/odhcpd.init $(1)/etc/init.d/odhcpd
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) ./files/odhcpd.defaults $(1)/etc/uci-defaults/15_odhcpd
endef

Package/odhcpd-ipv6only/install = $(Package/odhcpd/install)

$(eval $(call BuildPackage,odhcpd))
$(eval $(call BuildPackage,odhcpd-ipv6only))
