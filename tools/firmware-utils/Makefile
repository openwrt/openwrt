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
PKG_SOURCE_DATE:=2023-12-29
PKG_SOURCE_VERSION:=8cad449bdb5ccda4e8eebcd8500be941350f4f16
PKG_MIRROR_HASH:=cd578322e5c5a6f494ee437d2d431796d5276b8399cd0ee8517b75a3c3bb208c

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call HostBuild))
