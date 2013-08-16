#
# Copyright (C) 2010-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=mm-macros
PKG_VERSION:=0.9.6

PKG_SOURCE_URL:=http://ftp.gnome.org/pub/GNOME/sources/mm-common/0.9
PKG_SOURCE:=mm-common-$(PKG_VERSION).tar.xz
PKG_MD5SUM:=3b23749ab39441c392c016ce74632678

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/mm-common-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/share/aclocal
	$(INSTALL_DATA) $(HOST_BUILD_DIR)/macros/*.m4 $(STAGING_DIR_HOST)/share/aclocal/
endef

define Host/Clean
	-$(MAKE) -C $(HOST_BUILD_DIR) uninstall
	$(call Host/Clean/Default)
endef

$(eval $(call HostBuild))
