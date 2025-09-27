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
PKG_SOURCE_DATE:=2025-09-23
PKG_SOURCE_VERSION:=7e6f69b444c33a6ca4e9efb98832820e3d8e87ba
PKG_MIRROR_HASH:=714053067b2b64689a6db9de0f49d1ce4fa20d9824d8f08e3c598b903eed8c03

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call HostBuild))
