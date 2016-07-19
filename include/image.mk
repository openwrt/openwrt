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
include $(INCLUDE_DIR)/image-commands.mk
include $(INCLUDE_DIR)/image-legacy.mk

override MAKE:=$(_SINGLE)$(SUBMAKE)
override NO_TRACE_MAKE:=$(_SINGLE)$(NO_TRACE_MAKE)

param_get = $(patsubst $(1)=%,%,$(filter $(1)=%,$(2)))
param_mangle = $(subst $(space),_,$(strip $(1)))
param_unmangle = $(subst _,$(space),$(1))

KDIR=$(KERNEL_BUILD_DIR)
KDIR_TMP=$(KDIR)/tmp
DTS_DIR:=$(LINUX_DIR)/arch/$(LINUX_KARCH)/boot/dts

EXTRA_NAME_SANITIZED=$(call sanitize,$(EXTRA_IMAGE_NAME))

IMG_PREFIX:=$(VERSION_DIST_SANITIZED)-$(if $(CONFIG_VERSION_FILENAMES),$(VERSION_NUMBER)-)$(if $(EXTRA_NAME_SANITIZED),$(EXTRA_NAME_SANITIZED)-)$(BOARD)$(if $(SUBTARGET),-$(SUBTARGET))

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
fs-types-$(CONFIG_TARGET_ROOTFS_UBIFS) += ubifs
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

PROFILE_SANITIZED := $(call sanitize,$(PROFILE))

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
		-n '$(call toupper,$(ARCH)) LEDE Linux-$(LINUX_VERSION)' -d $(4) $(5)
endef

define Image/BuildKernel/MkFIT
	$(TOPDIR)/scripts/mkits.sh \
		-D $(1) -o $(KDIR)/fit-$(1).its -k $(2) $(if $(3),-d $(3)) -C $(4) -a $(5) -e $(6) \
		-A $(ARCH) -v $(LINUX_VERSION)
	PATH=$(LINUX_DIR)/scripts/dtc:$(PATH) mkimage -f $(KDIR)/fit-$(1).its $(KDIR)/fit-$(1)$(7).itb
endef

# $(1) source dts file
# $(2) target dtb file
# $(3) extra CPP flags
# $(4) extra DTC flags
define Image/BuildDTB
	$(TARGET_CROSS)cpp -nostdinc -x assembler-with-cpp \
		-I$(DTS_DIR) \
		-I$(DTS_DIR)/include \
		-undef -D__DTS__ $(3) \
		-o $(2).tmp $(1)
	$(LINUX_DIR)/scripts/dtc/dtc -O dtb \
		-i$(dir $(1)) $(4) \
		-o $(2) $(2).tmp
	$(RM) $(2).tmp
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
	$(STAGING_DIR_HOST)/bin/mksquashfs4 $(TARGET_DIR) $(KDIR)/root.squashfs -nopad -noappend -root-owned -comp $(SQUASHFSCOMP) $(SQUASHFSOPT) -processors $(if $(CONFIG_PKG_BUILD_JOBS),$(CONFIG_PKG_BUILD_JOBS),1) $(if $(SOURCE_DATE_EPOCH),-fixed-time $(SOURCE_DATE_EPOCH))
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
		"$(BIN_DIR)/$(IMG_PREFIX)-$(1)-$(2)-sysupgrade.tar" sysupgrade-$(1) \
			$(if $(SOURCE_DATE_EPOCH),--mtime="@$(SOURCE_DATE_EPOCH)") \
	)
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

define Image/mkfs/ubifs
	$(STAGING_DIR_HOST)/bin/mkfs.ubifs \
		$(UBIFS_OPTS) $(call param_unmangle,$(call param_get,fs,$(1))) \
		$(if $(CONFIG_TARGET_UBIFS_FREE_SPACE_FIXUP),--space-fixup) \
		$(if $(CONFIG_TARGET_UBIFS_COMPRESSION_NONE),--force-compr=none) \
		$(if $(CONFIG_TARGET_UBIFS_COMPRESSION_LZO),--force-compr=lzo) \
		$(if $(CONFIG_TARGET_UBIFS_COMPRESSION_ZLIB),--force-compr=zlib) \
		$(if $(shell echo $(CONFIG_TARGET_UBIFS_JOURNAL_SIZE)),--jrn-size=$(CONFIG_TARGET_UBIFS_JOURNAL_SIZE)) \
		--squash-uids \
		-o $@ -d $(TARGET_DIR)
endef

