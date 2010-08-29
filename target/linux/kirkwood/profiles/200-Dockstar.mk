#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Dockstar
  NAME:=Seagate Dockstar
  PACKAGES:=
endef

define Profile/Dockstar/Description
        Seagate Dockstar Profile
endef

$(eval $(call Profile,Dockstar))
