# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=autoconf
PKG_VERSION:=2.62

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@GNU/autoconf
PKG_MD5SUM:=e1fb8fe0b22e651240afdfa2be537a3c

include $(INCLUDE_DIR)/host-build.mk

define Build/Configure
	$(call Build/Configure/Default,\
		--datarootdir=$(STAGING_DIR_HOST)/share \
	)
endef

define Build/Compile
	export SHELL="$(BASH)"; $(MAKE) -C $(PKG_BUILD_DIR)
endef

define Build/Install
	export SHELL="$(BASH)"; $(MAKE) -C $(PKG_BUILD_DIR) install
endef

define Build/Clean
	export SHELL="$(BASH)"; $(MAKE) -C $(PKG_BUILD_DIR) uninstall
	$(call Build/Clean/Default)
endef

$(eval $(call HostBuild))
