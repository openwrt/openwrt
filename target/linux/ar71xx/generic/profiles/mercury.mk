#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#


define Profile/MAC1200R
	NAME:=MERCURY MAC1200R
	PACKAGES:=kmod-ath10k
endef

define Profile/MAC1200R/Description
	Package set optimized for the MERCURY MAC1200R.
endef
$(eval $(call Profile,MAC1200R))
