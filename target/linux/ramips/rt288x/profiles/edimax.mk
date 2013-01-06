#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/BR6524N
       NAME:=Edimax BR6524N
       PACKAGES:=kmod-switch-ip17xx kmod-swconfig swconfig
endef

define Profile/BR6524N/Description
       Package set for Edimax BR6524N
endef

$(eval $(call Profile,BR6524N))

