#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/UR-336UN
	NAME:=UPVEL UR-336UN
	PACKAGES:=\
		kmod-usb-core kmod-usb-dwc2 \
		kmod-ledtrig-usbdev
endef

define Profile/UR-336UN/Description
	Default package set compatible with URVEL router board.
endef
$(eval $(call Profile,UR-336UN))

define Profile/UR-326N4G
	NAME:=UPVEL UR-326N4G
	PACKAGES:=\
		kmod-usb-core kmod-usb-dwc2 \
		kmod-ledtrig-usbdev
endef

define Profile/UR-326N4G/Description
	Default package set compatible with URVEL router board.
endef
$(eval $(call Profile,UR-326N4G))
