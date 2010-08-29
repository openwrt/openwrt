#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Sheevaplug
  NAME:=Globalscale Sheevaplug
  PACKAGES:=
endef

define Profile/Sheevaplug/Description
        Globalscale Sheevaplug Profile
endef

$(eval $(call Profile,Sheevaplug))
