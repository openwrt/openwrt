#
# Copyright (C) 2013,2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

# for routers with USB:
packages_KMOD_USB:=kmod-usb-core kmod-usb-rt305x-dwc_otg kmod-usb-ohci kmod-usb2 kmod-usb-storage kmod-ledtrig-usbdev
packages_LUCI_USB:=luci-proto-3g
packages_NET_USB:=kmod-usb-acm kmod-usb-net kmod-usb-net-rndis kmod-usb-serial kmod-usb-serial-option usb-modeswitch
packages_FS_USB:=kmod-fs-ext4 kmod-nls-cp1251
packages_USB:=$(packages_KMOD_USB) $(packages_LUCI_USB) $(packages_NET_USB) $(packages_FS_USB)

packages_NET_ETHERNET:=xl2tpd ppp-mod-pptp
packages_LUCI_GENERIC:=luci
packages_GENERIC:=kmod-ledtrig-netdev kmod-nls-utf8
packages_IPv6:=luci-proto-ipv6

packages_8M:=ntfs-3g kmod-fs-vfat zram-swap

define Profile/KEENETIC
	NAME:=ZyXEL Keenetic
	PACKAGES:=$(packages_GENERIC) $(packages_NET_ETHERNET) $(packages_USB) $(packages_IPv6) $(packages_LUCI_GENERIC) $(packages_8M)
endef

define Profile/KEENETIC/Description
	Package set for ZyXEL Keenetic board
endef
$(eval $(call Profile,KEENETIC))
