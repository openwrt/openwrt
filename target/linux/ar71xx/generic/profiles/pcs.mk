#
# Copyright (C) 2009 OpenWrt.org
# Copyright (C) 2012 PowerCloud Systems
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/UBDEV01
	NAME:=PowerCloud Systems ubdev01 model
	PACKAGES:=
endef

define Profile/UBDEV01/Description
	Package set optimized for the PowerCloud Systems ubdev01 board.
endef

$(eval $(call Profile,UBDEV01))

define Profile/DLRTDEV01
	NAME:=PowerCloud Systems dlrtdev01 model
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev
endef

define Profile/DLRTDEV01/Description
	Package set optimized for the PowerCloud Systems dlrtdev01 board.
endef

$(eval $(call Profile,DLRTDEV01))
