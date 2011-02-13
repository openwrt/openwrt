#
# Copyright (C) 2006-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=soloscli
PKG_VERSION:=0.11
PKG_RELEASE:=1

PKG_SOURCE:=solos-pci-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/openadsl
PKG_MD5SUM:=ab3f3fa654ef7c3a402980eca094e690

PKG_INSTALL:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/solos-pci-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/soloscli
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Configuration utility for Solos ADSL2+ modems
  DEPENDS:=+kmod-solos-pci
  URL:=http://sourceforge.net/projects/openadsl
endef

define Package/soloscli/description
  This package contains the soloscli utility
  for interrogating Traverse Technologies' Solos ADSL2+ modems.
endef

define Package/soloscli/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/soloscli/soloscli $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc/hotplug.d/atm
	$(INSTALL_DATA) ./files/etc/hotplug.d/atm/15-solos-init $(1)/etc/hotplug.d/atm/
endef

$(eval $(call BuildPackage,soloscli))
