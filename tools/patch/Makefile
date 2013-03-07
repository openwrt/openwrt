# 
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=patch
PKG_VERSION:=2.7.1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=ftp://ftp.gnu.org/gnu/patch
PKG_MD5SUM:=e9ae5393426d3ad783a300a338c09b72

include $(INCLUDE_DIR)/host-build.mk

$(eval $(call HostBuild))
