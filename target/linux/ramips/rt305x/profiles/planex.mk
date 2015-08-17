#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/MZK-DP150N
	NAME:=Planex MZK-DP150N
	PACKAGES:= kmod-spi-dev
endef

define Profile/MZK-DP150N/Description
	Package set optimized for the Planex MZK-DP150N.
endef
$(eval $(call Profile,MZK-DP150N))
