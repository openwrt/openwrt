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

PKG_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)-$(PKG_VERSION).orig

include $(INCLUDE_DIR)/host-build.mk

CFLAGS := $(HOST_CFLAGS) -I../include
ifneq ($(HOST_OS),Linux)
CFLAGS += -Dloff_t=off_t -D__BYTE_ORDER=BYTE_ORDER -include getline.h -include endian.h
endif

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/util LINUXDIR="$(LINUX_HEADERS_DIR)" CFLAGS="$(CFLAGS)" TARGETS=mkfs.jffs2
endef

define Build/Install
	$(CP) $(PKG_BUILD_DIR)/util/mkfs.jffs2 $(STAGING_DIR_HOST)/bin/
endef

define Build/Clean
	rm -f $(STAGING_DIR_HOST)/bin/mkfs.jffs2
endef

$(eval $(call HostBuild))
