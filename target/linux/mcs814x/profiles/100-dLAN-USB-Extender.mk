#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/dLAN_USB_Extender
  NAME:=Devolo dLAN USB Extender
  PACKAGES:=hpavcfg foot kmod-usb-serial kmod-usb-serial-ftdi \
	    wmbus-repeater
endef

define Profile/dLAN_USB_Extender/Description
	Package set specifically tuned for the Devolo dLAN USB Extender device.
endef
$(eval $(call Profile,dLAN_USB_Extender))

