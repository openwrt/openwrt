# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=openssl
PKG_VERSION:=0.9.8b
PKG_RELEASE:=1
PKG_MD5SUM:=12cedbeb6813a0d7919dbf1f82134b86

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

define Build/Configure
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
endef

define Build/Compile
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
endef

define Build/InstallDev
	mkdir -p $(STAGING_DIR)/usr/include
	$(CP) $(PKG_INSTALL_DIR)/usr/include/openssl $(STAGING_DIR)/usr/include/
	mkdir -p $(STAGING_DIR)/usr/lib/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib{crypto,ssl}.{a,so*} $(STAGING_DIR)/usr/lib/
endef
	
define Build/UninstallDev
	rm -rf \
		$(STAGING_DIR)/usr/include/openssl \
		$(STAGING_DIR)/usr/lib/lib{crypto,ssl}.{a,so*}
endef


define Package/libopenssl
 SECTION:=base
 CATEGORY:=Libraries
 TITLE:=Open source SSL libraries
 DEPENDS:=+zlib
 DESCRIPTION:=Open source SSL (Secure Socket Layer) libraries
 URL:=http://www.openssl.org/
endef

define Package/openssl-util
 $(call Package/libopenssl)
 DEPENDS:=libopenssl
 TITLE:=OpenSSL command line utilities
 DESCRIPTION:=OpenSSL command line utilities
endef


define Package/libopenssl/install
	install -d -m0755 $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/lib{crypto,ssl}.so.* $(1)/usr/lib/
	chmod 0644 $(1)/usr/lib/*
endef

define Package/openssl-util/install
	install -d -m0755 $(1)/etc/ssl
	$(CP) $(PKG_INSTALL_DIR)/etc/ssl/openssl.cnf $(1)/etc/ssl/
	install -d -m0755 $(1)/etc/ssl/certs
	install -d -m0755 $(1)/etc/ssl/private
	chmod 0700 $(1)/etc/ssl/private
	install -d -m0755 $(1)/usr/bin
	$(CP) $(PKG_INSTALL_DIR)/usr/bin/openssl $(1)/usr/bin/
endef


$(eval $(call BuildPackage,libopenssl))
$(eval $(call BuildPackage,openssl-util))
