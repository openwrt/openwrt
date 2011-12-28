#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/F5D8235V1
       NAME:=Belkin F5D8235 V1
       PACKAGES:=kmod-switch-rtl8366s kmod-swconfig swconfig
endef

define Profile/F5D8235_V1/Description
       Package set for Belkin F5D8235 V1
endef

$(eval $(call Profile,F5D8235V1))

