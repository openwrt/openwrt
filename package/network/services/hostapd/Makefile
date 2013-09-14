#
# Copyright (C) 2006-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=hostapd
PKG_VERSION:=20130807
PKG_RELEASE:=1
PKG_REV:=d2ba3d6bd9336ef8fd761a0cc7999824d4c4da41

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=git://w1.fi/srv/git/hostap.git
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_REV)
PKG_SOURCE_PROTO:=git
PKG_MIRROR_MD5SUM:=4e7c1f97edd7514535056fce54ae053a

PKG_MAINTAINER:=Felix Fietkau <nbd@openwrt.org>

PKG_BUILD_PARALLEL:=1

PKG_BUILD_DEPENDS:= \
	PACKAGE_kmod-madwifi:madwifi \

PKG_CONFIG_DEPENDS:= \
	CONFIG_WPA_SUPPLICANT_NO_TIMESTAMP_CHECK \
	CONFIG_PACKAGE_kmod-ath9k \
	CONFIG_PACKAGE_kmod-mac80211 \
	CONFIG_PACKAGE_kmod-madwifi \
	CONFIG_PACKAGE_hostapd \
	CONFIG_PACKAGE_hostapd-mini \
	CONFIG_PACKAGE_kmod-hostap \
	CONFIG_WPA_RFKILL_SUPPORT \
	CONFIG_DRIVER_WEXT_SUPPORT \
	CONFIG_DRIVER_11N_SUPPORT

LOCAL_TYPE=$(strip \
		$(if $(findstring wpad,$(BUILD_VARIANT)),wpad, \
		$(if $(findstring supplicant,$(BUILD_VARIANT)),supplicant, \
		hostapd \
	)))
LOCAL_VARIANT=$(patsubst wpad-%,%,$(patsubst supplicant-%,%,$(BUILD_VARIANT)))

ifeq ($(LOCAL_TYPE),supplicant)
  ifeq ($(LOCAL_VARIANT),full)
    PKG_CONFIG_DEPENDS += \
		CONFIG_WPA_SUPPLICANT_INTERNAL \
		CONFIG_WPA_SUPPLICANT_OPENSSL
  endif
endif

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(BUILD_VARIANT)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

STAMP_CONFIGURED:=$(STAMP_CONFIGURED)_$(CONFIG_WPA_MSG_MIN_PRIORITY)

ifneq ($(CONFIG_DRIVER_11N_SUPPORT),)
  HOSTAPD_IEEE80211N:=y
endif

DRIVER_MAKEOPTS= \
	CONFIG_DRIVER_NL80211=$(CONFIG_PACKAGE_kmod-mac80211) \
	CONFIG_DRIVER_MADWIFI=$(CONFIG_PACKAGE_kmod-madwifi) \
	CONFIG_DRIVER_HOSTAP=$(CONFIG_PACKAGE_kmod-hostap) \
	CONFIG_IEEE80211N=$(HOSTAPD_IEEE80211N) \
	CONFIG_IEEE80211W=$(CONFIG_PACKAGE_kmod-ath9k) \
	CONFIG_DRIVER_WEXT=$(CONFIG_DRIVER_WEXT_SUPPORT) \
	$(if $(CONFIG_WPA_RFKILL_SUPPORT),NEED_RFKILL=y)

ifneq ($(LOCAL_TYPE),hostapd)
  ifdef CONFIG_WPA_SUPPLICANT_OPENSSL
    ifeq ($(LOCAL_VARIANT),full)
      DRIVER_MAKEOPTS += CONFIG_TLS=openssl
      TARGET_LDFLAGS += -lcrypto -lssl
    endif
  endif
  ifdef CONFIG_WPA_SUPPLICANT_NO_TIMESTAMP_CHECK
    TARGET_CFLAGS += -DNO_TIMESTAMP_CHECK
  endif
  DRIVER_MAKEOPTS += \
	CONFIG_DRIVER_ROBOSWITCH=$(CONFIG_PACKAGE_kmod-switch)
endif

DRV_DEPENDS:=+PACKAGE_kmod-mac80211:libnl-tiny @(!TARGET_avr32||BROKEN)

define Package/hostapd/Default
  SECTION:=net
  CATEGORY:=Network
  TITLE:=IEEE 802.1x Authenticator
  URL:=http://hostap.epitest.fi/
  DEPENDS:=$(DRV_DEPENDS) +libubus
endef

define Package/hostapd
$(call Package/hostapd/Default)
  TITLE+= (full)
  VARIANT:=full
endef

define Package/hostapd/description
 This package contains a full featured IEEE 802.1x/WPA/EAP/RADIUS
 Authenticator.
endef

define Package/hostapd-mini
$(call Package/hostapd/Default)
  TITLE+= (WPA-PSK only)
  VARIANT:=mini
endef

