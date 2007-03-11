# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile 6372 2007-02-25 23:34:57Z nbd $

include $(TOPDIR)/rules.mk

PKG_NAME:=libcgi
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/libcgi
PKG_MD5SUM:=110af367081d33c7ed6527a1a60fc274
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Build/InstallDev
	$(MAKE) -C $(PKG_BUILD_DIR) \
		prefix="$(STAGING_DIR)/usr" \
		install
endef

$(eval $(call Build/DefaultTargets))
