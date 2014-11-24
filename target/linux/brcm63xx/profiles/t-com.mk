#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/SPW303V
  NAME:=T-Com Speedport W 303V
  PACKAGES:=kmod-b43 wpad-mini
endef
define Profile/SPW303V/Description
  Package set optimized for SPW303V.
endef
$(eval $(call Profile,SPW303V))


define Profile/SPW500V
  NAME:=T-Com Speedport W 500V
  PACKAGES:=kmod-b43 wpad-mini
endef
define Profile/SPW500V/Description
  Package set optimized for SPW500V.
endef
$(eval $(call Profile,SPW500V))
