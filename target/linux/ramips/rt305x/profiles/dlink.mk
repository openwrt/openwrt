#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/DIR615H1
	NAME:=Dlink DIR-615 H1
        PACKAGES:=\
                -kmod-usb-core -kmod-usb-dwc2 \
                -kmod-ledtrig-usbdev
endef

define Profile/DIR615H1/Description
	Package set for Dlink DIR-615 H1 board
endef

$(eval $(call Profile,DIR615H1))

define Profile/DIR-615-D
	NAME:=Dlink DIR-615 D
        PACKAGES:=\
                -kmod-usb-core -kmod-usb-dwc2 \
                -kmod-ledtrig-usbdev
endef

define Profile/DIR-615-D/Description
	Package set for Dlink DIR-615 D board
endef

$(eval $(call Profile,DIR-615-D))

define Profile/DCS930
	NAME:=Dlink DCS-930
        PACKAGES:=kmod-video-core kmod-video-uvc \
		kmod-sound-core kmod-usb-audio \
		kmod-usb-core kmod-usb-dwc2
endef

define Profile/DCS930/Description
	Package set for Dlink DCS-930 board
endef

$(eval $(call Profile,DCS930))

define Profile/DIR610A1
	NAME:=Dlink DIR-610 A1
	PACKAGES:=kmod-ledtrig-netdev kmod-ledtrig-timer
endef

define Profile/DIR610A1/Description
	Package set for Dlink DIR610 A1 board
endef

$(eval $(call Profile,DIR610A1))
