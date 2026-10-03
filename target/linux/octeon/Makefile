# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2013 OpenWrt.org

include $(TOPDIR)/rules.mk

ARCH:=mips64
BOARD:=octeon
BOARDNAME:=Cavium Networks Octeon
FEATURES:=squashfs ramdisk pci usb
CPU_TYPE:=octeonplus
SUBTARGETS:=generic dap2690

KERNEL_PATCHVER:=6.18

define Target/Description
	Build firmware images for Cavium Networks Octeon-based boards.
endef

include $(INCLUDE_DIR)/target.mk

ifeq ($(SUBTARGET),dap2690)
  KERNEL_MAKE_FLAGS += load-y=0xffffffff84000000
endif

DEFAULT_PACKAGES += mkf2fs e2fsprogs kmod-usb-dwc3-octeon

$(eval $(call BuildTarget))