define Image/mkfs/cpiogz
	( cd $(TARGET_DIR); find . | cpio -o -H newc | gzip -9n >$(BIN_DIR)/$(IMG_PREFIX)-rootfs.cpio.gz )
endef

define Image/mkfs/targz
	$(TAR) -cp --numeric-owner --owner=0 --group=0 --sort=name \
		$(if $(SOURCE_DATE_EPOCH),--mtime="@$(SOURCE_DATE_EPOCH)") \
		-C $(TARGET_DIR)/ . | gzip -9n > $(BIN_DIR)/$(IMG_PREFIX)$(if $(PROFILE_SANITIZED),-$(PROFILE_SANITIZED))-rootfs.tar.gz
endef

E2SIZE=$(shell echo $$(($(CONFIG_TARGET_ROOTFS_PARTSIZE)*1024*1024)))

define Image/mkfs/ext4
	$(STAGING_DIR_HOST)/bin/make_ext4fs \
		-l $(E2SIZE) -b $(CONFIG_TARGET_EXT4_BLOCKSIZE) \
		-i $(CONFIG_TARGET_EXT4_MAXINODE) \
		-m $(CONFIG_TARGET_EXT4_RESERVED_PCT) \
		$(if $(CONFIG_TARGET_EXT4_JOURNAL),,-J) \
		$(if $(SOURCE_DATE_EPOCH),-T $(SOURCE_DATE_EPOCH)) \
		$(KDIR)/root.ext4 $(TARGET_DIR)/
endef

define Image/mkfs/prepare/default
	# Use symbolic permissions to avoid clobbering SUID/SGID/sticky bits
	- $(FIND) $(TARGET_DIR) -type f -not -perm /0100 -not -name 'ssh_host*' -not -name 'shadow' -print0 | $(XARGS) -0 chmod u+rw,g+r,o+r
	- $(FIND) $(TARGET_DIR) -type f -perm /0100 -print0 | $(XARGS) -0 chmod u+rwx,g+rx,o+rx
	- $(FIND) $(TARGET_DIR) -type d -print0 | $(XARGS) -0 chmod u+rwx,g+rx,o+rx
	$(INSTALL_DIR) $(TARGET_DIR)/tmp $(TARGET_DIR)/overlay
	chmod 1777 $(TARGET_DIR)/tmp
endef

define Image/mkfs/prepare
	$(call Image/mkfs/prepare/default)
endef


define Image/Checksum
	( cd ${BIN_DIR} ; \
		$(FIND) -maxdepth 1 -type f \! -name 'md5sums'  -printf "%P\n" | sort | xargs $1 > $2 \
	)
endef

$(KDIR)/root.%: kernel_prepare
	$(call Image/mkfs/$(word 1,$(subst +,$(space),$*)),$(subst +,$(space),$*))

define Device/InitProfile
  PROFILES := $(PROFILE)
  DEVICE_TITLE :=
  DEVICE_PACKAGES :=
  DEVICE_DESCRIPTION = Build firmware images for $$(DEVICE_TITLE)
endef

define Device/Init
  DEVICE_NAME := $(1)
  KERNEL:=
  KERNEL_INITRAMFS = $$(KERNEL)
  KERNEL_SIZE:=
  CMDLINE:=

  IMAGE_PREFIX := $(IMG_PREFIX)-$(1)
  IMAGE_NAME = $$(IMAGE_PREFIX)-$$(1)-$$(2)
  KERNEL_PREFIX = $$(IMAGE_PREFIX)
  KERNEL_SUFFIX := -kernel.bin
  KERNEL_INITRAMFS_SUFFIX = $$(KERNEL_SUFFIX)
  KERNEL_IMAGE = $$(KERNEL_PREFIX)$$(KERNEL_SUFFIX)
  KERNEL_INITRAMFS_PREFIX = $$(IMAGE_PREFIX)-initramfs
  KERNEL_INITRAMFS_IMAGE = $$(KERNEL_INITRAMFS_PREFIX)$$(KERNEL_INITRAMFS_SUFFIX)
  KERNEL_INITRAMFS_NAME = $$(KERNEL_NAME)-initramfs
  KERNEL_INSTALL :=
  KERNEL_NAME := vmlinux
  KERNEL_DEPENDS :=
  KERNEL_SIZE :=

  UBOOTENV_IN_UBI :=
  KERNEL_IN_UBI :=
  BLOCKSIZE :=
  PAGESIZE :=
  SUBPAGESIZE :=
  UBINIZE_OPTS := -E 5
  MKUBIFS_OPTS :=

  FS_OPTIONS/ubifs = $$(MKUBIFS_OPTS)

  DEVICE_DTS :=
  DEVICE_DTS_DIR :=

  FILESYSTEMS := $(TARGET_FILESYSTEMS)
