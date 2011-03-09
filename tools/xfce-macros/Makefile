#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=xfce-macros
PKG_VERSION:=4.8.0

PKG_SOURCE_URL:=http://archive.xfce.org/xfce/4.8/src
PKG_SOURCE:=xfce4-dev-tools-$(PKG_VERSION).tar.bz2
PKG_MD5SUM:=9591299c49d92d00ba47974c42a735fa

HOST_BUILD_DIR:=$(BUILD_DIR)/xfce4-dev-tools-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

define Host/Configure
endef

define Host/Compile
endef

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/share/aclocal
	$(INSTALL_DATA) $(HOST_BUILD_DIR)/m4macros/*.m4 $(STAGING_DIR_HOST)/share/aclocal/
endef

$(eval $(call HostBuild))
