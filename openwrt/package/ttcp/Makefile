# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ttcp
PKG_VERSION:=3.8
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/ttcp

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,TTCP,ttcp,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.prepared:
	mkdir -p $@
	touch $@

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(TARGET_CC) $(TARGET_CFLAGS) -o $(PKG_BUILD_DIR)/ttcp ttcp.c
	touch $@

$(IPKG_TTCP):
	install -d -m0755 $(IDIR_TTCP)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/ttcp $(IDIR_TTCP)/usr/bin/
	$(RSTRIP) $(IDIR_TTCP)
	$(IPKG_BUILD) $(IDIR_TTCP) $(PACKAGE_DIR)
