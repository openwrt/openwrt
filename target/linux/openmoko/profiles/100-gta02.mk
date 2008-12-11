#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/openmoko-gta02
  NAME:=Openmoko GTA-02 (default)
endef

define Profile/openmoko-gta02/Description
	Package set compatible with the Openmoko GTA-02 hardware
endef
$(eval $(call Profile,openmoko-gta02))

