#
# Copyright (C) 2006-2010 OpenWrt.org
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

IMG_PREFIX:=openwrt-$(BOARD)$(if $(SUBTARGET),-$(SUBTARGET))

ifneq ($(CONFIG_BIG_ENDIAN),)
  JFFS2OPTS     :=  --pad --big-endian --squash-uids -v
else
  JFFS2OPTS     :=  --pad --little-endian --squash-uids -v
endif

ifeq ($(CONFIG_JFFS2_RTIME),y)
  JFFS2OPTS += -X rtime
endif
ifeq ($(CONFIG_JFFS2_ZLIB),y) 
  JFFS2OPTS += -X zlib
endif
ifeq ($(CONFIG_JFFS2_LZMA),y)
  JFFS2OPTS += -X lzma --compression-mode=size
endif
ifneq ($(CONFIG_JFFS2_RTIME),y)
  JFFS2OPTS +=  -x rtime
endif
ifneq ($(CONFIG_JFFS2_ZLIB),y)
  JFFS2OPTS += -x zlib
endif
ifneq ($(CONFIG_JFFS2_LZMA),y)
  JFFS2OPTS += -x lzma
endif

SQUASHFS_BLOCKSIZE := 256k
SQUASHFSOPT := -b $(SQUASHFS_BLOCKSIZE)
SQUASHFSCOMP := gzip
LZMA_XZ_OPTIONS := -Xpreset 9 -Xe -Xlc 0 -Xlp 2 -Xpb 2
ifeq ($(CONFIG_SQUASHFS_LZMA),y)
  SQUASHFSCOMP := lzma $(LZMA_XZ_OPTIONS)
endif
ifeq ($(CONFIG_SQUASHFS_XZ),y)
  SQUASHFSCOMP := xz $(LZMA_XZ_OPTIONS)
endif

JFFS2_BLOCKSIZE ?= 64k 128k

define add_jffs2_mark
	echo -ne '\xde\xad\xc0\xde' >> $(1)
endef

# pad to 4k, 8k, 64k, 128k 256k and add jffs2 end-of-filesystem mark
define prepare_generic_squashfs
	$(STAGING_DIR_HOST)/bin/padjffs2 $(1) 4 8 64 128 256
endef


ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)

  define Image/BuildKernel
		cp $(KDIR)/vmlinux.elf $(BIN_DIR)/$(IMG_PREFIX)-vmlinux.elf
		$(call Image/Build/Initramfs)
  endef

else

  ifneq ($(CONFIG_TARGET_ROOTFS_JFFS2),)
    define Image/mkfs/jffs2/sub
		# FIXME: removing this line will cause strange behaviour in the foreach loop below
		$(STAGING_DIR_HOST)/bin/mkfs.jffs2 $(JFFS2OPTS) -e $(patsubst %k,%KiB,$(1)) -o $(KDIR)/root.jffs2-$(1) -d $(TARGET_DIR) -v 2>&1 1>/dev/null | awk '/^.+$$$$/'
		$(call add_jffs2_mark,$(KDIR)/root.jffs2-$(1))
		$(call Image/Build,jffs2-$(1))
    endef
    define Image/mkfs/jffs2
		$(foreach SZ,$(JFFS2_BLOCKSIZE),$(call Image/mkfs/jffs2/sub,$(SZ)))
    endef
  endif

  ifneq ($(CONFIG_TARGET_ROOTFS_SQUASHFS),)
    define Image/mkfs/squashfs
		@mkdir -p $(TARGET_DIR)/overlay
		$(STAGING_DIR_HOST)/bin/mksquashfs4 $(TARGET_DIR) $(KDIR)/root.squashfs -nopad -noappend -root-owned -comp $(SQUASHFSCOMP) $(SQUASHFSOPT) -processors $(if $(CONFIG_PKG_BUILD_JOBS),$(CONFIG_PKG_BUILD_JOBS),1)
		$(call Image/Build,squashfs)
    endef
  endif

  ifneq ($(CONFIG_TARGET_ROOTFS_UBIFS),)
    define Image/mkfs/ubifs
		$(CP) ./ubinize.cfg $(KDIR)
		$(STAGING_DIR_HOST)/bin/mkfs.ubifs $(UBIFS_OPTS) -o $(KDIR)/root.ubifs -d $(TARGET_DIR)
		(cd $(KDIR); \
		$(STAGING_DIR_HOST)/bin/ubinize $(UBINIZE_OPTS) -o $(KDIR)/root.ubi ubinize.cfg)
		$(call Image/Build,ubi)
    endef
  endif

