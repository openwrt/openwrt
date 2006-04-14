# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=busybox
PKG_RELEASE:=1

PKG_VERSION:=1.1.2
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.busybox.net/downloads
PKG_MD5SUM:=0b5d78072302fe687407956537bdb9e3
PKG_CAT:=bzcat

include $(TOPDIR)/package/rules.mk

define Package/busybox
CONFIGFILE:=config/Config.in
SECTION:=base
CATEGORY:=Base system
TITLE:=Core utilities
DESCRIPTION:=Core utilities for embedded Linux systems
endef

define Build/Configure
	$(SCRIPT_DIR)/gen_busybox_config.pl $(TOPDIR)/.config > $(PKG_BUILD_DIR)/.config
	yes '' | $(MAKE) CC=$(TARGET_CC) CROSS="$(TARGET_CROSS)" -C $(PKG_BUILD_DIR) oldconfig
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) \
		CROSS="$(TARGET_CROSS)" \
		PREFIX="$(IDIR_BUSYBOX)" \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" \
		ARCH="$(ARCH)" 
endef

define Package/busybox/install
	$(MAKE) CC=$(TARGET_CC) CROSS="$(TARGET_CROSS)" PREFIX="$(1)" \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" -C $(PKG_BUILD_DIR) install
	$(STRIP) $(1)/bin/busybox
endef

$(eval $(call BuildPackage,busybox))
