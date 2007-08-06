#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/ar525w
  NAME:=AirLink101 AR525W
  PACKAGES:=kmod-rt61-pci kmod-r6040
endef
$(eval $(call Profile,ar525w))

