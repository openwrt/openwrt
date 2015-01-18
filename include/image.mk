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
include $(INCLUDE_DIR)/version.mk

override MAKE:=$(_SINGLE)$(SUBMAKE)
override NO_TRACE_MAKE:=$(_SINGLE)$(NO_TRACE_MAKE)

KDIR=$(KERNEL_BUILD_DIR)
DTS_DIR:=$(LINUX_DIR)/arch/$(ARCH)/boot/dts/

IMG_PREFIX:=openwrt-$(if $(CONFIG_VERSION_FILENAMES),$(VERSION_NUMBER)-)$(BOARD)$(if $(SUBTARGET),-$(SUBTARGET))

MKFS_DEVTABLE_OPT := -D $(INCLUDE_DIR)/device_table.txt

ifneq ($(CONFIG_BIG_ENDIAN),)
  JFFS2OPTS     :=  --big-endian --squash-uids -v
else
  JFFS2OPTS     :=  --little-endian --squash-uids -v
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
  JFFS2OPTS += -x rtime
endif
ifneq ($(CONFIG_JFFS2_ZLIB),y)
  JFFS2OPTS += -x zlib
endif
ifneq ($(CONFIG_JFFS2_LZMA),y)
  JFFS2OPTS += -x lzma
endif

JFFS2OPTS += $(MKFS_DEVTABLE_OPT)

SQUASHFS_BLOCKSIZE := $(CONFIG_TARGET_SQUASHFS_BLOCK_SIZE)k
SQUASHFSOPT := -b $(SQUASHFS_BLOCKSIZE)
SQUASHFSOPT += -p '/dev d 755 0 0' -p '/dev/console c 600 0 0 5 1'
SQUASHFSCOMP := gzip
LZMA_XZ_OPTIONS := -Xpreset 9 -Xe -Xlc 0 -Xlp 2 -Xpb 2
ifeq ($(CONFIG_SQUASHFS_LZMA),y)
  SQUASHFSCOMP := lzma $(LZMA_XZ_OPTIONS)
endif
ifeq ($(CONFIG_SQUASHFS_XZ),y)
  ifneq ($(filter arm x86 powerpc sparc,$(LINUX_KARCH)),)
    BCJ_FILTER:=-Xbcj $(LINUX_KARCH)
  endif
  SQUASHFSCOMP := xz $(LZMA_XZ_OPTIONS) $(BCJ_FILTER)
endif

JFFS2_BLOCKSIZE ?= 64k 128k

fs-types-$(CONFIG_TARGET_ROOTFS_SQUASHFS) += squashfs
fs-types-$(CONFIG_TARGET_ROOTFS_JFFS2) += $(addprefix jffs2-,$(JFFS2_BLOCKSIZE))
fs-types-$(CONFIG_TARGET_ROOTFS_JFFS2_NAND) += $(addprefix jffs2-nand-,$(NAND_BLOCKSIZE))
fs-types-$(CONFIG_TARGET_ROOTFS_EXT4FS) += ext4
fs-types-$(CONFIG_TARGET_ROOTFS_ISO) += iso
fs-subtypes-$(CONFIG_TARGET_ROOTFS_JFFS2) += $(addsuffix -raw,$(addprefix jffs2-,$(JFFS2_BLOCKSIZE)))
fs-subtypes-$(CONFIG_TARGET_ROOTFS_CPIOGZ) += cpiogz
fs-subtypes-$(CONFIG_TARGET_ROOTFS_TARGZ) += targz

TARGET_FILESYSTEMS := $(fs-types-y)

FS_64K := $(filter-out jffs2-%,$(TARGET_FILESYSTEMS)) jffs2-64k
FS_128K := $(filter-out jffs2-%,$(TARGET_FILESYSTEMS)) jffs2-128k
FS_256K := $(filter-out jffs2-%,$(TARGET_FILESYSTEMS)) jffs2-256k

define add_jffs2_mark
	echo -ne '\xde\xad\xc0\xde' >> $(1)
endef

define toupper
$(shell echo $(1) | tr '[:lower:]' '[:upper:]')
endef

define split_args
$(foreach data, \
	$(subst |,$(space),\
		$(subst $(space),^,$(1))), \
	$(call $(2),$(strip $(subst ^,$(space),$(data)))))
endef

define build_cmd
$(if $(Build/$(word 1,$(1))),,$(error Missing Build/$(word 1,$(1))))
$(call Build/$(word 1,$(1)),$(wordlist 2,$(words $(1)),$(1)))

