#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/GL-MT300A
	NAME:=GL-MT300A 
	PACKAGES:=kmod-usb-core kmod-usb-dwc2 kmod-usb2 kmod-usb-ohci kmod-mt76
endef

define Profile/GL-MT300A/Description
	Support for gl-mt300a Router
endef
$(eval $(call Profile,GL-MT300A))
