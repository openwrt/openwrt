#
# Copyright (C) 2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/BR6475ND
	NAME:=Edimax BR-6475nD
	PACKAGES:=swconfig
endef

define Profile/BR6475ND/Description
	Package set compatible with the Edimax BR-6475nD board.
endef
$(eval $(call Profile,BR6475ND))
