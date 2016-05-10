#
# Copyright (C) 2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=socfpga
BOARDNAME:=Altera SoCFPGA
FEATURES:=display fpu gpio rtc usb usbgadget targz ext4 ubifs ramdisk
CPU_TYPE:=cortex-a9
CPU_SUBTYPE:=neon
MAINTAINER:=Marek Vasut <marex@denx.de>
DEVICE_TYPE:=other

KERNEL_PATCHVER:=4.4
KERNELNAME:=zImage dtbs

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES +=				\
	uboot-envtools				\
	ubi-utils				\
	kmod-button-hotplug			\
	kmod-ledtrig-netdev			\
	kmod-usb-core				\
	kmod-usb-dwc2				\
	kmod-usb-phy-nop			\
	kmod-usb-gadget				\
	kmod-usb-lib-composite			\
	kmod-usb-storage			\
	kmod-usb-storage-gadget			\
	dnsmasq iptables ip6tables 		\
	kmod-nf-nathelper firewall		\
	odhcpd odhcp6c

$(eval $(call BuildTarget))
