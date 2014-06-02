#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/CYSWR1100
	NAME:=Samsung CY-SWR1100
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2 swconfig
endef

define Profile/CYSWR1100/Description
	Package set compatible with the Samsung CY-SWR1100 board.
endef
$(eval $(call Profile,CYSWR1100))
