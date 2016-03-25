#
# Copyright (c) 2014 The Linux Foundation. All rights reserved.
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/D7800
	NAME:=Netgear Nighthawk X4 D7800
	PACKAGES:= \
		kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev \
		kmod-usb3 kmod-usb-dwc3-qcom kmod-usb-phy-qcom-dwc3 \
		kmod-ath10k wpad-mini ath10k-firmware-qca99x0
endef

define Profile/D7800/Description
	Package set for the Netgear Nighthawk X4 D7800.
endef

define Profile/R7500
	NAME:=Netgear Nighthawk X4 R7500
	PACKAGES:= \
		kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev \
		kmod-usb3 kmod-usb-dwc3-qcom kmod-usb-phy-qcom-dwc3 \
		kmod-ath10k wpad-mini
endef

define Profile/R7500/Description
	Package set for the Netgear Nighthawk X4 R7500.
endef
$(eval $(call Profile,D7800))
$(eval $(call Profile,R7500))
