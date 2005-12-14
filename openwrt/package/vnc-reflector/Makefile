# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=vnc-reflector
PKG_VERSION:=1.2.4
PKG_RELEASE:=1
PKG_MD5SUM:=c3f88bc62f228b335c25c07f9744ab0c

PKG_SOURCE_URL:=@SF/vnc-reflector
PKG_SOURCE:=vnc_reflector-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/vnc_reflector

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,VNC_REFLECTOR,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC=$(TARGET_CC) \
		OFLAGS="$(TARGET_CFLAGS)" \
		STAGING_DIR=$(STAGING_DIR) \
		default
	touch $@

$(IPKG_VNC_REFLECTOR):
	install -d -m0755 $(IDIR_VNC_REFLECTOR)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/vncreflector $(IDIR_VNC_REFLECTOR)/usr/sbin/
	$(RSTRIP) $(IDIR_VNC_REFLECTOR)
	$(IPKG_BUILD) $(IDIR_VNC_REFLECTOR) $(PACKAGE_DIR)
