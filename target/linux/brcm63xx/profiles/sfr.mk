#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Neufbox4
  NAME:=SFR Neufbox4
  PACKAGES:=kmod-b43 wpad-mini \
	kmod-usb2 kmod-usb-ohci
endef
define Profile/Neufbox4/Description
  Package set optimized for Neufbox4.
endef
$(eval $(call Profile,Neufbox4))

define Profile/Neufbox6
  NAME:=SFR Neufbox6
  PACKAGES:=kmod-b43 wpad-mini \
	kmod-usb2 kmod-usb-ohci
endef
define Profile/Neufbox6/Description
  Package set optimized for Neufbox6.
endef
$(eval $(call Profile,Neufbox6))
