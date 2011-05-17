# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=mkelfimage
PKG_VERSION:=2.7

PKG_SOURCE:=$(PKG_NAME)_$(PKG_VERSION).orig.tar.gz
PKG_SOURCE_URL:=http://ftp.debian.org/debian/pool/main/m/mkelfimage/
PKG_MD5SUM:=e505cb87e9c0cdc44cf03d2c4ea8c74b

include $(INCLUDE_DIR)/host-build.mk

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/bin
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/objdir/sbin/mkelfImage $(STAGING_DIR_HOST)/bin
endef

$(eval $(call HostBuild))
