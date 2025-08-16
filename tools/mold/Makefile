# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=mold
PKG_VERSION:=2.40.1
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL_FILE:=v$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/rui314/mold/archive/refs/tags
PKG_HASH:=d1ce09a69941f8158604c3edcc96c7178231e7dba2da66b20f5ef6e112c443b7

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_HOST_OPTIONS += \
	-DMOLD_LTO=ON \
	-DMOLD_USE_SYSTEM_MIMALLOC=OFF \
	-DMOLD_USE_SYSTEM_TBB=OFF

$(eval $(call HostBuild))
