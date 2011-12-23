#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/ESR9753
	NAME:=EnGenius Profile
	PACKAGES:=
endef

define Profile/ESR9753/Description
	EnGenius profile.
endef
$(eval $(call Profile,ESR9753))
