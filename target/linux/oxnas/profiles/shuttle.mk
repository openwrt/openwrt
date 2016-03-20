# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/KD20
  NAME:=Shuttle KD20
  PACKAGES:= \
	kmod-usb3 kmod-usb-storage kmod-i2c-gpio kmod-rtc-pcf8563 \
	kmod-gpio-beeper kmod-hwmon-core kmod-hwmon-gpiofan
endef

define Profile/KD20/Description
 Profile with built-in ox820 KD20 device-tree
endef

$(eval $(call Profile,KD20))
