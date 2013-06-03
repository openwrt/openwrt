#
# Copyright (C) 2009-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=gmp
PKG_VERSION:=5.1.2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=@GNU/gmp/
PKG_MD5SUM:=06fe2ca164221c59ce74867155cfc1ac

HOST_FIXUP:=autoreconf

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

unexport CFLAGS

HOST_CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared \
	--enable-cxx \
	--enable-mpbsd

$(eval $(call HostBuild))
