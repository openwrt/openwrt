#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/P-334WT
	NAME:=ZyXEL Prestige P-334WT
	PACKAGES:=kmod-acx
endef

define Profile/P-334WT/Description
	Package set optimized for the ZyXEL Prestige P-334WT board.
endef

define Profile/P-335WT
	NAME:=ZyXEL Prestige P-335WT
	PACKAGES:=kmod-acx kmod-usb-core kmod-usb-adm5120
endef

define Profile/P-335WT/Description
	Package set optimized for the ZyXEL Prestige P-335WT board.
endef

$(eval $(call Profile,P-334WT))
$(eval $(call Profile,P-335WT))
