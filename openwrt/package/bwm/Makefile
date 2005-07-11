# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=bwm
PKG_VERSION:=1.1.0
PKG_RELEASE:=1
PKG_MD5SUM:=51021a036acb92d2bda0c0c0483f9552

PKG_SOURCE_URL:=http://ftp.debian.org/debian/pool/main/b/bwm
PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).orig.tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION).orig
PKG_CAT:=zcat

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,BWM,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(TARGET_CC) $(TARGET_CFLAGS) $(PKG_BUILD_DIR)/bwm.c -o $(PKG_BUILD_DIR)/bwm
	touch $@

$(IPKG_BWM):
	mkdir -p $(IDIR_BWM)/usr/bin
	install -m 755 $(PKG_BUILD_DIR)/$(PKG_NAME) $(IDIR_BWM)/usr/bin/
	$(STRIP) $(IDIR_BWM)/usr/bin/*
	$(IPKG_BUILD) $(IDIR_BWM) $(PACKAGE_DIR)
