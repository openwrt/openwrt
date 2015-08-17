#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/M2M
	NAME:=Intenso Memory 2 Move
	PACKAGES:=\
		kmod-ledtrig-netdev kmod-ledtrig-timer \
		kmod-usb-core kmod-usb2 kmod-usb-storage kmod-scsi-core \
		kmod-fs-ext4 kmod-fs-vfat block-mount
endef

define Profile/M2M/Description
	Package set for Intenso Memory 2 Move USB 3.0
endef
$(eval $(call Profile,M2M))
