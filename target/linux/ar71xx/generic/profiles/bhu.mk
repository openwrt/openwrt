#
# Copyright (C) 2013 BHU Networks.
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/BXU2000N2
	NAME:=BHU BXU2000n-2
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-usb-storage
endef

define Profile/BXU2000N2/Description
	Package set optimized for the BHU BXU2000n-2.
endef

$(eval $(call Profile,BXU2000N2))
