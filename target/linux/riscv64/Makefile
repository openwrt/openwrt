# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

ARCH:=riscv64
BOARD:=riscv64
BOARDNAME:=QEMU RISC-V Virtual Machine
FEATURES:=fpu pci pcie rtc usb cpiogz ext4 ramdisk squashfs targz

KERNEL_PATCHVER:=6.1

KERNELNAME:=Image

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += mkf2fs e2fsprogs

$(eval $(call BuildTarget))
