#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/ar525w
	NAME:=Airlink AR525W
	PACKAGES:=kmod-rt61-pci
endef
$(eval $(call Profile,ar525w))
