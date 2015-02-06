#
# Copyright (C) 2013-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Mamba
  NAME:=Linksys WRT1900AC (Mamba)
  PACKAGES:= \
	kmod-usb2 kmod-usb3 kmod-usb-storage \
	kmod-of-i2c kmod-i2c-core kmod-i2c-mv64xxx \
	kmod-ata-core kmod-ata-marvell-sata \
	kmod-rtc-marvell kmod-thermal-armada \
	kmod-gpio-button-hotplug kmod-hwmon-tmp421 \
	kmod-hwmon-pwmfan kmod-leds-tlc59116 \
	kmod-ledtrig-usbdev kmod-mwlwifi wpad-mini \
	swconfig
endef

define Profile/Mamba/Description
 Package set compatible with the Linksys WRT1900AC (Mamba).
endef

$(eval $(call Profile,Mamba))
