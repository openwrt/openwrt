#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/JCG
	NAME=JCG Routers
endef

define Profile/JCG/Description
	Builds images for JHR-N805R, JHR-N825R, and JHR-N926R.
endef
$(eval $(call Profile,JCG))

define Profile/JHR-N805R
	NAME:=JCG JHR-N805R
endef

define Profile/JHR-N805R/Description
	Package set for JCG JHR-N805R
endef
$(eval $(call Profile,JHR-N805R))

define Profile/JHR-N825R
	NAME:=JCG JHR-N825R
endef

define Profile/JHR-N825R/Description
	Package set for JCG JHR-N825R
endef
$(eval $(call Profile,JHR-N825R))

define Profile/JHR-N926R
	NAME:=JCG JHR-N926R
endef

define Profile/JHR-N926R/Description
	Package set for JCG JHR-N926R
endef
$(eval $(call Profile,JHR-N926R))

