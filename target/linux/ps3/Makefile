#
# Copyright (C) 2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=powerpc
BOARD:=ps3
BOARDNAME:=Sony PS3 Game Console

LINUX_VERSION:=2.6.25.17

KERNEL_CC:=

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build bootloader images for the Sony PS3 Game Console
endef

# no generic patches required
define Kernel/Prepare
	bzcat $(DL_DIR)/$(LINUX_SOURCE) | $(TAR) -C $(KERNEL_BUILD_DIR) $(TAR_OPTIONS)
	$(call PatchDir,$(PATCH_DIR),)
endef

KERNELNAME:="zImage"

$(eval $(call BuildTarget))
