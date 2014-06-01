#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/bananaPi
	NAME:=bananaPi
	PACKAGES:=\
		uboot-sunxi-bananapi kmod-rtc-sunxi
endef

define Profile/bananaPi/Description
	Package set optimized for the bananaPi
endef

$(eval $(call Profile,bananaPi))
