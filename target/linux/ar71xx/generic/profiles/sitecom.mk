#
# Copyright (C) 2009-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
define Profile/WLR8100
        NAME:=Sitecom WLR-8100
        PACKAGES:=kmod-usb-core kmod-usb2 kmod-ledtrig-usbdev kmod-usb3
endef

define Profile/WLR8100/Description
        Package set optimized for the Sitecom WLR-8100
endef

$(eval $(call Profile,WLR8100))
