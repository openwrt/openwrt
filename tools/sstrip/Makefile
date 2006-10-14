# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/host-build.mk

PKG_BUILD_DIR:=$(TOOL_BUILD_DIR)/sstrip

OS:=$(shell uname)
ifeq ($(HOST_OS),Darwin)
  CFLAGS += -I./include
endif
CFLAGS += -I $(TOPDIR)/tools/include -include endian.h

define Build/Compile
	$(CC) $(CFLAGS) -o $(PKG_BUILD_DIR)/sstrip src/sstrip.c
endef

define Build/Install
	mkdir -p $(STAGING_DIR)/bin
	$(CP) $(PKG_BUILD_DIR)/sstrip $(STAGING_DIR)/bin/
endef

define Build/Clean
	rm -f $(STAGING_DIR)/bin/sstrip
endef

$(eval $(call HostBuild))
