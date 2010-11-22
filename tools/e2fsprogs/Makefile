#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=e2fsprogs
PKG_VERSION:=1.41.10
PKG_MD5SUM:=f9c7bb5c036a119453ce02fa871038da
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/e2fsprogs

include $(INCLUDE_DIR)/host-build.mk

TARGET_CFLAGS += $(FPIC)

CONFIGURE_ARGS += \
	--disable-shared \
	--enable-static \
	--disable-rpath \
	--enable-elf-shlibs \
	--enable-dynamic-e2fsck \
	--disable-tls

define Host/Install
	$(Host/Install/Default)
	$(INSTALL_DATA) $(HOST_BUILD_DIR)/lib/uuid/libuuid.a $(STAGING_DIR_HOST)/lib/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/e2fsck
	rm -f $(STAGING_DIR_HOST)/bin/tune2fs
endef

$(eval $(call HostBuild))
