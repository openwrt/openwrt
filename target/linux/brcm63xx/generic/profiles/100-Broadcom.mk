#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Broadcom
  NAME:=Broadcom WiFi (default)
  PACKAGES:=kmod-b43 wpad-mini
endef

define Profile/Broadcom/Description
	Package set compatible with hardware using Broadcom WiFi cards
endef
$(eval $(call Profile,Broadcom))

