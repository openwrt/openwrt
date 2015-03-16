#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/CVG834G
  NAME:=Netgear CVG834G
  PACKAGES:=
endef
define Profile/CVG834G/Description
  Package set optimized for CVG834G.
endef
$(eval $(call Profile,CVG834G))

define Profile/DG834GTPN
  NAME:=Netgear DG834GT/PN
  PACKAGES:=kmod-ath5k wpad-mini
endef
define Profile/DG834GTPN/Description
  Package set optimized for DG834GT/PN.
endef
$(eval $(call Profile,DG834GTPN))

define Profile/DGND3700v1_3800B
  NAME:=Netgear DGND3700 v1 / DGND3800B
  PACKAGES:=kmod-b43 wpad-mini \
	kmod-usb2 kmod-usb-ohci kmod-ledtrig-usbdev
endef
define Profile/DGND3700v1_3800B/Description
  Package set optimized for DGND3700 v1 / DGND3800B.
endef
$(eval $(call Profile,DGND3700v1_3800B))
