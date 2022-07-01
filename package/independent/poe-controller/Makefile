#
# Copyright (C) 2014-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=poe-controller
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_MAINTAINER:=Huang Haicheng <13560178202@163.com>
PKG_LICENSE:=GPL-2.0
 
include $(INCLUDE_DIR)/package.mk

define Package/poe-controller/Default
  SECTION:=Applications
  CATEGORY:=Independent
  DEPENDS:=+i2c-tools @TARGET_ramips_mt7621
endef

define Package/poe-controller
  $(call Package/poe-controller/Default)
  TITLE:=poe controller
endef

define Package/poe-controller/description
  8-Port PSE Controller for PoE Systems.
endef

define Package/poe-controller/conffiles
endef

define Package/poe-controller/postinst
endef

define Package/poe-controller/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/poe-controller.init $(1)/etc/init.d/poe-controller
endef

define Build/Configure
  true
endef

define Build/Prepare
  true
endef

define Build/Compile
  true
endef

$(eval $(call BuildPackage,poe-controller))

