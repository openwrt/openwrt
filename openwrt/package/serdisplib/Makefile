include $(TOPDIR)/rules.mk

PKG_NAME:=serdisplib
PKG_VERSION:=1.96
PKG_RELEASE:=1
PKG_MD5SUM:=b074e02d8d63f34cba8ec45c23f63f78

PKG_SOURCE_URL:=http://dl.sourceforge.net/sourceforge/serdisplib
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,SERDISPLIB,serdisplib,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib" \
		./configure \
			--target=$(GNU_TARGET_NAME) \
			--host=$(GNU_TARGET_NAME) \
			--build=$(GNU_HOST_NAME) \
			--prefix=/usr \
			--without-libiconv-prefix \
			--without-libintl-prefix \
			--disable-nls \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)/usr/bin
	mkdir -p $(PKG_INSTALL_DIR)/usr/lib
	$(MAKE) -C $(PKG_BUILD_DIR)/src \
		prefix="$(PKG_INSTALL_DIR)/usr"
	cp -fpR $(PKG_BUILD_DIR)/src/testserdisp $(PKG_INSTALL_DIR)/usr/bin
	cp -fpR $(PKG_BUILD_DIR)/lib/libserdisp.so* $(PKG_INSTALL_DIR)/usr/lib
	touch $@

$(IPKG_SERDISPLIB):
	install -d -m0755 $(IDIR_SERDISPLIB)/usr/bin
	cp -fpR $(PKG_INSTALL_DIR)/usr/bin/testserdisp $(IDIR_SERDISPLIB)/usr/bin
	install -d -m0755 $(IDIR_SERDISPLIB)/usr/lib
	cp -fpR $(PKG_INSTALL_DIR)/usr/lib/libserdisp.so* $(IDIR_SERDISPLIB)/usr/lib
	$(RSTRIP) $(IDIR_SERDISPLIB)
	$(IPKG_BUILD) $(IDIR_SERDISPLIB) $(PACKAGE_DIR)

mostlyclean:
	make -C $(PKG_BUILD_DIR) clean
	rm $(PKG_BUILD_DIR)/.built
