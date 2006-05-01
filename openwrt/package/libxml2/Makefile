# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libxml2
PKG_VERSION:=2.6.20
PKG_RELEASE:=1
PKG_MD5SUM:=8f0b3ce721bda11401e656b90ba4e78c

PKG_SOURCE_URL:=http://xmlsoft.org/sources/ \
	ftp://fr.rpmfind.net/pub/libxml/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBXML2,libxml2,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(strip $(TARGET_CFLAGS))" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/lib -L$(STAGING_DIR)/usr/lib" \
		./configure \
			--target=$(GNU_TARGET_NAME) \
			--host=$(GNU_TARGET_NAME) \
			--build=$(GNU_HOST_NAME) \
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
			$(DISABLE_LARGEFILE) \
			$(DISABLE_NLS) \
			--enable-shared \
			--enable-static \
			--disable-rpath \
			--with-gnu-ld \
			--without-c14n \
			--without-catalog \
			--without-debug \
			--without-docbook \
			--with-html \
			--without-ftp \
			--without-http \
			--without-iconv \
			--without-iso8859x \
			--without-legacy \
			--with-output \
			--without-pattern \
			--without-push \
			--without-python \
			--with-reader \
			--without-readline \
			--without-regexps \
			--with-sax1 \
			--with-schemas \
			--with-threads \
			--with-tree \
			--with-valid \
			--with-writer \
			--with-xinclude \
			--with-xpath \
			--with-xptr \
			--with-zlib \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LIBXML2):
	install -m0755 -d $(IDIR_LIBXML2)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libxml2.so.* $(IDIR_LIBXML2)/usr/lib/
	$(RSTRIP) $(IDIR_LIBXML2)
	$(IPKG_BUILD) $(IDIR_LIBXML2) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libxml2.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/xml2-config $(STAGING_DIR)/usr/bin/
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/libxml2 $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libxml2.{a,so*} $(STAGING_DIR)/usr/lib/
	mkdir -p $(STAGING_DIR)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/libxml-2.0.pc $(STAGING_DIR)/usr/lib/pkgconfig/
	$(SED) 's,-I$${includedir}/libxml2,,g' $(STAGING_DIR)/usr/bin/xml2-config
	$(SED) 's,-I$${includedir},,g' $(STAGING_DIR)/usr/bin/xml2-config
	$(SED) 's,-I$(STAGING_DIR)/usr/include,,g' $(STAGING_DIR)/usr/bin/xml2-config
	$(SED) 's,-L$${libdir},,g' $(STAGING_DIR)/usr/bin/xml2-config
	$(SED) 's,-L$(STAGING_DIR)/usr/lib,,g' $(STAGING_DIR)/usr/bin/xml2-config
	$(SED) 's,-I$${includedir}/libxml2,,g' $(STAGING_DIR)/usr/lib/pkgconfig/libxml-2.0.pc
	$(SED) 's,-L$${libdir},,g' $(STAGING_DIR)/usr/lib/pkgconfig/libxml-2.0.pc
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libxml2.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/bin/xml2-config \
		$(STAGING_DIR)/usr/include/libxml2 \
		$(STAGING_DIR)/usr/lib/libxml2.{a,so*} \
		$(STAGING_DIR)/usr/lib/pkgconfig/libxml-2.0.pc \

compile-targets: install-dev
clean-targets: uninstall-dev

mostlyclean:
	make -C $(PKG_BUILD_DIR) clean
	rm $(PKG_BUILD_DIR)/.built

