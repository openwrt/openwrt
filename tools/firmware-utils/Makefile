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
PKG_SOURCE_DATE:=2024-03-23
PKG_SOURCE_VERSION:=6b242991a995c41769977efb010dc9f4e4ec3da2
PKG_MIRROR_HASH:=00ff2661fda2eb299ccbfaa07dff8fbcf46b3441a6e9f6f46eaa119e66dbe142

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call HostBuild))