endef

DEFAULT_DEVICE_VARS := \
  DEVICE_NAME KERNEL KERNEL_INITRAMFS KERNEL_INITRAMFS_IMAGE \
  DEVICE_DTS DEVICE_DTS_DIR \
  UBOOTENV_IN_UBI KERNEL_IN_UBI \
  BLOCKSIZE PAGESIZE SUBPAGESIZE \
  UBINIZE_OPTS

define Device/ExportVar
  $(1) : $(2):=$$($(2))

endef
define Device/Export
  $(foreach var,$(DEVICE_VARS) $(DEFAULT_DEVICE_VARS),$(call Device/ExportVar,$(1),$(var)))
  $(1) : FILESYSTEM:=$(2)
endef

ifdef IB
  DEVICE_CHECK_PROFILE = $(filter $(1),$(PROFILE))
else
  DEVICE_CHECK_PROFILE = $(CONFIG_TARGET_$(if $(CONFIG_TARGET_MULTI_PROFILE),DEVICE_)$(call target_conf,$(BOARD)$(if $(SUBTARGET),_$(SUBTARGET)))_$(1))
endif

define Device/Check
  _PROFILE_SET = $$(strip $$(foreach profile,$$(PROFILES) DEVICE_$(1),$$(call DEVICE_CHECK_PROFILE,$$(profile))))
  _TARGET := $$(if $$(_PROFILE_SET),install-images,install-disabled)
  ifndef IB
    _COMPILE_TARGET := $$(if $(CONFIG_IB)$$(_PROFILE_SET),compile,compile-disabled)
  endif
endef

ifndef IB
define Device/Build/initramfs
  $(call Device/Export,$(KDIR)/tmp/$$(KERNEL_INITRAMFS_IMAGE),$(1))
  $$(_TARGET): $$(if $$(KERNEL_INITRAMFS),$(BIN_DIR)/$$(KERNEL_INITRAMFS_IMAGE))

  $(KDIR)/$$(KERNEL_INITRAMFS_NAME):: image_prepare
  $(BIN_DIR)/$$(KERNEL_INITRAMFS_IMAGE): $(KDIR)/tmp/$$(KERNEL_INITRAMFS_IMAGE)
	cp $$^ $$@

  $(KDIR)/tmp/$$(KERNEL_INITRAMFS_IMAGE): $(KDIR)/$$(KERNEL_INITRAMFS_NAME) $(CURDIR)/Makefile $$(KERNEL_DEPENDS)
	@rm -f $$@
	$$(call concat_cmd,$$(KERNEL_INITRAMFS))
endef
endif

define Device/Build/check_size
	@[ $$(($(subst k,* 1024,$(subst m, * 1024k,$(1))))) -ge "$$(stat -c%s $@)" ] || { \
		echo "WARNING: Image file $@ is too big" >&2; \
		rm -f $@; \
	}
endef

define Device/Build/compile
  $$(_COMPILE_TARGET): $(KDIR)/$(1)
  $(eval $(call Device/Export,$(KDIR)/$(1)))
  $(KDIR)/$(1):
	$$(call concat_cmd,$(COMPILE/$(1)))

endef

define Device/Build/kernel
  _KERNEL_IMAGES += $(KDIR)/$$(KERNEL_NAME)
  $(KDIR)/$$(KERNEL_NAME):: image_prepare
  $$(_TARGET): $$(if $$(KERNEL_INSTALL),$(BIN_DIR)/$$(KERNEL_IMAGE))
  $(call Device/Export,$(KDIR)/$$(KERNEL_IMAGE),$(1))
  $(BIN_DIR)/$$(KERNEL_IMAGE): $(KDIR)/$$(KERNEL_IMAGE)
	cp $$^ $$@
  ifndef IB
    ifdef CONFIG_IB
      install: $(KDIR)/$$(KERNEL_IMAGE)
    endif
    $(KDIR)/$$(KERNEL_IMAGE): $(KDIR)/$$(KERNEL_NAME) $(CURDIR)/Makefile $$(KERNEL_DEPENDS)
	@rm -f $$@
	$$(call concat_cmd,$$(KERNEL))
	$$(if $$(KERNEL_SIZE),$$(call Device/Build/check_size,$$(KERNEL_SIZE)))
  endif
