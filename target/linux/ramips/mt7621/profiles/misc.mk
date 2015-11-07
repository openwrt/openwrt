#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/PBR-M1
	NAME:=PBR-M1 Device
	FEATURES+=rtc
	PACKAGES:=\
		kmod-usb-core kmod-usb3 kmod-usb-hid kmod-sdhci-mt7620 \
		kmod-ledtrig-usbdev kmod-ata-core kmod-ata-ahci \
		kmod-rtc-pcf8563
endef

define Profile/PBR-M1/Description
	Default package set compatible with most boards.
endef
$(eval $(call Profile,PBR-M1))
