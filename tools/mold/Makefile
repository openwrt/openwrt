# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=mold
PKG_VERSION:=2.3.2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL_FILE:=v$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/rui314/mold/archive/refs/tags
PKG_HASH:=db172c0e97606565a81e37995bf5c911606d3f3b9f3829e92cd26985c9b0ed3b

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_HOST_OPTIONS += \
	-DMOLD_LTO=ON \
	-DMOLD_MOSTLY_STATIC=ON \
	-DMOLD_USE_SYSTEM_MIMALLOC=OFF \
	-DMOLD_USE_SYSTEM_TBB=OFF

$(eval $(call HostBuild))
