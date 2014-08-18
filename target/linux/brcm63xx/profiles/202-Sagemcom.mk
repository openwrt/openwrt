#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Sagemcom-2704V2
  NAME:=Sagemcom F@ST 2704 V2
  PACKAGES:=kmod-b43 wpad-mini \
    kmod-usb2 kmod-usb-ohci kmod-ledtrig-usbdev 
endef
define Profile/Sagemcom-2704V2/Description
  Package set optimized for Sagemcom F@ST 2704 using open-source b43 WiFi driver
endef
$(eval $(call Profile,Sagemcom-2704V2))
