#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=cmake
PKG_VERSION:=2.8.7

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.cmake.org/files/v2.8/
PKG_MD5SUM:=e1b237aeaed880f65dec9c20602452f6

include $(INCLUDE_DIR)/host-build.mk

HOST_CONFIGURE_CMD = \
	LDFLAGS="$(HOST_LDFLAGS) -static" ./configure

$(eval $(call HostBuild))
