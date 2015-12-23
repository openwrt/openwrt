#
# Copyright (C) 2015 Chris Blake (chrisrblake93@gmail.com)
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/C-55
	NAME:=AirTight Networks C-55
	PACKAGES:=kmod-ath9k
endef

define Profile/C-55/Description
	Package set optimized for the AirTight Networks C-55 AP.
endef

$(eval $(call Profile,C-55))
