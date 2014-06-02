#
# Copyright (C) 2011-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/OM2P
	NAME:=OpenMesh OM2P/OM2Pv2/OM2P-HS/OM2P-HSv2/OM2P-LC
	PACKAGES:=kmod-ath9k om-watchdog
endef

define Profile/OM2P/Description
	Package set optimized for the OpenMesh OM2P/OM2Pv2/OM2P-HS/OM2P-HSv2/OM2P-LC.
endef

$(eval $(call Profile,OM2P))

define Profile/MR600
        NAME:=OpenMesh MR600
        PACKAGES:=kmod-ath9k om-watchdog
endef

define Profile/MR600/Description
        Package set optimized for the OpenMesh MR600.
endef

$(eval $(call Profile,MR600))

define Profile/OPENMESH
	NAME:=OpenMesh products
	PACKAGES:=kmod-ath9k om-watchdog
endef

define Profile/OPENMESH/Description
	Build images for all OpenMesh products.
endef

$(eval $(call Profile,OPENMESH))

