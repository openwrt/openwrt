#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/DCS930
	NAME:=D-Link DCS-930
	PACKAGES:=kmod-video-core kmod-video-uvc \
		kmod-sound-core kmod-usb-audio \
		kmod-usb-core kmod-usb-dwc2
endef

define Profile/DCS930/Description
	Package set for D-Link DCS-930 board
endef
$(eval $(call Profile,DCS930))


define Profile/DCS930LB1
	NAME:=D-Link DCS-930L B1
	PACKAGES:=kmod-video-core kmod-video-uvc \
		kmod-sound-core kmod-usb-audio \
		kmod-usb-core kmod-usb-ohci kmod-usb2
endef

define Profile/DCS930LB1/Description
	Package set for D-Link DCS-930L B1 board
endef
$(eval $(call Profile,DCS930LB1))


define Profile/DIR610A1
	NAME:=D-Link DIR-610 A1
	PACKAGES:=kmod-ledtrig-netdev kmod-ledtrig-timer
endef

define Profile/DIR610A1/Description
	Package set for D-Link DIR610 A1 board
endef
$(eval $(call Profile,DIR610A1))


define Profile/DIR-615-D
	NAME:=D-Link DIR-615 D
	PACKAGES:=\
		-kmod-usb-core -kmod-usb-dwc2 \
		-kmod-ledtrig-usbdev
endef

define Profile/DIR-615-D/Description
	Package set for D-Link DIR-615 D board
endef
$(eval $(call Profile,DIR-615-D))


define Profile/DIR615H1
	NAME:=D-Link DIR-615 H1
	PACKAGES:=\
		-kmod-usb-core -kmod-usb-dwc2 \
		-kmod-ledtrig-usbdev
endef

define Profile/DIR615H1/Description
	Package set for D-Link DIR-615 H1 board
endef
$(eval $(call Profile,DIR615H1))
