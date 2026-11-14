# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=sanechips
BOARDNAME:=Sanechips ZX279128
FEATURES:=squashfs nand usb pci pcie ramdisk
CPU_TYPE:=cortex-a9
SUBTARGETS:=generic

KERNEL_PATCHVER:=6.18

KERNELNAME:=zImage dtbs

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += \
	kmod-leds-gpio kmod-gpio-button-hotplug \
	wpad-basic-mbedtls

$(eval $(call BuildTarget))
