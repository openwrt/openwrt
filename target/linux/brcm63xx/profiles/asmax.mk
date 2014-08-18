#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/AR1004G
  NAME:=Asmax AR 1004G
  PACKAGES:=kmod-b43 wpad-mini
endef
define Profile/AR1004G/Description
  Package set optimized for AR 1004G.
endef
$(eval $(call Profile,AR1004G))
