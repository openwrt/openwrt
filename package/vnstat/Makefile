# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=vnstat
PKG_VERSION:=1.4
PKG_RELEASE:=1
PKG_MD5SUM:=9184f79b5e60499bc059f670032291e5

PKG_SOURCE_URL:=http://humdi.net/vnstat
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,VNSTAT,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS)
	touch $@

$(IPKG_VNSTAT):
	install -d -m0755 $(IDIR_VNSTAT)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/src/vnstat $(IDIR_VNSTAT)/usr/bin/
	$(STRIP) $(IDIR_VNSTAT)/usr/bin/*
	$(IPKG_BUILD) $(IDIR_VNSTAT) $(PACKAGE_DIR)
