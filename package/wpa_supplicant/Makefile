# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=wpa_supplicant
PKG_VERSION:=0.5.7
PKG_RELEASE:=1
PKG_MD5SUM:=bd2436392ad3c6d2513da701a12f2d27

PKG_SOURCE_URL:=http://hostap.epitest.fi/releases
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz

include $(INCLUDE_DIR)/package.mk

define Package/wpa-supplicant
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libopenssl
  TITLE:=WPA Supplicant
  URL:=http://hostap.epitest.fi/wpa_supplicant/
endef

define Package/wpa-supplicant/Description
 WPA Supplicant
endef

define Package/wpa-cli
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=wpa-supplicant
  TITLE:=WPA Supplicant command line interface
endef

define Package/wpa-cli/Description
 
endef

define Package/wpa-passphrase
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=wpa-supplicant
  TITLE:=Set WPA passphrase for a SSID
endef

define Package/wpa-passphrase/Description
 
endef

define Build/Configure
	cp ./files/config.$(ARCH) $(PKG_BUILD_DIR)/.config
endef

define Build/Compile
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		KERNEL=$(LINUX_DIR) \
		CPPFLAGS="$(TARGET_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		all
	$(STRIP) $(PKG_BUILD_DIR)/wpa_supplicant
	$(STRIP) $(PKG_BUILD_DIR)/wpa_cli
		
endef

define Package/wpa-supplicant/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/wpa_supplicant $(1)/usr/sbin/
endef

define Package/wpa-cli/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/wpa_cli $(1)/usr/sbin/
endef

define Package/wpa-passphrase/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wpa_passphrase $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,wpa-supplicant))
$(eval $(call BuildPackage,wpa-cli))
$(eval $(call BuildPackage,wpa-passphrase))
