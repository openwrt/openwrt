#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/F5D7633
  NAME:=Belkin F5D7633
  PACKAGES:=kmod-b43 wpad-mini
endef
define Profile/F5D7633/Description
  Package set optimized for F5D7633.
endef
$(eval $(call Profile,F5D7633))
