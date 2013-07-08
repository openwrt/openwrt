# 
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=bc
PKG_VERSION:=1.06.95

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://alpha.gnu.org/gnu/bc
PKG_MD5SUM:=5126a721b73f97d715bb72c13c889035

include $(INCLUDE_DIR)/host-build.mk

$(eval $(call HostBuild))
