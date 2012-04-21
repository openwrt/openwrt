#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/W306R_V20
	NAME:=Tenda W306R V2.0
	PACKAGES:=
endef

define Profile/W306R_V20/Description
	Package set for Tenda W306R V2.0 board
endef

$(eval $(call Profile,W306R_V20))
