#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/DV201AMR
  NAME:=Davolink DV-201AMR
  PACKAGES:=kmod-b43 wpad-mini
endef
define Profile/DV201AMR/Description
  Package set optimized for DV-201AMR.
endef
$(eval $(call Profile,DV201AMR))
