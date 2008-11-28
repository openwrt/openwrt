#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Ralink
  NAME:=Ralink WiFi
  PACKAGES:=kmod-rt61-pci
endef
$(eval $(call Profile,Ralink))

