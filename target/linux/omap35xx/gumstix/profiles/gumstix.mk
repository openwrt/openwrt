#
# Copyright (C) 2006 - 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/Gumstix
  NAME:=gumstix
  PACKAGES:=-iptables -ppp -ppp-mod-pppoe -kmod-ipt-nathelper \
		-firewall -mtd -hotplug2 -udevtrigger \
		ext2 usb udev kmod-bluetooth ntpclient wpa-supplicant \
		wireless-tools wpa-cli \
		bluez-utils bluez-hcidump hegw usbutils python pyserial \
		python-mini python-openssl python-sqlite3 libstdcpp
endef

define Profile/Gumstix/Description
        Gumstix www.gumstix.com
endef
$(eval $(call Profile,Gumstix))
