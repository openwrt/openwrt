# Copyright (c) 2026 The Linux Foundation. All rights reserved.
# Abdulkader Alrezej <alrazj.abdulkader@gmail.com>
#

include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=alpine
BOARDNAME:=Annapurna Labs Alpine
FEATURES:=squashfs nand fpu ramdisk usb pci pcie
CPU_TYPE:=cortex-a15
CPU_SUBTYPE:=neon-vfpv4
SUBTARGETS:=generic

KERNEL_PATCHVER:=6.12

KERNELNAME:=zImage Image dtbs

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += \
	kmod-leds-gpio kmod-gpio-button-hotplug \
	kmod-al-eth kmod-mdio-al-gpio \
	ethtool tcpdump \
	uboot-envtools

$(eval $(call BuildTarget))
