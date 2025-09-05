# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2010 OpenWrt.org

include $(TOPDIR)/rules.mk

ARCH:=powerpc
BOARD:=mpc85xx
BOARDNAME:=Freescale MPC85xx
CPU_TYPE:=8548
FEATURES:=squashfs ramdisk nand
SUBTARGETS:=p1010 p1020 p2020

KERNEL_PATCHVER:=6.6
KERNEL_TESTING_PATCHVER:=6.12

KERNELNAME:=zImage

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += \
	kmod-input-core kmod-input-gpio-keys kmod-button-hotplug \
	kmod-leds-gpio swconfig kmod-ath9k wpad-basic-mbedtls kmod-usb2 \
	uboot-envtools kmod-crypto-hw-talitos

$(eval $(call BuildTarget))
