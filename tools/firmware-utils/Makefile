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
# WIP: temporary fork carrying mkbossimg, needed by the Bintec elmeg RS123.
# Revert to $(PROJECT_GIT)/project/firmware-utils.git once
# "mkbossimg: add tool for Bintec BOSS firmware images" is merged upstream.
PKG_SOURCE_URL:=https://github.com/simonwunderlich/firmware-utils.git
PKG_SOURCE_DATE:=2026-09-17
PKG_SOURCE_VERSION:=e450dfd1b78a715bb0b1f903bc569b71b8f667fb
PKG_MIRROR_HASH:=4deabf65af072e3a088619e5d1586c6c46c3bc7b5aa41d0a955824cbd3350824

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call HostBuild))
