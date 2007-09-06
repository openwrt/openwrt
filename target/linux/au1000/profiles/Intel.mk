#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Intel
  NAME:=Intel IPW2200 WiFi
  PACKAGES:=kmod-net-ipw2200
endef
$(eval $(call Profile,Intel))

