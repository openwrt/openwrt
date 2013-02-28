# 
# Copyright (C) 2008-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=bison
PKG_VERSION:=2.7

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=@GNU/$(PKG_NAME)
PKG_MD5SUM:=234cdfac99257cf99ac4a03c898f37b9
PKG_CAT:=xzcat

HOST_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/host-build.mk

HOST_LDFLAGS += $(HOST_STATIC_LINKING)

define Host/Clean
	-$(MAKE) -C $(HOST_BUILD_DIR) uninstall
	$(call Host/Clean/Default)
endef

$(eval $(call HostBuild))
