# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=wificonf
PKG_RELEASE:=5

PKG_BUILD_DIR:=$(BUILD_DIR)/wificonf

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,WIFICONF,$(PKG_NAME),$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.prepared:
	mkdir -p $@
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(TARGET_CC) $(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include -o $(PKG_BUILD_DIR)/wifi wificonf.c -L$(STAGING_DIR)/usr/lib -lnvram -lshared $(STAGING_DIR)/usr/lib/libiw.so
	touch $@

$(IPKG_WIFICONF):
	install -d -m0755 $(IDIR_WIFICONF)/sbin
	install -m0755 $(PKG_BUILD_DIR)/wifi $(IDIR_WIFICONF)/sbin/
	$(RSTRIP) $(IDIR_WIFICONF)
	$(IPKG_BUILD) $(IDIR_WIFICONF) $(PACKAGE_DIR)
