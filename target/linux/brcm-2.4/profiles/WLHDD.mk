#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WLHDD
  NAME:=WL-HDD
  PACKAGES:=kmod-brcm-wl kmod-usb-core kmod-usb-ohci kmod-ide-core kmod-ide-pdc202xx wlc nas kmod-wlcompat kmod-rtc-rv5c386a
endef

define Profile/WLHDD/Description
	Package set optimized for the WL-HDD with USB and IDE support
endef
$(eval $(call Profile,WLHDD))

