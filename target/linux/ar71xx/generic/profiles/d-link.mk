#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/DIR600A1
	NAME:=D-Link DIR-600 rev. A1
	PACKAGES:=
endef

define Profile/DIR600A1/Description
	Package set optimized for the D-Link DIR-600 rev. A1.
endef

$(eval $(call Profile,DIR600A1))

define Profile/DIR601A1
	NAME:=D-Link DIR-601 rev. A1
	PACKAGES:=
endef

define Profile/DIR601A1/Description
	Package set optimized for the D-Link DIR-601 rev. A1.
endef

$(eval $(call Profile,DIR601A1))

define Profile/DIR615C1
	NAME:=D-Link DIR-615 rev. C1
	PACKAGES:=
endef

define Profile/DIR615C1/Description
	Package set optimized for the D-Link DIR-615 rev. C1.
endef

$(eval $(call Profile,DIR615C1))


define Profile/DIR615E4
	NAME:=D-Link DIR-615 rev. E4
	PACKAGES:=
endef

define Profile/DIR615E4/Description
	Package set optimized for the D-Link DIR-615 rev. E4.
endef

$(eval $(call Profile,DIR615E4))

define Profile/DIR825B1_ALL
	NAME:=D-Link DIR-825 rev. B1 - all flavours
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev
endef

define Profile/DIR825B1_ALL/Description
    Build all D-Link DIR-825 rev. B1 images
endef

$(eval $(call Profile,DIR825B1_ALL))

define Profile/DIR825B1
	NAME:=D-Link DIR-825 rev. B1
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev
endef

define Profile/DIR825B1/Description
	Package set optimized for the D-Link DIR-825 rev. B1.
	Copies the calibration data to the last erase block so that, if desired,
	an image optimized for use with OpenWrt (access to full flash) can be flashed.
endef

$(eval $(call Profile,DIR825B1))

define Profile/DIR825B1_OPENWRT
	NAME:=D-Link DIR-825 rev. B1 optimized for OpenWrt
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev
endef

define Profile/DIR825B1_OPENWRT/Description
	Package set optimized for the D-Link DIR-825 rev. B1. running OpenWrt
	Can only be flashed from OpenWrt because it requires that the calibration data
	be on the last sector of the flash (which the 'standard' version that can be flashed from
	stock will ensure).	
endef

$(eval $(call Profile,DIR825B1_OPENWRT))

define Profile/DIR825B1_TOSTOCK
	NAME:=D-Link DIR-825 rev. B1 that undoes OpenWrt optimization
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev
endef

define Profile/DIR825B1_TOSTOCK/Description
	Package set optimized for the D-Link DIR-825 rev. B1. that undoes OpenWrt optimization
	Copies the calibration data from the last erase block to the erase block normally used
	by the stock firmware for said data.
endef

$(eval $(call Profile,DIR825B1_TOSTOCK))

