# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 7006 2007-04-19 12:06:39Z kaloz $

include $(TOPDIR)/rules.mk

PKG_NAME:=spca5xx-view
PKG_VERSION:=1
PKG_RELEASE:=1

PKG_SOURCE:=spca5xx-view.tar.bz2
PKG_SOURCE_URL:=http://www.acmesystems.it/download/owrt
PKG_MD5SUM:=

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/spca5xx-view
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=spca5xx streaming applicaton
  DEPENDS:=+kmod-video-spca5xx-le +libpthread
  URL:=http://www.acmesystems.it/
endef

define Package/spca5xx-view/description
 This package contains a streaming daemon for spca5xx based webcams 
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/servfox/ CFLAGS="$(TARTGET_CFLAGS)" CC="$(TARGET_CC)" all
	$(MAKE) -C $(PKG_BUILD_DIR)/servfox/ PREFIX="$(PKG_INSTALL_DIR)" install
endef

define Package/spca5xx-view/install
	$(INSTALL_DIR) $(1)/
	$(CP) $(PKG_INSTALL_DIR)/* $(1)
	$(INSTALL_DIR) $(1)/www/
	$(CP) $(PKG_BUILD_DIR)/html-files/* $(1)/www/
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/servfox.config $(1)/etc/config/servfox
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/servfox.init $(1)/etc/init.d/servfox
endef

$(eval $(call BuildPackage,spca5xx-view))
