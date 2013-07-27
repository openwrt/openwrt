#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/DIR615H1
	NAME:=Dlink DIR-615 H1
        PACKAGES:=\
                -kmod-usb-core -kmod-usb-rt305x-dwc_otg \
                -kmod-ledtrig-usbdev
endef

define Profile/DIR615H1/Description
	Package set for Dlink DIR-615 H1 board
endef

$(eval $(call Profile,DIR615H1))

