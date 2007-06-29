#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/DSMG600RevA
  NAME:=DSM-G600 Rev A
  PACKAGES:=kmod-via-velocity -kmod-ixp4xx-npe kmod-pata-artop kmod-usb2
endef

define Profile/DSMG600RevA/Description
	Package set optimized for the DSM-G600 Rev A
endef
$(eval $(call Profile,DSMG600RevA))

