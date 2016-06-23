#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Yun
	NAME:=Arduino Yun based on Atheros AR9331
	PACKAGES:=kmod-usb-core kmod-usb2
endef

define Profile/Yun/Description
	Package set optimized for the Arduino Yun.
	This is sometimes also named Genuino Yun or Linino Yun.
endef

$(eval $(call Profile,Yun))

