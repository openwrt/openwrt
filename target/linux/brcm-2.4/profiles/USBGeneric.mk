#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/USBGeneric
  NAME:=Generic USB
  PACKAGES:=kmod-brcm-wl kmod-usb-core kmod-usb-ohci kmod-usb2 wlc nas kmod-wlcompat
endef

define Profile/USBGeneric/Description
 Package set with for Broadcom routers with USB/USB2 (OHCI/EHCI) support.
 Includes the WRTSL54GS, WL-520gU, WL-500gP v2, any other BCM5354 routers
 with USB.
endef
$(eval $(call Profile,USBGeneric))

