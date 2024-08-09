# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2013-2014 OpenWrt.org

include $(TOPDIR)/rules.mk

BOARD:=imx
BOARDNAME:=NXP i.MX
FEATURES:=audio display fpu gpio pcie rtc usb usbgadget squashfs targz nand ubifs boot-part rootfs-part
SUBTARGETS:=cortexa7 cortexa9 cortexa53

KERNEL_PATCHVER:=6.6

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += uboot-envtools mkf2fs e2fsprogs blkid

$(eval $(call BuildTarget))
