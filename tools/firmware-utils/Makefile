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
PKG_SOURCE_DATE:=2026-07-19
PKG_SOURCE_VERSION:=3e6b426938e86efa0aaced903f6e22a1db2d2bc7
PKG_MIRROR_HASH:=5f313d1c40935e837909c42bd6b198feafdfd9fd0520c3bba277719aa3fbb76e

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call HostBuild))
