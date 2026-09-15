# SPDX-License-Identifier: GPL-2.0-only
#
# cfw-leds: stock LED behavior emulation for the COMFAST CF-WA350
#
# Installed via DEVICE_PACKAGES of the comfast_cf-wa350 device definition.

include $(TOPDIR)/rules.mk

PKG_NAME:=cfw-leds
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only
PKG_MAINTAINER:=Salah Ahmed <haddad2inc@gmail.com>
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/cfw-leds
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=COMFAST CF-WA350 stock LED behavior emulation
  DEPENDS:=@TARGET_ath79 +ethtool
endef

define Package/cfw-leds/description
Small POSIX-shell daemon reproducing the LED behavior of the stock
COMFAST CF-WA350 firmware for the three external LEDs:

 - OFF when the port has no cable, SOLID on link without traffic,
   BLINK while traffic flows, with the blink rate scaling with
   throughput, followed by a gradually decaying afterglow so that
   bursty traffic (e.g. video streaming) does not flicker.

On this DSA-based device the WAN/LAN rates are read from the QCA8337
hardware MIB counters (RxGoodByte/TxByte) via ethtool, so traffic
forwarded in hardware by the DSA bridge offload is counted as well;
the 2.4 GHz LED follows the on-board 2.4 GHz radio's netdevs. The daemon
takes exclusive ownership of blue:ext-wan, blue:ext-lan and
blue:ext-wlan2g; the internal LEDs remain with /etc/init.d/led.
endef

define Build/Compile
endef

define Package/cfw-leds/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) ./files/cfw_led_ctrl $(1)/usr/bin/cfw_led_ctrl
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/cfw-leds.init $(1)/etc/init.d/cfw-leds
endef

$(eval $(call BuildPackage,cfw-leds))
