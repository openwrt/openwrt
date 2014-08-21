#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/P870HW_51a_v2
  NAME:=ZyXEL P870HW-51a v2
  PACKAGES:=kmod-b43 wpad-mini
endef
define Profile/P870HW_51a_v2/Description
  Package set optimized for P870HW-51a v2.
endef
$(eval $(call Profile,P870HW_51a_v2))
