#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Default
  NAME:=Default Profile
  PACKAGES:=kmod-b43 wpad-mini
endef
define Profile/Default/description
  Package set compatible with most boards.
endef
$(eval $(call Profile,Default))
