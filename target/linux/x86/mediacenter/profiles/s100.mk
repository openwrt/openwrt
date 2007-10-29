#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/s100
  NAME:=T-Vision S-100
  FEATURES+=usb
  PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-usb-hid kmod-p54-pci kmod-8139too kmod-ata-core kmod-ata-piix
endef

define Profile/s100/Description
	T-Online streaming client. Contains WLAN and USB support.
endef
$(eval $(call Profile,s100))
