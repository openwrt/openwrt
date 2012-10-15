#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/sitecom
	NAME:=Devices from Sitecom (WL-153, DC-230)
	PACKAGES:=kmod-rt61-pci kmod-usb-core kmod-usb-ohci kmod-usb2
endef
$(eval $(call Profile,sitecom))
