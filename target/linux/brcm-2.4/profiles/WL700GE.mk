#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WL700GE
  NAME:=WL-700gE
  PACKAGES:=-ppp -ppp-mod-pppoe -kmod-ipt-nathelper -iptables -dnsmasq -bridge kmod-ide-core kmod-ide-aec62xx kmod-fs-ext3 fdisk e2fsprogs
endef

define Profile/WL700GE/Description
	Minimal package set optimized for booting the WL-700gE from flash with a writable filesystem
	and the utilities to create a new filesystem on the HDD
endef
$(eval $(call Profile,WL700GE))


