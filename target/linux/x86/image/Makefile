# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

export PATH=$(TARGET_PATH):/sbin
BOOTOPTS=$(strip $(subst ",, $(CONFIG_X86_GRUB_BOOTOPTS)))
ROOTPART=$(strip $(subst ",, $(CONFIG_X86_GRUB_ROOTPART)))
#"))")) # fix vim's broken syntax highlighting


ifeq ($(CONFIG_X86_GRUB_IMAGES),y)
  define Image/cmdline/squashfs
    block2mtd.block2mtd=$(ROOTPART),65536,rootfs root=/dev/mtdblock0 rootfstype=squashfs init=/etc/preinit
  endef

  define Image/cmdline/jffs2-64k
    block2mtd.block2mtd=$(ROOTPART),65536,rootfs root=/dev/mtdblock0 rootfstype=jffs2 init=/etc/preinit
  endef

  define Image/cmdline/jffs2-128k
    block2mtd.block2mtd=$(ROOTPART),131072,rootfs root=/dev/mtdblock0 rootfstype=jffs2 init=/etc/preinit
  endef

  define Image/cmdline/ext2
    root=$(ROOTPART) rootfstype=ext2 init=/etc/preinit
  endef
  
  define Image/Build/grub
	# left here because the image builder doesnt need these
	$(INSTALL_DIR) $(KDIR)/root.grub/boot/grub
	$(CP) \
		$(KDIR)/*stage* \
		$(KDIR)/root.grub/boot/grub/
	$(CP) $(KDIR)/bzImage $(KDIR)/root.grub/boot/vmlinuz
	sed \
		-e 's#@CMDLINE@#$(strip $(call Image/cmdline/$(1))) $(BOOTOPTS)#g' \
		-e 's#@BAUDRATE@#$(CONFIG_X86_GRUB_BAUDRATE)#g' \
		./menu.lst > $(KDIR)/root.grub/boot/grub/menu.lst
	PADDING="$(CONFIG_X86_GRUB_IMAGES_PAD)" PATH="$(TARGET_PATH)" ./gen_image.sh $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).image $(CONFIG_X86_GRUB_KERNELPART) $(KDIR)/root.grub $(CONFIG_TARGET_ROOTFS_FSPART) $(KDIR)/root.$(1)
	$(call Image/Build/grub/$(1))
  endef
endif

ifeq ($(CONFIG_X86_GRUB_IMAGES),y)
  define Image/Prepare/grub
	# for the image builder
	$(CP) \
		$(STAGING_DIR_HOST)/usr/lib/grub/i386-pc/stage1 \
		$(STAGING_DIR_HOST)/usr/lib/grub/i386-pc/stage2 \
		$(STAGING_DIR_HOST)/usr/lib/grub/i386-pc/e2fs_stage1_5 \
		$(KDIR)/
  endef
endif

define Image/Prepare
	$(CP) $(LINUX_DIR)/arch/i386/boot/bzImage $(KDIR)/bzImage
	$(call Image/Prepare/grub)
endef
  
define Image/Build/squashfs
    $(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build/iso 
	$(CP) \
		$(STAGING_DIR_HOST)/usr/lib/grub/i386-pc/stage2_eltorito \
		$(KDIR)/root.grub/boot/grub/stage2_eltorito 
	sed -i \
		-e 's#@CMDLINE@#$(strip $(call Image/cmdline/$(1))) $(BOOTOPTS)#g' \
		-e 's#@BAUDRATE@#$(CONFIG_X86_GRUB_BAUDRATE)#g' \
		-e 's#(hd0,0)#(cd)#g' \
		 $(KDIR)/root.grub/boot/grub/menu.lst 
	$(CP) $(KDIR)/bzImage $(KDIR)/root.grub/boot/vmlinuz 
	mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table \
		-o $(KDIR)/root.iso $(KDIR)/root.grub 
endef 

define Image/BuildKernel
	$(CP) $(KDIR)/bzImage $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-vmlinuz
endef

define Image/Build
	$(call Image/Build/$(1))
	$(call Image/Build/grub,$(1))
	$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-$(1).fs
	$(CP) $(KDIR)/bzImage $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-vmlinuz
endef

$(eval $(call BuildImage))

