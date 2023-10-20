# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=mold
PKG_VERSION:=2.2.0

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL_FILE:=v$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/rui314/mold/archive/refs/tags
PKG_HASH:=78ddddaaa004e50f8d92a13d8e792a46a1b37745fab48d39ad16aeb5a776e7c6

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_HOST_OPTIONS += \
	-DMOLD_LTO=ON \
	-DMOLD_MOSTLY_STATIC=ON \
	-DMOLD_USE_SYSTEM_MIMALLOC=OFF \
	-DMOLD_USE_SYSTEM_TBB=OFF

$(eval $(call HostBuild))
