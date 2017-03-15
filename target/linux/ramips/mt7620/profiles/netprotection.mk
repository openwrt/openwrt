#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/NETPROTECTION
	NAME:=Netprotection np-A105003201
	PACKAGES:=boost-thread busybox confuse dnsmasq dropbear fstools iptables-mod-nfqueue kmod-gpio-button-hotplug kmod-ipt-nathelper kmod-leds-gpio kmod-ledtrig-usbdev kmod-rt2800-soc kmod-tun kmod-usb-dwc2 kmod-usb-ohci kmod-usb-printer kmod-usb-serial-option kmod-usb-uhci kmod-usb2 libiw libnetfilter-conntrack libnetfilter-queue libprotobuf-c libstdcpp libubox luasocket luci luci-app-ddns luci-app-diag-devinfo luci-app-qos luci-i18n-portuguese-brazilian maccalc miniupnpc miniupnpd mtd odhcp6c odhcpd opkg ppp ppp-mod-pppoe protobuf squid swconfig uci usb-modeswitch usbutils wpad-mini
endef

define Profile/NETPROTECTION/Description
	Default package set for Netprotection np-A105003201
endef
$(eval $(call Profile,NETPROTECTION))
