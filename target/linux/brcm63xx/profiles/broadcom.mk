#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/BCM96318REF
  NAME:=Broadcom BCM9618REF reference board
  PACKAGES:= kmod-b43 wpad-mini kmod-usb-ohci kmod-usb2 kmod-bcm63xx-udc
endef
define Profile/BCM96318REF/Description
  Package set optimized for the Broadcom BCM96318REF reference board.
endef
$(eval $(call Profile,BCM96318REF))

define Profile/BCM96318REF_P300
  NAME:=Broadcom BCM96318REF_P300 reference board
  PACKAGES:= kmod-b43 wpad-mini kmod-usb-ohci kmod-usb2 kmod-bcm63xx-udc
endef
define Profile/BCM96318REF_P300/Description
  Package set optimized for the Broadcom BCM96318REF_P300 reference board.
endef
$(eval $(call Profile,BCM96318REF_P300))

define Profile/BCM963268BU_P300
  NAME:=Broadcom BCM963268BU_P300 reference board
  PACKAGES:= kmod-usb-ohci kmod-usb2 kmod-bcm63xx-udc
endef
define Profile/BCM963268BU_P300/Description
  Package set optimized for the Broadcom BCM963268BU_P300 reference board.
endef
$(eval $(call Profile,BCM963268BU_P300))

define Profile/BCM963269BHR
  NAME:=Broadcom BCM963269BHR reference board
  PACKAGES:= kmod-usb-ohci kmod-usb2 kmod-bcm63xx-udc
endef
define Profile/BCM963269BHR/Description
  Package set optimized for the Broadcom BCM963269BHR reference board.
endef
$(eval $(call Profile,BCM963269BHR))
