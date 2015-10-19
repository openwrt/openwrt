#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/mt7688
	NAME:=mt7688
	PACKAGES:=\
		kmod-usb-core kmod-usb2 kmod-usb-ohci \
		kmod-ledtrig-usbdev
endef

define Profile/mt7688/Description
	Default package set compatible with most boards.
endef
$(eval $(call Profile,mt7688))

define Profile/LinkIt7688
	NAME:=LinkIt7688
	PACKAGES:=\
		kmod-usb-core kmod-usb2 kmod-usb-ohci \
		uboot-envtools kmod-ledtrig-netdev
endef

define Profile/LinkIt7688/Description
	Default package set compatible with LinkIt Smart7688 dev board.
endef
$(eval $(call Profile,LinkIt7688))
