# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2017 Yousong Zhou <yszhou4tech@gmail.com>

include $(TOPDIR)/rules.mk

BOARD:=arm64
BOARDNAME:=ARM64 Standard / Boot Base Systems
FEATURES:=fpu pci rtc usb boot-part rootfs-part
FEATURES+=cpiogz ext4 ramdisk targz vmdk

SUBTARGETS:=efi

KERNEL_PATCHVER:=5.10

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += mkf2fs e2fsprogs partx-utils

$(eval $(call BuildTarget))