endef

define concat_cmd
$(call split_args,$(1),build_cmd)
endef

# pad to 4k, 8k, 16k, 64k, 128k, 256k and add jffs2 end-of-filesystem mark
define prepare_generic_squashfs
	$(STAGING_DIR_HOST)/bin/padjffs2 $(1) 4 8 16 64 128 256
endef

define Image/BuildKernel/Initramfs
	$(call Image/Build/Initramfs)
endef

define Image/BuildKernel/MkuImage
	mkimage -A $(ARCH) -O linux -T kernel -C $(1) -a $(2) -e $(3) \
		-n '$(call toupper,$(ARCH)) OpenWrt Linux-$(LINUX_VERSION)' -d $(4) $(5)
endef

define Image/BuildKernel/MkFIT
	$(TOPDIR)/scripts/mkits.sh \
		-D $(1) -o $(KDIR)/fit-$(1).its -k $(2) -d $(3) -C $(4) -a $(5) -e $(6) \
		-A $(ARCH) -v $(LINUX_VERSION)
	PATH=$(LINUX_DIR)/scripts/dtc:$(PATH) mkimage -f $(KDIR)/fit-$(1).its $(KDIR)/fit-$(1)$(7).itb
endef

define Image/mkfs/jffs2/sub
		$(STAGING_DIR_HOST)/bin/mkfs.jffs2 $(3) --pad -e $(patsubst %k,%KiB,$(1)) -o $(KDIR)/root.jffs2-$(2) -d $(TARGET_DIR) -v 2>&1 1>/dev/null | awk '/^.+$$$$/'
		$(call add_jffs2_mark,$(KDIR)/root.jffs2-$(2))
endef

define Image/mkfs/jffs2/sub-raw
		$(STAGING_DIR_HOST)/bin/mkfs.jffs2 $(3) -e $(patsubst %k,%KiB,$(1)) -o $(KDIR)/root.jffs2-$(2)-raw -d $(TARGET_DIR) -v 2>&1 1>/dev/null | awk '/^.+$$$$/'
endef

define Image/mkfs/jffs2/template
  Image/mkfs/jffs2-$(1) = $$(call Image/mkfs/jffs2/sub,$(1),$(1),$(JFFS2OPTS))
  Image/mkfs/jffs2-$(1)-raw = $$(call Image/mkfs/jffs2/sub-raw,$(1),$(1),$(JFFS2OPTS))

endef

define Image/mkfs/jffs2-nand/template
  Image/mkfs/jffs2-nand-$(1) = \
	$$(call Image/mkfs/jffs2/sub, \
		$(word 2,$(subst -, ,$(1))),nand-$(1), \
			$(JFFS2OPTS) --no-cleanmarkers --pagesize=$(word 1,$(subst -, ,$(1))))

endef

$(eval $(foreach S,$(JFFS2_BLOCKSIZE),$(call Image/mkfs/jffs2/template,$(S))))
$(eval $(foreach S,$(NAND_BLOCKSIZE),$(call Image/mkfs/jffs2-nand/template,$(S))))

define Image/mkfs/squashfs
	$(STAGING_DIR_HOST)/bin/mksquashfs4 $(TARGET_DIR) $(KDIR)/root.squashfs -nopad -noappend -root-owned -comp $(SQUASHFSCOMP) $(SQUASHFSOPT) -processors $(if $(CONFIG_PKG_BUILD_JOBS),$(CONFIG_PKG_BUILD_JOBS),1)
endef

# $(1): board name
# $(2): rootfs type
# $(3): kernel image
ifneq ($(CONFIG_NAND_SUPPORT),)
   define Image/Build/SysupgradeNAND
	mkdir -p "$(KDIR_TMP)/sysupgrade-$(1)/"
	echo "BOARD=$(1)" > "$(KDIR_TMP)/sysupgrade-$(1)/CONTROL"
	[ -z "$(2)" ] || $(CP) "$(KDIR)/root.$(2)" "$(KDIR_TMP)/sysupgrade-$(1)/root"
	[ -z "$(3)" ] || $(CP) "$(3)" "$(KDIR_TMP)/sysupgrade-$(1)/kernel"
	(cd "$(KDIR_TMP)"; $(TAR) cvf \
		"$(BIN_DIR)/$(IMG_PREFIX)-$(1)-$(2)-sysupgrade.tar" sysupgrade-$(1))
   endef

