# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libupnp
PKG_VERSION:=1.2.1a
PKG_RELEASE:=1
PKG_MD5SUM:=e72b3550bf064eedf080f16f09688891

PKG_SOURCE_URL:=@SF/upnp
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBUPNP,libupnp,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	touch $@

$(PKG_BUILD_DIR)/.built: $(PKG_BUILD_DIR)/.configured
	$(MAKE) -C $(PKG_BUILD_DIR)/upnp \
		$(TARGET_CONFIGURE_OPTS) \
		OPT_FLAGS="$(TARGET_CFLAGS)" \
		STRIP=/bin/true
	touch $@

$(IPKG_LIBUPNP):
	install -m0755 -d $(IDIR_LIBUPNP)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/upnp/bin/libixml.so $(IDIR_LIBUPNP)/usr/lib/ ; \
	$(CP) $(PKG_BUILD_DIR)/upnp/bin/libthreadutil.so $(IDIR_LIBUPNP)/usr/lib/ ; \
	$(CP) $(PKG_BUILD_DIR)/upnp/bin/libupnp.so $(IDIR_LIBUPNP)/usr/lib/ ; \
	$(RSTRIP) $(IDIR_LIBUPNP)
	$(IPKG_BUILD) $(IDIR_LIBUPNP) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libupnp.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include/upnp
	$(CP) $(PKG_BUILD_DIR)/upnp/inc/*.h $(STAGING_DIR)/usr/include/upnp/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/upnp/bin/libixml.so $(STAGING_DIR)/usr/lib/ ; \
	$(CP) $(PKG_BUILD_DIR)/upnp/bin/libthreadutil.so $(STAGING_DIR)/usr/lib/ ; \
	$(CP) $(PKG_BUILD_DIR)/upnp/bin/libupnp.so $(STAGING_DIR)/usr/lib/ ; \
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libupnp.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/include/upnp \
		$(STAGING_DIR)/usr/lib/libixml.so \
		$(STAGING_DIR)/usr/lib/libthreadutil.so \
		$(STAGING_DIR)/usr/lib/libupnp.so \

compile-targets: install-dev
clean-targets: uninstall-dev
