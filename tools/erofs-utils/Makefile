#
# Copyright (C) 2009-2025 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=erofs-utils
PKG_VERSION:=1.9.4
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://git.kernel.org/pub/scm/linux/kernel/git/xiang/erofs-utils.git
PKG_MIRROR_HASH:=322cc1862aed67a511519c5c82140b44b2f67de74e5e0ff5204312ec54e858b4
PKG_SOURCE_DATE:=2026-08-19
PKG_SOURCE_VERSION:=f36cadb5c563995ab3aa8572a60ed6b721b9557d
PKG_CPE_ID:=cpe:/a:erofs-utils_project:erofs-utils

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/host-build.mk

HOST_CONFIGURE_ARGS=\
	--enable-multithreading \
	--enable-lzma \
	--enable-lz4 \
	--disable-fuse \
	--with-uuid

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/mkfs/mkfs.erofs $(STAGING_DIR_HOST)/bin/
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/fsck/fsck.erofs $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/mkfs.erofs
	rm -f $(STAGING_DIR_HOST)/bin/fsck.erofs
endef

$(eval $(call HostBuild))
