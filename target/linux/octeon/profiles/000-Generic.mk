#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/generic
  NAME:=Generic Octeon board
  PACKAGES:=
endef

define Profile/generic/Description
	Base packages for Octeon boards.
endef
$(eval $(call Profile,generic))

