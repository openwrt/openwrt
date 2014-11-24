#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/A226G
  NAME:=Pirelli A226G
  PACKAGES:=kmod-b43 wpad-mini\
	kmod-usb2 kmod-usb-ohci
endef
define Profile/A226G/Description
  Package set optimized for A226G.
endef
$(eval $(call Profile,A226G))

define Profile/A226M
  NAME:=Pirelli A226M/A226M-FWB
  PACKAGES:=kmod-usb2 kmod-usb-ohci
endef
define Profile/A226M/Description
  Package set optimized for A226M/A226M-FWB.
endef
$(eval $(call Profile,A226M))

define Profile/AGPF_S0
  NAME:=Pirelli Alice Gate VoIP 2 Plus Wi-Fi AGPF-S0
  PACKAGES:=kmod-b43 wpad-mini\
	kmod-usb2 kmod-usb-ohci
endef
define Profile/AGPF_S0/Description
  Package set optimized for AGPF-S0.
endef
$(eval $(call Profile,AGPF_S0))
