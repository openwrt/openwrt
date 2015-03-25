#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/BCM963268BU_P300
  NAME:=Broadcom BCM96328BU_P300 reference board
  PACKAGES:= kmod-usb-ohci kmod-usb2 kmod-bcm63xx-udc
endef
define Profile/BCM963268BU_P300/Description
  Package set optimized for the Broadcom BCM963268BU_P300 reference boar.
endef
$(eval $(call Profile,BCM963268BU_P300))
