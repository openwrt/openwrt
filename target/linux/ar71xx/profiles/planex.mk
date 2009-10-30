#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/MZKW04NU
	NAME:=Planex MZK-W04NU
	PACKAGES:=kmod-ath9k hostapd-mini kmod-usb-core kmod-usb-ohci kmod-usb2
endef

define Profile/MZKW04NU/Description
	Package set optimized for the Planex MZK-W04NU.
endef

$(eval $(call Profile,MZKW04NU))

define Profile/MZKW300NH
	NAME:=Planex MZK-W300NH
	PACKAGES:=kmod-ath9k hostapd-mini
endef

define Profile/MZKW300NH/Description
	Package set optimized for the Planex MZK-W300NH.
endef

$(eval $(call Profile,MZKW300NH))
