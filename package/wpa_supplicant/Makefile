#
# Copyright (C) 2008-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=wpa_supplicant
PKG_VERSION:=0.6.9
PKG_RELEASE:=2
PKG_MD5SUM:=0efb8fcedf0a8acf6f423dfdb0658fdd

PKG_SOURCE_URL:=http://hostap.epitest.fi/releases
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz

PKG_BUILD_DEPENDS:= \
	PACKAGE_kmod-madwifi:madwifi \

PKG_CONFIG_DEPENDS:= \
	CONFIG_PACKAGE_kmod-madwifi \
	CONFIG_WPA_SUPPLICANT_NO_TIMESTAMP_CHECK \

include $(INCLUDE_DIR)/package.mk

define Package/wpa-supplicant
  SECTION:=net
  CATEGORY:=Network
  TITLE:=WPA Supplicant
  DEPENDS:=$(if $(CONFIG_WPA_SUPPLICANT_OPENSSL),+libopenssl)
  URL:=http://hostap.epitest.fi/wpa_supplicant/
endef

define Package/wpa-supplicant/Description
 WPA Supplicant
endef

define Package/wpa-supplicant/config
	source "$(SOURCE)/Config.in"
endef

define Package/wpa-cli
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=wpa-supplicant
  TITLE:=WPA Supplicant command line interface
endef

define Package/wpa-cli/Description
 WPA Supplicant control utility
endef

CONFIG=$(firstword $(wildcard ./files/config.$(BOARD) ./files/config.$(ARCH) ./config))
TARGET_CFLAGS += -I$(STAGING_DIR)/usr/include/madwifi $(if $(CONFIG_WPA_SUPPLICANT_NO_TIMESTAMP_CHECK),-DNO_TIMESTAMP_CHECK)

define Build/Configure
	cp $(CONFIG) $(PKG_BUILD_DIR)/wpa_supplicant/.config
	$(if $(CONFIG_PACKAGE_kmod-madwifi),,$(SED) 's,^CONFIG_DRIVER_MADWIFI,#CONFIG_DRIVER_MADWIFI,g' $(PKG_BUILD_DIR)/wpa_supplicant/.config)
	$(if $(CONFIG_PACKAGE_kmod-hostap),,$(SED) 's,^CONFIG_DRIVER_HOSTAP,#CONFIG_DRIVER_HOSTAP,g' $(PKG_BUILD_DIR)/wpa_supplicant/.config)
	$(if $(CONFIG_WPA_SUPPLICANT_OPENSSL),$(SED) 's|^CONFIG_TLS.*|CONFIG_TLS=openssl|g' $(PKG_BUILD_DIR)/wpa_supplicant/.config)
endef

define Build/Compile
	rm -rf $(PKG_INSTALL_DIR)
	mkdir -p $(PKG_INSTALL_DIR)
	CFLAGS="$(TARGET_CFLAGS)" \
	$(MAKE) -C $(PKG_BUILD_DIR)/wpa_supplicant \
		CC="$(TARGET_CC)" \
		KERNEL=$(LINUX_DIR) \
		CPPFLAGS="$(TARGET_CPPFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		all
endef

define Package/wpa-supplicant/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) \
		$(PKG_BUILD_DIR)/wpa_supplicant/wpa_supplicant \
		$(PKG_BUILD_DIR)/wpa_supplicant/wpa_passphrase \
		$(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/lib/wifi
	$(INSTALL_DATA) ./files/wpa_supplicant.sh $(1)/lib/wifi/wpa_supplicant.sh
endef

define Package/wpa-cli/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/wpa_supplicant/wpa_cli $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,wpa-supplicant))
$(eval $(call BuildPackage,wpa-cli))
