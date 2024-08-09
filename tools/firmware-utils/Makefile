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
PKG_SOURCE_DATE:=2024-08-09
PKG_SOURCE_VERSION:=26c7f054b5fe8408d64ca7b06372b303cf888245
PKG_MIRROR_HASH:=7fad7164ae09cc6f250f71f97c5d5f144d0edebe1c46f116ee5cfc7099d3903e

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call HostBuild))
