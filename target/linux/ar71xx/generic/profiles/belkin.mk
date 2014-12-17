#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/F9K1115V2
	NAME:=Belkin AC1750DB (F9K1115V2)
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-usb3 kmod-ledtrig-usbdev \
		kmod-ath10k
endef

define Profile/F9K1115V2/Description
	Package set optimized for the Belkin AC1750DB (F9K1115V2) board.
endef

$(eval $(call Profile,F9K1115V2))
