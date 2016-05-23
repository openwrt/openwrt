# Copyright (c) 2015 OpenWrt.org
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=mediatek
BOARDNAME:=Mediatek Ralink ARM
FEATURES:=squashfs jffs2
CPU_TYPE:=cortex-a7
MAINTAINER:=John Crispin <john@phrozen.org>

KERNEL_PATCHVER:=4.4

KERNELNAME:=Image dtbs zImage

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES += \
	kmod-mt76 kmod-leds-gpio kmod-gpio-button-hotplug swconfig \
	wpad-mini

$(eval $(call BuildTarget))
