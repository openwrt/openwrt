#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/AUDI
  NAME:=Linksys EA3500 (Audi)
  PACKAGES:= \
	kmod-mwl8k kmod-usb2 kmod-usb-storage \
	swconfig wpad-mini
endef

define Profile/AUDI/Description
 Package set compatible with Linksys EA3500 board.
endef

$(eval $(call Profile,AUDI))

define Profile/VIPER
  NAME:=Linksys E4200v2 / EA4500 (Viper)
  PACKAGES:= \
	kmod-mwl8k kmod-usb2 kmod-usb-storage \
	swconfig wpad-mini
endef

define Profile/VIPER/Description
 Package set compatible with Linksys E4200v2 and EA4500 boards.
endef

$(eval $(call Profile,VIPER))
