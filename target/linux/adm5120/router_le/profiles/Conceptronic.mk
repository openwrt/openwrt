#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/C54BSR4
	NAME:=Conceptronic C54BSR4 (Unofficial)
	PACKAGES:=kmod-net-ipw2200
endef

define Profile/C54BSR4/Description
	Package set optimized for the C54BSR4
endef

$(eval $(call Profile,C54BSR4))
