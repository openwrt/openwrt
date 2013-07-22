#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/FONERA20N
	NAME:=Fonera 2.0N
	PACKAGES:=\
		kmod-usb-core kmod-usb-dwc2 \
		kmod-ledtrig-usbdev
endef

define Profile/FONERA20N/Description
	Package set for Fonera 2.0N
endef

$(eval $(call Profile,FONERA20N))
