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
PKG_SOURCE_DATE:=2022-09-11
PKG_SOURCE_VERSION:=71e1db65a5487bcbb18b2757655d2b59ae96292a
PKG_MIRROR_HASH:=3f9e7648afc82432a8011491af6354f8a82ccc3ba6d762af72649117e22d7054

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call HostBuild))
