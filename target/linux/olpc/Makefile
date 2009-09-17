#
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH=i386
BOARD:=olpc
BOARDNAME:=OLPC XO-1
FEATURES:=squashfs ext2

LINUX_VERSION:=2.6.27.34

include $(INCLUDE_DIR)/target.mk
DEFAULT_PACKAGES += kmod-natsemi kmod-ne2k-pci

$(eval $(call Target,generic))
$(eval $(call BuildTarget))
$(eval $(call $(if $(CONFIG_TARGET_ROOTFS_ISO),RequireCommand,Ignore),mkisofs, \
   	Please install mkisofs. \
))
