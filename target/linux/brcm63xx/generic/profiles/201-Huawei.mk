#
# Copyright (C) 2012-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/HW553
  NAME:=Huawei HG553
  PACKAGES:=kmod-b43 wpad-mini \
	kmod-usb2 kmod-usb-ohci kmod-ledtrig-usbdev
endef
define Profile/HW553/Description
	Package set optimized for Huawei HG553 using open-source b43 WiFi driver
endef
$(eval $(call Profile,HW553))

define Profile/HW556
  NAME:=HW556 WiFi
  PACKAGES:=kmod-rt2800-pci kmod-ath9k wpad-mini
endef

define Profile/HW556/Description
	Package set compatible with hardware using Atheros or Ralink WiFi cards (Huawei HG556a)
endef
$(eval $(call Profile,HW556))
