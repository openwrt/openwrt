#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WNDR3700V5
	NAME:=Netgear WNDR3700v5
	PACKAGES:=\
		kmod-usb-core kmod-usb3
endef

define Profile/WNDR3700V5/Description
	Package set compatible with the Netgear WNDR3700v5.
endef
$(eval $(call Profile,WNDR3700V5))
