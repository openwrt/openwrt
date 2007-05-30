# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(INCLUDE_DIR)/prereq.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/host.mk
KDIR:=$(BUILD_DIR)/linux-$(KERNEL)-$(BOARD)

ifneq ($(CONFIG_BIG_ENDIAN),y)
JFFS2OPTS     :=  --pad --little-endian --squash
SQUASHFS_OPTS :=  -le
else
JFFS2OPTS     :=  --pad --big-endian --squash
SQUASHFS_OPTS :=  -be
endif

define add_jffs2_mark
	echo -ne '\xde\xad\xc0\xde' >> $(1)
endef

# pad to 64k and add jffs2 end-of-filesystem mark
# do this twice to make sure that this works with 128k blocksize as well
define prepare_generic_squashfs
	dd if=$(1) of=$(KDIR)/tmpfile.1 bs=64k conv=sync
	$(call add_jffs2_mark,$(KDIR)/tmpfile.1)
	dd of=$(1) if=$(KDIR)/tmpfile.1 bs=64k conv=sync
	$(call add_jffs2_mark,$(1))
endef

ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
  ifeq ($(CONFIG_TARGET_ROOTFS_JFFS2),y)
    define Image/mkfs/jffs2
		rm -rf $(BUILD_DIR)/root/jffs
		
		$(STAGING_DIR)/bin/mkfs.jffs2 $(JFFS2OPTS) -e 0x10000 -o $(KDIR)/root.jffs2-64k -d $(BUILD_DIR)/root
		$(STAGING_DIR)/bin/mkfs.jffs2 $(JFFS2OPTS) -e 0x20000 -o $(KDIR)/root.jffs2-128k -d $(BUILD_DIR)/root

		
		# add End-of-Filesystem markers
		$(call add_jffs2_mark,$(KDIR)/root.jffs2-64k)
		$(call add_jffs2_mark,$(KDIR)/root.jffs2-128k)
	
		$(call Image/Build,jffs2-64k)
		$(call Image/Build,jffs2-128k)
    endef
  endif
    
  ifeq ($(CONFIG_TARGET_ROOTFS_SQUASHFS),y)
    define Image/mkfs/squashfs
		@mkdir -p $(BUILD_DIR)/root/jffs
		$(STAGING_DIR)/bin/mksquashfs-lzma $(BUILD_DIR)/root $(KDIR)/root.squashfs -nopad -noappend -root-owned $(SQUASHFS_OPTS)
		$(call Image/Build,squashfs)
    endef
  endif
    
  ifeq ($(CONFIG_TARGET_ROOTFS_TGZ),y)
    define Image/mkfs/tgz
		tar -zcf $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-rootfs.tgz --owner=root --group=root -C $(BUILD_DIR)/root/ .
    endef
  endif
else
  define Image/BuildKernel
	cp $(KDIR)/vmlinux.elf $(BIN_DIR)/openwrt-$(BOARD)-$(KERNEL)-vmlinux.elf
	$(call Image/Build/Initramfs)
  endef
endif


ifeq ($(CONFIG_TARGET_ROOTFS_EXT2FS),y)
  E2SIZE=$(shell echo $$(($(CONFIG_TARGET_ROOTFS_FSPART)*1024)))
  
  define Image/mkfs/ext2
		$(STAGING_DIR)/bin/genext2fs -U -b $(E2SIZE) -I $(CONFIG_TARGET_ROOTFS_MAXINODE) -d $(BUILD_DIR)/root/ $(KDIR)/root.ext2
		$(call Image/Build,ext2)
  endef
endif


define Image/mkfs/prepare/default
	find $(BUILD_DIR)/root -type f -not -perm +0100 -not -name 'ssh_host*' | $(XARGS) chmod 0644
	find $(BUILD_DIR)/root -type f -perm +0100 | $(XARGS) chmod 0755
	find $(BUILD_DIR)/root -type d | $(XARGS) chmod 0755
	mkdir -p $(BUILD_DIR)/root/tmp
	chmod 0777 $(BUILD_DIR)/root/tmp
endef

define Image/mkfs/prepare
	$(call Image/mkfs/prepare/default)
endef

define BuildImage
download:
prepare:
ifneq ($(IB),1)
  compile: compile-targets
	$(call Build/Compile)
else
  compile:
endif

ifneq ($(IB),1)
  install: compile install-targets
	$(call Image/Prepare)
	$(call Image/mkfs/prepare)
	$(call Image/BuildKernel)
	$(call Image/mkfs/jffs2)
	$(call Image/mkfs/squashfs)
	$(call Image/mkfs/tgz)
	$(call Image/mkfs/ext2)
else
  install: compile install-targets
	$(call Image/BuildKernel)
	$(call Image/mkfs/jffs2)
	$(call Image/mkfs/squashfs)
	$(call Image/mkfs/tgz)
	$(call Image/mkfs/ext2)
endif
	
ifneq ($(IB),1)
  clean: clean-targets
	$(call Build/Clean)
else
  clean:
endif

compile-targets:
install-targets:
clean-targets:
endef


