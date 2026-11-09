# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

PKG_NAME:=bluetooth-uart
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=LICENSE
PKG_MAINTAINER:=Satoshi Ikeuchi <satosan.ikm@gmail.com>

include $(INCLUDE_DIR)/package.mk

define Package/bluetooth-uart
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Bluetooth UART attachment service
  DEPENDS:=+kmod-hci-uart +bluez-utils
endef

define Package/bluetooth-uart/description
 Supervised Bluetooth UART attachment using BlueZ btattach.
 The UART, protocol and speed are configured through UCI.
endef

define Package/bluetooth-uart/conffiles
/etc/config/bluetooth-uart
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) $(TOPDIR)/LICENSES/GPL-2.0 $(PKG_BUILD_DIR)/LICENSE
endef

define Build/Compile
endef

define Package/bluetooth-uart/install
	$(INSTALL_DIR) $(1)/etc/config $(1)/etc/init.d
	$(INSTALL_CONF) ./files/bluetooth-uart.config $(1)/etc/config/bluetooth-uart
	$(INSTALL_BIN) ./files/bluetooth-uart.init $(1)/etc/init.d/bluetooth-uart
endef

$(eval $(call BuildPackage,bluetooth-uart))
