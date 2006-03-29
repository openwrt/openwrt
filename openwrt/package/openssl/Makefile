# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=openssl
PKG_VERSION:=0.9.8a
PKG_RELEASE:=3
PKG_MD5SUM:=1d16c727c10185e4d694f87f5e424ee1

PKG_SOURCE_URL:=http://www.openssl.org/source/ \
	ftp://ftp.funet.fi/pub/crypt/cryptography/libs/openssl/source/ \
	ftp://ftp.webmonster.de/pub/openssl/source/ \
	ftp://ftp.sunet.se/pub/security/tools/net/openssl/source/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

OPENSSL_NO_CIPHERS:= no-idea no-md2 no-mdc2 no-rc5 no-sha0 no-rmd160 no-aes192
OPENSSL_OPTIONS:= shared no-ec no-err no-fips no-hw no-krb5 no-threads zlib-dynamic no-engines

include $(TOPDIR)/package/rules.mk

$(eval $(call PKG_template,LIBOPENSSL,libopenssl,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))
$(eval $(call PKG_template,OPENSSL_UTIL,openssl-util,$(PKG_VERSION)-$(PKG_RELEASE),$(ARCH)))

$(PKG_BUILD_DIR)/.configured:
	$(SED) 's,/CFLAG=,/CFLAG= $(TARGET_SOFT_FLOAT) ,g' $(PKG_BUILD_DIR)/Configure
	$(SED) s/OPENWRT_OPTIMIZATION_FLAGS/$(BR2_TARGET_OPTIMIZATION)/g $(PKG_BUILD_DIR)/Configure
	(cd $(PKG_BUILD_DIR); \
		PATH=$(TARGET_PATH) \
		./Configure linux-openwrt \
		  --prefix=/usr \
		  --openssldir=/etc/ssl \
		  -I$(STAGING_DIR)/usr/include \
		  -L$(STAGING_DIR)/usr/lib -ldl \
		  -DOPENSSL_SMALL_FOOTPRINT \
		  $(OPENSSL_NO_CIPHERS) \
		  $(OPENSSL_OPTIONS) \
	)
	touch $@

$(PKG_BUILD_DIR)/.built:
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) -j1 \
		MAKEDEPPROG="$(TARGET_CC)" \
		depend
	$(MAKE) -C $(PKG_BUILD_DIR) -j1 \
		CC="$(TARGET_CC)" \
		AR="$(TARGET_CROSS)ar r" \
		RANLIB="$(TARGET_CROSS)ranlib" \
		all build-shared
	# Work around openssl build bug to link libssl.so with libcrypto.so.
	-rm $(PKG_BUILD_DIR)/libssl.so.*.*.*
	$(MAKE) -C $(PKG_BUILD_DIR) -j1 \
		CC=$(TARGET_CC) \
		CCOPTS="$(TARGET_CFLAGS) -fomit-frame-pointer" \
		do_linux-shared
	$(MAKE) -C $(PKG_BUILD_DIR) -j1 \
		INSTALL_PREFIX="$(PKG_INSTALL_DIR)" \
		install
	touch $@

$(IPKG_LIBOPENSSL):
	install -d -m0755 $(IDIR_LIBOPENSSL)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib{crypto,ssl}.so.* $(IDIR_LIBOPENSSL)/usr/lib/
	chmod 0644 $(IDIR_LIBOPENSSL)/usr/lib/*
	$(RSTRIP) $(IDIR_LIBOPENSSL)
	$(IPKG_BUILD) $(IDIR_LIBOPENSSL) $(PACKAGE_DIR)

$(IPKG_OPENSSL_UTIL):
	install -d -m0755 $(IDIR_OPENSSL_UTIL)/etc/ssl
	$(CP) $(PKG_INSTALL_DIR)/etc/ssl/openssl.cnf $(IDIR_OPENSSL_UTIL)/etc/ssl/
	install -d -m0755 $(IDIR_OPENSSL_UTIL)/etc/ssl/certs
	install -d -m0755 $(IDIR_OPENSSL_UTIL)/etc/ssl/private
	chmod 0700 $(IDIR_OPENSSL_UTIL)/etc/ssl/private
	install -d -m0755 $(IDIR_OPENSSL_UTIL)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/openssl $(IDIR_OPENSSL_UTIL)/usr/bin/
	$(RSTRIP) $(IDIR_OPENSSL_UTIL)
	$(IPKG_BUILD) $(IDIR_OPENSSL_UTIL) $(PACKAGE_DIR)

$(STAGING_DIR)/usr/lib/libssl.so: $(PKG_BUILD_DIR)/.built
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/openssl $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib{crypto,ssl}.{a,so*} $(STAGING_DIR)/usr/lib/
	touch $@
	
install-dev: $(STAGING_DIR)/usr/lib/libssl.so

uninstall-dev:
	rm -rf \
		$(STAGING_DIR)/usr/include/openssl \
		$(STAGING_DIR)/usr/lib/lib{crypto,ssl}.{a,so*} \

compile-targets: install-dev
clean-targets: uninstall-dev
