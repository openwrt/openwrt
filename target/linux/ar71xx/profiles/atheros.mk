#
# Copyright (C) 2009-2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/AP81
	NAME:=Atheros AP81 reference board
	PACKAGES:=hostapd-mini kmod-ath9k kmod-usb-core kmod-usb2
endef

define Profile/AP81/Description
	Package set optimized for the Atheros AP81 reference board.
endef

$(eval $(call Profile,AP81))

define Profile/AP83
	NAME:=Atheros AP83 reference board
	PACKAGES:=hostapd-mini kmod-ath9k kmod-usb-core kmod-usb2
endef

define Profile/AP83/Description
	Package set optimized for the Atheros AP83 reference board.
endef

$(eval $(call Profile,AP83))

define Profile/PB42
	NAME:=Atheros PB42 reference board
	PACKAGES:=hostapd-mini kmod-ath9k kmod-usb-core kmod-usb-ohci kmod-usb2
endef

define Profile/PB42/Description
	Package set optimized for the Atheros PB42 reference board.
endef

$(eval $(call Profile,PB42))

define Profile/PB44
	NAME:=Atheros PB44 reference board
	PACKAGES:=hostapd-mini kmod-ath9k kmod-usb-core kmod-usb-ohci kmod-usb2
endef

define Profile/PB44/Description
	Package set optimized for the Atheros PB44 reference board.
endef

$(eval $(call Profile,PB44))
