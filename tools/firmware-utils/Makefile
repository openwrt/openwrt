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
PKG_SOURCE_DATE:=2024-01-31
PKG_SOURCE_VERSION:=c46b4b4ad7e77aab555bb41d6adc5554d11a83e8
PKG_MIRROR_HASH:=4affc0a85368ec8fc6df3719c628dd88b13ce6cc46ba2a52741a61d089e25e51

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call HostBuild))
