#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/DIR600A1
	NAME:=D-Link DIR-600 rev. A1
	PACKAGES:=
endef

define Profile/DIR600A1/Description
	Package set optimized for the D-Link DIR-600 rev. A1.
endef

$(eval $(call Profile,DIR600A1))

define Profile/DIR601A1
	NAME:=D-Link DIR-601 rev. A1
	PACKAGES:=
endef

define Profile/DIR601A1/Description
	Package set optimized for the D-Link DIR-601 rev. A1.
endef

$(eval $(call Profile,DIR601A1))

define Profile/DIR615C1
	NAME:=D-Link DIR-615 rev. C1
	PACKAGES:=
endef

define Profile/DIR615C1/Description
	Package set optimized for the D-Link DIR-615 rev. C1.
endef

$(eval $(call Profile,DIR615C1))


define Profile/DIR615E4
	NAME:=D-Link DIR-615 rev. E4
	PACKAGES:=
endef

define Profile/DIR615E4/Description
	Package set optimized for the D-Link DIR-615 rev. E4.
endef

$(eval $(call Profile,DIR615E4))


define Profile/DIR825B1
	NAME:=D-Link DIR-825 rev. B1
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev
endef

define Profile/DIR825B1/Description
	Package set optimized for the D-Link DIR-825 rev. B1.
endef

$(eval $(call Profile,DIR825B1))

define Profile/DIR825C1
	NAME:=D-Link DIR-825 rev. C1
	PACKAGES:=kmod-usb-core mod-usb2 kmod-ledtrig-usbdev
endef

define Profile/DIR825C1/Description
	Package set optimized for the D-Link DIR-825 rev. C1.
endef

$(eval $(call Profile,DIR825C1))
