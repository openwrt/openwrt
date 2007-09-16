# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk

PKG_NAME:=hostapd
PKG_VERSION:=0.5.8
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://hostap.epitest.fi/releases/
PKG_MD5SUM:=40416b48cae9c78e5a2452caf214aff3

PKG_BUILD_DEPENDS:= mac80211 madwifi

include $(INCLUDE_DIR)/package.mk

define Package/hostapd/Default
  SECTION:=net
  CATEGORY:=Network
  TITLE:=IEEE 802.1x Authenticator
  URL:=http://hostap.epitest.fi/
  DEPENDS:=@!TARGET_avr32 @!TARGET_etrax
endef

define Package/hostapd
$(call Package/hostapd/Default)
  TITLE+= (full)
  DEPENDS+= +libopenssl
endef

define Package/hostapd/conffiles
/etc/hostapd.conf
endef

define Package/hostapd/description
 This package contains a full featured IEEE 802.1x/WPA/EAP/RADIUS 
 Authenticator.
endef

define Package/hostapd-mini
$(call Package/hostapd/Default)
  TITLE+= (WPA-PSK only)
endef

define Package/hostapd-mini/conffiles
/etc/hostapd.conf
endef

define Package/hostapd-mini/description
 This package contains a minimal IEEE 802.1x/WPA/EAP/RADIUS Authenticator 
 (WPA-PSK only).
endef

define Package/hostapd-utils
  $(call Package/hostapd/Default)
  TITLE+= (utils)
  DEPENDS:=@PACKAGE_hostapd||PACKAGE_hostapd-mini
endef

define Package/hostapd-utils/description
 This package contains a command line utility to control the 
 IEEE 802.1x/WPA/EAP/RADIUS Authenticator.
endef

define Build/ConfigureTarget
	$(CP) $(PKG_BUILD_DIR) $(PKG_BUILD_DIR)_$(1)
	$(CP) ./files/$(1).config $(PKG_BUILD_DIR)_$(1)/.config
ifneq ($(CONFIG_PACKAGE_kmod-madwifi),)
	echo "CONFIG_DRIVER_MADWIFI=y" >> $(PKG_BUILD_DIR)_$(1)/.config
endif
ifneq ($(CONFIG_PACKAGE_kmod-mac80211),)
	echo "CONFIG_DRIVER_DEVICESCAPE=y" >> $(PKG_BUILD_DIR)_$(1)/.config
endif
endef

define Build/CompileTarget
	$(MAKE) -C $(PKG_BUILD_DIR)_$(1) \
		$(TARGET_CONFIGURE_OPTS) \
		OPTFLAGS="$(TARGET_CFLAGS)" \
		CPPFLAGS="$(TARGET_CPPFLAGS) -I$(STAGING_DIR)/usr/include/madwifi -I$(STAGING_DIR)/usr/include/mac80211" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		hostapd hostapd_cli
	$(CP) $(PKG_BUILD_DIR)_$(1)/hostapd_cli $(PKG_BUILD_DIR)/
endef

define Package/InstallTemplate
	if [ \! -f "$(PKG_BUILD_DIR)_$(2)/hostapd" ]; then \
		rm -f $(PKG_BUILD_DIR)/.built; \
		$(MAKE) $(PKG_BUILD_DIR)/.built; \
	fi
	$(INSTALL_DIR) $$(1)/lib/wifi
	$(INSTALL_DATA) ./files/hostapd.sh $$(1)/lib/wifi/hostapd.sh
	$(INSTALL_DIR) $$(1)/etc
ifneq ($(CONFIG_PACKAGE_kmod-madwifi),)
	$(INSTALL_CONF) $(PKG_BUILD_DIR)_$(2)/madwifi.conf $$(1)/etc/hostapd.conf
else
	$(INSTALL_CONF) $(PKG_BUILD_DIR)_$(2)/hostapd.conf $$(1)/etc/hostapd.conf
endif
	$(INSTALL_DIR) $$(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)_$(2)/hostapd $$(1)/usr/sbin/
endef

define Package/Template
 ifneq ($(CONFIG_PACKAGE_$(1)),)
  define Build/Configure/$(2)
	$(call Build/ConfigureTarget,$(2))
  endef
  define Build/Compile/$(2)
	$(call Build/CompileTarget,$(2))
  endef
  define Package/$(1)/install
	$(call Package/InstallTemplate,$(1),$(2))
  endef
 endif
endef

define Build/Configure
	$(call Build/Configure/default)
	$(call Build/Configure/mini)
endef

define Build/Compile
	$(call Build/Compile/default)
	$(call Build/Compile/mini)
endef

define Build/Clean
	rm -rf $(PKG_BUILD_DIR)_default
	rm -rf $(PKG_BUILD_DIR)_mini
endef

define Package/hostapd-utils/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hostapd_cli $(1)/usr/sbin/
endef

$(eval $(call Package/Template,hostapd,default))
$(eval $(call Package/Template,hostapd-mini,mini))

$(eval $(call BuildPackage,hostapd))
$(eval $(call BuildPackage,hostapd-mini))
$(eval $(call BuildPackage,hostapd-utils))
