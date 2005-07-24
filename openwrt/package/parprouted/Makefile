# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=parprouted
PKG_VERSION:=0.63
PKG_RELEASE:=1
PKG_MD5SUM:=12753098a22e82997d1941d6d2284750

PKG_SOURCE_URL:=http://www.hazard.maks.net/parprouted/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,PARPROUTED,parprouted,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="" \
		all
	touch $@

$(IPKG_PARPROUTED): 
	install -d -m0755 $(IDIR_PARPROUTED)/etc/default
	install -m0755 ./files/parprouted.default $(IDIR_PARPROUTED)/etc/default/parprouted
	install -d -m0755 $(IDIR_PARPROUTED)/etc/init.d
	install -m0755 ./files/parprouted.init $(IDIR_PARPROUTED)/etc/init.d/parprouted
	ln -sf parprouted $(IDIR_PARPROUTED)/etc/init.d/S50parpouted
	install -d -m0755 $(IDIR_PARPROUTED)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/parprouted $(IDIR_PARPROUTED)/usr/sbin/
	$(RSTRIP) $(IDIR_PARPROUTED)
	$(IPKG_BUILD) $(IDIR_PARPROUTED) $(PACKAGE_DIR)

mostlyclean:
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built
