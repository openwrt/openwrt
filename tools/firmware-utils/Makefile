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
PKG_SOURCE_DATE:=2026-10-21
PKG_SOURCE_VERSION:=b2a93c6f77bac3a6525e93b383c987b073b4633b
PKG_MIRROR_HASH:=5b4fa33c4ab5e0d1d3f26d6956c3fe99d658e73ad88308197cdb07e10ad9dc0f

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call HostBuild))
