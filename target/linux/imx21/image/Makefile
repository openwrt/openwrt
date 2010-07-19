#
# Copyright (C) 2010 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

JFFS2_BLOCKSIZE=128k 16k
#JFFS2OPTS += -n -s 2048
JFFS2OPTS += -n

define Image/BuildKernel
	mkdir -p $(BIN_DIR)
	cp $(KDIR)/vmlinuz $(BIN_DIR)/$(IMG_PREFIX)-zImage
endef

define Image/Prepare
	cp $(LINUX_DIR)/arch/arm/boot/zImage $(KDIR)/vmlinuz
endef


define Image/Build/squashfs
    $(call prepare_generic_squashfs,$(BIN_DIR)/$(IMG_PREFIX)-root.$(1))
endef

$(eval $(call BuildImage))
