#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Cubieboard
	NAME:=Cubieboard
	PACKAGES:=\
		uboot-sunxi-Cubieboard kmod-ata-sunxi kmod-sun4i-emac
endef

define Profile/Cubieboard/Description
	Package set optimized for the Cubieboard
endef

$(eval $(call Profile,Cubieboard))
