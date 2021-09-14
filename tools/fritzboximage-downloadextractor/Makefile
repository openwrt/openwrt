# 
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

SQUASHFS4_HOST_BE_VERSION:=4.3

PKG_NAME:=squashfs
PKG_VERSION:=4.3

PKG_SOURCE:=$(PKG_NAME)$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/squashfs
PKG_HASH:=d92ab59aabf5173f2a59089531e30dbf

FRITZBOXMODEL:=7490
FRITZIMAGEVERSION:=07.28
FRITZIMAGENAME:=FRITZ.Box_$(FRITZBOXMODEL)-$(FRITZIMAGEVERSION).image

HOST_BUILD_DIR:=$(BUILD_DIR_HOST)/$(PKG_NAME)$(PKG_VERSION)

$(info	VAR is $(HOST_BUILD_DIR))
include $(INCLUDE_DIR)/host-build.mk

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR)/squashfs-tools \
		CC="$(HOSTCC)" \
		EXTRA_CFLAGS="$(HOST_CFLAGS) -fcommon -DTARGET_FORMAT=AVM_BE " \
		LEGACY_FORMATS_SUPPORT=1 \
		GZIP_SUPPORT=1 \
		LZMA_XZ_SUPPORT=1 \
		XZ_SUPPORT=1 \
		XZ_DIR="$(abspath $(STAGING_DIR_HOST)/lib)" \
		COMP_DEFAULT=xz \
		XATTR_SUPPORT=0 \
		XATTR_DEFAULT=0  \
		mksquashfs unsquashfs
	wget -O $(HOST_BUILD_DIR)/$(FRITZIMAGENAME) ftp://ftp.avm.de/fritzbox/fritzbox-$(FRITZBOXMODEL)/deutschland/fritz.os/$(FRITZIMAGENAME)
	7z e -aoa $(HOST_BUILD_DIR)/$(FRITZIMAGENAME) -r filesystem.image -o"$(HOST_BUILD_DIR)"
endef

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/squashfs-tools/mksquashfs $(STAGING_DIR_HOST)/bin/mksquashfs-avm-be
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/squashfs-tools/unsquashfs $(STAGING_DIR_HOST)/bin/unsquashfs-avm-be
	$(STAGING_DIR_HOST)/bin/unsquashfs-avm-be -d $(STAGING_DIR_HOST)/squashfs-root -f $(HOST_BUILD_DIR)/filesystem.image filesystem_core.squashfs
	$(STAGING_DIR_HOST)/bin/unsquashfs-avm-be -d $(STAGING_DIR_HOST)/squashfs-root -f $(STAGING_DIR_HOST)/squashfs-root/filesystem_core.squashfs
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/mksquashfs-avm-be
	rm -f $(STAGING_DIR_HOST)/bin/unsquashfs-avm-be
endef

$(eval $(call HostBuild))
