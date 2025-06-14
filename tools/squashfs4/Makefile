#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=squashfs4
PKG_CPE_ID:=cpe:/a:phillip_lougher:squashfs
PKG_VERSION:=4.7.0
PKG_RELEASE=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/plougher/squashfs-tools
PKG_SOURCE_DATE:=2025-06-04
PKG_SOURCE_VERSION:=2e87d42ed089dc31990d83eeb07437b9d085d6d1
PKG_MIRROR_HASH:=ff5c545b8d64e1c3a1316abde0b6ba297c267fa3daed264eff2038dc99649869

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

define Host/Compile
	+$(HOST_MAKE_VARS) \
	$(MAKE) $(HOST_JOBS) -C $(HOST_BUILD_DIR)/squashfs-tools \
		LZ4_SUPPORT=0 \
		LZO_SUPPORT=0 \
		ZSTD_SUPPORT=0 \
		GZIP_SUPPORT=1 \
		XZ_SUPPORT=1 \
		LZMA_XZ_SUPPORT=1 \
		XZ_EXTENDED_OPTIONS=1 \
		EXTRA_CFLAGS="-I$(STAGING_DIR_HOST)/include" \
		mksquashfs unsquashfs
endef

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/squashfs-tools/mksquashfs $(STAGING_DIR_HOST)/bin/mksquashfs4
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/squashfs-tools/unsquashfs $(STAGING_DIR_HOST)/bin/unsquashfs4
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/mksquashfs4
	rm -f $(STAGING_DIR_HOST)/bin/unsquashfs4
endef

$(eval $(call HostBuild))
