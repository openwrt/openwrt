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

# for routers without USB:
# packages_FS_NET:=davfs2 libneon posixovl
packages_NOUSB:=$(packages_FS_NET)
packages_NOIPv6:=-kmod-ip6tables -kmod-ipv6 -odhcp6c -odhcpd -libip6tc

packages_NET_ETHERNET:=xl2tpd ppp-mod-pptp
packages_LUCI_GENERIC:=luci
packages_GENERIC:=kmod-ledtrig-netdev kmod-nls-utf8
packages_IPv6:=luci-proto-ipv6

packages_8M:=ntfs-3g kmod-fs-vfat zram-swap
packages_4M:=-dropbear

define Profile/KEENETIC
	NAME:=ZyXEL Keenetic
	PACKAGES:=$(packages_GENERIC) $(packages_NET_ETHERNET) $(packages_USB) $(packages_IPv6) $(packages_LUCI_GENERIC) $(packages_8M)
endef

define Profile/KEENETIC/Description
	Package set for ZyXEL Keenetic board
endef
$(eval $(call Profile,KEENETIC))

define Profile/KEENETIC_LITE_A
	NAME:=ZyXEL Keenetic Lite rev.A
	PACKAGES:=$(packages_GENERIC) $(packages_NET_ETHERNET) $(packages_NOUSB) $(packages_NOIPv6) $(packages_LUCI_GENERIC) $(packages_4M) kmod-ledtrig-heartbeat
endef

define Profile/KEENETIC_LITE_A/Description
	Package set for ZyXEL Keenetic Lite rev.A board
endef

$(eval $(call Profile,KEENETIC_LITE_A))

define Profile/NBG4104
	NAME:=ZyXEL Keenetic Lite rev.B/NBG4104
	PACKAGES:=$(packages_GENERIC) $(packages_NET_ETHERNET) $(packages_NOUSB) $(packages_IPv6) $(packages_LUCI_GENERIC)
endef

define Profile/NBG4104/Description
	Package set for ZyXEL NBG4104 board
endef

$(eval $(call Profile,NBG4104))

define Profile/KEENETIC_4G_A
	NAME:=ZyXEL Keenetic 4G rev.A
	PACKAGES:=$(packages_GENERIC) $(packages_NET_ETHERNET) $(packages_USB) $(packages_NOIPv6) $(packages_LUCI_GENERIC) $(packages_4M) kmod-ledtrig-heartbeat block-mount
endef

define Profile/KEENETIC_4G_A/Description
	Package set for ZyXEL Keenetic 4G rev.A board
endef

$(eval $(call Profile,KEENETIC_4G_A))

define Profile/NBG4114
	NAME:=ZyXEL Keenetic 4G rev.B/NBG4114
	PACKAGES:=$(packages_GENERIC) $(packages_NET_ETHERNET) $(packages_USB) $(packages_IPv6) $(packages_LUCI_GENERIC) $(packages_8M) kmod-ledtrig-heartbeat
endef

define Profile/NBG4114/Description
	Package set for ZyXEL NBG4114 board
endef

$(eval $(call Profile,NBG4114))
