# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=miax
PKG_VERSION:=1.4
PKG_RELEASE:=1
PKG_MD5SUM:=44f0d2ef46ee2697d890b7b96846adc7

PKG_SOURCE_URL:=http://mesh.dl.sourceforge.net/sourceforge/miax/
PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,MIAX,miax,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built:
	$(MAKE) -C "$(PKG_BUILD_DIR)" \
		CC="$(TARGET_CC)" \
		OFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
		all
	touch $@

$(IPKG_MIAX):
	install -d -m0755 $(IDIR_MIAX)/usr/bin
	install -m0755 $(PKG_BUILD_DIR)/miax $(IDIR_MIAX)/usr/bin/
	$(RSTRIP) $(IDIR_MIAX)
	$(IPKG_BUILD) $(IDIR_MIAX) $(PACKAGE_DIR)
