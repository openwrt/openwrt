#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Livebox
  NAME:=Inventel Livebox 1
  PACKAGES:=kmod-b43 wpad-mini kmod-usb-ohci
endef
define Profile/Livebox/Description
  Package set optimized for Inventel Livebox 1.
endef
$(eval $(call Profile,Livebox))
