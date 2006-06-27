# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=mtd
PKG_VERSION:=20050122

PKG_SOURCE=$(PKG_NAME)_$(PKG_VERSION).orig.tar.gz
PKG_MD5SUM:=1f42c2cae08eb9e7b52d0c188f8d6338
PKG_SOURCE_URL=http://ftp.debian.org/debian/pool/main/m/mtd
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(TOOL_BUILD_DIR)/mtd-20050122.orig

include $(INCLUDE_DIR)/host-build.mk

define Build/Compile
	$(MAKE) LINUXDIR=$(LINUX_HEADERS_DIR) -C $(PKG_BUILD_DIR)/util
endef

define Build/Install
	$(CP) $(PKG_BUILD_DIR)/util/mkfs.jffs2 $(STAGING_DIR)/bin/
endef

$(eval $(call HostBuild))
