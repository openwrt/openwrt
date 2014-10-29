#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/A5-V11
	NAME:=A5-V11
	PACKAGES:=\
		kmod-usb-core kmod-usb-ohci kmod-usb2
endef

define Profile/A5-V11/Description
	Package set for A5-V11
endef

$(eval $(call Profile,A5-V11))
