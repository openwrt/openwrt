#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=squashfs4
PKG_CPE_ID:=cpe:/a:phillip_lougher:squashfs
PKG_VERSION:=4.6.1
PKG_RELEASE=3

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/plougher/squashfs-tools
PKG_SOURCE_DATE:=2023-03-26
PKG_SOURCE_VERSION:=d8cb82d9840330f9344ec37b992595b5d7b44184
PKG_MIRROR_HASH:=e84026de1ab187f3f76d1b781a29259d818f887e1651225f850a62d6f90b1b9e

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk
HOST_BUILD_SUBDIR:=$(HOST_BUILD_DIR)/squashfs-tools
HOST_MAKE_VARS += INSTALL_PREFIX="$(HOST_BUILD_PREFIX)"
HOST_MAKE_FLAGS += -C $(HOST_BUILD_SUBDIR)
HOST_MAKE_FLAGS += XATTR_SUPPORT=1 XATTR_DEFAULT=0
HOST_MAKE_FLAGS += GZIP_SUPPORT=1
HOST_MAKE_FLAGS += LZ4_SUPPORT=1
HOST_MAKE_FLAGS += LZMA_XZ_SUPPORT=1
HOST_MAKE_FLAGS += LZO_SUPPORT=1
HOST_MAKE_FLAGS += XZ_SUPPORT=1 XZ_EXTENDED_OPTIONS=1
HOST_MAKE_FLAGS += ZSTD_SUPPORT=1
ifeq ($(CONFIG_KERNEL_SQUASHFS_COMP_DEFAULT_GZIP),y)
	HOST_MAKE_FLAGS += COMP_DEFAULT=gzip
endif
ifeq ($(CONFIG_KERNEL_SQUASHFS_COMP_DEFAULT_LZ4),y)
	HOST_MAKE_FLAGS += COMP_DEFAULT=lz4
endif
ifeq ($(CONFIG_KERNEL_SQUASHFS_COMP_DEFAULT_LZO),y)
	HOST_MAKE_FLAGS += COMP_DEFAULT=lzo
endif
ifeq ($(CONFIG_KERNEL_SQUASHFS_COMP_DEFAULT_XZ),y)
	HOST_MAKE_FLAGS += COMP_DEFAULT=xz
endif
ifeq ($(CONFIG_KERNEL_SQUASHFS_COMP_DEFAULT_ZSTD),y)
	HOST_MAKE_FLAGS += COMP_DEFAULT=zstd
endif
ifeq ($(CONFIG_KERNEL_SQUASHFS_COMP_DEFAULT_UNSPEC),y)
	HOST_MAKE_FLAGS += COMP_DEFAULT=zstd
endif

define Host/Configure
endef

define Host/Compile
	$(call Host/Compile/Default,mksquashfs)
	$(call Host/Compile/Default,unsquashfs)
endef

define Host/Install
	$(call Host/Compile/Default,install)
endef

define Host/Uninstall
	$(call Host/Compile/Default,uninstall)
	$(call Host/Compile/Default,clean)
endef

define Host/Clean
endef

$(eval $(call HostBuild))
