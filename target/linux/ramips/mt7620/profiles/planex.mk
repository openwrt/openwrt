#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/MZK-750DHP
	NAME:=Planex MZK-750DHP
	PACKAGES:= kmod-mt76 kmod-mt7610e
endef

define Profile/MZK-750DHP/Description
	Package set optimized for the Planex MZK-750DHP.
endef
$(eval $(call Profile,MZK-750DHP))
