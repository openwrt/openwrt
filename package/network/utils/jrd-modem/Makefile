include $(TOPDIR)/rules.mk

PKG_NAME:=jrd-modem
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only

PKG_MAINTAINER:=Lucian CRISTIAN <lucian.cristian@gmail.com>
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/jrd-modem
  SECTION:=net
  CATEGORY:=Network
  SUBMENU:=WWAN
  TITLE:=JRD/Alcatel modem control and netifd protocol
  DEPENDS:=+netifd +ucode-mod-fs +ucode-mod-uci
endef

define Package/jrd-modem/description
 Control JRD/Alcatel cellular modems exposing the kcap JSON-RPC service
 over TCP. Includes jrdctl and a native netifd ucode protocol handler.
endef

define Package/jrd-modem/conffiles
/etc/config/jrd
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/jrdctl.c $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CPPFLAGS) $(TARGET_CFLAGS) \
		-std=c99 -Wall -Wextra \
		-o $(PKG_BUILD_DIR)/jrdctl \
		$(PKG_BUILD_DIR)/jrdctl.c \
		$(TARGET_LDFLAGS)
endef

define Package/jrd-modem/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/jrdctl $(1)/usr/bin/jrdctl
	$(INSTALL_DIR) $(1)/lib/netifd/proto
	$(INSTALL_BIN) ./files/jrd.uc $(1)/lib/netifd/proto/jrd.uc
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/jrd.config $(1)/etc/config/jrd
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_BIN) ./files/95-jrd-ethernet-led $(1)/etc/hotplug.d/iface/95-jrd-ethernet-led
endef

$(eval $(call BuildPackage,jrd-modem))
