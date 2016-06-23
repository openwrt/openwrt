#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/CARAMBOLA2
        NAME:=Carambola2 board from 8Devices
        PACKAGES:=kmod-usb-core kmod-usb2
endef

define Profile/CARAMBOLA2/Description
        Package set optimized for the 8devices Carambola2 board.
endef

$(eval $(call Profile,CARAMBOLA2))
