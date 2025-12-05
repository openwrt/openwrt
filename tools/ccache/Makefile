#
# Copyright (C) 2006-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=ccache
PKG_VERSION:=4.12.1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/ccache/ccache/releases/download/v$(PKG_VERSION)
PKG_HASH:=a3da50ab0fb0d42f60c17d1450312e6ace9b681f6221cb77c8a09a845f9d760c

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_HOST_OPTIONS += \
	-DCCACHE_DEV_MODE=OFF \
	-DCMAKE_C_COMPILER_LAUNCHER="" \
	-DCMAKE_CXX_COMPILER_LAUNCHER="" \
	-DCMAKE_SKIP_RPATH=FALSE \
	-DCMAKE_INSTALL_RPATH="${STAGING_DIR_HOST}/lib" \
	-DENABLE_DOCUMENTATION=OFF \
	-DREDIS_STORAGE_BACKEND=OFF \
	-DENABLE_TESTING=OFF

$(eval $(call HostBuild))
