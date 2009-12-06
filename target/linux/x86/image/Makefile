# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

export PATH=$(TARGET_PATH):/sbin

ifneq ($(CONFIG_TARGET_x86_olpc),y)
BOOTOPTS=$(strip $(subst ",, $(CONFIG_X86_GRUB_BOOTOPTS)))
ROOTPART=$(strip $(subst ",, $(CONFIG_X86_GRUB_ROOTPART)))
#"))")) # fix vim's broken syntax highlighting
endif

ifeq ($(CONFIG_TARGET_x86_olpc),y)
ROOTPART=$(strip $(subst ",, $(CONFIG_OLPC_BOOTSCRIPT_ROOTPART)))
endif
#"))")) # fix vim's broken syntax highlighting

ifeq ($(CONFIG_X86_GRUB_IMAGES),y)
ifneq ($(HOST_OS),Darwin)
  define Image/cmdline/squashfs
    block2mtd.block2mtd=$(ROOTPART),65536,rootfs root=/dev/mtdblock0 rootfstype=squashfs rootwait
  endef

  define Image/cmdline/jffs2-64k
    block2mtd.block2mtd=$(ROOTPART),65536,rootfs root=/dev/mtdblock0 rootfstype=jffs2 rootwait
  endef

  define Image/cmdline/jffs2-128k
    block2mtd.block2mtd=$(ROOTPART),131072,rootfs root=/dev/mtdblock0 rootfstype=jffs2 rootwait
  endef

  define Image/cmdline/ext2
    root=$(ROOTPART) rootfstype=ext2 rootwait
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
	PADDING="$(CONFIG_X86_GRUB_IMAGES_PAD)" PATH="$(TARGET_PATH)" ./gen_image_x86.sh $(BIN_DIR)/openwrt-$(BOARD)-$(1).image $(CONFIG_X86_GRUB_KERNELPART) $(KDIR)/root.grub $(CONFIG_TARGET_ROOTFS_FSPART) $(KDIR)/root.$(1)
	$(call Image/Build/grub/$(1))
  endef

  define Image/Prepare/grub
	# for the image builder
	$(CP) \
		$(STAGING_DIR_HOST)/usr/lib/grub/i386-*/stage1 \
		$(STAGING_DIR_HOST)/usr/lib/grub/i386-*/stage2 \
		$(STAGING_DIR_HOST)/usr/lib/grub/i386-*/e2fs_stage1_5 \
		$(KDIR)/
  endef
else
  define Image/Build/grub
	PADDING="$(CONFIG_X86_GRUB_IMAGES_PAD)" PATH="$(TARGET_PATH)" NOGRUB=1 ./gen_image_x86.sh $(BIN_DIR)/openwrt-$(BOARD)-$(1).image $(CONFIG_X86_GRUB_KERNELPART) "" $(CONFIG_TARGET_ROOTFS_FSPART) $(KDIR)/root.$(1)
  endef
endif
endif

ifeq ($(CONFIG_X86_VDI_IMAGES),y)
  define Image/Build/vdi
    # left here because the image builder doesnt need these
    ifeq ($(1),ext2)
		rm $(BIN_DIR)/openwrt-$(BOARD)-ext2.vdi || true
		VBoxManage convertfromraw -format VDI $(BIN_DIR)/openwrt-$(BOARD)-ext2.image $(BIN_DIR)/openwrt-$(BOARD)-ext2.vdi
    endif
  endef
endif

ifeq ($(CONFIG_X86_VMDK_IMAGES),y)
  define Image/Build/vmdk
    # left here because the image builder doesnt need these
    ifeq ($(1),ext2)
		rm $(BIN_DIR)/openwrt-$(BOARD)-ext2.vmdk || true
		qemu-img convert -f raw $(BIN_DIR)/openwrt-$(BOARD)-ext2.image \
			-O vmdk $(BIN_DIR)/openwrt-$(BOARD)-ext2.vmdk
    endif
  endef
endif

ROOTDELAY=10

ifeq ($(CONFIG_OLPC_BOOTSCRIPT_IMAGES),y)
  define Image/cmdline/squashfs
    block2mtd.block2mtd=$(ROOTPART),65536,rootfs root=/dev/mtdblock0 rootfstype=squashfs rootdelay=$(ROOTDELAY)
  endef

  define Image/cmdline/jffs2-64k
    block2mtd.block2mtd=$(ROOTPART),65536,rootfs root=/dev/mtdblock0 rootfstype=jffs2 rootdelay=$(ROOTDELAY)
  endef

  define Image/cmdline/jffs2-128k
    block2mtd.block2mtd=$(ROOTPART),131072,rootfs root=/dev/mtdblock0 rootfstype=jffs2 rootdelay=$(ROOTDELAY)
  endef

  define Image/cmdline/ext2
    root=$(ROOTPART) rootfstype=ext2 rootwait
  endef

  define Image/Build/bootscript
	# left here because the image builder doesnt need these
	$(INSTALL_DIR) $(KDIR)/root.bootscript/boot
	$(CP) $(KDIR)/bzImage $(KDIR)/root.bootscript/boot/vmlinuz
	sed -e 's#@CMDLINE@#$(strip $(call Image/cmdline/$(1))) $(BOOTOPTS)#g' \
		./olpc.fth > $(KDIR)/root.bootscript/boot/olpc.fth
	PADDING="$(CONFIG_OLPC_BOOTSCRIPT_IMAGES_PAD)" PATH="$(TARGET_PATH)" ./gen_image_olpc.sh $(BIN_DIR)/openwrt-$(BOARD)-$(1).image $(CONFIG_OLPC_BOOTSCRIPT_KERNELPART) $(KDIR)/root.bootscript $(CONFIG_TARGET_ROOTFS_FSPART) $(KDIR)/root.$(1)
  endef
endif

define Image/Prepare
	$(CP) $(LINUX_DIR)/arch/x86/boot/bzImage $(KDIR)/bzImage
ifeq ($(CONFIG_TARGET_x86_olpc),y)
	$(call Image/Prepare/bootscript)
else
	$(call Image/Prepare/grub)
endif
endef

define Image/Build/squashfs
    $(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/Build/iso 
	$(INSTALL_DIR) $(KDIR)/root.grub/boot/grub
	$(CP) \
		$(STAGING_DIR_HOST)/usr/lib/grub/i386-openwrt/stage2_eltorito \
		$(KDIR)/root.grub/boot/grub/stage2_eltorito 
	sed \
		-e 's#@CMDLINE@#$(strip $(call Image/cmdline/$(1))) $(BOOTOPTS)#g' \
		-e 's#@BAUDRATE@#$(CONFIG_X86_GRUB_BAUDRATE)#g' \
		-e 's#(hd0,0)#(cd)#g' \
		./menu.lst > $(KDIR)/root.grub/boot/grub/menu.lst
	$(CP) $(KDIR)/bzImage $(KDIR)/root.grub/boot/vmlinuz 
	mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table \
		-o $(KDIR)/root.iso $(KDIR)/root.grub 
endef 

define Image/BuildKernel
	$(CP) $(KDIR)/bzImage $(BIN_DIR)/openwrt-$(BOARD)-vmlinuz
endef

define Image/Build
	$(call Image/Build/$(1))
	$(call Image/Build/bootscript,$(1))
ifneq ($(1),iso)
	$(call Image/Build/grub,$(1))
	$(call Image/Build/vdi,$(1))
	$(call Image/Build/vmdk,$(1))
endif
	$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/openwrt-$(BOARD)-$(1).fs
	$(CP) $(KDIR)/bzImage $(BIN_DIR)/openwrt-$(BOARD)-vmlinuz
endef

$(eval $(call BuildImage))

ifeq ($(CONFIG_X86_VDI_IMAGES),y)
  $(eval $(call RequireCommand,VBoxManage, \
	You need VBoxManage to generate VirtualBox images. \
  ))
endif

ifeq ($(CONFIG_X86_VMDK_IMAGES),y)
   $(eval $(call RequireCommand,qemu-img, \
	You need qemu-img to generate VMware images. \
   ))
endif
