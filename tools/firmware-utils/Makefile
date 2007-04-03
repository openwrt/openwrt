# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/host-build.mk

PKG_NAME := firmware-utils
PKG_BUILD_DIR:=$(TOOL_BUILD_DIR)/firmware-utils

CFLAGS := -O2 -I $(STAGING_DIR)/include-host -include endian.h

define cc
	$(CC) $(CFLAGS) -o $(PKG_BUILD_DIR)/bin/$(1) src/$(1).c
endef

define Build/Compile
	mkdir -p $(PKG_BUILD_DIR)/bin
	$(call cc,addpattern)
	$(call cc,trx)
	$(call cc,motorola-bin)
	$(call cc,dgfirmware)
	$(call cc,trx2usr)
	$(call cc,ptgen)
	$(call cc,airlink)
	$(call cc,srec2bin)
	$(call cc,mkmylofw)
	$(call cc,mkcsysimg)
endef

define Build/Install
	$(CP) $(PKG_BUILD_DIR)/bin/* $(STAGING_DIR)/bin
endef

$(eval $(call HostBuild))
