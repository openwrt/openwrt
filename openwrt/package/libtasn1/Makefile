# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=libtasn1
PKG_VERSION:=0.2.13
PKG_RELEASE:=0
PKG_MD5SUM:=d124b11908cfb88883fe71472c25d53a

PKG_SOURCE_URL:=ftp://ftp.gnutls.org/pub/gnutls/libtasn1/ \
	ftp://ftp.gnupg.org/gcrypt/alpha/gnutls/libtasn1/ \
	http://www.mirrors.wiretapped.net/security/network-security/gnutls/libtasn1/ \
	ftp://ftp.mirrors.wiretapped.net/pub/security/network-security/gnutls/libtasn1/ \
	http://josefsson.org/gnutls/releases/libtasn1/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBTASN1,libtasn1,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
		touch configure.ac ; \
		touch acinclude.m4 ; \
		touch aclocal.m4 ; \
		touch Makefile.in ; \
		touch config.h.in ; \
		touch configure ; \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="-I$(STAGING_DIR)/usr/include" \
		LDFLAGS="-L$(STAGING_DIR)/usr/lib" \
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
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LIBTASN1):
	install -m0755 -d $(IDIR_LIBTASN1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libtasn1.so.* $(IDIR_LIBTASN1)/usr/lib/
	$(RSTRIP) $(IDIR_LIBTASN1)
	$(IPKG_BUILD) $(IDIR_LIBTASN1) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libtasn1.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/libtasn1-config $(STAGING_DIR)/usr/bin/
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/libtasn1.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libtasn1.{a,so*} $(STAGING_DIR)/usr/lib/
	mkdir -p $(STAGING_DIR)/usr/share/aclocal
	$(CP) $(PKG_INSTALL_DIR)/usr/share/aclocal/libtasn1.m4 $(STAGING_DIR)/usr/share/aclocal/
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libtasn1.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/bin/libtasn1-config \
		$(STAGING_DIR)/usr/include/libtasn1.h \
		$(STAGING_DIR)/usr/lib/libtasn1.{a,so*} \
		$(STAGING_DIR)/usr/share/aclocal/libtasn1.m4 \

compile-targets: install-dev
clean-targets: uninstall-dev

mostlyclean:
	make -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built
