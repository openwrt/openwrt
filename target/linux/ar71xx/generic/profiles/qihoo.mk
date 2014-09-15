#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/QIHOO360
	NAME:=Qihoo 360 C301
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev kmod-ath10k
endef

define Profile/QIHOO360/Description
  Package set optimized for the Qihoo 360 C301 device.
endef
$(eval $(call Profile,QIHOO360))
