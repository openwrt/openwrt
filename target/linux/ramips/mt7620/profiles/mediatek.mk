#
# Copyright (C) 2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/MT7620a
	NAME:=MT7620a EVB
endef

define Profile/Default/Description
	Default package set compatible with MT7620a eval board
endef
$(eval $(call Profile,MT7620a))
