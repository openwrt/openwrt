# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME := squashfs
PKG_VERSION := 3.0

PKG_SOURCE := $(PKG_NAME)$(PKG_VERSION).tar.gz
PKG_SOURCE_URL := @SF/squashfs
PKG_MD5SUM := 9fd05d0bfbb712f5fb95edafea5bc733
PKG_CAT := zcat

PKG_BUILD_DIR := $(TOOL_BUILD_DIR)/squashfs$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/squashfs-tools mksquashfs-lzma LZMAPATH=$(STAGING_DIR)/host/lib
endef

define Build/Install
	$(CP) $(PKG_BUILD_DIR)/squashfs-tools/mksquashfs-lzma $(STAGING_DIR)/bin/
endef

$(eval $(call HostBuild))
