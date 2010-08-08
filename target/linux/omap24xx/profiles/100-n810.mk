#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/n810
  NAME:=Nokia n810
  PACKAGES:=
endef

define Profile/n810/Description
	Package set compatible with Nokia n810 hardware
endef
$(eval $(call Profile,n810))