# $(1) board name
# $(2) ubinize-image options (e.g. --uboot-env and/or --kernel kernelimage)
# $(3) rootfstype (e.g. squashfs or ubifs)
# $(4) options to pass-through to ubinize (i.e. $($(PROFILE)_UBI_OPTS)))
   define Image/Build/UbinizeImage
	sh $(TOPDIR)/scripts/ubinize-image.sh $(2) \
		"$(KDIR)/root.$(3)" \
		"$(KDIR)/$(IMG_PREFIX)-$(1)-$(3)-ubinized.bin" \
		$(4)
   endef

endif

ifneq ($(CONFIG_TARGET_ROOTFS_UBIFS),)
    define Image/mkfs/ubifs/generate
	$(CP) ./ubinize$(1).cfg $(KDIR)
	( cd $(KDIR); \
		$(STAGING_DIR_HOST)/bin/ubinize \
		$(if $($(PROFILE)_UBI_OPTS), \
			$(shell echo $($(PROFILE)_UBI_OPTS)), \
			$(shell echo $(UBI_OPTS)) \
		) \
		-o $(KDIR)/root$(1).ubi \
		ubinize$(1).cfg \
	)
    endef

    define Image/mkfs/ubifs

        ifneq ($($(PROFILE)_UBIFS_OPTS)$(UBIFS_OPTS),)
		$(STAGING_DIR_HOST)/bin/mkfs.ubifs \
			$(if $($(PROFILE)_UBIFS_OPTS), \
				$(shell echo $($(PROFILE)_UBIFS_OPTS)), \
				$(shell echo $(UBIFS_OPTS)) \
			) \
			$(if $(CONFIG_TARGET_UBIFS_FREE_SPACE_FIXUP),--space-fixup) \
			$(if $(CONFIG_TARGET_UBIFS_COMPRESSION_NONE),--force-compr=none) \
			$(if $(CONFIG_TARGET_UBIFS_COMPRESSION_LZO),--force-compr=lzo) \
			$(if $(CONFIG_TARGET_UBIFS_COMPRESSION_ZLIB),--force-compr=zlib) \
			$(if $(shell echo $(CONFIG_TARGET_UBIFS_JOURNAL_SIZE)),--jrn-size=$(CONFIG_TARGET_UBIFS_JOURNAL_SIZE)) \
			--squash-uids \
			-o $(KDIR)/root.ubifs \
			-d $(TARGET_DIR)
        endif
	$(call Image/Build,ubifs)

        ifneq ($($(PROFILE)_UBI_OPTS)$(UBI_OPTS),)
		$(if $(wildcard ./ubinize.cfg),$(call Image/mkfs/ubifs/generate,))
		$(if $(wildcard ./ubinize-overlay.cfg),$(call Image/mkfs/ubifs/generate,-overlay))
        endif
	$(if $(wildcard ./ubinize.cfg),$(call Image/Build,ubi))
    endef
endif

define Image/mkfs/cpiogz
	( cd $(TARGET_DIR); find . | cpio -o -H newc | gzip -9 >$(BIN_DIR)/$(IMG_PREFIX)-rootfs.cpio.gz )
endef

define Image/mkfs/targz
	$(TAR) -czpf $(BIN_DIR)/$(IMG_PREFIX)$(if $(PROFILE),-$(PROFILE))-rootfs.tar.gz --numeric-owner --owner=0 --group=0 -C $(TARGET_DIR)/ .
endef

E2SIZE=$(shell echo $$(($(CONFIG_TARGET_ROOTFS_PARTSIZE)*1024*1024/$(CONFIG_TARGET_EXT4_BLOCKSIZE))))

define Image/mkfs/ext4
# generate an ext2 fs
	$(STAGING_DIR_HOST)/bin/genext2fs -U -B $(CONFIG_TARGET_EXT4_BLOCKSIZE) -b $(E2SIZE) -N $(CONFIG_TARGET_EXT4_MAXINODE) -d $(TARGET_DIR)/ $(KDIR)/root.ext4 -m $(CONFIG_TARGET_EXT4_RESERVED_PCT) $(MKFS_DEVTABLE_OPT)
# convert it to ext4
	$(STAGING_DIR_HOST)/bin/tune2fs $(if $(CONFIG_TARGET_EXT4_JOURNAL),-j) -O extents,uninit_bg,dir_index $(KDIR)/root.ext4
