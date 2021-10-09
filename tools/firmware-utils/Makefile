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
PKG_SOURCE_URL=https://github.com/swegener/openwrt-firmware-utils.git
PKG_SOURCE_DATE:=2022-12-15
PKG_SOURCE_VERSION:=92aae363dd30c6d2ba96793936779bcd5eb56172
#PKG_MIRROR_HASH:=4dc754b403b10f43dc2871f17ef7b223c85ad3b14c57899497f0b6bb304756d9

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call HostBuild))
