#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=mkimage
PKG_VERSION:=2014.10

PKG_SOURCE:=u-boot-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=\
	http://mirror2.openwrt.org/sources \
	ftp://ftp.denx.de/pub/u-boot
PKG_MD5SUM:=3ddcaee2f05b7c464778112ec83664b5
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
	$(MAKE) -C $(HOST_BUILD_DIR) defconfig   HOSTCFLAGS="$(HOST_CPPFLAGS) $(HOST_CFLAGS)" HOSTLDFLAGS="$(HOST_LDFLAGS)"
	$(MAKE) -C $(HOST_BUILD_DIR) tools-only  HOSTCFLAGS="$(HOST_CPPFLAGS) $(HOST_CFLAGS)" HOSTLDFLAGS="$(HOST_LDFLAGS)"
endef

define Host/Install
	$(CP) $(HOST_BUILD_DIR)/tools/mkimage $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/mkimage
endef

$(eval $(call HostBuild))
