#
## Copyright (C) 2010 Gabor Juhos <juhosg@openwrt.org>
#  Copyright (C) 2018 Chris Schimp <silverchris@gmail.com>
#  
## This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
# #
include $(TOPDIR)/rules.mk

PKG_NAME:=rbextract
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

PKG_FLAGS:=nonshared

include $(INCLUDE_DIR)/package.mk

define Package/rbextract
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Boot Loaders
  TITLE:=RouterBOOT ART Extractor
  DEPENDS:=@TARGET_ath79
endef

define Package/rbextract/description
 This utility is for extracting the WLAN radio calibration data from Mikrotik
 Routerboards.
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/rbextract/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rbextract $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,rbextract))
