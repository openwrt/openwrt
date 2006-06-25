# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=gnutls
PKG_VERSION:=1.0.25
PKG_RELEASE:=0
PKG_MD5SUM:=3585b5b204135e51e0efc9084b3e028b

PKG_SOURCE_URL:=ftp://ftp.gnutls.org/pub/gnutls/ \
	ftp://ftp.gnupg.org/gcrypt/alpha/gnutls/ \
	http://www.mirrors.wiretapped.net/security/network-security/gnutls/ \
	ftp://ftp.mirrors.wiretapped.net/pub/security/network-security/gnutls/ \
	http://josefsson.org/gnutls/releases/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBGNUTLS,libgnutls,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,GNUTLS_UTILS,gnutls-utils,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,LIBGNUTLS_OPENSSL,libgnutls-openssl,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		touch configure.in ; \
		touch acinclude.m4 ; \
		touch aclocal.m4 ; \
		touch Makefile.in ; \
		touch config.h.in ; \
		touch configure ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
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
		$(DISABLE_NLS) \
		$(DISABLE_LARGEFILE) \
		--enable-shared \
		--enable-static \
		--disable-rpath \
		--disable-srp-authentication \
		--disable-anon-authentication \
		--disable-openpgp-authentication \
		--with-included-opencdk=yes \
		--with-included-libtasn1=yes \
		--with-included-libcfg=yes \
		--without-zlib \
		--without-lzo \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LIBGNUTLS):
	install -m0755 -d $(IDIR_LIBGNUTLS)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgnutls{,-extra}.so.* $(IDIR_LIBGNUTLS)/usr/lib/
	$(RSTRIP) $(IDIR_LIBGNUTLS)
	$(IPKG_BUILD) $(IDIR_LIBGNUTLS) $(PACKAGE_DIR)

$(IPKG_LIBGNUTLS_OPENSSL):
	install -m0755 -d $(IDIR_LIBGNUTLS_OPENSSL)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgnutls-openssl.so.* $(IDIR_LIBGNUTLS_OPENSSL)/usr/lib/
	$(RSTRIP) $(IDIR_LIBGNUTLS_OPENSSL)
	$(IPKG_BUILD) $(IDIR_LIBGNUTLS_OPENSSL) $(PACKAGE_DIR)

$(IPKG_GNUTLS_UTILS):
	install -m0755 -d $(IDIR_GNUTLS_UTILS)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/certtool $(IDIR_GNUTLS_UTILS)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/gnutls-{cli,serv} $(IDIR_GNUTLS_UTILS)/usr/bin/
	$(RSTRIP) $(IDIR_GNUTLS_UTILS)
	$(IPKG_BUILD) $(IDIR_GNUTLS_UTILS) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libgnutls.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/libgnutls{,-extra}-config $(STAGING_DIR)/usr/bin/
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/gnutls $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libgnutls{,-extra,-openssl}.{a,so*} $(STAGING_DIR)/usr/lib/
	mkdir -p $(STAGING_DIR)/usr/lib/pkgconfig
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/pkgconfig/gnutls{,-extra}.pc $(STAGING_DIR)/usr/lib/pkgconfig/
	mkdir -p $(STAGING_DIR)/usr/share/aclocal
	$(CP) $(PKG_INSTALL_DIR)/usr/share/aclocal/libgnutls{,-extra}.m4 $(STAGING_DIR)/usr/share/aclocal/
	$(SED) 's,-I$${includedir},,g' $(STAGING_DIR)/usr/lib/pkgconfig/gnutls{,-extra}.pc
	$(SED) 's,-L$${libdir},,g' $(STAGING_DIR)/usr/lib/pkgconfig/gnutls{,-extra}.pc
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libgnutls.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/bin/libgnutls{,-extra}-config \
		$(STAGING_DIR)/usr/include/gnutls \
		$(STAGING_DIR)/usr/lib/libgnutls{,-extra,-openssl}.{a,so*} \
		$(STAGING_DIR)/usr/lib/pkgconfig/gnutls{,-extra}.pc \
		$(STAGING_DIR)/usr/share/aclocal/libgnutls{,-extra}.m4 \

compile-targets: install-dev
clean-targets: uninstall-dev

mostlyclean:
	make -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built
