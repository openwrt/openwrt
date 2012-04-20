#
# Copyright (C) 20011-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/flexibity-minimal
  NAME:=Flexibity Connect (minimal)
  PACKAGES:=
endef

define Profile/flexibity-minimal/Description
	Minimal packages set for the Flexibity Connect device.
endef

$(eval $(call Profile,flexibity-minimal))

