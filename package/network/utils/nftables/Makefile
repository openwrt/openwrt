# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=nftables
PKG_VERSION:=0.4+2015-01-12
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=git://git.netfilter.org/nftables
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=5eec8c95e7ed4e4261f4afc943c7fc792cc95365
PKG_MAINTAINER:=Steven Barth <steven@midlink.org>
PKG_LICENSE:=GPL-2.0

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
        --with-mini-gmp \
        --without-cli \

define Package/nftables
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Firewall
  TITLE:=nftables packet filtering userspace utility
  DEPENDS:=+kmod-nft-core +kmod-nft-nat +libnftnl
  URL:=http://netfilter.org/projects/nftables/
endef

define Package/nftables/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/src/nft $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,nftables))
