#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/OpenBlocks-AX-3-4
  NAME:=Plat'Home OpenBlocks AX3
  PACKAGES:=
endef

define Profile/OpenBlocks-AX-3-4/Description
 Package set compatible with the Plat'Home OpenBlocks AX3.
endef

$(eval $(call Profile,OpenBlocks-AX-3-4))
