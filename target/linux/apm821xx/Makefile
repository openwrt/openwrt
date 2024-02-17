# SPDX-License-Identifier: GPL-2.0-only

include $(TOPDIR)/rules.mk

ARCH:=powerpc
BOARD:=apm821xx
BOARDNAME:=AppliedMicro APM821xx
CPU_TYPE:=464fp
FEATURES:=fpu dt gpio ramdisk squashfs usb
SUBTARGETS:=nand sata

KERNEL_PATCHVER:=6.1

define Target/Description
	Build images for AppliedMicro APM821xx based boards.
endef

include $(INCLUDE_DIR)/target.mk

KERNELNAME:=uImage

DEFAULT_PACKAGES += \
	kmod-leds-gpio kmod-i2c-core kmod-gpio-button-hotplug uboot-envtools \
	kmod-hw-crypto-4xx

$(eval $(call BuildTarget))
