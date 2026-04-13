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
PKG_SOURCE_DATE:=2026-04-09
PKG_SOURCE_VERSION:=f3b02a2f2e39f580b5334efbc382b4b9afe61f0b
PKG_MIRROR_HASH:=919afcde55baee1ded81ee4e5738a928451a5fe21425ffd40b9e1a2a5930134d

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call HostBuild))
