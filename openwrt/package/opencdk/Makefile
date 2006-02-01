# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=opencdk
PKG_VERSION:=0.5.5
PKG_RELEASE:=0
PKG_MD5SUM:=aaff60107f0153873192ffda00699a96

PKG_SOURCE_URL:=ftp://ftp.gnutls.org/pub/gnutls/opencdk/ \
	ftp://ftp.gnupg.org/gcrypt/alpha/gnutls/opencdk/ \
	http://www.mirrors.wiretapped.net/security/network-security/gnutls/opencdk/ \
	ftp://ftp.mirrors.wiretapped.net/pub/security/network-security/gnutls/opencdk/ \
	http://josefsson.org/gnutls/releases/opencdk/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_CAT:=bzcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBOPENCDK,libopencdk,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

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
		--with-libgcrypt-prefix="$(STAGING_DIR)/usr" \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LIBOPENCDK):
	install -m0755 -d $(IDIR_LIBOPENCDK)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libopencdk.so.* $(IDIR_LIBOPENCDK)/usr/lib/
	$(RSTRIP) $(IDIR_LIBOPENCDK)
	$(IPKG_BUILD) $(IDIR_LIBOPENCDK) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libopencdk.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/opencdk-config $(STAGING_DIR)/usr/bin/
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/opencdk.h $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libopencdk.{a,so*} $(STAGING_DIR)/usr/lib/
	touch $@

install-dev: $(STAGING_DIR)/usr/lib/libopencdk.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/bin/opencdk-config \
		$(STAGING_DIR)/usr/include/opencdk.h \
		$(STAGING_DIR)/usr/lib/libopencdk.{a,so*} \

compile-targets: install-dev
clean-targets: uninstall-dev
mostlyclean:
	make -C $(PKG_BUILD_DIR) clean
	rm -f $(PKG_BUILD_DIR)/.built
