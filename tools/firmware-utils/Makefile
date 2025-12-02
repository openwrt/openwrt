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
PKG_SOURCE_DATE:=2025-11-11
PKG_SOURCE_VERSION:=c42a3bc53da869d02c8ae4d62b3b08a95ff5833f
PKG_MIRROR_HASH:=c9cbe9aafc3476dac9d9bd1b26f932048f895bce775e8220f82fae097463c539

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call HostBuild))
