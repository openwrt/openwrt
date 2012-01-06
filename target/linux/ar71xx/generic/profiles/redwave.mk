#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/RW2458N
	NAME:=Redwave RW2458N
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-ath9k wpad-mini
endef

define Profile/RW2458N/Description
	Package set optimized for the Redwave RW2458N board.
endef

$(eval $(call Profile,RW2458N))
