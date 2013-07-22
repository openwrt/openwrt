#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/FREESTATION5
	NAME:=FreeStation5
	PACKAGES:=\
		 kmod-usb-dwc2 kmod-rt2500-usb kmod-rt2800-usb kmod-rt2x00-usb
endef

define Profile/FREESTATION5/Description
	Package set for ARC FreeStation, Flex mARC, iFlex, and SplitStation
endef

$(eval $(call Profile,FREESTATION5))
