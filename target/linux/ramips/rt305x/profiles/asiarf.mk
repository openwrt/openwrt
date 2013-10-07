#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/AWM002EVB
	NAME:=AsiaRF AWM002-EVB
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev \
	kmod-i2c-core kmod-i2c-gpio
endef

define Profile/AWM002EVB/Description
	Package set for AsiaRF AWM002 Evaluation Board
endef

$(eval $(call Profile,AWM002EVB))
