#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WZRHPG30XNH
	NAME:=Buffalo WZR-HP-G30XNH
	PACKAGES:=kmod-ath9k wpad-mini kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef

define Profile/WZRHPG30XNH/Description
	Package set optimized for the Buffalo WZR-HP-G300NH and WZR-HP-G301NH
endef

$(eval $(call Profile,WZRHPG30XNH))
