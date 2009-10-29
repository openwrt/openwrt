#
# Copyright (C) 2007-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

export PATH=$(TARGET_PATH):/sbin
ROOTPART=$(strip $(subst ",, $(CONFIG_OLPC_BOOTSCRIPT_ROOTPART)))
#"))")) # fix vim's broken syntax highlighting

ROOTDELAY=5

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
	PADDING="$(CONFIG_OLPC_BOOTSCRIPT_IMAGES_PAD)" PATH="$(TARGET_PATH)" ./gen_image.sh $(BIN_DIR)/openwrt-$(BOARD)-$(1).image $(CONFIG_OLPC_BOOTSCRIPT_KERNELPART) $(KDIR)/root.bootscript $(CONFIG_TARGET_ROOTFS_FSPART) $(KDIR)/root.$(1)
  endef
endif

define Image/Prepare
	cp $(LINUX_DIR)/arch/x86/boot/bzImage $(KDIR)/bzImage
	$(call Image/Prepare/bootscript)
endef

define Image/Build/squashfs
    $(call prepare_generic_squashfs,$(KDIR)/root.squashfs)
endef

define Image/BuildKernel
	$(CP) $(KDIR)/bzImage $(BIN_DIR)/openwrt-$(BOARD)-vmlinuz
endef

define Image/Build
	$(call Image/Build/$(1))
	$(call Image/Build/bootscript,$(1))
	$(CP) $(KDIR)/root.$(1) $(BIN_DIR)/openwrt-$(BOARD)-$(1).fs
	$(CP) $(KDIR)/bzImage $(BIN_DIR)/openwrt-$(BOARD)-vmlinuz
endef

$(eval $(call BuildImage))

