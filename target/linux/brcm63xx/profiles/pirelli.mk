#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/AGPF_S0
  NAME:=Pirelli Alice Gate VoIP 2 Plus Wi-Fi AGPF-S0
  PACKAGES:=kmod-b43 wpad-mini\
	kmod-usb2 kmod-usb-ohci
endef
define Profile/AGPF_S0/Description
  Package set optimized for AGPF-S0.
endef
$(eval $(call Profile,AGPF_S0))
