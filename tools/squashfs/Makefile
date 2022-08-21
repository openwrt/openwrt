# 
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=squashfs
PKG_CPE_ID:=cpe:/a:phillip_lougher:squashfs
PKG_VERSION:=4.5.1

PKG_SOURCE:=$(PKG_NAME)$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/squashfs
PKG_HASH:=d7f6af5ddb900d5b89912c0fc2b7689549604aff33fd3694da320150662a6ba3
PKG_CAT:=zcat

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR)/squashfs-tools \
		CPPFLAGS="$(HOST_CFLAGS)" \
		LDFLAGS="$(HOST_LDFLAGS)" \
		LZMAPATH=$(STAGING_DIR_HOST)/lib \
		mksquashfs unsquashfs
endef

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/squashfs-tools/mksquashfs $(STAGING_DIR_HOST)/bin/mksquashfs-lzma
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/squashfs-tools/unsquashfs $(STAGING_DIR_HOST)/bin/unsquashfs-lzma
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/mksquashfs-lzma
	rm -f $(STAGING_DIR_HOST)/bin/unsquashfs-lzma
endef

$(eval $(call HostBuild))
