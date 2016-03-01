#
# Copyright (C) 2015-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/MINIBOXV1
	NAME:=Gainstrong MiniBox V1.0
	PACKAGES:=kmod-usb-core kmod-usb2  kmod-ledtrig-usbdev
endef


define Profile/MINIBOXV1/Description
	Package set optimized for the Gainstrong MiniBox V1.0
endef
$(eval $(call Profile,MINIBOXV1))

define Profile/OOLITE
	NAME:=Gainstrong OOLITE
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef


define Profile/OOLITE/Description
	Package set optimized for the Gainstrong OOLITE.
endef
$(eval $(call Profile,OOLITE))
