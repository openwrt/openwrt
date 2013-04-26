#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/LMU5000
	NAME:=CalAmp LMU5000
	PACKAGES:= \
		kmod-rtc-pcf2123 \
		kmod-usb-acm \
		kmod-usb-serial \
		kmod-usb-serial-option \
		kmod-usb-serial-sierrawireless \
		kmod-gpio-mcp23s08
endef

define Profile/LMU5000/Description
	CalAmp LMU5000
endef

$(eval $(call Profile,LMU5000))
