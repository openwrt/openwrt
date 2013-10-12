#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=yaffs2_android
PKG_VERSION:=2008-12-18

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=https://android.googlesource.com/platform/external/yaffs2
PKG_SOURCE_PROTO:=git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)
PKG_SOURCE_VERSION:=d333fc232d7e5ae3370080d5d6f7d88ea9c6b3a1
PKG_MIRROR_MD5SUM:=8145d4dd9b46face26121f27ad60cc1c
HOST_BUILD_DIR=$(BUILD_DIR_HOST)/$(PKG_NAME)

include $(INCLUDE_DIR)/host-build.mk

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR)/yaffs2/utils \
		CC="$(HOSTCC) $(HOST_STATIC_LINKING)" \
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
