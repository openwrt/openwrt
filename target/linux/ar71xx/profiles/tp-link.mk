#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/TLWR941ND
	NAME:=TP-LINK TL-WR941ND
	PACKAGES:=kmod-ath9k
endef

define Profile/TLWR941ND/Description
	Package set optimized for the TP-LINK TL-WR941ND.
endef

$(eval $(call Profile,TLWR941ND))
