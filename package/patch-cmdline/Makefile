# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: $

include $(TOPDIR)/rules.mk

PKG_NAME:=patch-cmdline

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/patch-cmdline
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Patch the kernel command line for RouterBoards
  DEPENDS:=@LINUX_2_6_ADM5120||@LINUX_2_6_RB532
  DESCRIPTION:=\
	A tool to patch the kernel command line on RouterBoards
  URL:=https://dev.openwrt.org/browser/trunk/package/patch-cmdline/src/patch-cmdline.c
  VERSION:=1
endef

define Build/Prepare
	$(CP) ./src/$(PKG_NAME).c $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) -o $(PKG_BUILD_DIR)/$(PKG_NAME) $(PKG_BUILD_DIR)/$(PKG_NAME).c
endef

define Package/patch-cmdline/install
	$(INSTALL_DIR) $(1)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/sbin/
endef

$(eval $(call BuildPackage,patch-cmdline))
