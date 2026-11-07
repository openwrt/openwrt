include $(TOPDIR)/rules.mk

PKG_NAME:=ucode-mod-mosquitto
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-or-later
PKG_MAINTAINER:=Janusz Dziedzic <janusz.dziedzic@gmail.com>

include $(INCLUDE_DIR)/package.mk

define Package/ucode-mod-mosquitto
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=ucode MQTT client module (libmosquitto)
  DEPENDS:=+libucode +libmosquitto +libubox
endef

define Package/ucode-mod-mosquitto/description
Native ucode C module wrapping libmosquitto with uloop integration.
Provides full MQTT v3.1.1 client support including QoS 0/1/2, will messages,
authentication and manual reconnect via the on_disconnect callback. TLS is
available when the libmosquitto-ssl variant is selected; calling tls_set()
against libmosquitto-nossl returns a runtime error. Scripts must call
client.destroy() when finished, analogous to the cancel() requirement on
ucode's uloop.timer.
endef

define Build/Configure
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CPPFLAGS) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) $(FPIC) \
		-Wall -ffunction-sections -Wl,--gc-sections -shared \
		-o $(PKG_BUILD_DIR)/mosquitto.so $(PKG_BUILD_DIR)/mosquitto.c \
		-lmosquitto -lubox
endef

define Package/ucode-mod-mosquitto/install
	$(INSTALL_DIR) $(1)/usr/lib/ucode
	$(CP) $(PKG_BUILD_DIR)/mosquitto.so $(1)/usr/lib/ucode/
endef

$(eval $(call BuildPackage,ucode-mod-mosquitto))
