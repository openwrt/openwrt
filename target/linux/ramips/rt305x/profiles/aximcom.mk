#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/MR102N
	NAME:=AXIMCom MR-102N
	PACKAGES:=kmod-usb-core kmod-usb-dwc2 kmod-ledtrig-usbdev
endef

define Profile/MR102N/Description
	Package set for AXIMCom MR-102N
endef
$(eval $(call Profile,MR102N))
