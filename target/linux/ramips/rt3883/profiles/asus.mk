#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/RTN56U
	NAME:=Asus RT-N56U
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2 swconfig
endef

define Profile/RTN56U/Description
	Package set compatible with the Asus RT-N56U board.
endef
$(eval $(call Profile,RTN56U))
