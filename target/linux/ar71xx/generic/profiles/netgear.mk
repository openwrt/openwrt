#
# Copyright (C) 2009-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WNDAP360
        NAME:=NETGEAR WNDAP360
        PACKAGES:=
endef

define Profile/WNDAP360/Description
        Package set optimized for the NETGEAR WNDAP360
endef

$(eval $(call Profile,WNDAP360))

define Profile/WNDR3700
	NAME:=NETGEAR WNDR3700/WNDR3800/WNDRMAC
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev kmod-leds-wndr3700-usb
endef

define Profile/WNDR3700/Description
	Package set optimized for the NETGEAR WNDR3700/WNDR3800/WNDRMAC
endef

$(eval $(call Profile,WNDR3700))


define Profile/WNR2000V3
	NAME:=NETGEAR WNR2000V3
endef

define Profile/WNR2000V3/Description
	Package set optimized for the NETGEAR WNR2000V3
endef

$(eval $(call Profile,WNR2000V3))


define Profile/WNR612V2
	NAME:=NETGEAR WNR612V2 / On Networks N150
endef

define Profile/WNR612V2/Description
	Package set optimized for the NETGEAR WNR612V2 / On Networks N150
endef

$(eval $(call Profile,WNR612V2))


define Profile/WNR2200
	NAME:=NETGEAR WNR2200
endef

define Profile/WNR2200/Description
	Package set optimized for the NETGEAR WNR2200
endef

$(eval $(call Profile,WNR2200))
