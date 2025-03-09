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
PKG_SOURCE_URL=https://github.com/dmascord/firmware-utils.git
PKG_MIRROR_HASH:=62b5747f5d34271c37bf8440a9b6fd08f79cea932367048e59110c40357cecfb
PKG_SOURCE_DATE:=2024-10-03
PKG_SOURCE_VERSION:=84614ec6bc2b5a6e8044e489e17e5d3021c30a26

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call HostBuild))