define Package/hostapd-mini/description
 This package contains a minimal IEEE 802.1x/WPA Authenticator (WPA-PSK only).
endef

define Package/hostapd-utils
  $(call Package/hostapd/Default)
  TITLE+= (utils)
  DEPENDS:=@PACKAGE_hostapd||PACKAGE_hostapd-mini||PACKAGE_wpad||PACKAGE_wpad-mini
endef

define Package/hostapd-utils/description
 This package contains a command line utility to control the
 IEEE 802.1x/WPA/EAP/RADIUS Authenticator.
endef

define Package/wpad/Default
  SECTION:=net
  CATEGORY:=Network
  TITLE:=IEEE 802.1x Authenticator/Supplicant
  URL:=http://hostap.epitest.fi/
endef

define Package/wpad
$(call Package/wpad/Default)
  TITLE+= (full)
  DEPENDS:=$(DRV_DEPENDS) +WPA_SUPPLICANT_OPENSSL:libopenssl +libubus
  VARIANT:=wpad-full
endef

define Package/wpad/description
 This package contains a full featured IEEE 802.1x/WPA/EAP/RADIUS
 Authenticator and Supplicant
endef

define Package/wpad-mini
$(call Package/wpad/Default)
  TITLE+= (WPA-PSK only)
  DEPENDS:=$(DRV_DEPENDS) +libubus
  VARIANT:=wpad-mini
endef

define Package/wpad-mini/description
 This package contains a minimal IEEE 802.1x/WPA Authenticator and Supplicant (WPA-PSK only).
endef

define Package/wpa-supplicant
  SECTION:=net
  CATEGORY:=Network
  TITLE:=WPA Supplicant
  URL:=http://hostap.epitest.fi/wpa_supplicant/
  DEPENDS:=$(DRV_DEPENDS) +WPA_SUPPLICANT_OPENSSL:libopenssl
  VARIANT:=supplicant-full
endef

define Package/wpa-supplicant/Description
  WPA Supplicant
endef

define Package/wpa-supplicant/config
	source "$(SOURCE)/Config.in"
endef

define Package/wpa-supplicant-p2p
  $(Package/wpa-supplicant)
  TITLE:=WPA Supplicant (with Wi-Fi P2P support)
  DEPENDS:=$(DRV_DEPENDS)
  VARIANT:=supplicant-p2p
endef

define Package/wpa-supplicant-p2p/Description
  WPA Supplicant (with Wi-Fi P2P support)
endef

define Package/wpa-supplicant-mini
  $(Package/wpa-supplicant)
  TITLE:=WPA Supplicant (minimal version)
  DEPENDS:=$(DRV_DEPENDS)
  VARIANT:=supplicant-mini
endef

define Package/wpa-supplicant-mini/Description
  WPA Supplicant (minimal version)
endef

define Package/wpa-cli
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=@PACKAGE_wpa-supplicant||PACKAGE_wpa-supplicant-p2p||PACKAGE_wpad-mini||PACKAGE_wpad
  TITLE:=WPA Supplicant command line interface
endef

define Package/wpa-cli/Description
  WPA Supplicant control utility
endef


ifneq ($(wildcard $(PKG_BUILD_DIR)/.config_*),$(subst .configured_,.config_,$(STAMP_CONFIGURED)))
  define Build/Configure/rebuild
	$(FIND) $(PKG_BUILD_DIR) -name \*.o -or -name \*.a | $(XARGS) rm -f
	rm -f $(PKG_BUILD_DIR)/hostapd/hostapd
	rm -f $(PKG_BUILD_DIR)/wpa_supplicant/wpa_supplicant
	rm -f $(PKG_BUILD_DIR)/.config_*
	touch $(subst .configured_,.config_,$(STAMP_CONFIGURED))
  endef
endif

define Build/Configure
	$(Build/Configure/rebuild)
	$(if $(wildcard ./files/hostapd-$(LOCAL_VARIANT).config), \
		$(CP) ./files/hostapd-$(LOCAL_VARIANT).config $(PKG_BUILD_DIR)/hostapd/.config \
	)
	$(CP) ./files/wpa_supplicant-$(LOCAL_VARIANT).config $(PKG_BUILD_DIR)/wpa_supplicant/.config
endef

TARGET_CPPFLAGS := \
	-I$(STAGING_DIR)/usr/include/libnl-tiny \
	-I$(PKG_BUILD_DIR)/src/crypto \
	$(TARGET_CPPFLAGS) \
	-I$(CURDIR)/madwifi \
	-DCONFIG_LIBNL20 \
	-D_GNU_SOURCE \
	$(if $(CONFIG_WPA_MSG_MIN_PRIORITY),-DCONFIG_MSG_MIN_PRIORITY=$(CONFIG_WPA_MSG_MIN_PRIORITY))

