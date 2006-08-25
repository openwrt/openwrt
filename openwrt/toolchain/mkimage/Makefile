# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/host-build.mk

PKG_BUILD_DIR := $(TOOL_BUILD_DIR)/mkimage

define Build/Compile
	$(CC) -O -c src/crc32.c -o $(PKG_BUILD_DIR)/crc32.o
	$(CC) -O -c src/mkimage.c -o $(PKG_BUILD_DIR)/mkimage.o
	$(CC) -O -o $(PKG_BUILD_DIR)/mkimage $(PKG_BUILD_DIR)/mkimage.o $(PKG_BUILD_DIR)/crc32.o
endef

define Build/Install
	$(CP) $(PKG_BUILD_DIR)/mkimage $(STAGING_DIR)/bin
endef

define Build/Clean
	rm -f $(STAGING_DIR)/bin/mkimage
endef

$(eval $(call HostBuild))
