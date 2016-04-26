#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/ZYBO
	NAME:=ZYBO development baord
endef

define Profile/ZYBO/Description
	Build firmware image for Digilent ZYBO development board.
endef

$(eval $(call Profile,ZYBO))
