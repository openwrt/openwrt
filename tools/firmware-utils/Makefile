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
PKG_SOURCE_DATE:=2021-11-04
PKG_SOURCE_VERSION:=f4d126316457345fdf9d6868cda0a33bcf4368c6
PKG_MIRROR_HASH:=a2c64e34827992c0f0c71974aa0b66e0a2aafb38a838649500ecd78095098d4c

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call HostBuild))
