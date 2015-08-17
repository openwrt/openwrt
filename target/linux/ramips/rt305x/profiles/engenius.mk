#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/ESR-9753
	NAME:=EnGenius ESR-9753
	PACKAGES:=
endef

define Profile/ESR-9753/Description
	EnGenius ESR-9753 profile.
endef
$(eval $(call Profile,ESR-9753))
