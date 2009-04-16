#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Default
  NAME:=Default PS3 Petitboot profile
  PACKAGES:=-dnsmasq -iptables -ppp -ppp-mod-pppoe -kmod-ipt-nathelper \
	    -firewall \
	    kexec-tools petitboot
endef

define Profile/Default/Description
        Default PS3 Petitboot profile
endef
$(eval $(call Profile,Default))

