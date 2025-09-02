#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=squashfs4
PKG_CPE_ID:=cpe:/a:phillip_lougher:squashfs
PKG_VERSION:=4.7.2
PKG_RELEASE=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/plougher/squashfs-tools
PKG_SOURCE_DATE:=2025-08-19
PKG_SOURCE_VERSION:=99d23a31b471433c51e9c145aeba2ab1536e34df
PKG_MIRROR_HASH:=229a1b8f465fa0f576b5ace1753a8e9d28a2c4254ec2b019668baa676094f6c3

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
