#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Firefly
	NAME:=Firefly Device
	PACKAGES:=\
		kmod-usb-core kmod-usb3 kmod-usb-hid  kmod-usb-storage mount-utils usbutils \
        kmod-sdhci-mt7620 \
		kmod-fs-ext3 kmod-fs-vfat kmod-fs-ntfs \
		kmod-nls-cp437 kmod-nls-iso8859-1 kmod-nls-utf8 \
		kmod-ledtrig-usbdev \
		kmod-ata-core \
		luci \
		kmod-mt76
endef

define Profile/Firefly/Description
	Default package set compatible with most boards.
endef
$(eval $(call Profile,Firefly))
