# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/STG212
  NAME:=MitraStar STG-212
endef

define Profile/STG212/Description
 Profile with built-in ox820 STG-212 device-tree
endef

$(eval $(call Profile,STG212))
