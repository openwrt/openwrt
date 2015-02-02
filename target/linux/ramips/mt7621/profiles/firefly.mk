#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Firefly
	NAME:=Firefly Device
	PACKAGES:=\
		kmod-usb-core kmod-usb3 kmod-usb-hid kmod-sdhci-mt7620 \
		kmod-ledtrig-usbdev kmod-mt76
endef

define Profile/Firefly/Description
	Default package set compatible with most boards.
endef
$(eval $(call Profile,Firefly))
