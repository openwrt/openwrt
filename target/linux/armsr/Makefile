# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2017 Yousong Zhou <yszhou4tech@gmail.com>

include $(TOPDIR)/rules.mk

BOARD:=armsr
BOARDNAME:=Arm SystemReady (EFI) compliant
FEATURES:=fpu pci pcie rtc usb boot-part rootfs-part
FEATURES+=cpiogz ext4 separate_ramdisk squashfs targz vmdk

KERNEL_PATCHVER:=5.15

include $(INCLUDE_DIR)/target.mk

# blkid used for resolving PARTUUID in sysupgrade
DEFAULT_PACKAGES += mkf2fs e2fsprogs blkid

INITRAMFS_OTHER_FILES = $(PLATFORM_DIR)/other-files/init

# Overwrite global LINUX_KCONFIG_LIST to add arch/arm64/configs/defconfig before
# armv8 configs
#
# Discard global generic linux and target/board (armsr) linux configs as
# upstream configs provide all required configs
#
# Prefer not creating this condition and put the body in armv8/target.mk, but
# it looks that makefile is called before global target.mk and
# LINUX_KCONFIG_LIST is not set as expected
ifeq ($(SUBTARGET),armv8)
LINUX_KCONFIG_LIST = \
	$(LINUX_DIR)/arch/arm64/configs/defconfig \
	$(PLATFORM_SUBDIR)/config-5.15-openwrt \
	$(PLATFORM_SUBDIR)/config-5.15-upstream
endif

$(eval $(call BuildTarget))
