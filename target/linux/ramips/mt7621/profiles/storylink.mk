#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/SAP-G3200U3
	NAME:=STORYLiNK SAP-G3200U3
	PACKAGES:=\
		kmod-usb-core kmod-usb3 kmod-usb-hid \
		kmod-ledtrig-usbdev
endef

define Profile/SAP-G3200U3/Description
	Package set compatible with the STORYLiNK SAP-G3200U3 board.
endef
$(eval $(call Profile,SAP-G3200U3))
