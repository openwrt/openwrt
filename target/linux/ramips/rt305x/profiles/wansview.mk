#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/NCS601W
	NAME:=Wansview NCS601W
        PACKAGES:=kmod-video-core kmod-video-uvc \
		kmod-usb-core kmod-usb-ohci
endef

define Profile/NCS601W/Description
	Package set for Wansview NCS601W board
endef

$(eval $(call Profile,NCS601W))
