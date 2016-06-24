#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/SK-WB8
	NAME:=SamKnows Whitebox 8
	PACKAGES:=\
		kmod-usb-core kmod-usb3 \
		kmod-ledtrig-usbdev uboot-envtools
endef

define Profile/SK-WB8/Description
	Package set compatible with the SamKnows Whitebox 8.
endef
$(eval $(call Profile,SK-WB8))
