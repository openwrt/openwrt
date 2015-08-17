#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/BSB
        NAME:=Smart Electronics Black Swift board
        PACKAGES:=kmod-usb-core kmod-usb2
endef

define Profile/BSB/Description
        Package set optimized for the Smart Electronics Black Swift board.
endef

$(eval $(call Profile,BSB))
