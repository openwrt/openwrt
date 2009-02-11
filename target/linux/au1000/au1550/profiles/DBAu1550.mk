#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/DBAu1550
 NAME:=AMD DBAu1550 Development Kit
 PACKAGES:=kmod-pcmcia kmod-pcmcia-au1000 kmod-usb-ohci kmod-usb-ehci
endef

$(eval $(call Profile,DBAu1550))
