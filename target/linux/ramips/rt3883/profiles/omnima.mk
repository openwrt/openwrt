#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/OMNIEMBHPM
	NAME:=Omnima EMB-HPM board
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2
endef

define Profile/OMNIEMBHPM/Description
	Package set compatible with the Omnima EMB-HPM board.
endef
$(eval $(call Profile,OMNIEMBHPM))
