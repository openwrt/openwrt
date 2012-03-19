#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/vhdl_no_fb
  NAME:=FOXVHDL no fb
endef

define Profile/vhdl_no_fb/Description
	Setup the Foxboard for FOXVHDL support with no framebuffer
endef
$(eval $(call Profile,vhdl_no_fb))

