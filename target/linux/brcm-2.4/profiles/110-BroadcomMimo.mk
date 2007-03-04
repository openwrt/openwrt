#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/BroadcomMimo
  NAME:=Generic, Broadcom WiFi (MIMO)
  PACKAGES:=kmod-brcm-wl-mimo wlc nas kmod-wlcompat
endef

define Profile/BroadcomMimo/Description
	Default package set compatible with 802.11n (draft) hardware
endef
$(eval $(call Profile,BroadcomMimo))

