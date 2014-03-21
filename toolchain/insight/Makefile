# 
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=insight
PKG_VERSION:=6.8-1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)a.tar.bz2
PKG_MD5SUM:=f7dd764a102beb75c2bb6b8d83455f8e
PKG_SOURCE_URL:=ftp://sourceware.org/pub/insight/releases
PKG_CAT:=bzcat

STAGING_DIR_HOST:=$(TOOLCHAIN_DIR)
BUILD_DIR_HOST:=$(BUILD_DIR_TOOLCHAIN)

include $(INCLUDE_DIR)/host-build.mk

define Host/Configure
	(cd $(HOST_BUILD_DIR); \
		gdb_cv_func_sigsetjmp=yes \
		CFLAGS="-O2" \
		$(HOST_BUILD_DIR)/configure \
		--prefix=$(TOOLCHAIN_DIR) \
		--build=$(GNU_HOST_NAME) \
		--host=$(GNU_HOST_NAME) \
		--target=$(REAL_GNU_TARGET_NAME) \
		--enable-threads \
		--enable-werror=no \
	);
endef

define Host/Compile
	$(MAKE) -C $(HOST_BUILD_DIR)
endef

define Host/Install
	mkdir -p $(TOOLCHAIN_DIR)/bin
	$(INSTALL_BIN) $(HOST_BUILD_DIR)/gdb/insight $(TOOLCHAIN_DIR)/bin/$(TARGET_CROSS)insight
	ln -fs $(TARGET_CROSS)insight $(TOOLCHAIN_DIR)/bin/$(GNU_TARGET_NAME)-insight
	strip $(TOOLCHAIN_DIR)/bin/$(TARGET_CROSS)insight
endef

define Host/Clean
	rm -rf \
		$(HOST_BUILD_DIR) \
		$(TOOLCHAIN_DIR)/bin/$(TARGET_CROSS)insight \
		$(TOOLCHAIN_DIR)/bin/$(GNU_TARGET_NAME)-insight
endef

$(eval $(call HostBuild))
