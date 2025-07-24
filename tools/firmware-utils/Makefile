#
# Copyright (C) 2006-2020 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=firmware-utils
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL=$(PROJECT_GIT)/project/firmware-utils.git
PKG_SOURCE_DATE:=2025-07-24
PKG_SOURCE_VERSION:=f29de74ecd7d952ee1f9a20577fec9f37c0d6f4a
PKG_MIRROR_HASH:=6abf06a2243acb1a9820a8c4d7c9a1f03f5b920fecf86ac2353d257e7963f7bb

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call HostBuild))
