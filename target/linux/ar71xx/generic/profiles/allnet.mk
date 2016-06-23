#
# Copyright (C) 20012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/ALL0305
	NAME:=Allnet ALL0305
	PACKAGES:=fconfig kmod-ath5k -kmod-ath9k
endef

define Profile/ALL0305/Description
	Package set optimized for the Allnet ALL0305.
endef

$(eval $(call Profile,ALL0305))

define Profile/ALL0258N
	NAME:=Allnet ALL0258N
	PACKAGES:=uboot-envtools rssileds
endef

define Profile/ALL0258N/Description
	Package set optimized for the Allnet ALL0258N.
endef

$(eval $(call Profile,ALL0258N))

define Profile/ALL0315N
	NAME:=Allnet ALL0315N
	PACKAGES:=uboot-envtools rssileds
endef

define Profile/ALL0315N/Description
	Package set optimized for the Allnet ALL0315N.
endef

$(eval $(call Profile,ALL0315N))
