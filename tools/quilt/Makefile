# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=quilt
PKG_VERSION:=0.47

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://download.savannah.gnu.org/releases/$(PKG_NAME)
PKG_MD5SUM:=d33d2442bd34387260b1c1db3e623af0

include $(INCLUDE_DIR)/host-build.mk

define Build/Configure
	$(call Build/Configure/Default)
	[ -f $(PKG_BUILD_DIR)/Makefile ]
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) SHELL="$(BASH)"
endef

define Build/Install
	$(MAKE) -C $(PKG_BUILD_DIR) SHELL="$(BASH)" install
endef

define Build/Clean
	rm -f $(STAGING_DIR_HOST)/bin/quilt
endef

$(eval $(call HostBuild))
