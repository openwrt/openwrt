#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Default
	NAME:=Default Profile
	PACKAGES:=\
		kmod-usb-core kmod-usb3 kmod-ledtrig-usbdev \
		kmod-usb-storage \
		kmod-i2c-gpio kmod-rtc-pcf8563 kmod-rtc-ds1307 \
		kmod-gpio-beeper kmod-hwmon-core kmod-hwmon-gpiofan
endef

define Profile/Default/Description
	Default package set compatible with most boards.
endef

$(eval $(call Profile,Default))
