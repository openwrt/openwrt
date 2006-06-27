# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
KDIR:=$(BUILD_DIR)/linux-$(KERNEL)-$(BOARD)

ifneq ($(CONFIG_BIG_ENDIAN),y)
JFFS2OPTS     :=  --pad --little-endian --squash
SQUASHFS_OPTS :=  -le
else
JFFS2OPTS     :=  --pad --big-endian --squash
SQUASHFS_OPTS :=  -be
endif

ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
  ifeq ($(CONFIG_TARGET_ROOTFS_JFFS2),y)
    define Image/mkfs/jffs2
		rm -rf $(BUILD_DIR)/root/jffs
		
		$(STAGING_DIR)/bin/mkfs.jffs2 $(JFFS2OPTS) -e 0x10000 -o $(KDIR)/root.jffs2-64k -d $(BUILD_DIR)/root
		$(STAGING_DIR)/bin/mkfs.jffs2 $(JFFS2OPTS) -e 0x20000 -o $(KDIR)/root.jffs2-128k -d $(BUILD_DIR)/root

		# add End-of-Filesystem markers
		echo -ne '\xde\xad\xc0\xde' >> $(KDIR)/root.jffs2-64k
		echo -ne '\xde\xad\xc0\xde' >> $(KDIR)/root.jffs2-128k
	
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
		tar -zcf $(BIN_DIR)/openwrt-rootfs.tgz --owner=root --group=root -C $(BUILD_DIR)/root/ .
    endef
  endif
endif

define Image/mkfs/prepare/default
	find $(BUILD_DIR)/root -type f -not -perm +0100 | xargs chmod 0644
	find $(BUILD_DIR)/root -type f -perm +0100 | xargs chmod 0755
	find $(BUILD_DIR)/root -type d | xargs chmod 0755
	mkdir -p $(BUILD_DIR)/root/tmp
	chmod 0777 $(BUILD_DIR)/root/tmp
endef

define Image/mkfs/prepare
	$(call Image/mkfs/prepare/default)
endef

define BuildImage
compile:
	$(call Build/Compile)

install:
	$(call Image/Prepare)
	$(call Image/mkfs/prepare)
	$(call Image/BuildKernel)
	$(call Image/mkfs/jffs2)
	$(call Image/mkfs/squashfs)
	$(call Image/mkfs/tgz)
	
clean:
	$(call Build/Clean)
endef

compile-targets:
install-targets:
clean-targets:

source:
prepare:
compile: compile-targets
install: compile install-targets
clean: clean-targets
