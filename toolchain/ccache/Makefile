# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/target.mk

PKG_NAME:=ccache
PKG_VERSION:=2.4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://samba.org/ftp/ccache/
PKG_MD5SUM:=73c1ed1e767c1752dd0f548ec1e66ce7
PKG_CAT:=zcat

PKG_BUILD_DIR:=$(TOOLCHAIN_BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/host-build.mk

ifneq ($(shell which ccache),)
  define Build/Configure
    	$(call Build/Configure/Default)
  endef

  define Build/Compile
    	$(MAKE) CC="$(HOSTCC)" -C $(PKG_BUILD_DIR)
  endef

  define Build/Install
    	$(MAKE) -C $(PKG_BUILD_DIR) \
   		DESTDIR="$(STAGING_DIR)" \
		install
  endef

  define Build/Clean
	$(MAKE) -C $(PKG_BUILD_DIR) uninstall
	-$(MAKE) -C $(PKG_BUILD_DIR) clean
  endef
endif
$(eval $(call HostBuild))
