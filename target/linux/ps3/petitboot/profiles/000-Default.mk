#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Default
  NAME:=Default PS3 Petitboot profile
  PACKAGES:=-dnsmasq -iptables -opkg -ppp -ppp-mod-pppoe -kmod-ipt-nathelper \
	    -firewall -mtd -hotplug2 -swconfig \
	    kexec-tools ps3-utils petitboot
endef

define Profile/Default/Description
        Default PS3 Petitboot profile
endef
$(eval $(call Profile,Default))

