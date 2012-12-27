#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=6distributed
PKG_VERSION:=2012-12-27
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=git://github.com/sbyx/6distributed.git
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=0cf0ed43f9387070eebbb8435186478288d1a137

PKG_MAINTAINER:=Steven Barth <steven@midlink.org>

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/6distributed
  SECTION:=ipv6
  CATEGORY:=IPv6
  TITLE:=IPv6 host prefix distribution daemon
  DEPENDS:=+kmod-ipv6 +libubus +libubox
endef

define Package/6distributed/description
 6distributed is a daemon that distributes IPv6 prefixes locally across
 downstream interfaces. It supports valid and preferred lifetimes and
 delayed allocation so that an already acquired prefix can be distributed
 on interfaces that come up later.
endef

define Package/6distributed/install
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/6distributed $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/6distributed.init $(1)/etc/init.d/6distributed
endef

$(eval $(call BuildPackage,6distributed))
