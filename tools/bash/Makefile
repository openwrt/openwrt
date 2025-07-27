#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=bash
PKG_CPE_ID:=cpe:/a:gnu:bash
PKG_VERSION:=5.3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/bash
PKG_HASH:=62dd49c44c399ed1b3f7f731e87a782334d834f08e098a35f2c87547d5dbb269

HOST_BUILD_PARALLEL := 1

include $(INCLUDE_DIR)/host-build.mk

define Host/Uninstall
	-$(call Host/Compile/Default,uninstall)
endef

$(eval $(call HostBuild))
