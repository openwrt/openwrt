#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/HW5503G
	NAME:=Aztech HW550-3G
	PACKAGES:=\
		kmod-usb-core kmod-usb-rt305x-dwc_otg \
		kmod-ledtrig-usbdev
endef

define Profile/HW5503G/Description
	Package set for Aztech HW550-3G
endef

$(eval $(call Profile,HW5503G))
