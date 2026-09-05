# SPDX-License-Identifier: GPL-2.0-only
#
# cfw-leds package: Emulates the original Comfast LED behavior on the CF-WA350
#



include $(TOPDIR)/rules.mk

PKG_NAME:=cfw-leds
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_LICENSE:=GPL-2.0-only
PKG_MAINTAINER:=Salah Ahmed <haddad2inc@gmail.com>
PKG_LICENSE_FILES:=LICENSE

include $(INCLUDE_DIR)/package.mk

define Package/cfw-leds
  SECTION:=base
  CATEGORY:=Base system
  TITLE:=Comfast CF-WA350 stock LED behavior emulation
  DEPENDS:=@TARGET_ath79
endef

define Package/cfw-leds/description
Emulates the behavior of the external LEDs in the original firmware of the
Comfast CF-WA350: detects link status from switch port MIB counters
and blinks proportionally to traffic throughput, with an afterglow after
traffic stops.
Manages: blue:ext_wan (switch port 1), blue:ext_lan (port 2),
and blue:ext_wlan2g (phy1-ap0 interface).
endef

define Build/Compile
endef

define Package/cfw-leds/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) ./files/cfw_led_ctrl $(1)/usr/bin/cfw_led_ctrl
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/cfw_leds.init $(1)/etc/init.d/cfw_leds
endef

# When installed via opkg on a running device: enable and start the service
define Package/cfw-leds/postinst
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] || {
	/etc/init.d/cfw_leds enable 2>/dev/null
	/etc/init.d/cfw_leds start 2>/dev/null
}
exit 0
endef

$(eval $(call BuildPackage,cfw-leds))
