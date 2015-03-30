#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/VG50
  NAME:=Inteno VG50 Multi-WAN CPE
  PACKAGES:= kmod-usb-ohci kmod-usb2
endef
define Profile/VG50/Description
  Package set optimized for the Inteno VG50 Multi-WAN CPE.
endef
$(eval $(call Profile,VG50))
