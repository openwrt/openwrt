# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=yamonenv
PKG_VERSION:=20051022
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)_gruen.4g__$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://downloads.openwrt.org/sources/
PKG_MD5SUM:=a3e4f24155aa3ba5aa502bc63fdaa6ad

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/yamonenv
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@TARGET_au1000
  TITLE:=YAMON configuration utility
  URL:=http://meshcube.org/nylon/stable/sources/
endef

define Build/Configure
endef

define Package/yamonenv/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/src/$(PKG_NAME) $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,yamonenv))
