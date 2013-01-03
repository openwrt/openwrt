#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Broadcom-wl
  NAME:=Broadcom BCM43xx WiFi (wl, proprietary)
  PACKAGES:=-wpad-mini kmod-b44 kmod-brcm-wl wlc nas
endef

define Profile/Broadcom-wl/Description
	Package set compatible with hardware using Broadcom BCM43xx cards
	using the proprietary broadcom wireless "wl" driver and b44 Ethernet
	driver.
endef

$(eval $(call Profile,Broadcom-wl))

