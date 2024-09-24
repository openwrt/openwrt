# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2013 OpenWrt.org

include $(TOPDIR)/rules.mk

ARCH:=mips64
BOARD:=octeon
BOARDNAME:=Cavium Networks Octeon
SUBTARGETS:=cn5020 cn6120 cn6130 cn7120 cn7130
FEATURES:=squashfs ramdisk pci usb emmc

KERNEL_PATCHVER:=6.6

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += mkf2fs e2fsprogs

$(eval $(call BuildTarget))
