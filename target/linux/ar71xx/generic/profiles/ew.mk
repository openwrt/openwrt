#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/EWDORIN
	NAME:=Embedded Wireless Dorin Platform
	PACKAGES:=
endef

define Profile/EWDORIN/Description
	Package set optimized for the Dorin Platform.
endef

$(eval $(call Profile,EWDORIN))

