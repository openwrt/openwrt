#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=gmp
PKG_VERSION:=4.2.2
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@GNU/gmp
PKG_MD5SUM:=7ce52531644e6d12f16911b7e3151f3f

include $(INCLUDE_DIR)/package.mk

define Package/libgmp
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=GNU multiprecision arithmetic library
  URL:=http://gmplib.org/
endef

define Package/libgmp/description
	GMP is a free library for arbitrary precision arithmetic, operating on
	signed integers, rational numbers, and floating point numbers.
endef

TARGET_CFLAGS += $(FPIC)
CONFIGURE_VARS += CC="$(TARGET_CROSS)gcc"
CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \

define Build/Compile
	$(call Build/Compile/Default, \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		CC="$(TARGET_CC)" \
		all install \
	)
endef

define Build/InstallDev
	mkdir -p $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/gmp* $(1)/usr/include/
	mkdir -p $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgmp.{a,so*} $(1)/usr/lib/
endef

define Package/libgmp/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgmp.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libgmp))
