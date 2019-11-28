#
# Copyright (C) 2010 Gabor Juhos <juhosg@openwrt.org>
# Copyright (C) 2018 Chris Schimp <silverchris@gmail.com>
# Copyright (C) 2019 Robert Marko <robimarko@gmail.com>
# Copyright (C) 2019 Roger Pueyo Centelles <roger.pueyo@guifi.net>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=rbextract
PKG_RELEASE:=2
CMAKE_INSTALL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/rbextract
  SECTION:=utils
  CATEGORY:=Utilities
  SUBMENU:=Boot Loaders
  TITLE:=RouterBOOT ART Extractor
  DEPENDS:=@TARGET_ath79 +liblzo
endef

define Package/rbextract/description
 This utility is for extracting the WLAN radio calibration data from Mikrotik
 Routerboards.
endef

define Package/rbextract/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rbextract $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,rbextract))
