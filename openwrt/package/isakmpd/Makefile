# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 4619 2006-08-22 09:50:02Z florian $

include $(TOPDIR)/rules.mk

PKG_NAME:=isakmpd
PKG_VERSION:=20041012
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION).orig
PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).orig.tar.gz
PKG_SOURCE_URL:=http://ftp.debian.org/debian/pool/main/i/isakmpd/
PKG_MD5SUM:=e6d25a9e232fb186e1a48dc06453bd57
PKG_CAT:=zcat

PKG_INSTALL_DIR:=$(PKG_BUILD_DIR)/ipkg-install

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/isakmpd
  SECTION:=base
  CATEGORY:=Network
  DEPENDS:=@BROKEN @LINUX_2_6 +libopenssl +keynote +libgmp
  TITLE:=IPsec management tools
  URL:=http://isakmpd.sourceforge.net/
endef

define Build/Compile
	$(call Build/Compile/Default,LINUX_DIR="$(LINUX_DIR)" \
		STAGING_DIR="$(STAGING_DIR)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
	)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		STAGING_DIR="$(STAGING_DIR)" \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		INSTALL="install -c" \
		install-bin
endef

define Package/isakmpd/install
	install -d -m0755 $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/* $(1)/
endef

$(eval $(call BuildPackage,isakmpd))
