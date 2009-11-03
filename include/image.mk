#
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

override TARGET_BUILD=
include $(INCLUDE_DIR)/prereq.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/host.mk

.NOTPARALLEL:
override MAKEFLAGS=
override MAKE:=$(SUBMAKE)
KDIR=$(KERNEL_BUILD_DIR)

ifneq ($(CONFIG_BIG_ENDIAN),y)
JFFS2OPTS     :=  --pad --little-endian --squash
SQUASHFS_OPTS :=  -le
else
JFFS2OPTS     :=  --pad --big-endian --squash
SQUASHFS_OPTS :=  -be
endif

ifneq ($(CONFIG_LINUX_2_4)$(CONFIG_LINUX_2_6_21)$(CONFIG_LINUX_2_6_25)$(CONFIG_LINUX_2_6_28),)
USE_SQUASHFS3 := y
endif

ifneq ($(USE_SQUASHFS3),)
MKSQUASHFS_CMD := $(STAGING_DIR_HOST)/bin/mksquashfs-lzma
else
MKSQUASHFS_CMD := $(STAGING_DIR_HOST)/bin/mksquashfs4
SQUASHFS_OPTS  := -comp lzma -processors 1
endif

JFFS2_BLOCKSIZE ?= 64k 128k

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
    define Image/mkfs/jffs2/sub
		# FIXME: removing this line will cause strange behaviour in the foreach loop below
		$(STAGING_DIR_HOST)/bin/mkfs.jffs2 $(JFFS2OPTS) -e $(patsubst %k,%KiB,$(1)) -o $(KDIR)/root.jffs2-$(1) -d $(TARGET_DIR)
		$(call add_jffs2_mark,$(KDIR)/root.jffs2-$(1))
		$(call Image/Build,jffs2-$(1))
    endef
    define Image/mkfs/jffs2
		rm -rf $(TARGET_DIR)/jffs
		$(foreach SZ,$(JFFS2_BLOCKSIZE),$(call Image/mkfs/jffs2/sub,$(SZ)))
    endef
  endif

  ifeq ($(CONFIG_TARGET_ROOTFS_SQUASHFS),y)
    define Image/mkfs/squashfs
		@mkdir -p $(TARGET_DIR)/jffs
		$(MKSQUASHFS_CMD) $(TARGET_DIR) $(KDIR)/root.squashfs -nopad -noappend -root-owned $(SQUASHFS_OPTS)
		$(call Image/Build,squashfs)
    endef
  endif

  ifeq ($(CONFIG_TARGET_ROOTFS_TGZ),y)
    define Image/mkfs/tgz
		$(TAR) -zcf $(BIN_DIR)/openwrt-$(BOARD)-rootfs.tgz --numeric-owner --owner=0 --group=0 -C $(TARGET_DIR)/ .
    endef
  endif

  ifeq ($(CONFIG_TARGET_ROOTFS_CPIOGZ),y)
    define Image/mkfs/cpiogz
		( cd $(TARGET_DIR); find . | cpio -o -H newc | gzip -9 >$(BIN_DIR)/openwrt-$(BOARD)-rootfs.cpio.gz )
    endef
  endif
else
  define Image/BuildKernel
	cp $(KDIR)/vmlinux.elf $(BIN_DIR)/openwrt-$(BOARD)-vmlinux.elf
	$(call Image/Build/Initramfs)
  endef
endif

define Image/Checksum
	( cd ${BIN_DIR} ; \
		$(FIND) -maxdepth 1 -type f \! -name 'md5sums'  -printf "%P\n" | xargs \
		md5sum --binary > md5sums \
	)
endef


ifeq ($(CONFIG_TARGET_ROOTFS_EXT2FS),y)
  E2SIZE=$(shell echo $$(($(CONFIG_TARGET_ROOTFS_FSPART)*1024)))

  define Image/mkfs/ext2
		$(STAGING_DIR_HOST)/bin/genext2fs -U -b $(E2SIZE) -N $(CONFIG_TARGET_ROOTFS_MAXINODE) -d $(TARGET_DIR)/ $(KDIR)/root.ext2
		$(call Image/Build,ext2)
  endef
endif

ifeq ($(CONFIG_TARGET_ROOTFS_ISO),y)
  define Image/mkfs/iso
		$(call Image/Build,iso)
  endef
endif


define Image/mkfs/prepare/default
	- find $(TARGET_DIR) -type f -not -perm +0100 -not -name 'ssh_host*' | $(XARGS) chmod 0644
	- find $(TARGET_DIR) -type f -perm +0100 | $(XARGS) chmod 0755
	- find $(TARGET_DIR) -type d | $(XARGS) chmod 0755
	$(INSTALL_DIR) $(TARGET_DIR)/tmp
	chmod 0777 $(TARGET_DIR)/tmp
endef

define Image/mkfs/prepare
	$(call Image/mkfs/prepare/default)
endef

define BuildImage
download:
prepare:
ifneq ($(IB),1)
  compile: compile-targets FORCE
	$(call Build/Compile)
else
  compile:
endif

ifneq ($(IB),1)
  install: compile install-targets FORCE
	$(call Image/Prepare)
	$(call Image/mkfs/prepare)
	$(call Image/BuildKernel)
	$(call Image/mkfs/jffs2)
	$(call Image/mkfs/squashfs)
	$(call Image/mkfs/tgz)
	$(call Image/mkfs/cpiogz)
	$(call Image/mkfs/ext2)
	$(call Image/mkfs/iso)
	$(call Image/Checksum)
else
  install: compile install-targets
	$(call Image/BuildKernel)
	$(call Image/mkfs/jffs2)
	$(call Image/mkfs/squashfs)
	$(call Image/mkfs/tgz)
	$(call Image/mkfs/cpiogz)
	$(call Image/mkfs/ext2)
	$(call Image/mkfs/iso)
	$(call Image/Checksum)
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


