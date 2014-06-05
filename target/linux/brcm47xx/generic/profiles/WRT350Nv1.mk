#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WRT350Nv1
  NAME:=Linksys WRT350Nv1
  PACKAGES:=kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-tg3 kmod-ocf-ubsec-ssb
endef

define Profile/WRT350Nv1/Description
	Package set compatible with the Linksys WRT350Nv1. Contains USB support
endef
$(eval $(call Profile,WRT350Nv1))