# fix it up
	$(STAGING_DIR_HOST)/bin/e2fsck -fy $(KDIR)/root.ext4
endef

define Image/mkfs/prepare/default
	# Use symbolic permissions to avoid clobbering SUID/SGID/sticky bits
	- $(FIND) $(TARGET_DIR) -type f -not -perm +0100 -not -name 'ssh_host*' -not -name 'shadow' -print0 | $(XARGS) -0 chmod u+rw,g+r,o+r
	- $(FIND) $(TARGET_DIR) -type f -perm +0100 -print0 | $(XARGS) -0 chmod u+rwx,g+rx,o+rx
	- $(FIND) $(TARGET_DIR) -type d -print0 | $(XARGS) -0 chmod u+rwx,g+rx,o+rx
	$(INSTALL_DIR) $(TARGET_DIR)/tmp $(TARGET_DIR)/overlay
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

define BuildImage/mkfs
  install: mkfs-$(1)
  .PHONY: mkfs-$(1)
  mkfs-$(1): mkfs_prepare
	$(Image/mkfs/$(1))
  $(KDIR)/root.$(1): mkfs-$(1)

endef

# Build commands that can be called from Device/* templates
define Build/uImage
	mkimage -A $(LINUX_KARCH) \
		-O linux -T kernel \
		-C $(1) -a $(KERNEL_LOADADDR) -e $(if $(KERNEL_ENTRY),$(KERNEL_ENTRY),$(KERNEL_LOADADDR)) \
		-n '$(call toupper,$(LINUX_KARCH)) OpenWrt Linux-$(LINUX_VERSION)' -d $@ $@.new
	@mv $@.new $@
endef

define Build/lzma
	$(STAGING_DIR_HOST)/bin/lzma e $@ -lc1 -lp2 -pb2 $(1) $@.new
	@mv $@.new $@
endef

define Build/kernel-bin
	rm -f $@
	cp $^ $@
endef

define Build/patch-cmdline
	$(STAGING_DIR_HOST)/bin/patch-cmdline $@ '$(CMDLINE)'
endef

define Build/append-kernel
	dd if=$(word 1,$^) $(if $(1),bs=$(1) conv=sync) >> $@
endef

define Build/append-rootfs
	dd if=$(word 2,$^) $(if $(1),bs=$(1) conv=sync) >> $@
endef

define Build/pad-rootfs
	$(call prepare_generic_squashfs,$@ $(1))
endef

define Build/pad-offset
	let \
		size="$$(stat -c%s $@)" \
		pad="$(word 1, $(1))" \
		offset="$(word 2, $(1))" \
		pad="(pad - ((size + offset) % pad)) % pad" \
		newsize='size + pad'; \
		dd if=$@ of=$@.new bs=$$newsize count=1 conv=sync
	mv $@.new $@
endef

define Build/check-size
	@[ $$(($(subst k,* 1024,$(subst m, * 1024k,$(1))))) -gt "$$(stat -c%s $@)" ] || { \
		echo "WARNING: Image file $@ is too big"; \
		rm -f $@; \
	}
endef


define Device/Init
  PROFILES := $(PROFILE)
  KERNEL:=
  KERNEL_INITRAMFS = $$(KERNEL)
  KERNEL_SIZE:=
  CMDLINE:=

  IMAGE_PREFIX := $(IMG_PREFIX)-$(1)
  IMAGE_NAME = $$(IMAGE_PREFIX)-$$(1)-$$(2)
  KERNEL_PREFIX = $(1)
  KERNEL_SUFFIX := -kernel.bin
  KERNEL_IMAGE = $$(KERNEL_PREFIX)$$(KERNEL_SUFFIX)
  KERNEL_INITRAMFS_PREFIX = $$(IMAGE_PREFIX)-initramfs
  KERNEL_INITRAMFS_IMAGE = $$(KERNEL_INITRAMFS_PREFIX)$$(KERNEL_SUFFIX)
  KERNEL_INSTALL :=
  KERNEL_NAME := vmlinux
  KERNEL_SIZE :=

  FILESYSTEMS := $(TARGET_FILESYSTEMS)
endef

define Device/ExportVar
  $(1) : $(2):=$$($(2))

endef
Device/Export = $(foreach var,$(DEVICE_VARS) KERNEL,$(call Device/ExportVar,$(1),$(var)))

define Device/Check
  _TARGET = $$(if $$(filter $(PROFILE),$$(PROFILES)),install,install-disabled)
endef

