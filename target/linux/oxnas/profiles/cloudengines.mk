# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/POGOPLUG_PRO
  NAME:=Cloud Engines Pogoplug Pro
endef

define Profile/POGOPLUG_PRO/Description
 Profile with built-in ox820 Pogoplug Pro device-tree
 (board with miniPCIe slot)
endef

define Profile/POGOPLUG_V3
  NAME:=Cloud Engines Pogoplug V3
endef

define Profile/POGOPLUG_V3/Description
 Profile with built-in ox820 Pogoplug V3 device-tree
 (board without miniPCIe slot)
endef

$(eval $(call Profile,POGOPLUG_PRO))
$(eval $(call Profile,POGOPLUG_V3))
