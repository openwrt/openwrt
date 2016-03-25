#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=i386
BOARD:=x86
BOARDNAME:=x86
FEATURES:=squashfs ext4 vdi vmdk pcmcia targz
SUBTARGETS=generic xen_domu ep80579 geode kvm_guest 64
MAINTAINER:=Felix Fietkau <nbd@openwrt.org>

DEFAULT_PACKAGES += partx-utils

KERNEL_PATCHVER:=4.4

KERNELNAME:=bzImage

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))

$(eval $(call $(if $(CONFIG_TARGET_ROOTFS_ISO),RequireCommand,Ignore),mkisofs, \
   	Please install mkisofs. \
))
