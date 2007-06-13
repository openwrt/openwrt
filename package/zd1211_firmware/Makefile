# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 6582 2007-03-16 20:21:39Z nbd $

include $(TOPDIR)/rules.mk

PKG_NAME:=zd1211-firmware
PKG_VERSION:=1.3
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

PKG_SOURCE:=$(PKG_NAME)$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://downloads.sourceforge.net/zd1211/
PKG_MD5SUM:=cfff76ea35f4f93c37f6c99aa2058140

include $(INCLUDE_DIR)/package.mk

define Package/zd1211-firmware
  SECTION:=net
  CATEGORY:=Network
  TITLE:=zd1211 wifi firmware
  DESCRIPTION:=\
	This package contains firmware needed to run a zd1211 wifi stick with the rw driver 
endef

define Package/zd1211-firmware/install
	$(INSTALL_DIR) $(PKG_INSTALL_DIR)/lib/firmware/zd1211/
	$(INSTALL_DIR) $(1)/lib/firmware/zd1211/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/zd* $(PKG_INSTALL_DIR)/lib/firmware/zd1211/
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/zd* $(1)/lib/firmware/zd1211/
endef

define Build/Compile
endef

$(eval $(call BuildPackage,zd1211-firmware))
