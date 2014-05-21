#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=mkimage
PKG_VERSION:=2014.04

PKG_SOURCE:=u-boot-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=ftp://ftp.denx.de/pub/u-boot
PKG_MD5SUM:=6d2116d1385a66e9a59742caa9d62a54
PKG_CAT:=bzcat

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/u-boot-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

define Host/Prepare
	$(Host/Prepare/Default)
	rm -f \
		$(HOST_BUILD_DIR)/include/errno.h \
		$(HOST_BUILD_DIR)/include/malloc.h \
		$(HOST_BUILD_DIR)/tools/.depend
	touch $(HOST_BUILD_DIR)/include/config.mk
	touch $(HOST_BUILD_DIR)/include/config.h
endef

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR) \
		HOSTLDFLAGS="$(HOST_STATIC_LINKING)" \
		tools-only
endef

define Host/Install
	$(CP) $(HOST_BUILD_DIR)/tools/mkimage $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/mkimage
endef

$(eval $(call HostBuild))
