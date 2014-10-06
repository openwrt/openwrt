#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#


define Profile/HT-TM02
	NAME:=HOOTOO HT-TM02
	PACKAGES:=\
		wpad-mini \
		kmod-ledtrig-netdev kmod-ledtrig-timer kmod-leds-gpio kmod-ledtrig-default-on \
		kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-usb-net usbutils \
		kmod-scsi-core kmod-scsi-generic kmod-fs-ext4 \
		kmod-usb-storage kmod-usb-storage-extras block-mount \
		kmod-usb-serial kmod-usb-serial-ftdi kmod-gpio-button-hotplug \
		kmod-nls-cp437 kmod-nls-iso8859-1 kmod-nls-utf8 luci luci-mod-admin-full \
		kmod-app-samba luci-theme-openwrt luci-proto-relay relayd nano \
		fstools 
endef

define Profile/HT-TM02/Description
	Package set for HOOTOO HT-TM02 board
endef

$(eval $(call Profile,HT-TM02))

