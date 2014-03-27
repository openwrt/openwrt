#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/OOLITE
	NAME:=OOLITE
	PACKAGES:=kmod-usb-core kmod-usb2  kmod-ledtrig-usbdev
endef


define Profile/OOLITE/Description
	Package set optimized for the OOLITE.
endef
$(eval $(call Profile,OOLITE))

