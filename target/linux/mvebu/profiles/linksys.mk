#
# Copyright (C) 2013-2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Caiman
  NAME:=Linksys WRT1200AC (Caiman)
  PACKAGES:= kmod-mwlwifi wpad-mini swconfig
endef

define Profile/Caiman/Description
 Package set compatible with the Linksys WRT1200AC (Caiman).
endef

$(eval $(call Profile,Caiman))


define Profile/Cobra
  NAME:=Linksys WRT1900ACv2 (Cobra)
  PACKAGES:= kmod-mwlwifi wpad-mini swconfig
endef

define Profile/Cobra/Description
 Package set compatible with the Linksys WRT1900AC (Cobra).
endef

$(eval $(call Profile,Cobra))


define Profile/Mamba
  NAME:=Linksys WRT1900AC (Mamba)
  PACKAGES:= kmod-mwlwifi wpad-mini swconfig
endef

define Profile/Mamba/Description
 Package set compatible with the Linksys WRT1900AC (Mamba).
endef

$(eval $(call Profile,Mamba))


define Profile/Shelby
  NAME:=Linksys WRT1900ACS (Shelby)
  PACKAGES:= kmod-mwlwifi wpad-mini swconfig
endef

define Profile/Shelby/Description
 Package set compatible with the Linksys WRT1900ACS (Shelby).
endef

$(eval $(call Profile,Shelby))
