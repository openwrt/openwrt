#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/UPVEL
	NAME:=UPVEL UR-336UN
	PACKAGES:=\
		kmod-usb-core kmod-usb-rt305x-dwc_otg \
		kmod-ledtrig-usbdev
endef

define Profile/UPVEL/Description
	Default package set compatible with URVEL router board.
endef
$(eval $(call Profile,UPVEL))
