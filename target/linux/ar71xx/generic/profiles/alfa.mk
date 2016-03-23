#
# Copyright (C) 2011-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/ALFAAP96
	NAME:=ALFA Network AP96 board
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-rtc-pcf2123
endef

define Profile/ALFAAP96/Description
	Package set optimized for the ALFA Network AP96 board.
endef

$(eval $(call Profile,ALFAAP96))


define Profile/ALFAAP120C
	NAME:=ALFA Network AP120C board
	PACKAGES:=uboot-envtools
endef

define Profile/ALFAAP120C/Description
	Package set optimized for the ALFA Network AP120C board.
endef

$(eval $(call Profile,ALFAAP120C))


define Profile/HORNETUB
	NAME:=ALFA Network Hornet-UB board (8MB flash, 32MB ram)
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef

define Profile/HORNETUB/Description
	Package set optimized for the ALFA Network Hornet-UB board with 8MB
	flash and 32MB ram.
endef

$(eval $(call Profile,HORNETUB))


define Profile/HORNETUBx2
	NAME:=ALFA Network Hornet-UB-x2 board (16MB flash, 64MB ram)
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef

define Profile/HORNETUBx2/Description
	Package set optimized for the ALFA Network Hornet-UB board with 16MB
	flash and 64MB ram.
endef

$(eval $(call Profile,HORNETUBx2))


define Profile/ALFANX
	NAME:=ALFA Network N2/N5 board
	PACKAGES:=
endef

define Profile/ALFANX/Description
	Package set optimized for the ALFA Network N2/N5 boards.
endef

$(eval $(call Profile,ALFANX))

define Profile/TUBE2H
	NAME:=ALFA Network Tube2H board
	PACKAGES:=
endef

define Profile/TUBE2H/Description
	Package set optimized for the ALFA Network Tube2H board.
endef

$(eval $(call Profile,TUBE2H))
