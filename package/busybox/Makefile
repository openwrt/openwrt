# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=busybox
PKG_VERSION:=1.4.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.busybox.net/downloads \
		http://distfiles.gentoo.org/distfiles/
PKG_MD5SUM:=5728403bce309cdabcffa414e2e64052
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_BUILDDEP:=libcgi

include $(INCLUDE_DIR)/package.mk

init-y :=
init-$(CONFIG_BUSYBOX_CONFIG_HTTPD) += httpd
init-$(CONFIG_BUSYBOX_CONFIG_CROND) += cron
init-$(CONFIG_BUSYBOX_CONFIG_TELNETD) += telnet

define Package/busybox
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=Core utilities for embedded Linux
  URL:=http://busybox.net/
  MENU:=1
endef

define Package/busybox/description
The Swiss Army Knife of embedded Linux.
	It slices, it dices, it makes Julian Fries.
endef

define Package/busybox/config
	source "$(SOURCE)/target-config.in"
	menu "Configuration"
		depends on PACKAGE_busybox
		source "$(SOURCE)/config/Config.in"
	endmenu
endef

define Build/Configure
	$(SCRIPT_DIR)/gen_busybox_config.pl $(TOPDIR)/.config > $(PKG_BUILD_DIR)/.config
	yes '' | $(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(ARCH)" \
		oldconfig
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		EXTRA_CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include" \
		ARCH="$(ARCH)" \
		IPKG_ARCH="$(ARCH)" \
		EXTRA_LIBS="$(STAGING_DIR)/usr/lib/libcgi.a" \
		all
endef

define Package/busybox/install
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		EXTRA_CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include" \
		ARCH="$(ARCH)" \
		IPKG_ARCH="$(ARCH)" \
		CONFIG_PREFIX="$(1)" \
		EXTRA_LIBS="$(STAGING_DIR)/usr/lib/libcgi.a" \
		install
	$(INSTALL_DIR) $(1)/etc/init.d
	for tmp in $(init-y); do \
		$(INSTALL_BIN) ./files/$$$$tmp $(1)/etc/init.d/$$$$tmp; \
	done
	-rm -rf $(1)/lib64
endef

$(eval $(call BuildPackage,busybox))
