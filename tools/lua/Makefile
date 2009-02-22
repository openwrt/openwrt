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

define Host/Configure
endef

TARGET_CFLAGS += -DLUA_USE_LINUX

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR)/src luac-host
endef

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/src/luac-host $(STAGING_DIR_HOST)/bin/luac
endef

define Host/Clean
	$(MAKE) -C $(HOST_BUILD_DIR)/src clean
	rm -f $(HOST_BUILD_DIR)/src/luac-host
	$(call Host/Clean/Default)
endef

$(eval $(call HostBuild))
