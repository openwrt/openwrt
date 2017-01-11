#
# Copyright (C) 2006-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=cmake
PKG_VERSION:=3.7.1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://cmake.org/files/v3.7/ \
		https://fossies.org/linux/misc/
PKG_HASH:=449a5bce64dbd4d5b9517ebd1a1248ed197add6ad27934478976fd5f1f9330e1

HOST_BUILD_PARALLEL:=1
HOST_CONFIGURE_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

HOST_CONFIGURE_CMD := \
	MAKEFLAGS="$(HOST_JOBS)" $(BASH) ./configure

HOST_CONFIGURE_VARS :=

HOST_CONFIGURE_ARGS := \
	--prefix=$(STAGING_DIR_HOST)

$(eval $(call HostBuild))
