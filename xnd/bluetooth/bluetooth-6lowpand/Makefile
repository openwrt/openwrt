#
# Copyright (C) 2016 Nordic Semiconductor ASA.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=bluetooth-6lowpand
PKG_VERSION:=0.0.1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=https://github.com/NordicSemiconductor/Linux-ble-6lowpan-joiner.git
PKG_SOURCE_VERSION:=5ce5b248846a6d4ac4a609eb0e8d023cf920b247
PKG_SOURCE_PROTO:=git

BLUEZ_DIR:=$(wildcard $(BUILD_DIR)/bluez-*)

TARGET_CFLAGS += -I$(BLUEZ_DIR)
TARGET_LDFLAGS += -L$(BLUEZ_DIR)/lib/.libs/ -L$(BLUEZ_DIR)/src/.libs/ -lshared-mainloop -lbluetooth-internal

include $(INCLUDE_DIR)/package.mk

define Package/bluetooth-6lowpand
  SECTION:=base
  CATEGORY:=Network
  TITLE:=Bluetooth LE 6lowpan joiner daemon
  URL:=http://www.nordicsemi.com/
  DEPENDS:=+libusb-1.0 +bluez-libs
endef

define Package/bluetooth-6lowpand/description
  Bluetooth Low Energy IPSP device scanner and connection daemon. 
  The Daemon can be used to whitelist certain IPSP Bluetooth LE MAC
  addresses, or autoconnect using SSID and Key derived from Wifi AP 
  setup to authenticate the devices in order to connect. Also, manual 
  configuration of software SSID and Key can be used.
endef

define Package/bluetooth-6lowpand/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/bluetooth_6lowpand.init $(1)/etc/init.d/bluetooth_6lowpand

	$(INSTALL_DIR) $(1)/etc/bluetooth
	$(INSTALL_DATA) ./files/bluetooth_6lowpand.conf $(1)/etc/bluetooth

	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/bluetooth_6lowpand $(1)/usr/sbin
endef

$(eval $(call BuildPackage,bluetooth-6lowpand))
