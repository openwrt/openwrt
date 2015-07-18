#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/RE6500
	NAME:=Linksys RE6500
endef

define Profile/RE6500/Description
	Package set compatible with the Linksys RE6500 board.
endef
$(eval $(call Profile,RE6500))
