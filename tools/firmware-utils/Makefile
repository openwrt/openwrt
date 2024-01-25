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
PKG_SOURCE_DATE:=2023-11-22
PKG_SOURCE_VERSION:=d87b6c4b6423201e595459840f51d0dced04a4eb
PKG_MIRROR_HASH:=f34fbf4dc4bd13d18ea94e6e25090b57cde11b3dc7133e46f6ce3bb9164ab8fb

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call HostBuild))
