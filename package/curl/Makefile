# 
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=curl
PKG_VERSION:=7.17.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://curl.haxx.se/download/ \
	http://www.mirrorspace.org/curl/ \
	http://curl.mirror.internet.tp/download/ \
	ftp://ftp.sunet.se/pub/www/utilities/curl/ \
	ftp://ftp.planetmirror.com/pub/curl/ \
	http://www.mirrormonster.com/curl/download/ \
	http://curl.mirrors.cyberservers.net/download/
PKG_MD5SUM:=1fa635d1dfde8337c98c82749d75c1de

PKG_FIXUP = libtool

include $(INCLUDE_DIR)/package.mk

define Package/curl/Default
  SECTION:=net
  CATEGORY:=Network
  URL:=http://curl.haxx.se/
endef

define Package/curl
  $(call Package/curl/Default)
  DEPENDS:=+libcurl
  TITLE:=A client-side URL transfer library
endef

define Package/libcurl
  $(call Package/curl/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=+libopenssl +zlib
  TITLE:=A client-side URL transfer utility
endef

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \
	--disable-thread \
	--enable-cookies \
	--enable-crypto-auth \
	--enable-nonblocking \
	--enable-file \
	--enable-ftp \
	--enable-http \
	--enable-ipv6 \
	--disable-ares \
	--disable-debug \
	--disable-dict \
	--disable-gopher \
	--disable-ldap \
	--disable-manual \
	--disable-sspi \
	--disable-telnet \
	--enable-tftp \
	--disable-verbose \
	--with-random="/dev/urandom" \
	--with-ssl="$(STAGING_DIR)/usr" \
	--without-ca-bundle \
	--without-gnutls \
	--without-krb4 \
	--without-libidn \
	--without-nss \
	--with-zlib="$(STAGING_DIR)/usr" \

CONFIGURE_VARS += \
	LDFLAGS="$$$$LDFLAGS -lcrypto -lssl -lz" \
	CC="$(filter-out ccache,$(TARGET_CC))"

define Build/Configure
	$(call Build/Configure/Default,,bash)
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		CC="$(TARGET_CC)" \
		install
endef

define Build/InstallDev
	$(INSTALL_DIR) $(2)/bin $(1)/usr/bin $(1)/usr/include $(1)/usr/lib $(1)/usr/lib/pkgconfig
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/curl-config $(1)/usr/bin/
	$(CP) $(PKG_INSTALL_DIR)/usr/include/curl $(1)/usr/include/
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcurl.{a,so*} $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/libcurl.pc $(1)/usr/lib/pkgconfig/
	$(SED) 's,-L$$$${exec_prefix}/lib,,g' $(1)/usr/bin/curl-config
	$(SED) 's,$(TARGET_LDFLAGS),,g' $(1)/usr/lib/pkgconfig/libcurl.pc
	ln -sf $(STAGING_DIR)/usr/bin/curl-config $(2)/bin/
endef

define Package/curl/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/bin/curl $(1)/usr/bin/
endef

define Package/libcurl/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/libcurl.so.* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,curl))
$(eval $(call BuildPackage,libcurl))
