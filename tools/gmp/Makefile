#
# Copyright (C) 2009 -2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=gmp
PKG_VERSION:=5.0.1

PKG_SOURCE_URL:=@GNU/gmp/
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_MD5SUM:=6bac6df75c192a13419dfd71d19240a7

include $(INCLUDE_DIR)/host-build.mk

unexport CFLAGS

HOST_CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared \
	--enable-cxx \
	--enable-mpbsd

$(eval $(call HostBuild))
