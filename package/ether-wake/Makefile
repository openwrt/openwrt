# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ether-wake
PKG_VERSION:=1.09
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,ETHER_WAKE,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.prepared:
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./files/$(PKG_NAME).c $(PKG_BUILD_DIR)/
	$(PATCH) $(PKG_BUILD_DIR) ./patches
	touch $@

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(TARGET_CC) $(TARGET_CFLAGS) -o $(PKG_BUILD_DIR)/$(PKG_NAME) $(PKG_BUILD_DIR)/$(PKG_NAME).c
	touch $@

$(IPKG_ETHER_WAKE):
	install -d -m0755 $(IDIR_ETHER_WAKE)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/$(PKG_NAME) $(IDIR_ETHER_WAKE)/usr/sbin/
	$(RSTRIP) $(IDIR_ETHER_WAKE)
	$(IPKG_BUILD) $(IDIR_ETHER_WAKE) $(PACKAGE_DIR)
