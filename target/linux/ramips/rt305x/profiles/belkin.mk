#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/F5D8235V2
	NAME:=Belkin F5D8235 v2
	PACKAGES:=\
		kmod-switch-rtl8366rb kmod-swconfig swconfig \
		kmod-usb-core kmod-usb-rt305x-dwc_otg \
		kmod-ledtrig-usbdev
endef

define Profile/F5D8235V2/Description
	Package set for Belkin F5D8235 v2
endef

$(eval $(call Profile,F5D8235V2))
