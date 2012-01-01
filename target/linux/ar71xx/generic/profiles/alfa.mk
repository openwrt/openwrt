#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/HORNETUB
	NAME:=ALFA Network Hornet-UB board
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef

define Profile/HORNETUB/Description
	Package set optimized for the ALFA Network Hornet-UB board.
endef

$(eval $(call Profile,HORNETUB))


define Profile/ALFANX
	NAME:=ALFA Network N2/N5 board
	PACKAGES:=
endef

define Profile/ALFANX/Description
	Package set optimized for the ALFA Network N2/N5 boards.
endef

$(eval $(call Profile,ALFANX))

