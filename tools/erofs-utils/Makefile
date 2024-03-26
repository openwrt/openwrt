
# Copyright (C) 2009-2024 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=erofs-utils
PKG_VERSION:=1.7.1
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=https://git.kernel.org/pub/scm/linux/kernel/git/xiang/erofs-utils.git
PKG_MIRROR_HASH:=f753c2671cec142adce257b65c3b8238ce07c61c7daa8b49de8810655b5a0336
PKG_SOURCE_DATE:=2023-10-20
PKG_SOURCE_VERSION:=83d94dc619075e71ca4d0f42941cfc18d269a2af

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/host-build.mk

HOST_CONFIGURE_ARGS=\
	--without-uuid \
	--enable-lzma

define Host/Install
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/mkfs/mkfs.erofs $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/mkfs.erofs
endef

$(eval $(call HostBuild))