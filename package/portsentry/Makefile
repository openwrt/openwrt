# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=portsentry
PKG_VERSION:=1.2
PKG_RELEASE:=1
PKG_MD5SUM:=3ebd3618ba9abfea2525e236bd44cebd

PKG_SOURCE_URL:=@SF/sentrytools
PKG_SOURCE:=$(PKG_NAME)-1.2.tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)_beta

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,PORTSENTRY,portsentry,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) linux
	touch $@

$(IPKG_PORTSENTRY): 
	mkdir -p $(IDIR_PORTSENTRY)/usr/bin
	mkdir -p $(IDIR_PORTSENTRY)/etc
	$(CP) $(PKG_BUILD_DIR)/$(PKG_NAME).conf $(IDIR_PORTSENTRY)/etc/
	$(CP) $(PKG_BUILD_DIR)/$(PKG_NAME) $(IDIR_PORTSENTRY)/usr/bin/
	$(STRIP) $(IDIR_PORTSENTRY)/usr/bin/*
	$(IPKG_BUILD) $(IDIR_PORTSENTRY) $(PACKAGE_DIR)

mostlyclean:
	$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built
