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
PKG_MIRROR_HASH:=a5efd895cecae1bdc975ffeebb00fe5067ac32f68154509d54f602e2bc76d463
PKG_SOURCE_DATE:=2025-08-08
PKG_SOURCE_VERSION:=8a327838caa114d33ac55acca561c90cc4ab3a1d

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call HostBuild))
