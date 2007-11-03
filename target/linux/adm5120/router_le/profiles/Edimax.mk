#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/BR6104K
	NAME:=Edimax BR-6104K (Unofficial)
endef

define Profile/BR6104K/Description
	Package set optimized for the Edimax BR-6104K
endef

define Profile/BR6104KP
	NAME:=Edimax BR-6104KP (Unofficial)
	PACKAGES:=kmod-usb-core kmod-usb-adm5120
endef

define Profile/BR6104KP/Description
	Package set optimized for the Edimax BR-6104KP
endef

$(eval $(call Profile,BR6104K))
$(eval $(call Profile,BR6104KP))
