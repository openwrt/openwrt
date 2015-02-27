#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/RTA1025W
  NAME:=Dynalink RTA1025W
  PACKAGES:=kmod-b43 wpad-mini
endef
define Profile/RTA1025W/Description
  Package set optimized for RTA1025W.
endef
$(eval $(call Profile,RTA1025W))

define Profile/RTA1320
  NAME:=Dynalink RTA1320
  PACKAGES:=
endef
define Profile/RTA1320/Description
  Package set optimized for RTA1320.
endef
$(eval $(call Profile,RTA1320))
