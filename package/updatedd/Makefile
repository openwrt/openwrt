# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=updatedd
PKG_VERSION:=2.5
PKG_RELEASE:=1
PKG_MD5SUM:=2957496de0c8e08e9c6492fabf1a88be

PKG_SOURCE_URL:=http://savannah.nongnu.org/download/updatedd/
PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,UPDATEDD,updatedd,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		./configure \
			--target=$(GNU_TARGET_NAME) \
			--host=$(GNU_TARGET_NAME) \
			--build=$(GNU_HOST_NAME) \
			--prefix=/usr \
			--sysconfdir=/etc \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(TARGET_CONFIGURE_OPTS) \
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_UPDATEDD):
	install -d -m0755 $(IDIR_UPDATEDD)/usr/bin
	cp -fpR $(PKG_INSTALL_DIR)/usr/bin/updatedd $(IDIR_UPDATEDD)/usr/bin/updatedd

	install -d -m0755 $(IDIR_UPDATEDD)/etc/init.d/
	install -m0755 ./files/S55ddns $(IDIR_UPDATEDD)/etc/init.d/S55ddns

	install -d -m0755 $(IDIR_UPDATEDD)/usr/lib/updatedd

ifeq ($(BR2_PACKAGE_UPDATEDD_CHANGEIP),y)
	install -m0644 $(PKG_INSTALL_DIR)/usr/lib/updatedd/changeip.so $(IDIR_UPDATEDD)/usr/lib/updatedd/changeip.so
endif
ifeq ($(BR2_PACKAGE_UPDATEDD_DYNDNS),y)
	install -m0644 $(PKG_INSTALL_DIR)/usr/lib/updatedd/dyndns.so $(IDIR_UPDATEDD)/usr/lib/updatedd/dyndns.so
endif
ifeq ($(BR2_PACKAGE_UPDATEDD_EURODYNDNS),y)
	install -m0644 $(PKG_INSTALL_DIR)/usr/lib/updatedd/eurodyndns.so $(IDIR_UPDATEDD)/usr/lib/updatedd/eurodyndns.so
endif
ifeq ($(BR2_PACKAGE_UPDATEDD_HN),y)
	install -m0644 $(PKG_INSTALL_DIR)/usr/lib/updatedd/hn.so $(IDIR_UPDATEDD)/usr/lib/updatedd/hn.so
endif
ifeq ($(BR2_PACKAGE_UPDATEDD_NOIP),y)
	install -m0644 $(PKG_INSTALL_DIR)/usr/lib/updatedd/noip.so $(IDIR_UPDATEDD)/usr/lib/updatedd/noip.so
endif
ifeq ($(BR2_PACKAGE_UPDATEDD_ODS),y)
	install -m0644 $(PKG_INSTALL_DIR)/usr/lib/updatedd/ods.so $(IDIR_UPDATEDD)/usr/lib/updatedd/ods.so
endif
ifeq ($(BR2_PACKAGE_UPDATEDD_OVH),y)
	install -m0644 $(PKG_INSTALL_DIR)/usr/lib/updatedd/ovh.so $(IDIR_UPDATEDD)/usr/lib/updatedd/ovh.so
endif
ifeq ($(BR2_PACKAGE_UPDATEDD_REGFISH),y)
	install -m0644 $(PKG_INSTALL_DIR)/usr/lib/updatedd/regfish.so $(IDIR_UPDATEDD)/usr/lib/updatedd/regfish.so
endif
ifeq ($(BR2_PACKAGE_UPDATEDD_TZO),y)
	install -m0644 $(PKG_INSTALL_DIR)/usr/lib/updatedd/tzo.so $(IDIR_UPDATEDD)/usr/lib/updatedd/tzo.so
endif

	$(RSTRIP) $(IDIR_UPDATEDD)
	$(IPKG_BUILD) $(IDIR_UPDATEDD) $(PACKAGE_DIR)
