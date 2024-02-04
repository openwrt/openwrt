#
# Copyright (C) 2017-2018 Jian Chang <aa65535@live.com>
#
# Copyright (C) 2021 ImmortalWrt
# <https://immortalwrt.org>
#
# This is free software, licensed under the GNU General Public License v3.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ssocks
PKG_VERSION:=0.0.14
PKG_RELEASE:=3

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/tostercx/ssocks.git
PKG_SOURCE_DATE:=2020-07-09
PKG_SOURCE_VERSION:=c2024789c1ee076d171fd6061d7c133302216ea7
PKG_MIRROR_HASH:=3654e9f1fb54646e1e89c19e7aa3e105b84b2e20b5b634662c21c66691d1d22e

PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENSE
PKG_MAINTAINER:=Tianling Shen <cnsztl@immortalwrt.org>

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/ssocks/template
	SECTION:=net
	CATEGORY:=Network
	TITLE:=sSocks
	URL:=https://github.com/tostercx/ssocks
	DEPENDS:=+libopenssl
endef

define Package/ssocks
	$(call Package/ssocks/template)
	TITLE+= Relay
endef

define Package/ssocksd
	$(call Package/ssocks/template)
	TITLE+= Server
endef

define Package/ssocks/description/template
	sSocks is a package which contains: a socks5 server implements RFC 1928 (SOCKS V5) and
	RFC 1929 (Authentication for SOCKS V5), a reverse socks server and client, a netcat like tool
	and a socks5 relay.
endef

Package/ssocks/description = $(Package/ssocks/description/template)
Package/ssocksd/description = $(Package/ssocks/description/template)

define Build/Install
	true
endef

define Package/ssocks/install/template
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(2) $(1)/usr/bin/
endef

define Package/ssocks/install
	$(call Package/ssocks/install/template,$(1),ssocks)
endef

define Package/ssocksd/install
	$(call Package/ssocks/install/template,$(1),ssocksd)
endef

$(eval $(call BuildPackage,ssocks))
$(eval $(call BuildPackage,ssocksd))
