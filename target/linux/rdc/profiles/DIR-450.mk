#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/dir450
  NAME:=D-Link DIR-450
  PACKAGES:=kmod-madwifi kmod-pcmcia-core kmod-nozomi
endef

define Profile/dir450/description
  Packages set compatible with the D-Link DIR-450 3G router
endef

$(eval $(call Profile,dir450))
