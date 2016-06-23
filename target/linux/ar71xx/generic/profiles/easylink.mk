#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/ELM150
	NAME:=EasyLink EL-M150
	PACKAGES:=kmod-usb-core kmod-usb2
endef


define Profile/ELM150/Description
	Package set optimized for the EasyLink EL-M150.
endef
$(eval $(call Profile,ELM150))


define Profile/ELMINI
	NAME:=EasyLink EL-MINI
	PACKAGES:=kmod-usb-core kmod-usb2
endef


define Profile/ELMINI/Description
	Package set optimized for the EasyLink EL-MINI.
endef
$(eval $(call Profile,ELMINI))
