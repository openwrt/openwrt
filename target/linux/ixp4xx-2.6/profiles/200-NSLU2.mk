#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/NSLU2
  NAME:=Linksys NSLU2
  PACKAGES:=kmod-ixp4xx-npe kmod-usb2
endef

define Profile/NSLU2/Description
	Package set optimized for the Linksys NSLU2
endef
$(eval $(call Profile,NSLU2))

