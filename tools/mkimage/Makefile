# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=mkimage

include $(INCLUDE_DIR)/host-build.mk

define Build/Compile
	$(HOSTCC) $(HOST_CFLAGS) -O -c src/crc32.c -o $(PKG_BUILD_DIR)/crc32.o
	$(HOSTCC) $(HOST_CFLAGS) -O -c src/mkimage.c -o $(PKG_BUILD_DIR)/mkimage.o
	$(HOSTCC) $(HOST_CFLAGS) -O -o $(PKG_BUILD_DIR)/mkimage $(PKG_BUILD_DIR)/mkimage.o $(PKG_BUILD_DIR)/crc32.o
endef

define Build/Install
	$(CP) $(PKG_BUILD_DIR)/mkimage $(STAGING_DIR_HOST)/bin/
endef

define Build/Clean
	rm -f $(STAGING_DIR_HOST)/bin/mkimage
endef

$(eval $(call HostBuild))
