# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 4619 2006-08-22 09:50:02Z florian $

include $(TOPDIR)/rules.mk

PKG_NAME:=isakmpd
PKG_VERSION:=20040115cvs
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://downloads.openwrt.org/sources/
PKG_MD5SUM:=9f59b10d57cfed5e95743255f1c1620d
PKG_CAT:=bzcat

PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/isakmpd
SECTION:=base
CATEGORY:=Network
DEPENDS:=@LINUX_2_6 +libopenssl +keynote +libgmp
TITLE:=IPsec management tools
DESCRIPTION:=IPsec management tools
URL:=http://isakmpd.sourceforge.net/
endef

define Build/Compile
	$(call Build/Compile/Default,LINUX_DIR="$(LINUX_DIR)" \
		STAGING_DIR="$(STAGING_DIR)" \
		DESTDIR="$(PKG_INSTALL_DIR)")
	$(MAKE) -C $(PKG_BUILD_DIR) \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		INSTALL="install -c" \
		install-bin
endef

define Package/isakmpd/install
	install -d -m0755 $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
endef

$(eval $(call BuildPackage,isakmpd))
