#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/SMART-300
        NAME:=NC-LINK SMART-300
endef

define Profile/SMART-300/Description
        Package set optimized for the NC-LINK SMART-300.
endef
$(eval $(call Profile,SMART-300))
