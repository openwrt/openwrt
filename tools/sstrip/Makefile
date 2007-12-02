# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=sstrip

include $(INCLUDE_DIR)/host-build.mk

define Build/Compile
	$(CC) $(HOST_CFLAGS) -I./include -include endian.h -o $(PKG_BUILD_DIR)/sstrip src/sstrip.c
endef

define Build/Install
	$(CP) $(PKG_BUILD_DIR)/sstrip $(STAGING_DIR_HOST)/bin/
endef

define Build/Clean
	rm -f $(STAGING_DIR_HOST)/bin/sstrip
endef

$(eval $(call HostBuild))
