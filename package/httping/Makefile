# $Id: Makefile 1146 2005-06-05 13:32:28Z florian $

include $(TOPDIR)/rules.mk

PKG_NAME:=httping
PKG_VERSION:=1.0.10
PKG_RELEASE:=1
PKG_MD5SUM:=998b00b8babeb3196d28c20ad87d9c15

PKG_SOURCE_URL:=http://www.vanheusden.com/httping/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tgz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,HTTPING,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC)
	touch $@

$(IPKG_HTTPING): 
	mkdir -p $(IDIR_HTTPING)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/$(PKG_NAME) $(IDIR_HTTPING)/usr/sbin/
	$(STRIP) $(IDIR_HTTPING)/usr/sbin/*
	$(IPKG_BUILD) $(IDIR_HTTPING) $(PACKAGE_DIR)

mostlyclean:
	$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built
