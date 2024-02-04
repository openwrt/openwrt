# Copyright (C) 2022-2023 xiaorouji
#
# This is free software, licensed under the GNU General Public License v3.

include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-passwall2
PKG_VERSION:=1.25-4
PKG_RELEASE:=

PKG_CONFIG_DEPENDS:= \
	CONFIG_PACKAGE_$(PKG_NAME)_Iptables_Transparent_Proxy \
	CONFIG_PACKAGE_$(PKG_NAME)_Nftables_Transparent_Proxy \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_Brook \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_Haproxy \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_Hysteria \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_IPv6_Nat \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_NaiveProxy \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks_Libev_Client \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks_Libev_Server \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks_Rust_Client \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks_Rust_Server \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_ShadowsocksR_Libev_Client \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_ShadowsocksR_Libev_Server \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_Simple_Obfs \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_SingBox \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_tuic_client \
	CONFIG_PACKAGE_$(PKG_NAME)_INCLUDE_V2ray_Plugin

LUCI_TITLE:=LuCI support for PassWall 2
LUCI_PKGARCH:=all
LUCI_DEPENDS:=+coreutils +coreutils-base64 +coreutils-nohup +curl \
	+ip-full +libuci-lua +lua +luci-compat +luci-lib-jsonc +resolveip +tcping \
	+xray-core +v2ray-geoip +v2ray-geosite \
	+unzip \
	+PACKAGE_$(PKG_NAME)_INCLUDE_Brook:brook \
	+PACKAGE_$(PKG_NAME)_INCLUDE_Haproxy:haproxy \
	+PACKAGE_$(PKG_NAME)_INCLUDE_Hysteria:hysteria \
	+PACKAGE_$(PKG_NAME)_INCLUDE_IPv6_Nat:ip6tables-mod-nat \
	+PACKAGE_$(PKG_NAME)_INCLUDE_NaiveProxy:naiveproxy \
	+PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks_Libev_Client:shadowsocks-libev-ss-local \
	+PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks_Libev_Client:shadowsocks-libev-ss-redir \
	+PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks_Libev_Server:shadowsocks-libev-ss-server \
	+PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks_Rust_Client:shadowsocks-rust-sslocal \
	+PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks_Rust_Server:shadowsocks-rust-ssserver \
	+PACKAGE_$(PKG_NAME)_INCLUDE_ShadowsocksR_Libev_Client:shadowsocksr-libev-ssr-local \
	+PACKAGE_$(PKG_NAME)_INCLUDE_ShadowsocksR_Libev_Client:shadowsocksr-libev-ssr-redir \
	+PACKAGE_$(PKG_NAME)_INCLUDE_ShadowsocksR_Libev_Server:shadowsocksr-libev-ssr-server \
	+PACKAGE_$(PKG_NAME)_INCLUDE_Simple_Obfs:simple-obfs \
	+PACKAGE_$(PKG_NAME)_INCLUDE_SingBox:sing-box \
	+PACKAGE_$(PKG_NAME)_INCLUDE_tuic_client:tuic-client \
	+PACKAGE_$(PKG_NAME)_INCLUDE_V2ray_Plugin:v2ray-plugin

define Package/$(PKG_NAME)/config
menu "Configuration"

config PACKAGE_$(PKG_NAME)_Iptables_Transparent_Proxy
	bool "Iptables Transparent Proxy"
	select PACKAGE_dnsmasq-full
	select PACKAGE_dnsmasq_full_ipset
	select PACKAGE_ipset
	select PACKAGE_iptables
	select PACKAGE_iptables-nft
	select PACKAGE_iptables-zz-legacy
	select PACKAGE_iptables-mod-conntrack-extra
	select PACKAGE_iptables-mod-iprange
	select PACKAGE_iptables-mod-socket
	select PACKAGE_iptables-mod-tproxy
	select PACKAGE_kmod-ipt-nat
	depends on PACKAGE_$(PKG_NAME)
	default y if ! PACKAGE_firewall4
	
config PACKAGE_$(PKG_NAME)_Nftables_Transparent_Proxy
	bool "Nftables Transparent Proxy"
	select PACKAGE_dnsmasq-full
	select PACKAGE_dnsmasq_full_nftset
	select PACKAGE_nftables
	select PACKAGE_kmod-nft-socket
	select PACKAGE_kmod-nft-tproxy
	select PACKAGE_kmod-nft-nat
	depends on PACKAGE_$(PKG_NAME)
	default y if PACKAGE_firewall4

config PACKAGE_$(PKG_NAME)_INCLUDE_Brook
	bool "Include Brook"
	default n

config PACKAGE_$(PKG_NAME)_INCLUDE_Haproxy
	bool "Include Haproxy"
	default y if aarch64||arm||i386||x86_64

config PACKAGE_$(PKG_NAME)_INCLUDE_Hysteria
	bool "Include Hysteria"
	default n

config PACKAGE_$(PKG_NAME)_INCLUDE_IPv6_Nat
	depends on PACKAGE_ip6tables
	bool "Include IPv6 Nat"
	default n

config PACKAGE_$(PKG_NAME)_INCLUDE_NaiveProxy
	bool "Include NaiveProxy"
	depends on !(arc||(arm&&TARGET_gemini)||armeb||mips||mips64||powerpc)
	default n

config PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks_Libev_Client
	bool "Include Shadowsocks Libev Client"
	default y

config PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks_Libev_Server
	bool "Include Shadowsocks Libev Server"
	default n

config PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks_Rust_Client
	bool "Include Shadowsocks Rust Client"
	depends on aarch64||arm||i386||mips||mipsel||x86_64
	default y if aarch64

config PACKAGE_$(PKG_NAME)_INCLUDE_Shadowsocks_Rust_Server
	bool "Include Shadowsocks Rust Server"
	depends on aarch64||arm||i386||mips||mipsel||x86_64
	default n

config PACKAGE_$(PKG_NAME)_INCLUDE_ShadowsocksR_Libev_Client
	bool "Include ShadowsocksR Libev Client"
	default y

config PACKAGE_$(PKG_NAME)_INCLUDE_ShadowsocksR_Libev_Server
	bool "Include ShadowsocksR Libev Server"
	default n

config PACKAGE_$(PKG_NAME)_INCLUDE_Simple_Obfs
	bool "Include Simple-Obfs (Shadowsocks Plugin)"
	default y

config PACKAGE_$(PKG_NAME)_INCLUDE_SingBox
	bool "Include Sing-Box"
	default y if aarch64||arm||i386||x86_64

config PACKAGE_$(PKG_NAME)_INCLUDE_tuic_client
	bool "Include tuic-client"
	depends on aarch64||arm||i386||x86_64
	default n

config PACKAGE_$(PKG_NAME)_INCLUDE_V2ray_Plugin
	bool "Include V2ray-Plugin (Shadowsocks Plugin)"
	default y if aarch64||arm||i386||x86_64

endmenu
endef

define Package/$(PKG_NAME)/conffiles
/etc/config/passwall2
/etc/config/passwall2_server
/usr/share/passwall2/domains_excluded
endef

include $(TOPDIR)/feeds/luci/luci.mk

# call BuildPackage - OpenWrt buildroot signature
