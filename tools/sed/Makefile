# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=sed
PKG_VERSION:=4.1.2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/$(PKG_NAME)
PKG_MD5SUM:=928f0e06422f414091917401f1a834d0
PKG_CAT:=zcat
export SED:=

include $(INCLUDE_DIR)/host-build.mk

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR) SHELL="$(BASH)"
endef

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/sed/sed $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/sed
endef

$(eval $(call HostBuild))
