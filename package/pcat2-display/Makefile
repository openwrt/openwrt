include $(TOPDIR)/rules.mk

PKG_NAME:=pcat2-display
PKG_VERSION:=1.0.0
PKG_RELEASE:=1
PKG_LICENSE:=GPL-3.0-only

include $(INCLUDE_DIR)/package.mk

define Package/pcat2-display
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=PhotoniCAT 2 Mini Display
  DEPENDS:=@TARGET_rockchip_armv8
endef

define Package/pcat2-display/description
  Userspace driver for the PhotoniCAT 2 GC9307 172x320 SPI TFT display.
  Shows time/date, battery status, WAN/WiFi info, and system stats.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS) -Wall" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

define Package/pcat2-display/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/pcat2-display $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/pcat2-display.init $(1)/etc/init.d/pcat2-display
endef

$(eval $(call BuildPackage,pcat2-display))
