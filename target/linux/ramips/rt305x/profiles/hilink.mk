#
# Copyright (C) 2013 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/HLKRM04
	NAME:=HILINK HLK-RM04
endef

define Profile/HLKRM04/Description
	Package set for HiLink RM04 Module
endef
$(eval $(call Profile,HLKRM04))
