#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=yaffs2_android

PKG_SOURCE_URL:=https://android.googlesource.com/platform/external/yaffs2
PKG_SOURCE_PROTO:=git
PKG_SOURCE_DATE:=2008-12-18
PKG_SOURCE_VERSION:=d333fc232d7e5ae3370080d5d6f7d88ea9c6b3a1
PKG_MIRROR_HASH:=232ceeed180fb1b7c1c28d12e756c253246f2b5e8cd4c7efa6d884c5c18baf90

include $(INCLUDE_DIR)/host-build.mk

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR)/yaffs2/utils \
		CC="$(HOSTCC)" \
		CFLAGS="$(HOST_CFLAGS) -include endian.h" \
		mkyaffs2image
endef

define Host/Install
	$(INSTALL_DIR) $(STAGING_DIR_HOST)/bin
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/yaffs2/utils/mkyaffs2image $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/mkyaffs2image
endef

$(eval $(call HostBuild))
