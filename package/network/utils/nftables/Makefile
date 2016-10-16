# Copyright (C) 2015-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=nftables
PKG_VERSION:=0.6
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=ftp://ftp.netfilter.org/pub/nftables
PKG_MD5SUM:=fd320e35fdf14b7be795492189b13dae

PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
        --with-mini-gmp \
        --without-cli \

define Package/nftables
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Firewall
  TITLE:=nftables packet filtering userspace utility
  DEPENDS:=+kmod-nft-core +kmod-nft-nat +libnftnl +libmnl
  URL:=http://netfilter.org/projects/nftables/
endef

define Package/nftables/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/src/nft $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,nftables))
