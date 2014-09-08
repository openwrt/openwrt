#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/3G300M
	NAME:=Tenda 3G300M
	PACKAGES:=\
		kmod-usb-core kmod-usb-dwc2 \
		kmod-ledtrig-usbdev
endef

define Profile/3G300M/Description
	Package set for Tenda 3G300M board
endef

$(eval $(call Profile,3G300M))

define Profile/W306R_V20
	NAME:=Tenda W306R V2.0
	PACKAGES:=
endef

define Profile/W306R_V20/Description
	Package set for Tenda W306R V2.0 board
endef

$(eval $(call Profile,W306R_V20))
