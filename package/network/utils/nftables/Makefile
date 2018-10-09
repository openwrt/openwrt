# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=nftables
PKG_VERSION:=0.9.0
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=https://netfilter.org/projects/$(PKG_NAME)/files
PKG_HASH:=ad8181b5fcb9ca572f444bed54018749588522ee97e4c21922648bb78d7e7e91
PKG_MAINTAINER:=Steven Barth <steven@midlink.org>
PKG_LICENSE:=GPL-2.0

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)
PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

DISABLE_NLS:=

CONFIGURE_ARGS += \
        --disable-debug \
        --disable-man-doc \
        --with-mini-gmp \
        --without-cli \

define Package/nftables/default
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Firewall
  TITLE:=nftables userspace utility
  DEPENDS:=+kmod-nft-core +libnftnl
  URL:=http://netfilter.org/projects/nftables/
endef

define Package/nftables/default/description
  This is a packet filtering userspace utility of nftables. It replaces the
  popular {ip,ip6,arp,eb}tables, and provides a new in-kernel packet
  classification framework that is based on a network-specific Virtual
  Machine (VM) and a new nft userspace command line tool. Nftables also
  reuses the existing Netfilter subsystems such as the existing hook
  infrastructure, the connection tracking system, NAT, userspace queueing
  and logging subsystem.
endef

define Package/nftables
  $(call Package/nftables/default)
  VARIANT:=default
endef

define Package/nftables/description
  $(call Package/nftables/default/description)
endef

define Package/nftables-withjson
  $(call Package/nftables/default)
  TITLE+=with json support
  DEPENDS+= +jansson
  VARIANT:=withjson
  PROVIDES:=nftables
endef

define Package/nftables-withjson/description
  $(call Package/nftables/default/description)

  This is a variant with json support.
endef

ifeq ($(BUILD_VARIANT),withjson)
  CONFIGURE_ARGS += --with-json
endif

define Package/nftables/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/nft $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_INSTALL_DIR)/usr/lib/*.so* $(1)/usr/lib/
endef

Package/nftables-withjson/install = $(Package/nftables/install)

$(eval $(call BuildPackage,nftables))
$(eval $(call BuildPackage,nftables-withjson))