define Device/Build/initramfs
  $$(_TARGET): $(BIN_DIR)/$$(KERNEL_INITRAMFS_IMAGE)

  $(BIN_DIR)/$$(KERNEL_INITRAMFS_IMAGE): $(KDIR)/$$(KERNEL_INITRAMFS_IMAGE)
	cp $$^ $$@

  $(KDIR)/$$(KERNEL_INITRAMFS_IMAGE): $(KDIR)/$$(KERNEL_NAME)-initramfs
	@rm -f $$@
	$$(call concat_cmd,$$(KERNEL_INITRAMFS))
endef

define Device/Build/check_size
	@[ $$(($(subst k,* 1024,$(subst m, * 1024k,$(1))))) -gt "$$(stat -c%s $@)" ] || { \
		echo "WARNING: Image file $@ is too big"; \
		rm -f $@; \
	}
endef

define Device/Build/kernel
  $$(_TARGET): $$(if $$(KERNEL_INSTALL),$(BIN_DIR)/$$(KERNEL_IMAGE))
  $(BIN_DIR)/$$(KERNEL_IMAGE): $(KDIR)/$$(KERNEL_IMAGE)
	cp $$^ $$@
  $(KDIR)/$$(KERNEL_IMAGE): $(KDIR)/$$(KERNEL_NAME)
	@rm -f $$@
	$$(call concat_cmd,$$(KERNEL))
	$$(if $$(KERNEL_SIZE),$$(call Device/Build/check_size,$$(KERNEL_SIZE)))
endef

define Device/Build/image
  $$(_TARGET): $(BIN_DIR)/$(call IMAGE_NAME,$(1),$(2))
  $(eval $(call Device/Export,$(KDIR)/$(KERNEL_IMAGE)))
  $(eval $(call Device/Export,$(KDIR)/$(KERNEL_INITRAMFS_IMAGE)))
  $(eval $(call Device/Export,$(KDIR)/$(call IMAGE_NAME,$(1),$(2))))
  $(KDIR)/$(call IMAGE_NAME,$(1),$(2)): $(KDIR)/$$(KERNEL_IMAGE) $(KDIR)/root.$(1)
	@rm -f $$@
	[ -f $$(word 1,$$^) -a -f $$(word 2,$$^) ]
	$$(call concat_cmd,$(if $(IMAGE/$(2)/$(1)),$(IMAGE/$(2)/$(1)),$(IMAGE/$(2))))

  $(BIN_DIR)/$(call IMAGE_NAME,$(1),$(2)): $(KDIR)/$(call IMAGE_NAME,$(1),$(2))
	cp $$^ $$@

endef

define Device/Build
  $(if $(CONFIG_TARGET_ROOTFS_INITRAMFS),$(call Device/Build/initramfs,$(1)))
  $(call Device/Build/kernel,$(1))

  $$(eval $$(foreach image,$$(IMAGES), \
    $$(foreach fs,$$(filter $(TARGET_FILESYSTEMS),$$(FILESYSTEMS)), \
      $$(call Device/Build/image,$$(fs),$$(image),$(1)))))
endef

define Device
  $(call Device/Init,$(1))
  $(call Device/Default,$(1))
  $(call Device/Check,$(1))
  $(call Device/$(1),$(1))
  $(call Device/Build,$(1))

endef

define BuildImage

  download:
  prepare:
  compile:
  clean:
  image_prepare:

  ifeq ($(IB),)
    .PHONY: download prepare compile clean image_prepare mkfs_prepare kernel_prepare install
    compile:
		$(call Build/Compile)

    clean:
		$(call Build/Clean)

    image_prepare: compile
		$(call Image/Prepare)
  endif

  mkfs_prepare: image_prepare
	$(call Image/mkfs/prepare)

  kernel_prepare: mkfs_prepare
	$(call Image/BuildKernel)
	$(if $(CONFIG_TARGET_ROOTFS_INITRAMFS),$(call Image/BuildKernel/Initramfs))
	$(call Image/InstallKernel)

  $(foreach device,$(TARGET_DEVICES),$(call Device,$(device)))
  $(foreach fs,$(TARGET_FILESYSTEMS) $(fs-subtypes-y),$(call BuildImage/mkfs,$(fs)))

  install: kernel_prepare
	$(foreach fs,$(TARGET_FILESYSTEMS),
		$(call Image/Build,$(fs))
	)
	$(call Image/mkfs/ubifs)
	$(call Image/Checksum)

endef
