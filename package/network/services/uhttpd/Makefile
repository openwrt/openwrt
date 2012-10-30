#
# Copyright (C) 2010-2012 Jo-Philipp Wich <jow@openwrt.org>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=uhttpd
PKG_VERSION:=2012-10-30
PKG_RELEASE=$(PKG_SOURCE_VERSION)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=git://nbd.name/uhttpd.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=e57bf6d8bfa465a50eea2c30269acdfe751a46fd
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_MAINTAINER:=Jo-Philipp Wich <jow@openwrt.org>

PKG_CONFIG_DEPENDS := \
	CONFIG_PACKAGE_uhttpd_debug \
	CONFIG_PACKAGE_uhttpd-mod-lua \
	CONFIG_PACKAGE_uhttpd-mod-tls \
	CONFIG_PACKAGE_uhttpd-mod-tls_cyassl \
	CONFIG_PACKAGE_uhttpd-mod-tls_openssl \
	CONFIG_PACKAGE_uhttpd-mod-ubus

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
  DEPENDS:=uhttpd +PACKAGE_uhttpd-mod-tls_cyassl:libcyassl +PACKAGE_uhttpd-mod-tls_openssl:libopenssl
endef

define Package/uhttpd-mod-tls/description
 The TLS plugin adds HTTPS support to uHTTPd.
endef

define Package/uhttpd-mod-tls/config
  choice
    depends on PACKAGE_uhttpd-mod-tls
    prompt "TLS Provider"
    default PACKAGE_uhttpd-mod-tls_cyassl

    config PACKAGE_uhttpd-mod-tls_cyassl
      bool "CyaSSL"

    config PACKAGE_uhttpd-mod-tls_openssl
      bool "OpenSSL"
  endchoice
endef

UHTTPD_TLS:=none
TLS_CFLAGS:=
TLS_LDFLAGS:=

ifneq ($(CONFIG_PACKAGE_uhttpd-mod-tls_cyassl),)
  UHTTPD_TLS:=cyassl
  TLS_CFLAGS:=-I$(STAGING_DIR)/usr/include/cyassl
  TLS_LDFLAGS:=-lcyassl -lm
endif

ifneq ($(CONFIG_PACKAGE_uhttpd-mod-tls_openssl),)
  UHTTPD_TLS:=openssl
  TLS_LDFLAGS:=-lssl
endif


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


TARGET_LDFLAGS += -lubox -lcrypt

CMAKE_OPTIONS += \
	-DDEBUG=$(if $(CONFIG_PACKAGE_uhttpd_debug),ON,OFF) \
	-DLUA_SUPPORT=$(if $(CONFIG_PACKAGE_uhttpd-mod-lua),ON,OFF) \
	-DUBUS_SUPPORT=$(if $(CONFIG_PACKAGE_uhttpd-mod-ubus),ON,OFF) \
	-DTLS_SUPPORT=$(UHTTPD_TLS) \
	-DTLS_CFLAGS="$(TLS_CFLAGS)" \
	-DTLS_LDFLAGS="$(TLS_LDFLAGS)" \


define Package/uhttpd/conffiles
/etc/config/uhttpd
/etc/uhttpd.crt
/etc/uhttpd.key
endef

define Package/uhttpd/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/uhttpd.init $(1)/etc/init.d/uhttpd
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/uhttpd.config $(1)/etc/config/uhttpd
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/uhttpd $(1)/usr/sbin/uhttpd
endef

define Package/uhttpd-mod-tls/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/uhttpd_tls.so $(1)/usr/lib/
endef

define Package/uhttpd-mod-lua/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/uhttpd_lua.so $(1)/usr/lib/
endef

define Package/uhttpd-mod-ubus/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/uhttpd_ubus.so $(1)/usr/lib/
endef


$(eval $(call BuildPackage,uhttpd))
$(eval $(call BuildPackage,uhttpd-mod-tls))
$(eval $(call BuildPackage,uhttpd-mod-lua))
$(eval $(call BuildPackage,uhttpd-mod-ubus))
