#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Cellvision
  NAME:=Cellvision CAS-63x/77x cameras
  PACKAGES:=kmod-rt2500 kmod-cpia2 kmod-usb-ohci kmod-usb2 kmod-usb-audio
endef

define Profile/Cellvision/Description
  Package set compatible with the Cellvision CAS devices, including Wireless variants.
endef
$(eval $(call Profile,Cellvision))
