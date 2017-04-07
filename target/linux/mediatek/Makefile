# Copyright (c) 2015 OpenWrt.org
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=mediatek
BOARDNAME:=MediaTek Ralink ARM
FEATURES:=squashfs nand ubifs
CPU_TYPE:=cortex-a7
CPU_SUBTYPE:=neon-vfpv4
MAINTAINER:=John Crispin <john@phrozen.org>

KERNEL_PATCHVER:=4.9

KERNELNAME:=Image dtbs zImage

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES += \
	kmod-mt76 kmod-leds-gpio kmod-gpio-button-hotplug \
	wpad-mini

$(eval $(call BuildTarget))
