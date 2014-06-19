#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/RT-N10-PLUS
	NAME:=Asus RT-N10+
	PACKAGES:=kmod-leds-gpio wpa
endef

define Profile/RT-N10-PLUS/Description
	Package set for Asus RT-N10+.
endef

define Profile/RT-N13U
	NAME:=Asus RT-N13U
	PACKAGES:=kmod-leds-gpio kmod-rt2800-pci wpad-mini kmod-usb-rt305x-dwc_otg
endef

define Profile/RT-N13U/Description
	Package set for Asus RT-N13U.
endef

$(eval $(call Profile,RT-N10-PLUS))
$(eval $(call Profile,RT-N13U))
