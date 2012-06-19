#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WNDR3700
	NAME:=NETGEAR WNDR3700/WNDR3800/WNDRMAC
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev kmod-leds-wndr3700-usb
endef

define Profile/WNDR3700/Description
	Package set optimized for the NETGEAR WNDR3700/WNDR3800/WNDRMAC
endef

$(eval $(call Profile,WNDR3700))
