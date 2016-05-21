#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Mirabox
  NAME:=Globalscale Mirabox
  PACKAGES:=
endef

define Profile/Mirabox/Description
 Package set compatible with the Globalscale Mirabox.
endef

$(eval $(call Profile,Mirabox))
