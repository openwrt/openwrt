#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WEIO
        NAME:=WeIO
        PACKAGES:=kmod-usb-core kmod-usb2
endef

define Profile/WEIO/Description
        Package set optimized for the WeIO board.
endef

$(eval $(call Profile,WEIO))
