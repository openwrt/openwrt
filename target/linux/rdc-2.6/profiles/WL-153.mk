#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/wl153
  NAME:=Sitecom WL-153
  PACKAGES:=kmod-rt61-pci kmod-r6040
endef
$(eval $(call Profile,wl153))

