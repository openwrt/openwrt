#
# Copyright (c) 2014 The Linux Foundation. All rights reserved.
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Default
	NAME:=Default Profile (minimum package set)
	PACKAGES:= \
		kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev \
		kmod-usb3 kmod-usb-dwc3-qcom kmod-usb-phy-qcom-dwc3
endef

define Profile/Default/Description
	Default package set compatible with most boards.
endef
$(eval $(call Profile,Default))
