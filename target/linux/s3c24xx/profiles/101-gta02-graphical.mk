#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/openmoko-gta02-graphical
  NAME:=Openmoko GTA-02 (graphical)
  PACKAGES:=enlightenment xglamo
endef

define Profile/openmoko-gta02-graphical/Description
	Package set with accelerated x11-environment compatible with the Openmoko GTA-02 hardware
endef
$(eval $(call Profile,openmoko-gta02-graphical))

