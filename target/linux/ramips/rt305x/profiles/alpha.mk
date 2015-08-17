#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/ASL26555
	NAME:=Alpha ASL26555
	PACKAGES:=\
		kmod-usb-core kmod-usb-dwc2 \
		kmod-ledtrig-usbdev
endef

define Profile/ASL26555/Description
	Package set for Alpha ASL26555
endef
$(eval $(call Profile,ASL26555))
