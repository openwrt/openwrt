#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Ralink
  NAME:=Ralink RT61 Wifi
  PACKAGES:=kmod-rt61-pci hostapd-mini
endef

$(eval $(call Profile,Ralink))

