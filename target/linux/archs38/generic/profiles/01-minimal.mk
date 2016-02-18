#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Minimal
	NAME:=Minimal Profile (no drivers)
endef

define Profile/Minimal/Description
	Minimal package set compatible with most boards.
endef
$(eval $(call Profile,Minimal))
