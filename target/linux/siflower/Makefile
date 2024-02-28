# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

ARCH:=riscv64
BOARD:=siflower
BOARDNAME:=Siflower RISC-V SoCs
SUBTARGETS:=sf21a6826p
FEATURES:=gpio fpu pwm pci usb cpiogz ramdisk squashfs targz nand separate_ramdisk
KERNELNAME:=Image

KERNEL_PATCHVER:=6.6

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES += kmod-leds-gpio kmod-gpio-button-hotplug fitblk

$(eval $(call BuildTarget))
