#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Default
	NAME:=Default Profile (all drivers)
	PACKAGES:= \
		kmod-mmc kmod-mvsdio swconfig \
		kmod-usb2 kmod-usb3 kmod-usb-storage \
		kmod-i2c-core kmod-i2c-mv64xxx \
		kmod-ata-core kmod-ata-marvell-sata \
		kmod-rtc-marvell kmod-thermal-armada \
		kmod-gpio-button-hotplug kmod-hwmon-tmp421 \
		kmod-hwmon-pwmfan kmod-leds-tlc59116 \
		kmod-ledtrig-usbdev kmod-mwlwifi wpad-mini \
		kmod-ata-mvebu-ahci
endef

define Profile/Default/Description
	Default package set compatible with most boards.
endef

$(eval $(call Profile,Default))
