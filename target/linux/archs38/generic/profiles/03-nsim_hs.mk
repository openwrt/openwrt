#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/nsim_hs
	NAME:=Synopsys nSIM
endef

define Profile/nsim_hs/Description
	Package set compatible with hardware using Synopsys nSIM HS boards.
endef
$(eval $(call Profile,nsim_hs))
