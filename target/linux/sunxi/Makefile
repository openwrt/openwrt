# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2013-2016 OpenWrt.org

include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=sunxi
BOARDNAME:=Allwinner ARM SoCs
FEATURES:=ext4 display ramdisk rootfs-part rtc squashfs usb usbgadget
SUBTARGETS:=cortexa8 cortexa7 cortexa53 cortexa55 arm926ejs

KERNEL_PATCHVER:=6.18

KERNELNAME:=zImage dtbs

# cortexa8: A10/A13
# cortexa7: A20/A31/A80/H3/R40
# cortexa53: A64/H5/H6/H616/H618
# cortexa55: A523/A527/T527

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += uboot-envtools
DEFAULT_PACKAGES += partx-utils mkf2fs e2fsprogs

$(eval $(call BuildTarget))
