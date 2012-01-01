#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/ALFANX
	NAME:=ALFA Network N2/N5 board
	PACKAGES:=
endef

define Profile/ALFANX/Description
	Package set optimized for the ALFA Network N2/N5 boards.
endef

$(eval $(call Profile,ALFANX))
