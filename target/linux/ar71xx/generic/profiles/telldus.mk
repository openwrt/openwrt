#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/TELLSTICKZNETLITE
	NAME:=TellStick ZNet Lite
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-usb-acm kmod-usb-serial kmod-usb-serial-pl2303
endef


define Profile/TELLSTICKZNETLITE/Description
	Package set optimized for the TellStick ZNet Lite.
endef
$(eval $(call Profile,TELLSTICKZNETLITE))
