# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

BOARD:=riscv
BOARDNAME:=RISC-V QEMU virtual machine
SUBTARGETS:=64
INITRAMFS_EXTRA_FILES:=
FEATURES:=cpiogz ext4 ramdisk squashfs targz fpu
KERNELNAME:=Image

KERNEL_PATCHVER:=6.12

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += mkf2fs e2fsprogs fitblk

$(eval $(call BuildTarget))
