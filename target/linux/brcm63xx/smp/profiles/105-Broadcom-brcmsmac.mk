#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Broadcom-brcmsmac
  NAME:=Broadcom WiFi (brcmsmac)
  PACKAGES:=kmod-brcmsmac wpad-mini
endef

define Profile/Broadcom-brcmsmac/Description
	Package set compatible with hardware using Broadcom WiFi cards
endef
$(eval $(call Profile,Broadcom-brcmsmac))

