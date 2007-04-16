# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=i386
BOARD:=rdc
BOARDNAME:=RDC x86
FEATURES:=squashfs

define Target/Description
        Build firmware images for RDC3211 based routers
        (e.g. Airlink101 AR525W, Linksys WRT54R)
endef

include $(INCLUDE_DIR)/kernel-build.mk

ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
    define Kernel/SetInitramfs
	rm -f $(BUILD_DIR)/root/sbin/init
	$(CP) $(GENERIC_PLATFORM_DIR)/files/init $(BUILD_DIR)/root/sbin/
	$(CP) $(PLATFORM_DIR)/image/preinit.arch $(BUILD_DIR)/root/etc/
    endef
endif

# include the profiles
-include profiles/*.mk

$(eval $(call BuildKernel))
