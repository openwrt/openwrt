#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/TINY-AC
	NAME:=Dovado Tiny AC
	PACKAGES:=\
		kmod-usb-core kmod-usb2 kmod-usb-ohci
endef

define Profile/TINY-AC/Description
	Support for Dovado Tiny AC router
endef
$(eval $(call Profile,TINY-AC))
