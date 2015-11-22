#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/nsim_700
	NAME:=Synopsys nSIM
endef

define Profile/nsim_700/Description
	Package set compatible with hardware using Synopsys nSIM 700 boards.
endef
$(eval $(call Profile,nsim_700))
