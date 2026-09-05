include $(TOPDIR)/rules.mk

PKG_NAME:=modemsignal-leds
PKG_VERSION:=1
PKG_RELEASE:=1

PKG_MAINTAINER:=Carlo Szelinsky <github@szelinsky.de>
PKG_LICENSE:=GPL-2.0-or-later
PKG_LICENSE_FILES:=

include $(INCLUDE_DIR)/package.mk

define Package/modemsignal-leds
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=WWAN
  TITLE:=Drive cellular signal-strength LEDs from modem RSSI
  DEPENDS:=+jsonfilter +ubus
endef

define Package/modemsignal-leds/description
  Lights cellular signal-strength LEDs from a modem's reported signal,
  via AT, QMI or MBIM.
endef

define Package/modemsignal-leds/conffiles
/etc/config/modemsignal-leds
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_CPPFLAGS) -Wall \
		-o $(PKG_BUILD_DIR)/modemsignal-leds \
		$(PKG_BUILD_DIR)/modemsignal-leds.c $(TARGET_LDFLAGS)
endef

define Package/modemsignal-leds/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/modemsignal-leds $(1)/usr/sbin/modemsignal-leds
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/modemsignal-leds.init $(1)/etc/init.d/modemsignal-leds
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/modemsignal-leds.config $(1)/etc/config/modemsignal-leds
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) ./files/modemsignal-leds.defaults $(1)/etc/uci-defaults/91-modemsignal-leds
	$(INSTALL_DIR) $(1)/usr/libexec/modemsignal-leds
	$(INSTALL_BIN) ./files/backend $(1)/usr/libexec/modemsignal-leds/backend
	$(INSTALL_DIR) $(1)/usr/share/modemsignal-leds
	$(INSTALL_DATA) ./files/getcsq.gcom $(1)/usr/share/modemsignal-leds/getcsq.gcom
	$(INSTALL_DATA) ./files/getqcsq.gcom $(1)/usr/share/modemsignal-leds/getqcsq.gcom
	$(INSTALL_DIR) $(1)/usr/share/modemsignal-leds/profiles
	$(INSTALL_DATA) ./files/profiles/* $(1)/usr/share/modemsignal-leds/profiles/
endef

$(eval $(call BuildPackage,modemsignal-leds))
