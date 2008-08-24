# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=lua
PKG_VERSION:=5.1.4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.lua.org/ftp/ \
	http://ftp.gwdg.de/pub/languages/lua/ \
	http://mirrors.dotsrc.org/lua/ \
	http://www.tecgraf.puc-rio.br/lua/ftp/
PKG_MD5SUM:=d0870f2de55d59c1c8419f36e8fac150

include $(INCLUDE_DIR)/host-build.mk

define Build/Configure
endef

TARGET_CFLAGS += -DLUA_USE_LINUX

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/src luac-host
endef

define Build/Install
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/luac-host $(STAGING_DIR_HOST)/bin/luac
endef

define Build/Clean
	$(MAKE) -C $(PKG_BUILD_DIR)/src clean
	rm -f $(PKG_BUILD_DIR)/src/luac-host
	$(call Build/Clean/Default)
endef

$(eval $(call HostBuild))
