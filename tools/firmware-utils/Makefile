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
PKG_SOURCE_DATE:=2024-09-22
PKG_SOURCE_VERSION:=ed9c97fa3305c70cf08f9b11869cafdae5ea5068
PKG_MIRROR_HASH:=be743cfa5c5de736ed28e1b62f17b5fe2c7f04509432d4b0f951bc5745b6befe

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call HostBuild))
