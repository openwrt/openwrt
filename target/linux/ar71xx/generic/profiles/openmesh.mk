#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/OM2P
	NAME:=OpenMesh OM2P
	PACKAGES:=kmod-ath9k om2p-watchdog
endef

define Profile/OM2P/Description
	Package set optimized for the OpenMesh OM2P.
endef

$(eval $(call Profile,OM2P))

