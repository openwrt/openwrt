#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/RG100A
  NAME:=Alcatel RG100A
  PACKAGES:=kmod-b43 wpad-mini\
	kmod-usb2 kmod-usb-ohci
endef
define Profile/RG100A/Description
  Package set optimized for RG100A.
endef
$(eval $(call Profile,RG100A))
