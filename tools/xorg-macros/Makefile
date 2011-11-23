#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=xorg-macros
PKG_VERSION:=1.11.0

PKG_SOURCE_URL:=http://xorg.freedesktop.org/releases/individual/util/
PKG_SOURCE:=util-macros-$(PKG_VERSION).tar.bz2
PKG_MD5SUM:=22d5cdff672450cb6902e0d68c200dcb

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/util-macros-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/share/aclocal
	$(INSTALL_DATA) $(HOST_BUILD_DIR)/xorg-macros.m4 $(STAGING_DIR_HOST)/share/aclocal/
endef

define Host/Clean
	-$(MAKE) -C $(HOST_BUILD_DIR) uninstall
	$(call Host/Clean/Default)
endef

$(eval $(call HostBuild))
