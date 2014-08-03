#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WIZARD8800
	NAME:=EasyAcc WIZARD 8800
	PACKAGES:=\
		kmod-ledtrig-netdev kmod-ledtrig-timer kmod-leds-gpio \
		kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-usb-net usbutils \
		kmod-scsi-core kmod-scsi-generic kmod-fs-ext4 kmod-fs-msdos \
		kmod-usb-storage kmod-usb-storage-extras block-mount
endef

define Profile/WIZARD8800/Description
	Package set for EasyAcc Wi-Stor Wizard 4in1 wireless storage 8800
endef

$(eval $(call Profile,WIZARD8800))
