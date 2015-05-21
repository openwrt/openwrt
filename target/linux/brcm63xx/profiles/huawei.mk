#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/HG520v
  NAME:=Huawei EchoLife HG520v
  PACKAGES:=kmod-b43 wpad-mini
endef
define Profile/HG520v/Description
  Package set optimized for Huawei HG520v.
endef
$(eval $(call Profile,HG520v))

define Profile/HG553
  NAME:=Huawei EchoLife HG553
  PACKAGES:=kmod-b43 wpad-mini \
	kmod-usb2 kmod-usb-ohci kmod-ledtrig-usbdev
endef
define Profile/HG553/Description
  Package set optimized for Huawei HG553.
endef
$(eval $(call Profile,HG553))

define Profile/HG556a_AB
  NAME:=Huawei EchoLife HG556a (version A/B - Atheros)
  PACKAGES:=kmod-ath9k wpad-mini \
	kmod-usb2 kmod-usb-ohci kmod-ledtrig-usbdev
endef
define Profile/HG556a_AB/Description
  Package set optimized for Huawei HG556a version A/B (Atheros).
endef
$(eval $(call Profile,HG556a_AB))

define Profile/HG556a_C
  NAME:=Huawei EchoLife HG556a (version C - Ralink)
  PACKAGES:=kmod-rt2800-pci wpad-mini \
	kmod-usb2 kmod-usb-ohci kmod-ledtrig-usbdev
endef
define Profile/HG556a_C/Description
  Package set optimized for Huawei HG556a version C (Ralink).
endef
$(eval $(call Profile,HG556a_C))

define Profile/HG655b
  NAME:=Huawei HG655b
  PACKAGES:=kmod-rt2800-pci wpad-mini \
	kmod-usb2 kmod-usb-ohci kmod-ledtrig-usbdev
endef
define Profile/HG655b/Description
  Package set optimized for Huawei HG655b, HG655d.
endef
$(eval $(call Profile,HG655b))
