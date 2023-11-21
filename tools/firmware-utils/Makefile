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
PKG_SOURCE_DATE:=2023-11-21
PKG_SOURCE_VERSION:=12bf1a99bd6eebae90caa144bb4d8b0cd763ff8f
PKG_MIRROR_HASH:=923508e729b4c1fcc497f18ad5fa33c62dc74b9aeac1a3f215b3291892903baf

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call HostBuild))
