#
# Copyright (C) 2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/WifiAP
  NAME:=WifiAP
  PACKAGES:=kmod-madwifi wireless-tools \
	kmod-scsi-core \
	kmod-ata-core kmod-ata-artop \
	kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-usb-storage kmod-nls-iso8859-1 kmod-nls-cp437 \
	kmod-fs-ext2 kmod-fs-ext3 kmod-fs-vfat gpioctl openslp hostapd wpa-supplicant vim \
	libnetsnmp snmp-utils snmpd olsrd olsrd-mod-nameservice olsrd-mod-dyn-gw olsrd-mod-httpinfo \
	iperf
endef

define Profile/WifiAP/Description
	Full featured image for flash based ixp4xx devices used as industrial APs
endef
$(eval $(call Profile,WifiAP))

