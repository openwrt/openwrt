#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WRTNODE2Q
	NAME:=WRTnode2Q board
	PACKAGES:=kmod-usb-core kmod-usb2 kmod-usb-storage
endef

define Profile/WRTNODE2Q/Description
	Package set optimized for the WRTnode2Q board.
endef

$(eval $(call Profile,WRTNODE2Q))