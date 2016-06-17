#
# Copyright (C) 2011-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/OM2P
	NAME:=OpenMesh OM2P/OM2Pv2/OM2P-HS/OM2P-HSv2/OM2P-HSv3/OM2P-LC
	PACKAGES:=kmod-ath9k om-watchdog
endef

define Profile/OM2P/Description
	Package set optimized for the OpenMesh OM2P/OM2Pv2/OM2P-HS/OM2P-HSv2/OM2P-HSv3/OM2P-LC.
endef

$(eval $(call Profile,OM2P))

define Profile/OM5P
	NAME:=OpenMesh OM5P/OM5P-AN
	PACKAGES:=kmod-ath9k om-watchdog
endef

define Profile/OM5P/Description
	Package set optimized for the OpenMesh OM5P/OM5P-AN.
endef

$(eval $(call Profile,OM5P))

define Profile/OM5PAC
	NAME:=OpenMesh OM5P-AC/OM5P-ACv2
	PACKAGES:=kmod-ath9k kmod-ath10k om-watchdog ath10k-firmware-qca988x
endef

define Profile/OM5PAC/Description
	Package set optimized for the OpenMesh OM5P-AC/OM5P-ACv2.
endef

$(eval $(call Profile,OM5PAC))

define Profile/MR600
        NAME:=OpenMesh MR600
        PACKAGES:=kmod-ath9k om-watchdog
endef

define Profile/MR600/Description
        Package set optimized for the OpenMesh MR600.
endef

$(eval $(call Profile,MR600))

define Profile/MR900
        NAME:=OpenMesh MR900/MR900v2
        PACKAGES:=kmod-ath9k om-watchdog
endef

define Profile/MR900/Description
        Package set optimized for the OpenMesh MR900/MR900v2.
endef

$(eval $(call Profile,MR900))

define Profile/MR1750
        NAME:=OpenMesh MR1750/MR1750v2
        PACKAGES:=kmod-ath9k kmod-ath10k ath10k-firmware-qca988x
endef

define Profile/MR1750/Description
        Package set optimized for the OpenMesh MR1750/MR1750v2.
endef

$(eval $(call Profile,MR1750))

define Profile/OPENMESH
	NAME:=OpenMesh products
	PACKAGES:=kmod-ath9k kmod-ath10k om-watchdog
endef

define Profile/OPENMESH/Description
	Build images for all OpenMesh products.
endef

$(eval $(call Profile,OPENMESH))

