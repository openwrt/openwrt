# Copyright (c) 2015 OpenWrt.org
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=mediatek
BOARDNAME:=Mediatek Ralink ARM
FEATURES:=squashfs
CPU_TYPE:=cortex-a7
MAINTAINER:=John Crispin <blogic@openwrt.org>

KERNEL_PATCHVER:=4.1

KERNELNAME:=Image dtbs zImage

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES += \
	kmod-leds-gpio kmod-gpio-button-hotplug swconfig

$(eval $(call BuildTarget))
