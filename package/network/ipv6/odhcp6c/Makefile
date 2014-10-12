#
# Copyright (C) 2012-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=odhcp6c
PKG_VERSION:=2014-10-12
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=git://github.com/sbyx/odhcp6c.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=c52296c48abe2025abab03cea98f52a654b92ee2
PKG_MAINTAINER:=Steven Barth <steven@midlink.org>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

ifneq ($(CONFIG_PACKAGE_odhcp6c_ext_prefix_class),0)
  CMAKE_OPTIONS += -DEXT_PREFIX_CLASS=$(CONFIG_PACKAGE_odhcp6c_ext_prefix_class)
endif

ifneq ($(CONFIG_PACKAGE_odhcp6c_ext_cer_id),0)
  CMAKE_OPTIONS += -DEXT_CER_ID=$(CONFIG_PACKAGE_odhcp6c_ext_cer_id)
endif

define Package/odhcp6c
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Embedded DHCPv6-client for OpenWrt
  DEPENDS:=+kmod-ipv6
endef

define Package/odhcp6c/config
  config PACKAGE_odhcp6c_ext_prefix_class
    int "Prefix Class Extension ID (0 = disabled)"
    depends on PACKAGE_odhcp6c
    default 0

  config PACKAGE_odhcp6c_ext_cer_id
    int "CER-ID Extension ID (0 = disabled)"
    depends on PACKAGE_odhcp6c
    default 0
endef

define Package/odhcp6c/install
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/odhcp6c $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/lib/netifd/proto
	$(INSTALL_BIN) ./files/dhcpv6.sh $(1)/lib/netifd/proto/dhcpv6.sh
	$(INSTALL_BIN) ./files/dhcpv6.script $(1)/lib/netifd/
endef

$(eval $(call BuildPackage,odhcp6c))
