#
# Copyright (C) 2010-2015 Jo-Philipp Wich <jow@openwrt.org>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=uhttpd
PKG_VERSION:=2015-01-25
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://nbd.name/uhttpd2.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=7352ec423fb99a7970b0765287503abee05a56cd
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_MAINTAINER:=Felix Fietkau <nbd@openwrt.org>
PKG_LICENSE:=ISC

PKG_BUILD_DEPENDS = ustream-ssl

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/uhttpd/default
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Web Servers/Proxies
  TITLE:=uHTTPd - tiny, single threaded HTTP server
endef

define Package/uhttpd
  $(Package/uhttpd/default)
  DEPENDS:=+libubox
endef

define Package/uhttpd/description
 uHTTPd is a tiny single threaded HTTP server with TLS, CGI and Lua
 support. It is intended as a drop-in replacement for the Busybox
 HTTP daemon.
endef

define Package/uhttpd/config
  config PACKAGE_uhttpd_debug
    bool "Build with debug messages"
    default n
endef


define Package/uhttpd-mod-tls
  $(Package/uhttpd/default)
  TITLE+= (TLS plugin)
  DEPENDS:=uhttpd \
	+PACKAGE_uhttpd-mod-tls_polarssl:libustream-polarssl \
	+PACKAGE_uhttpd-mod-tls_cyassl:libustream-cyassl \
	+PACKAGE_uhttpd-mod-tls_openssl:libustream-openssl
endef

define Package/uhttpd-mod-tls/description
 The TLS plugin adds HTTPS support to uHTTPd.
endef

define Package/uhttpd-mod-tls/config
  choice
    depends on PACKAGE_uhttpd-mod-tls
    prompt "TLS Provider"
    default PACKAGE_uhttpd-mod-tls_polarssl

    config PACKAGE_uhttpd-mod-tls_polarssl
      bool "PolarSSL"

    config PACKAGE_uhttpd-mod-tls_cyassl
      bool "CyaSSL"

    config PACKAGE_uhttpd-mod-tls_openssl
      bool "OpenSSL"
  endchoice
endef

define Package/uhttpd-mod-lua
  $(Package/uhttpd/default)
  TITLE+= (Lua plugin)
  DEPENDS:=uhttpd +liblua
endef

define Package/uhttpd-mod-lua/description
 The Lua plugin adds a CGI-like Lua runtime interface to uHTTPd.
endef


define Package/uhttpd-mod-ubus
  $(Package/uhttpd/default)
  TITLE+= (ubus plugin)
  DEPENDS:=uhttpd +libubus +libblobmsg-json
endef

define Package/uhttpd-mod-ubus/description
 The ubus plugin adds a HTTP/JSON RPC proxy for ubus and publishes the
 session.* namespace and procedures.
endef

define Package/uhttpd/conffiles
/etc/config/uhttpd
/etc/uhttpd.crt
/etc/uhttpd.key
endef

TARGET_LDFLAGS += -lcrypt

CMAKE_OPTIONS = -DTLS_SUPPORT=on

define Package/uhttpd/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/uhttpd.init $(1)/etc/init.d/uhttpd
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/uhttpd.config $(1)/etc/config/uhttpd
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/uhttpd $(1)/usr/sbin/uhttpd
endef

define Package/uhttpd-mod-tls/install
	true
endef

define Package/uhttpd-mod-lua/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/uhttpd_lua.so $(1)/usr/lib/
endef

define Package/uhttpd-mod-ubus/install
	$(INSTALL_DIR) $(1)/usr/lib $(1)/etc/uci-defaults
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/uhttpd_ubus.so $(1)/usr/lib/
	$(INSTALL_DATA) ./files/ubus.default $(1)/etc/uci-defaults/00_uhttpd_ubus
endef


$(eval $(call BuildPackage,uhttpd))
$(eval $(call BuildPackage,uhttpd-mod-tls))
$(eval $(call BuildPackage,uhttpd-mod-lua))
$(eval $(call BuildPackage,uhttpd-mod-ubus))
