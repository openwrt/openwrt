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
$(eval $(call BuildPackage,BUSYBOX,busybox))

$(PKG_BUILD_DIR)/.configured:
	$(SCRIPT_DIR)/gen_busybox_config.pl $(TOPDIR)/.config > $(PKG_BUILD_DIR)/.config
	yes '' | $(MAKE) CC=$(TARGET_CC) CROSS="$(TARGET_CROSS)" -C $(PKG_BUILD_DIR) oldconfig
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) CC=$(TARGET_CC) CROSS="$(TARGET_CROSS)" PREFIX="$(IDIR_BUSYBOX)" \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" ARCH="$(ARCH)" -C $(PKG_BUILD_DIR)
	touch $@

$(IPKG_BUSYBOX):
	$(MAKE) CC=$(TARGET_CC) CROSS="$(TARGET_CROSS)" PREFIX="$(IDIR_BUSYBOX)" \
		EXTRA_CFLAGS="$(TARGET_CFLAGS)" -C $(PKG_BUILD_DIR) install
	$(STRIP) $(IDIR_BUSYBOX)/bin/busybox
	$(IPKG_BUILD) $(IDIR_BUSYBOX) $(PACKAGE_DIR)
