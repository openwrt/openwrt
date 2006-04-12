# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=udp-broadcast-relay
PKG_VERSION:=0.3
PKG_RELEASE:=1
PKG_MD5SUM:=a32f983b7063d6ac670e6b22be9b9d24

PKG_SOURCE_URL:=http://www.joachim-breitner.de/udp-broadcast-relay/files/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,UDP_BROADCAST_RELAY,udp-broadcast-relay,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(TARGET_CC) $(PKG_BUILD_DIR)/main.c -o $(PKG_BUILD_DIR)/$(PKG_NAME)
	touch $@

$(IPKG_UDP_BROADCAST_RELAY): 
	mkdir -p $(IDIR_UDP_BROADCAST_RELAY)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/$(PKG_NAME) $(IDIR_UDP_BROADCAST_RELAY)/usr/sbin/
	$(STRIP) $(IDIR_UDP_BROADCAST_RELAY)/usr/sbin/*
	$(IPKG_BUILD) $(IDIR_UDP_BROADCAST_RELAY) $(PACKAGE_DIR)

mostlyclean:
	$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built
