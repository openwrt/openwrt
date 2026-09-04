# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2026 OpenWrt.org
#

include $(TOPDIR)/rules.mk

ARCH:=aarch64
BOARD:=meson
BOARDNAME:=Amlogic Meson SoCs
FEATURES:=ext4 usb gpio fpu rootfs-part squashfs audio display usbgadget dt-overlay
SUBTARGETS:=gxbb

KERNEL_PATCHVER:=6.18

KERNELNAME:=Image dtbs

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += partx-utils e2fsprogs f2fs-tools

define Target/Description
	Build firmware images for Amlogic Meson SoC devices.
endef

$(eval $(call BuildTarget))
