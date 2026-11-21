include $(TOPDIR)/rules.mk

PKG_NAME:=dvf99-css-firmware
PKG_RELEASE:=1

PKG_MAINTAINER:=John Crispin <john@phrozen.org>
PKG_LICENSE:=GPL-2.0-only

include $(INCLUDE_DIR)/package.mk

define Package/dvf99-css-firmware
  SECTION:=firmware
  CATEGORY:=Firmware
  TITLE:=DSP Group DVF99 CSS firmware provisioning
  DEPENDS:=@TARGET_dspg_dvf99 +libopenssl
endef

define Package/dvf99-css-firmware/description
  Provisions the CSS co-processor firmware (css-loader) for the DSP Group
  DVF99 on the device. It recovers the per-file key from a vendor firmware
  update with the vendor public key from the rescuefs partition, decrypts
  and extracts css-loader in a stream, verifies it and installs it. It
  ships only the tool, never the firmware blob or the key.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_CPPFLAGS) $(TARGET_LDFLAGS) \
		-o $(PKG_BUILD_DIR)/dvf99-css-decrypt \
		$(PKG_BUILD_DIR)/css_decrypt.c -lcrypto
endef

define Package/dvf99-css-firmware/conffiles
/etc/config/dvf99-css-firmware
endef

define Package/dvf99-css-firmware/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dvf99-css-decrypt $(1)/usr/sbin/dvf99-css-decrypt
	$(INSTALL_BIN) ./files/dvf99-css-firmware.sh $(1)/usr/sbin/dvf99-css-firmware
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/dvf99-css-firmware.config $(1)/etc/config/dvf99-css-firmware
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/dvf99-css-firmware.init $(1)/etc/init.d/css
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) ./files/dvf99-css-firmware.defaults $(1)/etc/uci-defaults/90-dvf99-css-firmware
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_BIN) ./files/dvf99-css-firmware.hotplug $(1)/etc/hotplug.d/iface/90-dvf99-css-firmware
	$(INSTALL_DIR) $(1)/lib/upgrade/keep.d
	$(INSTALL_DATA) ./files/dvf99-css-firmware.keep $(1)/lib/upgrade/keep.d/dvf99-css-firmware
endef

$(eval $(call BuildPackage,dvf99-css-firmware))
