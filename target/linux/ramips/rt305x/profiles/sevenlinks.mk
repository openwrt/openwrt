#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/PX-4885
	NAME:=7Links PX-4885
	PACKAGES:=\
		kmod-usb-core kmod-usb-dwc2 kmod-usb2 kmod-usb-ohci \
		kmod-ledtrig-usbdev kmod-leds-gpio
endef

define Profile/PX-4885/Description
	Package set for 7Links PX-4885 wireless router.
endef
$(eval $(call Profile,PX-4885))