endif

ifneq ($(CONFIG_TARGET_ROOTFS_CPIOGZ),)
  define Image/mkfs/cpiogz
		( cd $(TARGET_DIR); find . | cpio -o -H newc | gzip -9 >$(BIN_DIR)/$(IMG_PREFIX)-rootfs.cpio.gz )
  endef
endif

ifneq ($(CONFIG_TARGET_ROOTFS_TARGZ),)
  define Image/mkfs/targz
		# Preserve permissions (-p) when building as non-root user
		$(TAR) -czpf $(BIN_DIR)/$(IMG_PREFIX)-rootfs.tar.gz --numeric-owner --owner=0 --group=0 -C $(TARGET_DIR)/ .
  endef
endif

ifneq ($(CONFIG_TARGET_ROOTFS_EXT4FS),)
  E2SIZE=$(shell echo $$(($(CONFIG_TARGET_ROOTFS_PARTSIZE)*1024)))

  define Image/mkfs/ext4
# generate an ext2 fs
	$(STAGING_DIR_HOST)/bin/genext2fs -U -b $(E2SIZE) -N $(CONFIG_TARGET_ROOTFS_MAXINODE) -d $(TARGET_DIR)/ $(KDIR)/root.ext4 -m $(CONFIG_TARGET_ROOTFS_RESERVED_PCT)
# convert it to ext4
	$(STAGING_DIR_HOST)/bin/tune2fs -O extents,uninit_bg,dir_index $(KDIR)/root.ext4
# fix it up
	$(STAGING_DIR_HOST)/bin/e2fsck -fy $(KDIR)/root.ext4
	$(call Image/Build,ext4)
  endef
endif

ifneq ($(CONFIG_TARGET_ROOTFS_ISO),)
  define Image/mkfs/iso
		$(call Image/Build,iso)
  endef
endif


define Image/mkfs/prepare/default
	# Use symbolic permissions to avoid clobbering SUID/SGID/sticky bits
	- $(FIND) $(TARGET_DIR) -type f -not -perm +0100 -not -name 'ssh_host*' -not -name 'shadow' -print0 | $(XARGS) -0 chmod u+rw,g+r,o+r
	- $(FIND) $(TARGET_DIR) -type f -perm +0100 -print0 | $(XARGS) -0 chmod u+rwx,g+rx,o+rx
	- $(FIND) $(TARGET_DIR) -type d -print0 | $(XARGS) -0 chmod u+rwx,g+rx,o+rx
	$(INSTALL_DIR) $(TARGET_DIR)/tmp
	chmod 1777 $(TARGET_DIR)/tmp
endef

define Image/mkfs/prepare
	$(call Image/mkfs/prepare/default)
endef


define Image/Checksum
	( cd ${BIN_DIR} ; \
		$(FIND) -maxdepth 1 -type f \! -name 'md5sums'  -printf "%P\n" | sort | xargs \
		md5sum --binary > md5sums \
	)
endef


define BuildImage

  download:
  prepare:

  ifeq ($(IB),)
    compile: compile-targets FORCE
		$(call Build/Compile)
  else
    compile:
  endif

  ifeq ($(IB),)
    install: compile install-targets FORCE
		$(call Image/Prepare)
		$(call Image/mkfs/prepare)
		$(call Image/BuildKernel)
		$(call Image/mkfs/cpiogz)
		$(call Image/mkfs/targz)
		$(call Image/mkfs/ext4)
		$(call Image/mkfs/iso)
		$(call Image/mkfs/jffs2)
		$(call Image/mkfs/squashfs)
		$(call Image/mkfs/ubifs)
		$(call Image/Checksum)
  else
    install: compile install-targets
		$(call Image/BuildKernel)
		$(call Image/mkfs/cpiogz)
		$(call Image/mkfs/targz)
		$(call Image/mkfs/ext4)
		$(call Image/mkfs/iso)
		$(call Image/mkfs/jffs2)
		$(call Image/mkfs/squashfs)
		$(call Image/mkfs/ubifs)
		$(call Image/Checksum)
  endif

  ifeq ($(IB),)
    clean: clean-targets
		$(call Build/Clean)
  else
    clean:
  endif

  compile-targets:
  install-targets:
  clean-targets:

endef
