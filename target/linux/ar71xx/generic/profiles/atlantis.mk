#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/A02RBW300N
	NAME:=Atlantis-Land A02-RB-W300N
	PACKAGES:=
endef

define Profile/A02RBW300N/Description
	Package set optimized for the Atlantis-Land A02-RB-W300N.
endef

$(eval $(call Profile,A02RBW300N))
