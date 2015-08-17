#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/MicroWRT
	NAME:=Microduino MicroWRT
	PACKAGES:= kmod-usb-core kmod-usb2 kmod-usb-ohci
endef

define Profile/MicroWRT/Description
	Package set compatible with MicroWRT
endef
$(eval $(call Profile,MicroWRT))
