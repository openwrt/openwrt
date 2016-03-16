#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/MiwifiNano
	NAME:=Xiaomi MiWiFi Nano
	PACKAGES:=\
		kmod-usb-core kmod-usb2 kmod-usb-ohci \
		kmod-ledtrig-usbdev
endef

define Profile/Default/Description
	Image for Xiaomi MiWiFi Nano
endef
$(eval $(call Profile,MiwifiNano))
