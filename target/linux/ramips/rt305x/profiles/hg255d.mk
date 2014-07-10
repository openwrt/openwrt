#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/HG255D
	NAME:=HuaWei HG255D
	PACKAGES:=kmod-leds-gpio wpad-mini kmod-usb-rt305x-dwc_otg
endef

define Profile/HG255D/Description
	Package set for HuaWei HG255D.
endef

$(eval $(call Profile,HG255D))
