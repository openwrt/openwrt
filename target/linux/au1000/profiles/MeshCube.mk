#
# Copyright (C) 2007-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/MeshCube
  NAME:=MeshCube MTX-1
  PACKAGES:=kmod-madwifi kmod-hostap kmod-hostap-pci kmod-net-prism54 \
	kmod-usb-core kmod-usb-ohci kmod-usb-storage kmod-fs-ext3 kmod-fs-vfat \
	ldconfig ldd hostap-utils ip tc wpa-supplicant wpa-cli
endef

define Profile/MeshCube/Description
	Package set for the 4G MeshCube (MTX-1)
endef

$(eval $(call Profile,MeshCube))
