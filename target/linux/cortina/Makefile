# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=cortina
BOARDNAME:=Cortina CS75xx
FEATURES:=squashfs nand pci usb rtc gpio display dt fpu ramdisk
CPU_TYPE:=cortex-a9
CPU_SUBTYPE:=neon

SUBTARGETS:=cs752x

KERNEL_PATCHVER:=6.18

define Target/Description
	Build firmware for Cortina CS75xx (Goldengate G2) SoCs
endef

KERNELNAME:=zImage dtbs

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += kmod-usb3 kmod-usb-storage kmod-gpio-button-hotplug

$(eval $(call BuildTarget))
