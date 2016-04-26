#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/ZED
	NAME:=ZedBoard development baord
endef

define Profile/ZED/Description
	Build firmware image for Avnet Digilent ZedBoard development board.
endef

$(eval $(call Profile,ZED))
