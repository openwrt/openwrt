#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

JFFS2_BLOCKSIZE=256k 512k

ifneq ($(CONFIG_TARGET_QI_LB60),)
UBIFS_OPTS = -m 4096 -e 516096 -c 4095
UBINIZE_OPTS = -m 4096 -p 512KiB
else
UBIFS_OPTS = -m 2048 -e 126KiB -c 4096
UBINIZE_OPTS = -m 2048 -p 128KiB -s 512
endif

ifneq ($(CONFIG_XBURST_UBOOT),)
  define Build/Clean
	$(MAKE) -C u-boot clean
  endef

  define Build/Compile
	$(MAKE) -C u-boot compile
  endef
endif

define Image/BuildKernel
	cp $(KDIR)/linux-$(LINUX_VERSION)/arch/mips/boot/uImage $(BIN_DIR)/openwrt-$(BOARD)-uImage.bin
endef

define Image/Build/squashfs
	$(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/openwrt-$(BOARD)-$(SUBTARGET)-root.$(1) bs=128k conv=sync
endef

$(eval $(call BuildImage))
