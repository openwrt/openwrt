# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2022 Guilherme Janczak <guilherme.janczak@yandex.com>

include $(TOPDIR)/rules.mk

PKG_NAME:=libobsd
PKG_VERSION:=1.1.1
PKG_RELEASE:=$(AUTORELEASE)

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=https://github.com/guijan/libobsd/releases/download/v$(PKG_VERSION)/
PKG_HASH:=332b72ba5f9a76c40f8a526771b78dae3ac3388f689f818c0ac4ab77ef52809d

PKG_MAINTAINER:=Guilherme Janczak <guilherme.janczak@yandex.com>
PKG_LICENSE:=ISC AND BSD-3-Clause AND BSD-4-Clause
PKG_LICENSE_FILES:=LICENSE.txt

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/meson.mk

define Package/libobsd
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Transparent OpenBSD compatibility library
  URL:=https://github.com/guijan/libobsd/
  PROVIDES:=libbsd
  CONFLICTS:=
endef

MESON_ARGS += -Ddefault_library=both -Dprovide_libbsd=true

define Package/libobsd/description
  Libobsd is much like libbsd.
  It is an attempt at writing a very similar library with more modern tools and
  practices to make it easy to port, maintain, and add functions to the library.
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/obsd $(1)/usr/include/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libobsd.{a,so*} $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libbsd-overlay.pc $(1)/usr/lib/pkgconfig/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libobsd.pc $(1)/usr/lib/pkgconfig/
endef

define Package/libobsd/install
	$(INSTALL_DIR) $(1)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libobsd.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libobsd))
