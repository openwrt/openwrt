# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

ARCH:=riscv64
BOARD:=spacemit
BOARDNAME:=SpacemiT RISC-V
SUBTARGETS:=k1
FEATURES:=boot-part dt emmc erofs fpu gpio pci pcie pinctrl regulator rootfs-part rtc separate_ramdisk usb

KERNEL_PATCHVER:=6.18

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += \
	kmod-leds-gpio \
	kmod-gpio-button-hotplug \
	mkf2fs \
	partx-utils

$(eval $(call BuildTarget))
