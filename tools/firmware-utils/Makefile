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
PKG_SOURCE_DATE:=2024-02-14
PKG_SOURCE_VERSION:=17de36575f1edf4287c1d92c7d7eb45c8ceca29e
PKG_MIRROR_HASH:=70ee16f2dd00a9f1a66b6cd0d90ba58ad36e45e713dae53af38831ed38fbe7e5

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call HostBuild))
