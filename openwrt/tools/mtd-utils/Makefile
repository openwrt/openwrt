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
PKG_SOURCE_URL=http://ftp.debian.org/debian/pool/main/m/mtd
PKG_MD5SUM:=1f42c2cae08eb9e7b52d0c188f8d6338
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(TOOL_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION).orig

include $(INCLUDE_DIR)/host-build.mk

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/util LINUXDIR=$(LINUX_HEADERS_DIR)
endef

define Build/Install
	mkdir -p $(STAGING_DIR)/bin
	$(CP) $(PKG_BUILD_DIR)/util/mkfs.jffs2 $(STAGING_DIR)/bin/
endef

define Build/Clean
	rm -f $(STAGING_DIR)/bin/mkfs.jffs2
endef

$(eval $(call HostBuild))
