# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=be7000-nfc-i2ctransfer
PKG_RELEASE:=1

PKG_LICENSE:=GPL-2.0-only

include $(INCLUDE_DIR)/package.mk

define Package/be7000-nfc-i2ctransfer
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Xiaomi BE7000 NFC I2C transfer helper
  DEPENDS:=+kmod-i2c-core
endef

define Package/be7000-nfc-i2ctransfer/description
 Small i2ctransfer-compatible helper used by the Xiaomi BE7000 NFC scripts.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CPPFLAGS) $(TARGET_CFLAGS) \
		-o $(PKG_BUILD_DIR)/be7000-nfc-i2ctransfer \
		$(PKG_BUILD_DIR)/be7000-nfc-i2ctransfer.c \
		$(TARGET_LDFLAGS)
endef

define Package/be7000-nfc-i2ctransfer/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/be7000-nfc-i2ctransfer \
		$(1)/usr/sbin/be7000-nfc-i2ctransfer
endef

$(eval $(call BuildPackage,be7000-nfc-i2ctransfer))
