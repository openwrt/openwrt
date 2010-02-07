#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/default
  NAME:=Normal (default)
endef

define Profile/default/Description
	Normal Foxboard setup
endef
$(eval $(call Profile,default))

