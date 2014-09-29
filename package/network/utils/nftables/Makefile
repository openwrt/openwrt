# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=nftables
PKG_VERSION:=0.3+2014-09-18
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=git://git.netfilter.org/nftables
PKG_SOURCE_PROTO:=git
PKG_SOURCE_VERSION:=013dbc6b0a8490ba24805a8ae35d7707183b9615
PKG_MAINTAINER:=Steven Barth <steven@midlink.org>

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

define Package/nftables
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=Firewall
  TITLE:=nftables packet filtering userspace utility
  DEPENDS:=+kmod-nft-core +kmod-nft-nat +libnftnl +libgmp
  URL:=http://netfilter.org/projects/nftables/
endef


define Package/nftables/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/src/nft $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,nftables))
