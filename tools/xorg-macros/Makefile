# 
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=xorg-macros
PKG_VERSION:=1.11.0

PKG_SOURCE:=util-macros-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://cgit.freedesktop.org/xorg/util/macros/snapshot/
PKG_MD5SUM:=2bb2645cfc2cc6768dd46f9406cab1d7

HOST_BUILD_DIR:=$(BUILD_DIR)/util-macros-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

define Host/Compile
	( cd $(HOST_BUILD_DIR); ./autogen.sh )
endef

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR)/host/share/aclocal
	$(INSTALL_DATA) $(HOST_BUILD_DIR)/xorg-macros.m4 $(STAGING_DIR)/host/share/aclocal/
endef

define Host/Clean
	-$(MAKE) -C $(HOST_BUILD_DIR) uninstall
	$(call Host/Clean/Default)
endef

$(eval $(call HostBuild))
