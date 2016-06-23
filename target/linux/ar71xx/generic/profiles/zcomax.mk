#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/ZCN1523H28
	NAME:=Zcomax ZCN-1523H-2-8
	PACKAGES:=
endef

define Profile/ZCN1523H28/Description
	Package set optimized for the Zcomax ZCN-1523H-2-8 board.
endef

$(eval $(call Profile,ZCN1523H28))

define Profile/ZCN1523H516
	NAME:=Zcomax ZCN-1523H-5-16
	PACKAGES:=
endef

define Profile/ZCN1523H516/Description
	Package set optimized for the Zcomax ZCN-1523H-5-16 board.
endef

$(eval $(call Profile,ZCN1523H516))