endef

define Device/Build/image
  $$(_TARGET): $(BIN_DIR)/$(call IMAGE_NAME,$(1),$(2))
  $(eval $(call Device/Export,$(KDIR)/tmp/$(call IMAGE_NAME,$(1),$(2)),$(1)))
  $(KDIR)/tmp/$(call IMAGE_NAME,$(1),$(2)): $(KDIR)/$$(KERNEL_IMAGE) $(KDIR)/root.$(1)$$(if $$(FS_OPTIONS/$(1)),+fs=$$(call param_mangle,$$(FS_OPTIONS/$(1))))
	@rm -f $$@
	[ -f $$(word 1,$$^) -a -f $$(word 2,$$^) ]
	$$(call concat_cmd,$(if $(IMAGE/$(2)/$(1)),$(IMAGE/$(2)/$(1)),$(IMAGE/$(2))))

  .IGNORE: $(BIN_DIR)/$(call IMAGE_NAME,$(1),$(2))
  $(BIN_DIR)/$(call IMAGE_NAME,$(1),$(2)): $(KDIR)/tmp/$(call IMAGE_NAME,$(1),$(2))
	cp $$^ $$@

endef

define Device/Build
  $(if $(CONFIG_TARGET_ROOTFS_INITRAMFS),$(call Device/Build/initramfs,$(1)))
  $(call Device/Build/kernel,$(1))

  $$(eval $$(foreach compile,$$(COMPILE), \
    $$(call Device/Build/compile,$$(compile),$(1))))

  $$(eval $$(foreach image,$$(IMAGES), \
    $$(foreach fs,$$(filter $(TARGET_FILESYSTEMS),$$(FILESYSTEMS)), \
      $$(call Device/Build/image,$$(fs),$$(image),$(1)))))
endef

define Device/DumpInfo
Target-Profile: DEVICE_$(1)
Target-Profile-Name: $(DEVICE_TITLE)
Target-Profile-Packages: $(DEVICE_PACKAGES)
Target-Profile-Description:
$(DEVICE_DESCRIPTION)
@@

endef

define Device/Dump
$$(eval $$(if $$(DEVICE_TITLE),$$(info $$(call Device/DumpInfo,$(1)))))
endef

define Device
  $(call Device/InitProfile,$(1))
  $(call Device/Init,$(1))
  $(call Device/Default,$(1))
  $(call Device/$(1),$(1))
  $(call Device/Check,$(1))
  $(call Device/$(if $(DUMP),Dump,Build),$(1))

endef

define BuildImage

  ifneq ($(DUMP),)
    all: dumpinfo
    dumpinfo: FORCE
	@true
  endif

  download:
  prepare:
  compile:
  clean:
  legacy-images-prepare:
  legacy-images:
  image_prepare:

  ifeq ($(IB),)
    .PHONY: download prepare compile clean image_prepare mkfs_prepare kernel_prepare install install-images
    compile:
		$(call Build/Compile)

    clean:
		$(call Build/Clean)

    image_prepare: compile
		mkdir -p $(BIN_DIR) $(KDIR)/tmp
		$(call Image/Prepare)

    legacy-images-prepare-make: image_prepare
		$(MAKE) legacy-images-prepare

  else
    image_prepare:
		mkdir -p $(BIN_DIR) $(KDIR)/tmp
  endif

  mkfs_prepare: image_prepare
	$(call Image/mkfs/prepare)

  kernel_prepare: mkfs_prepare
	$(call Image/BuildKernel)
	$(if $(CONFIG_TARGET_ROOTFS_INITRAMFS),$(if $(IB),,$(call Image/BuildKernel/Initramfs)))
	$(call Image/InstallKernel)

  $(foreach device,$(TARGET_DEVICES),$(call Device,$(device)))
  $(foreach device,$(LEGACY_DEVICES),$(call LegacyDevice,$(device)))

  install-images: kernel_prepare $(foreach fs,$(filter-out ubifs,$(TARGET_FILESYSTEMS) $(fs-subtypes-y)),$(KDIR)/root.$(fs))
	$(foreach fs,$(TARGET_FILESYSTEMS),
		$(call Image/Build,$(fs))
	)

  legacy-images-make: install-images
	$(call Image/mkfs/ubifs/legacy)
	$(MAKE) legacy-images

  install: install-images
	$(call Image/Checksum,md5sum --binary,md5sums)
	$(call Image/Checksum,openssl dgst -sha256,sha256sums)

endef
