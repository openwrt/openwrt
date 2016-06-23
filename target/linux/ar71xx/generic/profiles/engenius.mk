#
# Copyright (C) 2009-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/EAP300V2
	NAME:=EnGenius EAP300V2
	PACKAGES:=
endef

define Profile/EAP300V2/Description
	Package set optimized for the EnGenius EAP300V2.
endef

$(eval $(call Profile,EAP300V2))


define Profile/ESR900
	NAME:=EnGenius ESR900
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef

define Profile/ESR900/Description
	Package set optimized for the EnGenius ESR900
endef

$(eval $(call Profile,ESR900))


define Profile/ESR1750
        NAME:=EnGenius ESR1750
        PACKAGES:=kmod-usb-core kmod-usb2 kmod-usb-storage kmod-ath10k wpad-mini
endef

define Profile/ESR1750/Description
        Package set optimized for the EnGenius ESR1750
endef

$(eval $(call Profile,ESR1750))


define Profile/EPG5000
        NAME:=EnGenius EPG5000
        PACKAGES:=kmod-usb-core kmod-usb2 kmod-usb-storage kmod-ath10k wpad-mini
endef

define Profile/EPG5000/Description
        Package set optimized for the EnGenius EPG5000
endef

$(eval $(call Profile,EPG5000))
