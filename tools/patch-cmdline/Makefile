# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=patch-cmdline

PKG_BUILD_DIR:=$(TOOL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/host-build.mk

OS:=$(shell uname)
ifeq ($(HOST_OS),Darwin)
  CFLAGS += -I./include
endif
CFLAGS += -I $(TOPDIR)/tools/include -include endian.h

define Build/Compile
	$(CC) $(CFLAGS) -o $(PKG_BUILD_DIR)/$(PKG_NAME) src/$(PKG_NAME).c
endef

define Build/Install
	mkdir -p $(STAGING_DIR)/bin
	$(CP) $(PKG_BUILD_DIR)/patch-cmdline $(STAGING_DIR)/bin/
endef

define Build/Clean
	-rm -f $(STAGING_DIR)/bin/patch-cmdline
endef

$(eval $(call HostBuild))
