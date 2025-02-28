# 
# Copyright (C) 2013-2022 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=bc
PKG_VERSION:=1.08.1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=@GNU/bc
PKG_HASH:=515430115b3334c636317503460a0950dff79940aa3259ce2c1aa67c2881d023

PKG_FIXUP := autoreconf
PKG_CPE_ID:=cpe:/a:gnu:bc

include $(INCLUDE_DIR)/host-build.mk

define Host/Uninstall
	-$(call Host/Compile/Default,uninstall)
endef

$(eval $(call HostBuild))
