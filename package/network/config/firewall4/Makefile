#
# Copyright (C) 2021 Jo-Philipp Wich <jo@mein.io>
#

include $(TOPDIR)/rules.mk

PKG_NAME:=firewall4
PKG_RELEASE:=$(AUTORELEASE)

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(PROJECT_GIT)/project/firewall4.git
PKG_SOURCE_DATE:=2022-01-09
PKG_SOURCE_VERSION:=07579df54f38d75afea44b2a3b41a6d9af9497a1
PKG_MIRROR_HASH:=3326e5e9fd53fe9f679b62cdada83aea4951aefe95ed6d2b5eca5a4695add45c
PKG_MAINTAINER:=Jo-Philipp Wich <jo@mein.io>
PKG_LICENSE:=ISC

include $(INCLUDE_DIR)/package.mk

define Package/firewall4
  SECTION:=net
  CATEGORY:=Base system
  TITLE:=OpenWrt 4th gen firewall
  DEPENDS:= \
	+kmod-nft-core +kmod-nft-fib +kmod-nft-offload \
	+kmod-nft-nat +kmod-nft-nat6 \
	+nftables-json \
	+ucode +ucode-mod-fs +ucode-mod-ubus +ucode-mod-uci
  PROVIDES:=uci-firewall
endef

define Package/firewall4/description
 This package provides an nftables-based implementation of the UCI firewall
 sharing the same configuration format.
endef

define Package/firewall4/conffiles
/etc/config/firewall
/etc/nftables.d/
endef

define Package/firewall4/install
	$(CP) -a $(PKG_BUILD_DIR)/root/* $(1)/
endef

define Build/Compile
endef

$(eval $(call BuildPackage,firewall4))
