#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/F5D8235V2
	NAME:=Belkin F5D8235 v2
	PACKAGES:=\
		kmod-switch-rtl8366rb kmod-swconfig swconfig \
		kmod-usb-core kmod-usb-dwc2 \
		kmod-ledtrig-usbdev
endef

define Profile/F5D8235V2/Description
	Package set for Belkin F5D8235 v2
endef

define Profile/F7C027
	NAME:=Belkin F7C027
        PACKAGES:=\
                -kmod-usb-core -kmod-usb-dwc2 \
                -kmod-ledtrig-usbdev
endef

define Profile/F7C027/Description
	Package set for Belkin F7C027 - Plug with soc_ap and a relay
endef

$(eval $(call Profile,F5D8235V2))
$(eval $(call Profile,F7C027))
