#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/ZSUNSDREADER
        NAME:=ZSUN WiFi SD Card reader
        PACKAGES:=kmod-usb-core kmod-usb2 kmod-usb-storage
endef

define Profile/ZSUNSDREADER/Description
        Package set optimized for the ZSUN WiFi SD Card Reader.
endef

$(eval $(call Profile,ZSUNSDREADER))