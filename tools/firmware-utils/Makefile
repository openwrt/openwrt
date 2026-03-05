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
PKG_SOURCE_DATE:=2026-03-05
PKG_SOURCE_VERSION:=6a87eaf434cb89d4eba0b811a4b5d158fd9c519f
PKG_MIRROR_HASH:=b40d6b02dc896359e413dd2ae9d0d616b5c1270dd1eb825b317d27fa531582e2

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call HostBuild))
