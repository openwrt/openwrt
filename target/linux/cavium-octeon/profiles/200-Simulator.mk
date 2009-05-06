#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/simulator
  NAME:=Simulator
endef

define Profile/simulator/Description
	Package set compatible with the Cavium Octeon Simulator.
endef
$(eval $(call Profile,simulator))

