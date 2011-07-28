#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WZRHPG300NH
	NAME:=Buffalo WZR-HP-G300NH
	PACKAGES:=kmod-ath9k wpad-mini kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef

define Profile/WZRHPG300NH/Description
	Package set optimized for the Buffalo WZR-HP-G300NH and WZR-HP-G301NH
endef

$(eval $(call Profile,WZRHPG300NH))

define Profile/WZRHPAG300H
	NAME:=Buffalo WZR-HP-AG300H
	PACKAGES:=kmod-ath9k wpad-mini kmod-usb-ohci kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef

define Profile/WZRHPG30XNH/Description
	Package set optimized for the Buffalo WZR-HP-AG300H
endef

$(eval $(call Profile,WZRHPAG300H))
