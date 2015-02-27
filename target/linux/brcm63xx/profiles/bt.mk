#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/BTV2500V
  NAME:=BT Voyager V2500V
  PACKAGES:=kmod-b43 wpad-mini
endef
define Profile/BTV2500V/Description
  Package set optimized for BTV2500V.
endef
$(eval $(call Profile,BTV2500V))
