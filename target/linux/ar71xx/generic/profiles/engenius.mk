#
# Copyright (C) 2009-2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
define Profile/ESR1750
        NAME:=EnGenius ESR1750
        PACKAGES:=kmod-usb-core kmod-usb2 kmod-usb-storage kmod-ath10k wpad-mini
endef

define Profile/ESR1750/Description
        Package set optimized for the EnGenius ESR1750
endef

$(eval $(call Profile,ESR1750))
