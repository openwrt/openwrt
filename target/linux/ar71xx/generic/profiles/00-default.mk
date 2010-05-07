#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Default
	NAME:=Default Profile (all drivers)
	PACKAGES:=kmod-ath9k kmod-madwifi kmod-usb-core kmod-usb-ohci kmod-usb2 swconfig vsc7385-ucode-ap83 vsc7385-ucode-pb44 vsc7395-ucode-ap83 vsc7395-ucode-pb44 wpad-mini
endef

define Profile/Default/Description
	Default package set compatible with most boards.
endef
$(eval $(call Profile,Default))
