# 
# Copyright (C) 2006-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=findutils
PKG_VERSION:=4.6.0

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/$(PKG_NAME)
PKG_MD5SUM:=9936aa8009438ce185bea2694a997fc1

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

$(eval $(call HostBuild))
