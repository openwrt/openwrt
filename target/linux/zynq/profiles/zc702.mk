#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/ZC702
	NAME:=ZC702 development baord
endef

define Profile/ZC702/Description
	Build firmware image for Xilinx Zynq ZC702 development board.
endef

$(eval $(call Profile,ZC702))
