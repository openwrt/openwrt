#
# Copyright (C) 2007-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libaio
PKG_VERSION:=0.3.113
PKG_RELEASE:=3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://releases.pagure.org/libaio
PKG_HASH:=2c44d1c5fd0d43752287c9ae1eb9c023f04ef848ea8d4aafa46e9aedb678200b

PKG_MAINTAINER:=
PKG_LICENSE:=LGPL-2.1-only
PKG_LICENSE_FILES:=COPYING
PKG_CPE_ID:=cpe:/a:libaio_project:libaio

PKG_BUILD_PARALLEL:=1
PKG_BUILD_FLAGS:=no-mips16 no-lto
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/libaio
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Linux kernel AIO interface access library
  URL:=http://lse.sourceforge.net/io/aio.html
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/libaio.h $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libaio.{a,so*} $(1)/usr/lib/
endef

define Package/libaio/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libaio.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libaio))
