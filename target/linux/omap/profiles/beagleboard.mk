#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/BEAGLEBOARD
	NAME:=EBV BeagleBoard
	DEFAULT_PACKAGES += kmod-usb2 kmod-usb2-omap \
			kmod-usb-net kmod-usb-net-asix \
			kmod-usb-net-asix-ax88179 kmod-usb-net-hso \
			kmod-usb-net-kaweth kmod-usb-net-pegasus \
			kmod-usb-net-mcs7830 kmod-usb-net-smsc95xx \
			kmod-usb-net-dm9601-ether
endef

define Profile/BEAGLEBOARD/Description
	Package set for the BEAGLEBOARD and similar devices.
	Contains various USB-NET drivers for boards without Ethernet.
endef

$(eval $(call Profile,BEAGLEBOARD))
