#
# Copyright (C) 2015 Philippe DUCHEIN <pduchein@gmail.com>
# Copyright (C) 2009 OpenWrt.org
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/DR344
	NAME:=Wallys DR344
endef

define Profile/DR344/Description
	Package set optimized for the Wallys DR344 board.
endef

$(eval $(call Profile,DR344))

