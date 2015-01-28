#
# Copyright (C) 2007-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=curl
PKG_VERSION:=7.40.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://curl.haxx.se/download/ \
	http://www.mirrorspace.org/curl/ \
	ftp://ftp.sunet.se/pub/www/utilities/curl/ \
	ftp://ftp.planetmirror.com/pub/curl/ \
	http://www.mirrormonster.com/curl/download/ \
	http://curl.mirrors.cyberservers.net/download/
PKG_MD5SUM:=8d30594212e65657a5c32030f0998fa9

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=COPYING

PKG_FIXUP:=autoreconf
PKG_BUILD_PARALLEL:=1

PKG_CONFIG_DEPENDS := \
  LIBCURL_AXTLS \
  LIBCURL_COOKIES \
  LIBCURL_CRYPTO_AUTH \
  LIBCURL_CYASSL \
  LIBCURL_DICT \
  LIBCURL_FILE \
  LIBCURL_FTP \
  LIBCURL_GNUTLS \
  LIBCURL_GOPHER \
  LIBCURL_HTTP \
  LIBCURL_IMAP \
  LIBCURL_LDAP \
  LIBCURL_LDAPS \
  LIBCURL_LIBCURL_OPTION \
  LIBCURL_NOSSL \
  LIBCURL_OPENSSL \
  LIBCURL_POLARSSL \
  LIBCURL_POP3 \
  LIBCURL_PROXY \
  LIBCURL_RTSP \
  LIBCURL_SMTP \
  LIBCURL_SSPI \
  LIBCURL_TELNET \
  LIBCURL_TFTP \
  LIBCURL_THREADED_RESOLVER \
  LIBCURL_TLS-SRP \
  LIBCURL_ZLIB

include $(INCLUDE_DIR)/package.mk

define Package/curl/Default
  SECTION:=net
  CATEGORY:=Network
  URL:=http://curl.haxx.se/
  MAINTAINER:=Imre Kaloz <kaloz@openwrt.org>
endef

define Package/curl
  $(call Package/curl/Default)
  SUBMENU:=File Transfer
  DEPENDS:=+libcurl
  TITLE:=A client-side URL transfer utility
endef

define Package/libcurl
  $(call Package/curl/Default)
  SECTION:=libs
  CATEGORY:=Libraries
  DEPENDS:=+LIBCURL_POLARSSL:libpolarssl +LIBCURL_CYASSL:libcyassl +LIBCURL_AXTLS:libaxtls +LIBCURL_OPENSSL:libopenssl +LIBCURL_GNUTLS:libgnutls +LIBCURL_ZLIB:zlib +LIBCURL_THREADED_RESOLVER:libpthread
  TITLE:=A client-side URL transfer library using $(if $(CONFIG_LIBCURL_POLARSSL),PolarSSL)$(if $(CONFIG_LIBCURL_OPENSSL),OpenSSL)$(if $(CONFIG_LIBCURL_GNUTLS),GNUTLS)$(if $(CONFIG_LIBCURL_NOSSL),no SSL)
  MENU:=1
endef


define Package/libcurl/config
  source "$(SOURCE)/Config.in"
endef

TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \
	--disable-thread \
	--enable-nonblocking \
	--disable-ares \
	--disable-debug \
	--disable-manual \
	--disable-verbose \
	--without-ca-bundle \
	$(if $(CONFIG_LIBCURL_NOSSL),,--with-ca-path=/etc/ssl/certs/) \
	--without-krb4 \
	--without-libidn \
	--without-nss \
	--without-libssh2 \
	$(call autoconf_bool,CONFIG_IPV6,ipv6) \
	$(if $(CONFIG_LIBCURL_AXTLS),--with-axtls="$(STAGING_DIR)/usr",--without-axtls) \
	$(if $(CONFIG_LIBCURL_COOKIES),--enable,--disable)-cookies \
	$(if $(CONFIG_LIBCURL_CRYPTO-AUTH),--enable,--disable)-crypto-auth \
	$(if $(CONFIG_LIBCURL_CYASSL),--with-cyassl="$(STAGING_DIR)/usr",--without-cyassl) \
	$(if $(CONFIG_LIBCURL_DICT),--enable,--disable)-dict \
	$(if $(CONFIG_LIBCURL_FILE),--enable,--disable)-file \
	$(if $(CONFIG_LIBCURL_FTP),--enable,--disable)-ftp \
	$(if $(CONFIG_LIBCURL_GOPHER),--enable,--disable)-gopher \
	$(if $(CONFIG_LIBCURL_GNUTLS),--with-gnutls="$(STAGING_DIR)/usr",--without-gnutls) \
	$(if $(CONFIG_LIBCURL_HTTP),--enable,--disable)-http \
	$(if $(CONFIG_LIBCURL_IMAP),--enable,--disable)-imap \
	$(if $(CONFIG_LIBCURL_LDAP),--enable,--disable)-ldap \
	$(if $(CONFIG_LIBCURL_LDAPS),--enable,--disable)-ldaps \
	$(if $(CONFIG_LIBCURL_LIBCURL-OPTION),--enable,--disable)-libcurl-option \
	$(if $(CONFIG_LIBCURL_OPENSSL),--with-ssl="$(STAGING_DIR)/usr",--without-ssl) \
	$(if $(CONFIG_LIBCURL_POLARSSL),--with-polarssl="$(STAGING_DIR)/usr",--without-polarssl) \
	$(if $(CONFIG_LIBCURL_POP3),--enable,--disable)-pop3 \
	$(if $(CONFIG_LIBCURL_PROXY),--enable,--disable)-proxy \
	$(if $(CONFIG_LIBCURL_RTSP),--enable,--disable)-rtsp \
	$(if $(CONFIG_LIBCURL_TELNET),--enable,--disable)-telnet \
	$(if $(CONFIG_LIBCURL_TFTP),--enable,--disable)-tftp \
	$(if $(CONFIG_LIBCURL_SMTP),--enable,--disable)-smtp \
	$(if $(CONFIG_LIBCURL_SSPI),--enable,--disable)-sspi \
	$(if $(CONFIG_LIBCURL_THREADED_RESOLVER),--enable,--disable)-threaded-resolver \
	$(if $(CONFIG_LIBCURL_TLS-SRP),--enable,--disable)-tls-srp \
	$(if $(CONFIG_LIBCURL_ZLIB),--with-zlib="$(STAGING_DIR)/usr",--without-zlib) \

define Build/Compile
	+$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) \
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
	[ -n "$(TARGET_LDFLAGS)" ] && $(SED) 's#$(TARGET_LDFLAGS)##g' $(1)/usr/lib/pkgconfig/libcurl.pc || true
	$(LN) $(STAGING_DIR)/usr/bin/curl-config $(2)/bin/
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
