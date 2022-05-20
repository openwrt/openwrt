# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

ARCH:=riscv64
BOARD:=starfive
BOARDNAME:=StarFive
FEATURES:=ext4 squashfs ramdisk rootfs-part boot-part
KERNELNAME:=Image dtbs
SUBTARGETS:=jh7100
MAINTAINER:=Hauke Mehrtens <hauke@hauke-m.de>

KERNEL_PATCHVER:=5.15

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += partx-utils mkf2fs e2fsprogs

define Target/Description
	Build firmware images for the StarFive boards
endef

$(eval $(call BuildTarget))
