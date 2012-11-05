#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/ALL02393G
	NAME:=Allnet ALL0239-3G
	PACKAGES:=\
		kmod-usb-core kmod-usb-rt305x-dwc_otg \
		kmod-ledtrig-usbdev restorefactory
endef

define Profile/ALL02393G/Description
	Package set for Allnet ALL0239-3G
endef

$(eval $(call Profile,ALL02393G))

define Profile/ALL0256N
	NAME:=Allnet ALL0256N
	PACKAGES:=restorefactory rssileds
endef

define Profile/ALL0256N/Description
	Package set for Allnet ALL0256N
endef

$(eval $(call Profile,ALL0256N))

define Profile/ALL5002
	NAME:=Allnet ALL5002
	PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev
endef

define Profile/ALL5002/Description
	Package set for Allnet ALL5002
endef

$(eval $(call Profile,ALL5002))
