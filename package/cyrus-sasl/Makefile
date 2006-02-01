# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=cyrus-sasl
PKG_VERSION:=2.1.20
PKG_RELEASE:=1
PKG_MD5SUM:=268ead27f4ac39bcfe17d9e38e0f2977

PKG_SOURCE_URL:=http://ftp.andrew.cmu.edu/pub/cyrus-mail/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBSASL2,libsasl2,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	(cd $(PKG_BUILD_DIR); rm -rf config.{cache,status} ; \
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
		  --disable-sample \
		  --enable-staticdlopen \
		  --disable-java \
		  --disable-alwaystrue \
		  --disable-checkapop \
		  --enable-cram \
		  --enable-digest \
		  --disable-otp \
		  --disable-srp \
		  --disable-srp-setpass \
		  --disable-krb4 \
		  --disable-gssapi \
		  --disable-gss_mutexes \
		  --enable-plain \
		  --enable-anon \
		  --disable-login \
		  --disable-ntlm \
		  --disable-sql \
		  --with-dblib="none" \
		  --without-gdbm \
		  --with-devrandom="/dev/urandom" \
		  --without-pam \
		  --without-saslauthd \
		  --without-authdaemond \
		  --without-pwcheck \
		  --with-ipctype=unix \
		  --with-openssl="$(STAGING_DIR)/usr" \
		  --without-des \
		  --without-opie \
		  --without-ldap \
		  --without-mysql \
		  --without-pgsql \
		  --without-sqlite \
		  --without-rc4 \
		  --without-dmalloc \
		  --without-sfio \
	);
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR)/include \
		CC="$(HOSTCC)" \
		LINK="$(HOSTCC) -o makemd5 -lc" \
		CFLAGS="" \
		makemd5
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		all install
	touch $@

$(IPKG_LIBSASL2):
	install -m0755 -d $(IDIR_LIBSASL2)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libsasl2.so.* $(IDIR_LIBSASL2)/usr/lib/
	install -m0755 -d $(IDIR_LIBSASL2)/usr/lib/sasl2
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/sasl2/lib*.so.* $(IDIR_LIBSASL2)/usr/lib/sasl2/
	$(RSTRIP) $(IDIR_LIBSASL2)
	$(IPKG_BUILD) $(IDIR_LIBSASL2) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libsasl2.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/sasl $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libsasl2.{a,so*} $(STAGING_DIR)/usr/lib/
	ln -sf libsasl2.a $(STAGING_DIR)/usr/lib/libsasl.a
	ln -sf libsasl2.so $(STAGING_DIR)/usr/lib/libsasl.so
	mkdir -p $(STAGING_DIR)/usr/lib/sasl2
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/sasl2/lib*.{a,so*} $(STAGING_DIR)/usr/lib/sasl2/

install-dev: $(STAGING_DIR)/usr/lib/libsasl2.so

uninstall-dev:
	rm -rf $(STAGING_DIR)/usr/include/sasl
	rm -rf $(STAGING_DIR)/usr/lib/libsasl{2,}.{a,so*}
	rm -rf $(STAGING_DIR)/usr/lib/sasl2

compile-targets: install-dev
clean-targets: uninstall-dev
