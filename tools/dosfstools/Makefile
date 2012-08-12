# 
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=dosfstools
PKG_VERSION:=3.0.7
PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).orig.tar.gz
PKG_SOURCE_URL:=http://dosfstools.sourcearchive.com/downloads/3.0.7/
PKG_MD5SUM:=5d4d97c79e8dae60bb08c2fba36c94d6
PKG_CAT:=zcat

include $(INCLUDE_DIR)/host-build.mk

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR) SHELL="$(BASH)" LDFLAGS="$(HOST_LDFLAGS) $(HOST_STATIC_LINKING)"
endef

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/mkdosfs $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/mkdosfs
endef

$(eval $(call HostBuild))
