# 
# Copyright (C) 2008-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=m4
PKG_CPE_ID:=cpe:/a:gnu:m4
PKG_VERSION:=1.4.20

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/$(PKG_NAME)
PKG_HASH:=6ac4fc31ce440debe63987c2ebbf9d7b6634e67a7c3279257dc7361de8bdb3ef

PKG_GPG_KEYS_DIR:=$(CURDIR)/keys
PKG_SOURCE_SIG:=$(PKG_SOURCE).sig
PKG_VALIDPGPKEYS:=71C2CC22B1C4602927D2F3AAA7A16B4A2527436A

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

HOST_CONFIGURE_VARS += gl_cv_func_strstr_linear=no

define Host/Uninstall
	-$(call Host/Compile/Default,uninstall)
endef

$(eval $(call HostBuild))
