# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=palantir
PKG_VERSION:=2.6
PKG_RELEASE:=1
PKG_MD5SUM:=8e4d20e7f7fad26d7447483c68894ace

PKG_SOURCE_URL:=http://www.fastpath.it/products/palantir/pub/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tgz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

export CPPFLAGS=-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include
export LDFLAGS=-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,PALANTIR,palantir,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)/usr
	$(MAKE) -C $(PKG_BUILD_DIR)/server/libgsm \
		$(TARGET_CONFIGURE_OPTS) \
		CCINC="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		lib/libgsm.a
	$(MAKE) -C $(PKG_BUILD_DIR)/server \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="-Wall -D_REENTRANT $(TARGET_CFLAGS)" \
		palantir
	touch $@

$(IPKG_PALANTIR):
	install -d -m0755 $(IDIR_PALANTIR)/etc
	install -m0644 $(PKG_BUILD_DIR)/server/palantir-mips.conf.sample $(IDIR_PALANTIR)/etc/palantir.conf
	install -d -m0755 $(IDIR_PALANTIR)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/server/palantir $(IDIR_PALANTIR)/usr/sbin/
	$(RSTRIP) $(IDIR_PALANTIR)
	$(IPKG_BUILD) $(IDIR_PALANTIR) $(PACKAGE_DIR)
