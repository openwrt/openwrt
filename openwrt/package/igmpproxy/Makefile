# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=igmpproxy
PKG_VERSION:=0.1-beta2
PKG_RELEASE:=1
PKG_MD5SUM:=2a5a59480f44d4b14077a6b5319e9940

PKG_SOURCE_URL:=@SF/igmpproxy
PKG_SOURCE:=$(PKG_NAME)-src-$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_CAT:=zcat

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,IGMPPROXY,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) CC=$(TARGET_CC) CFLAGS="$(TARGET_CFLAGS)" -C $(PKG_BUILD_DIR)/src
	touch $@

$(IPKG_IGMPPROXY):
	mkdir -p $(IDIR_IGMPPROXY)/usr/sbin $(IDIR_IGMPPROXY)/etc
	$(CP) $(PKG_BUILD_DIR)/src/$(PKG_NAME) $(IDIR_IGMPPROXY)/usr/sbin/
	$(CP) $(PKG_BUILD_DIR)/src/$(PKG_NAME).conf $(IDIR_IGMPPROXY)/etc
	$(STRIP) $(IDIR_IGMPPROXY)/usr/sbin/*
	$(IPKG_BUILD) $(IDIR_IGMPPROXY) $(PACKAGE_DIR)
