#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/M3
	NAME:=Poray M3
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-ledtrig-netdev kmod-ledtrig-timer
endef

define Profile/M3/Description
	Package set for Poray M3 board
endef

$(eval $(call Profile,M3))

define Profile/M4
	NAME:=Poray M4
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-ledtrig-netdev kmod-ledtrig-timer
endef
define Profile/M4/Description
	Package set for Poray M4 board
endef

$(eval $(call Profile,M4))
