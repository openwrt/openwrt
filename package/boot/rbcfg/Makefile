#
# Copyright (C) 2010 Gabor Juhos <juhosg@openwrt.org>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=rbcfg
PKG_RELEASE:=1

PKG_LICENSE:=GPL-2.0
PKG_LICENSE_FILES:=

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/rbcfg
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=RouterBOOT configuration tool
  DEPENDS:=@TARGET_ar71xx
endef

define Package/rbcfg/description
 This package contains an utility to manipulate RouterBOOT configuration on the
 MikroTIK RB-4XX devices.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/rbcfg/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rbcfg $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,rbcfg))
