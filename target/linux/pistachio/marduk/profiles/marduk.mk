#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/marduk
    NAME:=Basic platform profile for (Ci40) Marduk with Cascoda ca8210
    PACKAGES:=kmod-ca8210 wpan-tools
endef

define Profile/marduk/Description
        Package set for basic platform support profile for (Ci40) Marduk with Cascoda ca8210
        board
endef

$(eval $(call Profile,marduk))
