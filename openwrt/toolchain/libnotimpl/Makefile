# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME := libnotimpl
PKG_BUILD_DIR := $(TOOL_BUILD_DIR)/libnotimpl

include $(INCLUDE_DIR)/host-build.mk

define cc
	$(TARGET_CC) $(TARGET_CFLAGS) -c src/$(1).c -o $(PKG_BUILD_DIR)/$(1).o
endef

define Build/Prepare
	rm -rf $(PKG_BUILD_DIR)
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile
	$(call cc,math)
	$(TARGET_CROSS)ar rc $(PKG_BUILD_DIR)/libnotimpl.a $(PKG_BUILD_DIR)/*.o
endef

define Build/Install
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libnotimpl.a $(STAGING_DIR)/usr/lib/
endef

define Build/Clean
	rm -rf \
		$(STAGING_DIR)/usr/lib/libnotimpl.a \
		$(PKG_BUILD_DIR)
endef

$(eval $(call HostBuild))

