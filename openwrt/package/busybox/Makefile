# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=busybox
PKG_VERSION:=1.1.3
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.busybox.net/downloads
PKG_MD5SUM:=19a0b475169335f17e421cf644616fe7
PKG_CAT:=bzcat

include $(TOPDIR)/include/package.mk

define Package/busybox
SECTION:=base
CATEGORY:=Base system
MENU:=1
DEFAULT:=y
TITLE:=Core utilities for embedded Linux
DESCRIPTION:=The Swiss Army Knife of embedded Linux. \\\
It slices, it dices, it makes Julian Fries.
URL:=http://busybox.net/
CONFIG:=\\\
	menu "Configuration" \\\
		depends on PACKAGE_busybox \\\
		source "$(SOURCE)/config/Config.in" \\\
	endmenu
endef

define Build/Configure
	$(SCRIPT_DIR)/gen_busybox_config.pl $(TOPDIR)/.config > $(PKG_BUILD_DIR)/.config
	yes '' | $(MAKE) CC=$(TARGET_CC) CROSS="$(TARGET_CROSS)" -C $(PKG_BUILD_DIR) oldconfig
endef

define Package/busybox/install
	$(MAKE) CC=$(TARGET_CC) CROSS="$(TARGET_CROSS)" PREFIX="$(1)" \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" -C $(PKG_BUILD_DIR) install
	-rm -rf $(1)/lib64
endef

$(eval $(call BuildPackage,busybox))
