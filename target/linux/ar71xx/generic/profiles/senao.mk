#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/CAP4200AG
	NAME:=Senao CAP4200AG
	PACKAGES:=kmod-ath9k
endef

define Profile/CAP4200AG/Description
	Package set optimized for the Senao CAP4200AG.
endef

$(eval $(call Profile,CAP4200AG))
