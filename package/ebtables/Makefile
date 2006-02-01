# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ebtables
PKG_VERSION:=2.0.6
PKG_RELEASE:=1
PKG_MD5SUM:=c4559af2366c764c6c42a3fdd40d60d3

PKG_SOURCE_URL:=@SF/ebtables
PKG_SOURCE:=$(PKG_NAME)-v$(PKG_VERSION).tar.gz
PKG_BUILD_DIR:=$(BUILD_DIR)/ebtables-v$(PKG_VERSION)
PKG_CAT:=zcat

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,EBTABLES,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) LD=$(TARGET_CROSS)ld \
		CFLAGS="$(TARGET_CFLAGS) -Wall"
	touch $@

$(IPKG_EBTABLES):
	mkdir -p $(IDIR_EBTABLES)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/ebtables $(IDIR_EBTABLES)/usr/sbin/
	$(IPKG_BUILD) $(IDIR_EBTABLES) $(PACKAGE_DIR)

