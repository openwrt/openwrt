# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=mold
PKG_VERSION:=2.30.0

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL_FILE:=v$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/rui314/mold/archive/refs/tags
PKG_HASH:=6e5178ccafe828fdb4ba0dd841d083ff6004d3cb41e56485143eb64c716345fd

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_HOST_OPTIONS += \
	-DMOLD_LTO=ON \
	-DMOLD_MOSTLY_STATIC=ON \
	-DMOLD_USE_SYSTEM_MIMALLOC=OFF \
	-DMOLD_USE_SYSTEM_TBB=OFF

$(eval $(call HostBuild))
