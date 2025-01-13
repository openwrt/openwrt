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
PKG_SOURCE_DATE:=2025-01-14
PKG_SOURCE_VERSION:=12c0b42231bee58b3cd7cbe95c33de2685484a4f
PKG_MIRROR_HASH:=50d872a5069f9f672de5ea9efcf00872bcce27ec52bbb69fff3785abcee1c77c

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call HostBuild))
