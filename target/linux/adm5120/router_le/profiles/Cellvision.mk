#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/CAS-771
	NAME:=Cellvision CAS-771 IP camera
	PACKAGES:=kmod-cpia2 kmod-usb-ohci kmod-usb2 kmod-usb-audio
endef

define Profile/CAS-771/Description
	Package set optimized for the Cellvision CAS-771 device.
endef

define Profile/CAS-771W
	NAME:=Cellvision CAS-771W IP camera
	PACKAGES:=kmod-cpia2 kmod-usb-ohci kmod-usb2 kmod-usb-audio kmod-rt2500
endef

define Profile/CAS-771W/Description
	Package set optimized for the Cellvision CAS-771W device.
endef

$(eval $(call Profile,CAS-771))
$(eval $(call Profile,CAS-771W))
