# Copyright (C) 2018-2019 Lienol
#
# This is free software, licensed under the Apache License, Version 2.0 .
#

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-passwall
PKG_VERSION:=2.0
PKG_RELEASE:=78-20190803

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PO2LMO:=./po2lmo

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)/config
menu "Configuration"
	
config PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks
	bool "Include Shadowsocks Redir (ss-redir)"
	default n
	
config PACKAGE_$(PKG_NAME)_INCLUDE_ShadowsocksR
	bool "Include ShadowsocksR Redir (ssr-redir)"
	default y

config PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks_socks
	bool "Include Shadowsocks Socks (ss-local)"
	default n
	
config PACKAGE_$(PKG_NAME)_INCLUDE_ShadowsocksR_socks
	bool "Include ShadowsocksR Socks (ssr-local)"
	default n
	
config PACKAGE_$(PKG_NAME)_INCLUDE_V2ray
	bool "Include V2ray"
	default n
	
config PACKAGE_$(PKG_NAME)_INCLUDE_Brook
	bool "Include Brook"
	default n
	
config PACKAGE_$(PKG_NAME)_INCLUDE_kcptun
	bool "Include kcptun"
	default n

config PACKAGE_$(PKG_NAME)_INCLUDE_haproxy
	bool "Include haproxy"
	default y
	
config PACKAGE_$(PKG_NAME)_INCLUDE_ChinaDNS
	bool "Include ChinaDNS"
	default y

config PACKAGE_$(PKG_NAME)_INCLUDE_pdnsd
	bool "Include pdnsd"
	default y
	
config PACKAGE_$(PKG_NAME)_INCLUDE_dns2socks
	bool "Include dns2socks"
	default n
	
config PACKAGE_$(PKG_NAME)_INCLUDE_dns-forwarder
	bool "Include dns-forwarder"
	default n

endmenu
endef

define Package/$(PKG_NAME)
  CATEGORY:=LuCI
  SUBMENU:=3. Applications
  TITLE:=LuCI support for PassWall(fanqiang) By Lienol
  PKGARCH:=all
  DEPENDS:=+curl +libcurl +libmbedtls +ca-bundle +ca-certificates +resolveip +iptables-mod-tproxy +kmod-ipt-tproxy +iptables-mod-ipopt +kmod-ipt-ipopt +ip +ipset +coreutils +coreutils-base64 +coreutils-nohup +luci-lib-jsonc +unzip \
  +dnsmasq-full \
  +PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks:shadowsocks-libev-ss-redir \
  +PACKAGE_$(PKG_NAME)_INCLUDE_ShadowsocksR:shadowsocksr-libev-alt \
  +PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks_socks:shadowsocks-libev-ss-local \
  +PACKAGE_$(PKG_NAME)_INCLUDE_ShadowsocksR_socks:shadowsocksr-libev-ssr-local \
  +PACKAGE_$(PKG_NAME)_INCLUDE_V2ray:v2ray \
  +PACKAGE_$(PKG_NAME)_INCLUDE_Brook:brook \
  +PACKAGE_$(PKG_NAME)_INCLUDE_kcptun:kcptun \
  +PACKAGE_$(PKG_NAME)_INCLUDE_haproxy:haproxy \
  +PACKAGE_$(PKG_NAME)_INCLUDE_ChinaDNS:ChinaDNS \
  +PACKAGE_$(PKG_NAME)_INCLUDE_pdnsd:pdnsd-alt \
  +PACKAGE_$(PKG_NAME)_INCLUDE_dns2socks:dns2socks \
  +PACKAGE_$(PKG_NAME)_INCLUDE_dns-forwarder:dns-forwarder
endef

define Build/Prepare
endef
 
define Build/Configure
endef
 
define Build/Compile
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/usr/share/passwall
	$(INSTALL_CONF) ./root/etc/config/passwall $(1)/etc/config/passwall
	$(INSTALL_CONF) ./root/etc/config/passwall_show $(1)/etc/config/passwall_show
	$(INSTALL_CONF) ./root/etc/uci-defaults/* $(1)/etc/uci-defaults
	$(INSTALL_BIN) ./root/etc/init.d/passwall $(1)/etc/init.d/passwall
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci
	cp -pR ./luasrc/* $(1)/usr/lib/lua/luci/
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/i18n
	cp -pR ./root/usr/share/passwall/* $(1)/usr/share/passwall
	chmod 755 $(PO2LMO)
	$(PO2LMO) ./po/zh-cn/passwall.po $(1)/usr/lib/lua/luci/i18n/passwall.zh-cn.lmo
endef

define Package/$(PKG_NAME)/postinst
#!/bin/sh
chmod a+x $${IPKG_INSTROOT}/usr/share/passwall/* >/dev/null 2>&1
rm -rf $${IPKG_INSTROOT}/etc/chinadns_chnroute.txt >/dev/null 2>&1
exit 0
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
