# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=elftosb
PKG_VERSION:=10.12.01

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://repository.timesys.com/buildsources/e/elftosb/elftosb-10.12.01/
PKG_MD5SUM:=e8005d606c1e0bb3507c82f6eceb3056

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR) -f makefile LDFLAGS="$(HOST_STATIC_LINKING)"
endef

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/bld/linux/elftosb $(STAGING_DIR_HOST)/bin/elftosb
endef

define Host/Clean
endef

$(eval $(call HostBuild))
