#
# Copyright (C) 2009-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=erofs-utils
PKG_VERSION:=1.5
PKG_RELEASE:=$(AUTORELEASE)
PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=git://git.kernel.org/pub/scm/linux/kernel/git/xiang/erofs-utils.git
PKG_SOURCE_DATE:=2022-05-17
PKG_SOURCE_VERSION:=a2821a66b42aee5430bccee82c280e38d1e9ab29
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
