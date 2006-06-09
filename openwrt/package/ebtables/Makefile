# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=ebtables
PKG_VERSION:=2.0.8-rc2
PKG_RELEASE:=1
PKG_MD5SUM:=f07111fcc1966be669278433c35dcc28

PKG_SOURCE_URL:=@SF/ebtables
PKG_SOURCE:=$(PKG_NAME)-v$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/ebtables-v$(PKG_VERSION)

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
	install -d -m0755 $(IDIR_EBTABLES)/etc
	install -m0644 $(PKG_BUILD_DIR)/ethertypes $(IDIR_EBTABLES)/etc/
	install -d -m0755 $(IDIR_EBTABLES)/usr/lib
	install -m0755 $(PKG_BUILD_DIR)/lib*.so $(IDIR_EBTABLES)/usr/lib/
	install -d -m0755 $(IDIR_EBTABLES)/usr/lib/ebtables
	install -m0755 $(PKG_BUILD_DIR)/extensions/*.so $(IDIR_EBTABLES)/usr/lib/ebtables/
	install -d -m0755 $(IDIR_EBTABLES)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/ebtables $(IDIR_EBTABLES)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/ebtables-save $(IDIR_EBTABLES)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/ebtables-restore $(IDIR_EBTABLES)/usr/sbin/
	$(RSTRIP) $(IDIR_EBTABLES)
	$(IPKG_BUILD) $(IDIR_EBTABLES) $(PACKAGE_DIR)
