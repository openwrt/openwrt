#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/CUBIEBOARD
  NAME:=Cubieboard
  PACKAGES:= \
	uboot-envtools
endef

$(eval $(call Profile,CUBIEBOARD))
