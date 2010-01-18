#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/DIR600A1
	NAME:=D-Link DIR-600 rev. A1
	PACKAGES:=kmod-ath9k hostapd-mini
endef

define Profile/DIR600A1/Description
	Package set optimized for the D-Link DIR-600 rev. A1.
endef

$(eval $(call Profile,DIR600A1))


define Profile/DIR615C1
	NAME:=D-Link DIR-615 rev. C1
	PACKAGES:=kmod-ath9k hostapd-mini
endef

define Profile/DIR615C1/Description
	Package set optimized for the D-Link DIR-615 rev. C1.
endef

$(eval $(call Profile,DIR615C1))


define Profile/DIR825B1
	NAME:=D-Link DIR-825 rev. B1
	PACKAGES:=kmod-ath9k hostapd-mini kmod-usb-core kmod-usb-ohci kmod-usb2
endef

define Profile/DIR825B1/Description
	Package set optimized for the D-Link DIR-825 rev. B1.
endef

$(eval $(call Profile,DIR825B1))
