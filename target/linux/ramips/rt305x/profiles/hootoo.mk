#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/HT-TM02
	NAME:=HooToo HT-TM02
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev
endef

define Profile/HT-TM02/Description
	Package set optimized for the HooToo HT-TM02.
endef
$(eval $(call Profile,HT-TM02))
