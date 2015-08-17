#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WT1520
	NAME:=Nexx WT1520
	PACKAGES:=kmod-usb2
endef

define Profile/WT1520/Description
	Package set for WT1520
endef
$(eval $(call Profile,WT1520))