TARGET_CFLAGS += -ffunction-sections -fdata-sections
TARGET_LDFLAGS += -Wl,--gc-sections
ifeq ($(findstring supplicant,$(BUILD_VARIANT)),)
  TARGET_LDFLAGS += -lubox -lubus
endif

ifdef CONFIG_PACKAGE_kmod-mac80211
  TARGET_LDFLAGS += -lm -lnl-tiny
endif

define Build/RunMake
	CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
	$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR)/$(1) \
		$(TARGET_CONFIGURE_OPTS) \
		$(DRIVER_MAKEOPTS) \
		LIBS="$(TARGET_LDFLAGS)" \
		BCHECK= \
		$(2)
endef

define Build/Compile/wpad
	echo ` \
		$(call Build/RunMake,hostapd,-s MULTICALL=1 dump_cflags); \
		$(call Build/RunMake,wpa_supplicant,-s MULTICALL=1 dump_cflags) | \
		sed -e 's,-n ,,g' -e 's,$(TARGET_CFLAGS),,' \
	` > $(PKG_BUILD_DIR)/.cflags
	+$(call Build/RunMake,hostapd, \
		CFLAGS="$$$$(cat $(PKG_BUILD_DIR)/.cflags)" \
		MULTICALL=1 \
		hostapd_cli hostapd_multi.a \
	)
	+$(call Build/RunMake,wpa_supplicant, \
		CFLAGS="$$$$(cat $(PKG_BUILD_DIR)/.cflags)" \
		MULTICALL=1 \
		wpa_cli wpa_supplicant_multi.a \
	)
	$(TARGET_CC) -o $(PKG_BUILD_DIR)/wpad \
		$(TARGET_CFLAGS) \
		./files/multicall.c \
		$(PKG_BUILD_DIR)/hostapd/hostapd_multi.a \
		$(PKG_BUILD_DIR)/wpa_supplicant/wpa_supplicant_multi.a \
		$(TARGET_LDFLAGS)
endef

define Build/Compile/hostapd
	$(call Build/RunMake,hostapd, \
		hostapd hostapd_cli \
	)
endef

define Build/Compile/supplicant
	$(call Build/RunMake,wpa_supplicant, \
		wpa_cli wpa_supplicant \
	)
endef

define Build/Compile
	$(Build/Compile/$(LOCAL_TYPE))
endef

define Install/hostapd
	$(INSTALL_DIR) $(1)/lib/wifi
	$(INSTALL_DATA) ./files/hostapd.sh $(1)/lib/wifi/hostapd.sh
	$(INSTALL_DIR) $(1)/usr/sbin
endef

define Install/supplicant
	$(INSTALL_DIR) $(1)/lib/wifi
	$(INSTALL_DATA) ./files/wpa_supplicant.sh $(1)/lib/wifi/wpa_supplicant.sh
	$(INSTALL_DIR) $(1)/usr/sbin
endef

define Package/hostapd/install
	$(call Install/hostapd,$(1))
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hostapd/hostapd $(1)/usr/sbin/
endef
Package/hostapd-mini/install = $(Package/hostapd/install)

ifneq ($(LOCAL_TYPE),supplicant)
  define Package/hostapd-utils/install
	$(INSTALL_DIR) $(1)/usr/sbin $(1)/etc/rc.button
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hostapd/hostapd_cli $(1)/usr/sbin/
	$(INSTALL_BIN) ./files/wps-hotplug.sh $(1)/etc/rc.button/wps
  endef
endif

define Package/wpad/install
	$(call Install/hostapd,$(1))
	$(call Install/supplicant,$(1))
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wpad $(1)/usr/sbin/
	ln -sf wpad $(1)/usr/sbin/hostapd
	ln -sf wpad $(1)/usr/sbin/wpa_supplicant
endef
Package/wpad-mini/install = $(Package/wpad/install)

define Package/wpa-supplicant/install
	$(call Install/supplicant,$(1))
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wpa_supplicant/wpa_supplicant $(1)/usr/sbin/
endef
Package/wpa-supplicant-mini/install = $(Package/wpa-supplicant/install)
Package/wpa-supplicant-p2p/install = $(Package/wpa-supplicant/install)

ifneq ($(LOCAL_TYPE),hostapd)
  define Package/wpa-cli/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_BUILD_DIR)/wpa_supplicant/wpa_cli $(1)/usr/sbin/
  endef
endif

$(eval $(call BuildPackage,hostapd))
$(eval $(call BuildPackage,hostapd-mini))
$(eval $(call BuildPackage,wpad))
$(eval $(call BuildPackage,wpad-mini))
$(eval $(call BuildPackage,wpa-supplicant))
$(eval $(call BuildPackage,wpa-supplicant-mini))
$(eval $(call BuildPackage,wpa-supplicant-p2p))
$(eval $(call BuildPackage,wpa-cli))
$(eval $(call BuildPackage,hostapd-utils))
