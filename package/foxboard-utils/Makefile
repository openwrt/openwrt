# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 7006 2007-04-19 12:06:39Z kaloz $

include $(TOPDIR)/rules.mk

PKG_NAME:=foxboard-utils
PKG_VERSION:=1
PKG_RELEASE:=1

PKG_SOURCE:=foxboard-utils.tar.bz2
PKG_SOURCE_URL:=http://www.acmesystems.it/download/owrt
PKG_MD5SUM:=

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/foxboard-utils
  SECTION:=utils
  CATEGORY:=Base system
  TITLE:=Foxboard base tools
  URL:=http://www.acmesystems.it
  DEPENDS:=@TARGET_etrax
endef

define Package/foxboard-utils/description
 This package contains a collection of tools for configuring the foxboard 
 gpio pins/leds.
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) CFLAGS="$(TARGET_CFLAGS)" CC="$(TARGET_CC)"
	$(MAKE) -C $(PKG_BUILD_DIR) PREFIX="$(PKG_INSTALL_DIR)" install
endef

define Package/foxboard-utils/install
	$(INSTALL_DIR) $(1)/
	$(CP) $(PKG_INSTALL_DIR)/* $(1)
	mkdir -p $(1)/www/cgi-bin
	cd $(1)/www/cgi-bin; ln -s ../../bin/editcgi.cgi .
endef

$(eval $(call BuildPackage,foxboard-utils))
