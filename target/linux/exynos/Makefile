# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=exynos
BOARDNAME:=Samsung Exynos
FEATURES:=boot-part ext4 fpu legacy-sdcard rootfs-part rtc squashfs usb
SUBTARGETS:=cortexa9

KERNEL_PATCHVER:=6.18

define Target/Description
	Build firmware images for Samsung Exynos based boards (Hardkernel ODROID).
endef

include $(INCLUDE_DIR)/target.mk

KERNELNAME:=zImage dtbs

DEFAULT_PACKAGES += e2fsprogs mkf2fs

$(eval $(call BuildTarget))
