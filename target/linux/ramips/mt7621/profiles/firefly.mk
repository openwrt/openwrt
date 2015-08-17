#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/FIREWRT
	NAME:=Firefly FireWRT
	PACKAGES:=\
		kmod-usb-core kmod-usb3 kmod-usb-hid kmod-sdhci-mt7620 \
		kmod-ledtrig-usbdev
endef

define Profile/FIREWRT/Description
	Package set compatible with the Firefly FireWRT board.
endef
$(eval $(call Profile,FIREWRT))
