#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Bcm4705-wl
  NAME:=BCM4705/BCM4785, BCM43xx WiFi (wl, proprietary)
  PACKAGES:=-wpad-mini kmod-brcm-wl wlc nas kmod-tg3
endef

define Profile/Bcm4705-wl/Description
	Package set compatible with hardware using Broadcom BCM43xx cards
	using the proprietary broadcom wireless "wl" driver and tg3 Ethernet
	driver.
endef

$(eval $(call Profile,Bcm4705-wl))

