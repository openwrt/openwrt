# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

export PATH=$(TARGET_PATH):/sbin
ROOTPART=$(strip $(subst ",, $(CONFIG_X86_GRUB_ROOTPART)))
#")) # fix vim's broken syntax highlighting


ifeq ($(CONFIG_X86_GRUB_IMAGES),y)
  define Build/Compile
	$(MAKE) -C grub compile
  endef

  define Build/Clean
	$(MAKE) -C grub clean
  endef

  define Image/cmdline/jffs2-64k
    block2mtd.block2mtd=$(ROOTPART),65536 root=/dev/mtdblock0 rootfstype=jffs2
  endef

  define Image/cmdline/jffs2-128k
    block2mtd.block2mtd=$(ROOTPART),131072 root=/dev/mtdblock0 rootfstype=jffs2
  endef

  define Image/cmdline/ext2
    root=$(ROOTPART) rootfstype=ext2 $(CONFIG_X86_GRUB_BOOTOPTS)
  endef

  define Image/Build/grub
	mkdir -p $(KDIR)/root.grub/boot/grub
	$(CP) \
		$(STAGING_DIR)/usr/lib/grub/i386-pc/stage1 \
		$(STAGING_DIR)/usr/lib/grub/i386-pc/stage2 \
		$(STAGING_DIR)/usr/lib/grub/i386-pc/e2fs_stage1_5 \
		$(KDIR)/root.grub/boot/grub/
	$(CP) $(LINUX_DIR)/arch/i386/boot/bzImage $(KDIR)/root.grub/boot/vmlinuz
	sed \
		-e 's#@CMDLINE@#$(strip $(call Image/cmdline/$(1)))#g' \
		-e 's#@BAUDRATE@#$(CONFIG_X86_GRUB_BAUDRATE)#g' \
		./grub/menu.lst > $(KDIR)/root.grub/boot/grub/menu.lst
	PATH="$(STAGING_DIR)/usr/sbin:$(STAGING_DIR)/bin:$(PATH)" ./gen_image.sh $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).image $(CONFIG_X86_GRUB_KERNELPART) $(KDIR)/root.grub $(CONFIG_TARGET_ROOTFS_FSPART) $(KDIR)/root.$(1)
  endef
endif

define Image/Build
	$(call Image/Build/grub,$(1))
	cp $(KDIR)/root.$(1) $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).fs
	cp $(LINUX_DIR)/arch/i386/boot/bzImage $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-vmlinuz
endef

$(eval $(call BuildImage))
