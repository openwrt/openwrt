#
# Copyright (C) 2014 OpenWrt.org
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

define Profile/AWM003EVB
	NAME:=AsiaRF AWM003-EVB
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev \
		kmod-i2c-core kmod-i2c-gpio
endef

define Profile/AWM003EVB/Description
	Package set for AsiaRF AWM003 Evaluation Board
endef

define Profile/AWAPN2403
	NAME:=AsiaRF AWAPN2403
endef

define Profile/AWAPN2403/Description
	Package set for AsiaRF AWAPN2403 Pocket Router
endef

$(eval $(call Profile,AWM002EVB))
$(eval $(call Profile,AWM003EVB))
$(eval $(call Profile,AWAPN2403))
