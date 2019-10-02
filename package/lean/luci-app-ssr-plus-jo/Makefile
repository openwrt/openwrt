include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-ssr-plus-jo
PKG_VERSION:=1
PKG_RELEASE:=116.5

PKG_CONFIG_DEPENDS:= CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_V2ray \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_Kcptun:kcptun \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_ShadowsocksR_Server \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks_Server \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_ShadowsocksR_Socks \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks_Socks \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_simple-obfs\
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_simple-obfs-server\
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_privoxy \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks_V2Ray \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_ChinaDNS \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_dnscrypt_proxy \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_dnsforwarder \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_udp2raw-tunnel \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_udpspeeder \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_haproxy

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)/config
config PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks
	bool "Include Shadowsocks New Version"
	default y

config PACKAGE_$(PKG_NAME)_INCLUDE_V2ray
	bool "Include V2ray"
	default y

config PACKAGE_$(PKG_NAME)_INCLUDE_Kcptun
	bool "Include Kcptun"
	default n

config PACKAGE_$(PKG_NAME)_INCLUDE_ShadowsocksR_Server
	bool "Include ShadowsocksR Server"
	default y

config PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks_Server
	bool "Include Shadowsocks Server"
	default y

config PACKAGE_$(PKG_NAME)_INCLUDE_ShadowsocksR_Socks
	bool "Include ShadowsocksR Socks and Tunnel"
	default y

config PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks_Socks
	bool "Include Shadowsocks Socks and Tunnel"
	default y

config PACKAGE_$(PKG_NAME)_INCLUDE_privoxy
	bool "privoxy http local"
	default n

config PACKAGE_$(PKG_NAME)_INCLUDE_simple-obfs
	bool "simple-obfsl"
	default n
	
config PACKAGE_$(PKG_NAME)_INCLUDE_simple-obfs-server
	bool "simple-obfs-server"
	default n

config PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks_V2Ray
	bool "Include Shadowsocks V2Ray Plugin"
	default n

config PACKAGE_$(PKG_NAME)_INCLUDE_ChinaDNS
	bool "ChinaDNS"
	default n

config PACKAGE_$(PKG_NAME)_INCLUDE_dnscrypt_proxy
	bool "dnscrypt-proxy-full"
	default n

config PACKAGE_$(PKG_NAME)_INCLUDE_dnsforwarder
	bool "dnsforwarder"
	default n

config PACKAGE_$(PKG_NAME)_INCLUDE_udp2raw-tunnel
	bool "udp2raw-tunnel"
	default n
	
config PACKAGE_$(PKG_NAME)_INCLUDE_udpspeeder
	bool "udpspeeder"
	default n	

config PACKAGE_$(PKG_NAME)_INCLUDE_haproxy
	bool "haproxy"
	default n
endef

define Package/luci-app-ssr-plus-jo
 	SECTION:=luci
	CATEGORY:=LuCI
	SUBMENU:=3. Applications
	TITLE:=SS/SSR/V2Ray LuCI interface
	PKGARCH:=all

	DEPENDS:=+shadowsocksr-libev-alt +ipset +ip-full +iptables-mod-tproxy +dnsmasq-full +coreutils +coreutils-base64 +bash +pdnsd-alt +wget \
       +PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks:shadowsocks-libev-ss-redir \
	+PACKAGE_$(PKG_NAME)_INCLUDE_V2ray:v2ray \
	+PACKAGE_$(PKG_NAME)_INCLUDE_Kcptun:kcptun \
	+PACKAGE_$(PKG_NAME)_INCLUDE_ShadowsocksR_Server:shadowsocksr-libev-server \
	+PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks_Server:shadowsocks-libev-ss-server \
	+PACKAGE_$(PKG_NAME)_INCLUDE_ShadowsocksR_Socks:shadowsocksr-libev-ssr-local \
	+PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks_Socks:shadowsocks-libev-ss-local \
	+PACKAGE_$(PKG_NAME)_INCLUDE_privoxy:privoxy \
        +PACKAGE_$(PKG_NAME)_INCLUDE_simple-obfs:simple-obfs \
        +PACKAGE_$(PKG_NAME)_INCLUDE_simple-obfs-server:simple-obfs-server \
	+PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks_V2Ray:v2ray-plugin \
	+PACKAGE_$(PKG_NAME)_INCLUDE_ChinaDNS:ChinaDNS \
	+PACKAGE_$(PKG_NAME)_INCLUDE_dnscrypt_proxy:dnscrypt-proxy-full \
	+PACKAGE_$(PKG_NAME)_INCLUDE_dnsforwarder:dnsforwarder \
	+PACKAGE_$(PKG_NAME)_INCLUDE_udp2raw-tunnel:udp2raw-tunnel \
	+PACKAGE_$(PKG_NAME)_INCLUDE_udpspeeder:udpspeeder \
	+PACKAGE_$(PKG_NAME)_INCLUDE_haproxy:haproxy
endef


define Build/Prepare
endef

define Build/Compile
endef

define Package/luci-app-ssr-plus-jo/install
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci
	cp -pR ./luasrc/* $(1)/usr/lib/lua/luci
	$(INSTALL_DIR) $(1)/
	cp -pR ./root/* $(1)/
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/i18n
	po2lmo ./po/zh-cn/ssr-plus.po $(1)/usr/lib/lua/luci/i18n/ssr-plus.zh-cn.lmo
endef

define Package/luci-app-ssr-plus-jo/postinst
#!/bin/sh
if [ -z "$${IPKG_INSTROOT}" ]; then
	( . /etc/uci-defaults/luci-ssr-plus ) && rm -f /etc/uci-defaults/luci-ssr-plus
	rm -f /tmp/luci-indexcache
	chmod 755 /etc/init.d/shadowsocksr >/dev/null 2>&1
	/etc/init.d/shadowsocksr enable >/dev/null 2>&1
fi
exit 0
endef

define Package/luci-app-ssr-plus-jo/prerm
#!/bin/sh
if [ -z "$${IPKG_INSTROOT}" ]; then
     /etc/init.d/shadowsocksr disable
     /etc/init.d/shadowsocksr stop
fi
exit 0
endef

$(eval $(call BuildPackage,luci-app-ssr-plus-jo))
