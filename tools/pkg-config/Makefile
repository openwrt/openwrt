# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=pkg-config
PKG_VERSION:=0.25

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://pkgconfig.freedesktop.org/releases/
PKG_MD5SUM:=a3270bab3f4b69b7dc6dbdacbcae9745

include $(INCLUDE_DIR)/host-build.mk

define Host/Install
	$(MAKE) -C $(HOST_BUILD_DIR) \
		install
	mv $(STAGING_DIR_HOST)/bin/pkg-config $(STAGING_DIR_HOST)/bin/pkg-config.real
	$(INSTALL_BIN) ./files/pkg-config $(STAGING_DIR_HOST)/bin/pkg-config
endef

define Host/Clean
	-$(MAKE) -C $(HOST_BUILD_DIR) uninstall
	$(call Host/Clean/Default)
endef

$(eval $(call HostBuild))
