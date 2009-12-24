#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WRT300Nv11
  NAME:=Linksys WRT300N v1.1
  PACKAGES:=kmod-brcm-wl-mimo wlc nas kmod-wlcompat kmod-brcm-57xx
endef

define Profile/WRT300Nv11/Description
	Package set optimized for the WRT300N v1.1
endef
$(eval $(call Profile,WRT300Nv11))

