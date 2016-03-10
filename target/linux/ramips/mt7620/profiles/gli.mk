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

define Profile/GL-MT300N
	NAME:=GL-MT300N
	PACKAGES:=kmod-usb-core kmod-usb-dwc2 kmod-usb2 kmod-usb-ohci kmod-mt76
endef

define Profile/GL-MT300N/Description
	Support for gl-mt300n Router
endef
$(eval $(call Profile,GL-MT300N))

define Profile/GL-MT750
	NAME:=GL-MT750
	PACKAGES:=kmod-usb-core kmod-usb-dwc2 kmod-usb2 kmod-usb-ohci kmod-mt76 kmod-mt7610e
endef

define Profile/GL-MT750/Description
	Support for gl-mt750 Router
endef
$(eval $(call Profile,GL-MT750))
