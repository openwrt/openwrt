# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libnl
PKG_VERSION:=1.0-pre7
PKG_RELEASE:=1

PKG_SOURCE:=libnl-1.0-pre7.tar.gz
PKG_SOURCE_URL:=http://www.suug.ch/~tgr/libnl/files/
PKG_MD5SUM:=db9f6265c370f09defd2d2fa3540758e

include $(INCLUDE_DIR)/package.mk

define Package/libnl
  SECTION:=net
  CATEGORY:=Base system
  TITLE:=netlink socket library
  URL:=http://www.suug.ch/~tgr/libnl/
endef

define Package/libnl/description
 This package contains a library for applications dealing with netlink sockets
endef

define Build/Compile
	$(call Build/Compile/Default)
	make -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		install
endef

ifneq ($(CONFIG_LINUX_2_6_23),)
  define Build/InstallDev
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
	mkdir -p $(1)/usr/include/libnl
	$(CP) $(PKG_BUILD_DIR)/include/linux $(1)/usr/include/libnl/
  endef

  define Build/UninstallDev
  endef

  define Package/libnl/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/lib/libnl.so.1 $(1)/usr/lib/
  endef
endif

$(eval $(call BuildPackage,libnl))
