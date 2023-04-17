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
PKG_SOURCE_DATE:=2023-04-17
PKG_SOURCE_VERSION:=e8191eb7efc2804d33feb18573c96e632a6c1086
PKG_MIRROR_HASH:=5b82fe3aee4b2baffe5e612f182b0fcd09a646aa09ec16b6cadde26749d9e2dc

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call HostBuild))
