#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/E1700
	NAME:=Linksys E1700
	PACKAGES:=swconfig
endef

define Profile/E1700/Description
	Package set compatible with the Linksys E1700.
endef
$(eval $(call Profile,E1700))
