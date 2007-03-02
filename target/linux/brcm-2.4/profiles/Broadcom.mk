#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Broadcom
  NAME:=Generic, Broadcom WiFi (default)
  PACKAGES:=kmod-brcm-wl wlc nas kmod-wlcompat
endef

define Profile/Broadcom/Description
	Default package set compatible with most BCM947xx hardware
endef
$(eval $(call Profile,Broadcom))

