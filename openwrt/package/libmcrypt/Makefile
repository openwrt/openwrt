# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libmcrypt
PKG_VERSION:=2.5.7
PKG_RELEASE:=1
PKG_MD5SUM:=b1be163143f8e8ed0474beeb642b3bad

PKG_SOURCE_URL:=@SF/mcrypt
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBMCRYPT,$(PKG_NAME),$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured: $(PKG_BUILD_DIR)/.prepared
	(cd $(PKG_BUILD_DIR); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib -L$(STAGING_DIR)/lib -lm" \
		./configure \
			--target=$(GNU_TARGET_NAME) \
			--host=$(GNU_TARGET_NAME) \
			--build=$(GNU_HOST_NAME) \
			--prefix=$(PKG_INSTALL_DIR) \
			--program-prefix="" \
			--program-suffix="" \
			--prefix=/usr \
			--exec-prefix=/usr \
			--bindir=/usr/bin \
			--datadir=/usr/share \
			--includedir=/usr/include \
			--infodir=/usr/share/info \
			--libdir=/usr/lib \
			--libexecdir=/usr/lib \
			--localstatedir=/var \
			--mandir=/usr/share/man \
			--sbindir=/usr/sbin \
			--sysconfdir=/etc \
			$(DISABLE_NLS) \
			$(DISABLE_LARGEFILE) \
			--enable-shared \
			--enable-static \
			--disable-rpath \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LIBMCRYPT):
	install -d -m0755 $(IDIR_LIBMCRYPT)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libmcrypt.so.* $(IDIR_LIBMCRYPT)/usr/lib
	$(RSTRIP) $(IDIR_LIBMCRYPT)
	$(IPKG_BUILD) $(IDIR_LIBMCRYPT) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libmcrypt.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/mcrypt.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libmcrypt.a $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libmcrypt.so* $(STAGING_DIR)/usr/lib/
	mkdir -p $(STAGING_DIR)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/libmcrypt-config $(STAGING_DIR)/usr/bin/
	$(SED) 's,-I/usr/include,,g' $(STAGING_DIR)/usr/bin/libmcrypt-config
	$(SED) 's,-L/usr/lib,,g' $(STAGING_DIR)/usr/bin/libmcrypt-config
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libmcrypt.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/include/mcrypt.h \
		$(STAGING_DIR)/usr/lib/libmcrypt.a \
		$(STAGING_DIR)/usr/lib/libmcrypt.so* \
		$(STAGING_DIR)/usr/bin/mcrypt-config \

compile-targets: install-dev
clean-targets: uninstall-dev

mostlyclean:
	$(MAKE) -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built
