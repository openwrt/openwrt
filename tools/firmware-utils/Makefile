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
PKG_SOURCE_DATE:=2024-01-07
PKG_SOURCE_VERSION:=40da9034bb4926ef04ec95c3fd38d850aed68142
PKG_MIRROR_HASH:=ee1694d4cddba32e32fcba85aeeaed6c5524e06ed29fca9d56cc466097aaa6f9

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call HostBuild))
