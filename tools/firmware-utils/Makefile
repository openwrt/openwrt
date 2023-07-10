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
PKG_SOURCE_DATE:=2023-06-29
PKG_SOURCE_VERSION:=9e2de8515be15e2b61ae518ce592e5b446ef2d48
PKG_MIRROR_HASH:=e43d1796dd482ec364e5196f224bb1563dbb0494f0f28fb4e206ef7ef31260fb

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call HostBuild))
