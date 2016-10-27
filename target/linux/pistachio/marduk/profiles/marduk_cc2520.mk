#
# Copyright (C) 2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/marduk_cc2520
    NAME:=Basic platform profile for Marduk with TI cc2520
    PACKAGES:=kmod-marduk-cc2520 wpan-tools
endef

define Profile/marduk_cc2520/Description
        Package set for basic platform support profile for Marduk with TI cc2520
        board
endef

$(eval $(call Profile,marduk_cc2520))
