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
SUBTARGETS=petitboot

LINUX_VERSION:=2.6.27.8

KERNEL_CC:=

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build bootloader images for the Sony PS3 Game Console
endef

ifeq ($(CONFIG_TARGET_ps3_petitboot),y)
# no patches required
# provide facility for specific patches
PATCH_DIR=patches-petitboot
define Kernel/Prepare
	bzcat $(DL_DIR)/$(LINUX_SOURCE) | $(TAR) -C $(KERNEL_BUILD_DIR) $(TAR_OPTIONS)
	$(call PatchDir,$(PATCH_DIR),)
endef
endif

KERNELNAME:="zImage"

$(eval $(call BuildTarget))
