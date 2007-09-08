# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=cris
BOARD:=etrax
BOARDNAME:=Foxboard (ETRAX 100LX)
FEATURES:=squashfs jffs2
LINUX_VERSION:=2.6.19.2

include $(INCLUDE_DIR)/target.mk

KERNELNAME:="zImage"
DEFAULT_PACKAGES += foxboard-utils

define Target/Description
	Build fimware images for the FOXBOARD made by acmesystems.it
endef

define Kernel/Prepare/Fox
	bzcat $(DL_DIR)/$(LINUX_SOURCE) | tar -C $(KERNEL_BUILD_DIR) $(TAR_OPTIONS)
	if [ -d ./files ]; then $(CP) ./files/* $(LINUX_DIR)/; fi
	if [ -d ./patches/generic_2.6 ]; then $(PATCH) $(LINUX_DIR) ./patches/generic_2.6; fi
	if [ -d ./patches/cris ]; then $(PATCH) $(LINUX_DIR) ./patches/cris; fi
	ln -sf $(KERNEL_BUILD_DIR)/linux-$(LINUX_VERSION)/include/asm-cris/arch-v10 $(KERNEL_BUILD_DIR)/linux-$(LINUX_VERSION)/include/asm-cris/arch
	ln -sf $(KERNEL_BUILD_DIR)/linux-$(LINUX_VERSION)/arch/cris/arch-v10 $(KERNEL_BUILD_DIR)/linux-$(LINUX_VERSION)/arch/cris/arch
endef

define Kernel/Prepare
	$(call Kernel/Prepare/Fox)
endef

#include the profiles
-include profiles/*.mk

$(eval $(call BuildTarget))
$(eval $(call RequireCommand,/usr/local/cris/gcc-cris, \
	Please install the binary cris toolchain. \
))
