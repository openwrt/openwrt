#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=fconfig
PKG_VERSION:=20080329
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://andrzejekiert.ovh.org/software/fconfig/
PKG_MD5SUM:=dac355e9f2a0f48c414c52e2034b6346

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/fconfig
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=RedBoot configuration editor
  URL:=http://andrzejekiert.ovh.org/software.html.en
endef

define Package/fconfig/description
	displays and (if writable) also edits the RedBoot configuration.
endef

define Build/Configure
endef

define Build/Compile
	$(call Build/Compile/Default)
endef

define Package/fconfig/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/fconfig $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,fconfig))
