# Copyright (c) 2013 The Linux Foundation. All rights reserved.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=ipq806x
BOARDNAME:=Qualcomm Atheros IPQ806X
FEATURES:=squashfs nand ubifs fpu
CPU_TYPE:=cortex-a9
CPU_SUBTYPE:=neon-vfpv4
MAINTAINER:=John Crispin <blogic@openwrt.org>

KERNEL_PATCHVER:=4.4

KERNELNAME:=zImage Image dtbs

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES += \
	kmod-leds-gpio kmod-gpio-button-hotplug swconfig \
	kmod-ata-core kmod-ata-ahci kmod-ata-ahci-platform \
	kmod-usb-core kmod-usb-ohci kmod-usb2 kmod-ledtrig-usbdev \
	kmod-usb3 kmod-usb-dwc3-qcom kmod-usb-phy-qcom-dwc3 \
	kmod-ath10k wpad-mini \
	uboot-envtools

$(eval $(call BuildTarget))
