#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/RTN10PLUS
	NAME:=Asus RT-N10+
	PACKAGES:=kmod-leds-gpio wpad-mini
endef

define Profile/RTN10PLUS/Description
	Package set for Asus RT-N10+.
endef
$(eval $(call Profile,RTN10PLUS))


define Profile/RT-N13U
	NAME:=Asus RT-N13U
	PACKAGES:=kmod-leds-gpio kmod-rt2800-pci wpad-mini kmod-usb-dwc2
endef

define Profile/RT-N13U/Description
	Package set for Asus RT-N13U.
endef
$(eval $(call Profile,RT-N13U))
