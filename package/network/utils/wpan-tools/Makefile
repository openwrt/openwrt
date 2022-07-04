# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2015 OpenWrt.org
#

include $(TOPDIR)/rules.mk

PKG_NAME:=wpan-tools
PKG_VERSION:=0.8
PKG_RELEASE=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/linux-wpan/wpan-tools/releases/download/$(PKG_NAME)-$(PKG_VERSION)/$(PKG_NAME)-$(PKG_VERSION).tar.gz?
PKG_HASH:=0e22cadbc40d4c12d0a66e8175bda12046f03237ce6da23d4fd26fd3ab557d8b

include $(INCLUDE_DIR)/package.mk

define Package/wpan-tools
  SECTION:=net
  CATEGORY:=Network
  TITLE:=cfg802154 interface configuration utility
  URL:=https://linux-wpan.org/wpan-tools.html
  DEPENDS:= +libnl
endef

define Package/wpan-tools/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/iwpan $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wpan-ping/wpan-ping $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,wpan-tools))
