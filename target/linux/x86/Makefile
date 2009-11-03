#
# Copyright (C) 2006-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH=i386
BOARD:=x86
BOARDNAME:=x86
FEATURES:=squashfs jffs2 ext2 vdi vmdk pcmcia tgz
SUBTARGETS=generic olpc

LINUX_VERSION:=2.6.30.9

include $(INCLUDE_DIR)/target.mk

$(eval $(call BuildTarget))

$(eval $(call $(if $(CONFIG_TARGET_ROOTFS_ISO),RequireCommand,Ignore),mkisofs, \
   	Please install mkisofs. \
))
