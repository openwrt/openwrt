#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/TDW8900GB
  NAME:=TP-Link TD-W8900GB
  PACKAGES:=kmod-b43 wpad-mini
endef
define Profile/TDW8900GB/Description
  Package set optimized for TD-W8900GB.
endef
$(eval $(call Profile,TDW8900GB))
