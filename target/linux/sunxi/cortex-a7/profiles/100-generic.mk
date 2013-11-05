#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Generic
  NAME:=Generic (default)
  PACKAGES:= \
	uboot-envtools
endef

define Profile/Generic/Description
 Package set compatible with most Allwinner A20, A31 and A31s SoC based boards.
endef

$(eval $(call Profile,Generic))
