#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WZRHPG300NH
	NAME:=Buffalo WZR-HP-G300NH
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef

define Profile/WZRHPG300NH/Description
	Package set optimized for the Buffalo WZR-HP-G300NH and WZR-HP-G301NH
endef

$(eval $(call Profile,WZRHPG300NH))

define Profile/WZRHPAG300H
	NAME:=Buffalo WZR-HP-AG300H
	PACKAGES:=kmod-usb-ohci kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev
endef

define Profile/WZRHPAG300H/Description
	Package set optimized for the Buffalo WZR-HP-AG300H
endef

$(eval $(call Profile,WZRHPAG300H))

define Profile/WHRG301N
	NAME:=Buffalo WHR-G301N
	PACKAGES:=
endef

define Profile/WHRG301N/Description
	Package set optimized for the Buffalo WHR-G301N.
endef

$(eval $(call Profile,WHRG301N))


define Profile/WHRHPG300N
	NAME:=Buffalo WHR-HP-G300N
	PACKAGES:=
endef

define Profile/WHRHPG300N/Description
	Package set optimized for the Buffalo WHR-HP-G300N
endef

$(eval $(call Profile,WHRHPG300N))
