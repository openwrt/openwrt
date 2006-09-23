# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=openssl
PKG_VERSION:=0.9.8b
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.openssl.org/source/ \
	ftp://ftp.funet.fi/pub/crypt/cryptography/libs/openssl/source/ \
	ftp://ftp.webmonster.de/pub/openssl/source/ \
	ftp://ftp.sunet.se/pub/security/tools/net/openssl/source/
PKG_MD5SUM:=12cedbeb6813a0d7919dbf1f82134b86
PKG_CAT:=zcat

PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(INCLUDE_DIR)/package.mk

define Package/openssl/Default
  TITLE:=Open source SSL toolkit
  DESCRIPTION:=\
	The OpenSSL Project is a collaborative effort to develop a robust, \\\
	commercial-grade, full-featured, and Open Source toolkit implementing the \\\
	Secure Sockets Layer (SSL v2/v3) and Transport Layer Security (TLS v1) \\\
	protocols as well as a full-strength general purpose cryptography library.
  URL:=http://www.openssl.org/
endef

define Package/libopenssl
  $(call Package/openssl/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=+zlib
  TITLE+= (libraries)
  DESCRIPTION+=\\\
	\\\
	This package contains the OpenSSL shared libraries, needed by other \\\
	programs.
endef

define Package/openssl-util
  $(call Package/openssl/Default)
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libopenssl
  TITLE+= (utility)
  DESCRIPTION+=\\\
	\\\
	This package contains the OpenSSL command-line utility.
endef

define Package/openssl-util/conffiles
/etc/ssl/openssl.cnf
endef

OPENSSL_NO_CIPHERS:= no-idea no-md2 no-mdc2 no-rc5 no-sha0 no-rmd160 no-aes192
OPENSSL_OPTIONS:= shared no-ec no-err no-fips no-hw no-krb5 no-threads zlib-dynamic no-engines

define Build/Configure
	$(SED) 's,/CFLAG=,/CFLAG= $(TARGET_SOFT_FLOAT) ,g' $(PKG_BUILD_DIR)/Configure
	$(SED) s/OPENWRT_OPTIMIZATION_FLAGS/$(BR2_TARGET_OPTIMIZATION)/g $(PKG_BUILD_DIR)/Configure
	(cd $(PKG_BUILD_DIR); \
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
		MAKEDEPPROG="$(TARGET_CROSS)gcc" \
		depend
	$(MAKE) -C $(PKG_BUILD_DIR) -j1 \
		CC="$(TARGET_CC)" \
		AR="$(TARGET_CROSS)ar r" \
		RANLIB="$(TARGET_CROSS)ranlib" \
		all build-shared
	# Work around openssl build bug to link libssl.so with libcrypto.so.
	-rm $(PKG_BUILD_DIR)/libssl.so.*.*.*
	$(MAKE) -C $(PKG_BUILD_DIR) -j1 \
		CC="$(TARGET_CC)" \
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
