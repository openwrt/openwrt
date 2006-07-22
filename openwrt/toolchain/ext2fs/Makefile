# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=genext2fs
PKG_VERSION:=1.4rc1

PKG_BUILD_DIR:=$(TOOL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_MD5SUM:=664431bf6737df1c265500e1f0b5d40c
PKG_CAT:=zcat

include $(INCLUDE_DIR)/host-build.mk

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) all
endef

define Build/Install
	install -m0755 $(PKG_BUILD_DIR)/genext2fs $(STAGING_DIR)/bin/
endef

$(eval $(call HostBuild))
