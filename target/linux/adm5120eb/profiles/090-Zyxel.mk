#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Zyxel
  NAME:=Zyxel Prestige P-334/335/335-WT (default)
  PACKAGES:=kmod-acx kmod-usb-core kmod-usb-adm5120
endef

define Profile/Zyxel/Description
	Package set compatible with the Zyxel prestige P-334/335/335-WT boards
endef
$(eval $(call Profile,Zyxel))

