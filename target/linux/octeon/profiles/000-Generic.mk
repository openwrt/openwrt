#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/generic
  NAME:=Generic Octeon board
  PACKAGES:=kmod-ath9k
endef

define Profile/generic/Description
	Base packages for Octeon boards.
endef
$(eval $(call Profile,generic))

