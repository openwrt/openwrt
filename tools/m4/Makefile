# 
# Copyright (C) 2008-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=m4
PKG_CPE_ID:=cpe:/a:gnu:m4
PKG_VERSION:=1.4.21

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/$(PKG_NAME)
PKG_HASH:=38ae59f7a30bf9c108193cc5c25fbb06014f21e230c7ede2eff614f7b7c37ed8

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

HOST_CONFIGURE_VARS += gl_cv_func_strstr_linear=no

define Host/Uninstall
	-$(call Host/Compile/Default,uninstall)
endef

$(eval $(call HostBuild))
