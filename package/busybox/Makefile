# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=busybox
PKG_VERSION:=1.2.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.busybox.net/downloads
PKG_MD5SUM:=362b3dc0f2023ddfda901dc1f1a74391
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/busybox
  SECTION:=base
  CATEGORY:=Base system
  DEFAULT:=y
  TITLE:=Core utilities for embedded Linux
  DESCRIPTION:=\
	The Swiss Army Knife of embedded Linux. \\\
	It slices, it dices, it makes Julian Fries.
  URL:=http://busybox.net/
  MENU:=1
  CONFIG:=\\\
	menu "Configuration" \\\
		depends on PACKAGE_busybox \\\
		source "$(SOURCE)/config/Config.in" \\\
	endmenu
endef

define Build/Configure
	$(SCRIPT_DIR)/gen_busybox_config.pl $(TOPDIR)/.config > $(PKG_BUILD_DIR)/.config
	yes '' | $(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CROSS="$(TARGET_CROSS)" \
		oldconfig
endef

define Package/busybox/install
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CROSS="$(TARGET_CROSS)" \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		PREFIX="$(1)" \
		install
	-rm -rf $(1)/lib64
endef

$(eval $(call BuildPackage,busybox))
