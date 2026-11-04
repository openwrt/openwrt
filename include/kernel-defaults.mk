# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2006-2020 OpenWrt.org

ifdef CONFIG_STRIP_KERNEL_EXPORTS
  KERNEL_MAKEOPTS_IMAGE += \
	EXTRA_LDSFLAGS="-I$(KERNEL_BUILD_DIR) -include symtab.h"
endif

INITRAMFS_EXTRA_FILES ?= $(GENERIC_PLATFORM_DIR)/image/initramfs-base-files.txt

INITRAMFS_INIT_FILE := $(GENERIC_PLATFORM_DIR)/other-files/init

export HOST_EXTRACFLAGS=-I$(STAGING_DIR_HOST)/include

# defined in quilt.mk
Kernel/Patch:=$(Kernel/Patch/Default)

ifneq (,$(findstring .xz,$(LINUX_SOURCE)))
  LINUX_CAT:=xzcat
else
  LINUX_CAT:=$(STAGING_DIR_HOST)/bin/libdeflate-gzip -dc
endif

ifeq ($(strip $(CONFIG_EXTERNAL_KERNEL_TREE)),"")
  ifeq ($(strip $(CONFIG_KERNEL_GIT_CLONE_URI)),"")
    define Kernel/Prepare/Default
	$(LINUX_CAT) $(DL_DIR)/$(LINUX_SOURCE) | $(TAR) -C $(KERNEL_BUILD_DIR) $(TAR_OPTIONS)
	$(Kernel/Patch)
    endef
  else
    define Kernel/Prepare/Default
	$(LINUX_CAT) $(DL_DIR)/$(LINUX_SOURCE) | $(TAR) -C $(KERNEL_BUILD_DIR) $(TAR_OPTIONS)
    endef
  endif
else
  define Kernel/Prepare/Default
	mkdir -p $(KERNEL_BUILD_DIR)
	if [ -d $(LINUX_DIR) ]; then \
		rmdir $(LINUX_DIR); \
	fi
	ln -s $(CONFIG_EXTERNAL_KERNEL_TREE) $(LINUX_DIR)
	if [ -d $(LINUX_DIR)/user_headers ]; then \
		rm -rf $(LINUX_DIR)/user_headers; \
	fi
  endef
endif

ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
  ifeq ($(strip $(CONFIG_EXTERNAL_CPIO)),"")
    define Kernel/SetInitramfs/PreConfigure
	{ \
		grep -v -e INITRAMFS -e CONFIG_RD_ -e CONFIG_BLK_DEV_INITRD $(2)/.config.old > $(2)/.config; \
		echo 'CONFIG_BLK_DEV_INITRD=y' >> $(2)/.config; \
		echo 'file /init $(INITRAMFS_INIT_FILE) 0755 0 0' > $(2).init-cpio-list; \
		echo 'CONFIG_INITRAMFS_SOURCE="$(strip $(1) $(2).init-cpio-list $(INITRAMFS_EXTRA_FILES))"' >> $(2)/.config; \
	}
    endef
  else
    define Kernel/SetInitramfs/PreConfigure
	{ \
		grep -v INITRAMFS $(2)/.config.old > $(2)/.config; \
		echo 'CONFIG_INITRAMFS_SOURCE="$(call qstrip,$(CONFIG_EXTERNAL_CPIO))"' >> $(2)/.config; \
	}
    endef
  endif

  define Kernel/SetInitramfs
	{ \
		rm -f $(2)/.config.prev; \
		mv $(2)/.config $(2)/.config.old; \
		$(call Kernel/SetInitramfs/PreConfigure,$(1),$(2)); \
		echo "# CONFIG_INITRAMFS_PRESERVE_MTIME is not set" >> $(2)/.config; \
		echo "CONFIG_INITRAMFS_ROOT_UID=$(shell id -u)" >> $(2)/.config; \
		echo "CONFIG_INITRAMFS_ROOT_GID=$(shell id -g)" >> $(2)/.config; \
		$(if $(CONFIG_TARGET_INITRAMFS_FORCE), \
			echo "CONFIG_INITRAMFS_FORCE=y" >> $(2)/.config;, \
			echo "# CONFIG_INITRAMFS_FORCE is not set" >> $(2)/.config;) \
		$(if $(CONFIG_TARGET_INITRAMFS_COMPRESSION_NONE), \
			echo "CONFIG_INITRAMFS_COMPRESSION_NONE=y" >> $(2)/.config;, \
			echo "# CONFIG_INITRAMFS_COMPRESSION_NONE is not set" >> $(2)/.config; ) \
		$(foreach ALGO,GZIP BZIP2 LZMA LZO XZ LZ4 ZSTD, \
			$(if $(CONFIG_TARGET_INITRAMFS_COMPRESSION_$(ALGO)), \
				echo "CONFIG_INITRAMFS_COMPRESSION_$(ALGO)=y" >> $(2)/.config; $\, \
				echo "# CONFIG_INITRAMFS_COMPRESSION_$(ALGO) is not set" >> $(2)/.config; $\) \
		) \
		$(call Kernel/SetRamdiskCompression,$(2)/.config) \
	}
  endef
else
endif

define Kernel/SetRamdiskCompression
	$(foreach ALGO,GZIP BZIP2 LZMA LZO XZ LZ4 ZSTD, \
		$(if $(CONFIG_TARGET_INITRAMFS_COMPRESSION_$(ALGO)), \
			echo "CONFIG_RD_$(ALGO)=y" >> $(1); $\, \
			echo "# CONFIG_RD_$(ALGO) is not set" >> $(1); $\) \
	)
endef

# With a separate initramfs, the initramfs kernel differs from the regular
# kernel only in ramdisk support. Enable it here too, so that one kernel
# build serves both images.
define Kernel/SetNoInitramfs
	mv $(LINUX_DIR)/.config.set $(LINUX_DIR)/.config.old
	grep -v INITRAMFS $(LINUX_DIR)/.config.old > $(LINUX_DIR)/.config.set
	echo 'CONFIG_INITRAMFS_SOURCE=""' >> $(LINUX_DIR)/.config.set
	echo '# CONFIG_INITRAMFS_FORCE is not set' >> $(LINUX_DIR)/.config.set
	echo "# CONFIG_INITRAMFS_PRESERVE_MTIME is not set" >> $(LINUX_DIR)/.config.set
	$(if $(CONFIG_TARGET_ROOTFS_INITRAMFS_SEPARATE), \
		echo 'CONFIG_BLK_DEV_INITRD=y' >> $(LINUX_DIR)/.config.set; \
		$(call Kernel/SetRamdiskCompression,$(LINUX_DIR)/.config.set))
endef

define Kernel/Configure/Default
	rm -f $(LINUX_DIR)/localversion
	$(LINUX_CONF_CMD) > $(LINUX_DIR)/.config.target
# copy CONFIG_KERNEL_* settings over to .config.target
	awk '/^(#[[:space:]]+)?CONFIG_KERNEL/{sub("CONFIG_KERNEL_","CONFIG_");print}' $(TOPDIR)/.config >> $(LINUX_DIR)/.config.target
	echo "# CONFIG_KALLSYMS_EXTRA_PASS is not set" >> $(LINUX_DIR)/.config.target
	echo "# CONFIG_KALLSYMS_ALL is not set" >> $(LINUX_DIR)/.config.target
	echo "CONFIG_KALLSYMS_UNCOMPRESSED=y" >> $(LINUX_DIR)/.config.target
	$(SCRIPT_DIR)/package-metadata.pl kconfig $(TMP_DIR)/.packageinfo $(TOPDIR)/.config $(KERNEL_PATCHVER) > $(LINUX_DIR)/.config.override
	$(SCRIPT_DIR)/kconfig.pl 'm+' '+' $(LINUX_DIR)/.config.target /dev/null $(LINUX_DIR)/.config.override > $(LINUX_DIR)/.config.set
	$(call Kernel/SetNoInitramfs)
	rm -rf $(KERNEL_BUILD_DIR)/modules
	cmp -s $(LINUX_DIR)/.config.set $(LINUX_DIR)/.config.prev || { \
		cp $(LINUX_DIR)/.config.set $(LINUX_DIR)/.config; \
		cp $(LINUX_DIR)/.config.set $(LINUX_DIR)/.config.prev; \
	}
	$(_SINGLE) [ -d $(LINUX_DIR)/user_headers ] || $(KERNEL_MAKE) $(if $(findstring uml,$(BOARD)),ARCH=$(ARCH)) INSTALL_HDR_PATH=$(LINUX_DIR)/user_headers headers_install
	grep '=[ym]' $(LINUX_DIR)/.config.set | LC_ALL=C sort | $(MKHASH) md5 > $(LINUX_DIR)/.vermagic
endef

define Kernel/Configure/Initramfs
	$(call Kernel/SetInitramfs,$(1),$(2))
endef

# The image link reads symtab.h through EXTRA_LDSFLAGS. It sits above
# $(LINUX_DIR), so the scan below has to name it. A package built module
# changes it without a change in the kernel tree.
KERNEL_MAKE_DEPENDS_image := $(if $(CONFIG_STRIP_KERNEL_EXPORTS),$(KERNEL_BUILD_DIR)/symtab.h)

# $(1): stamp name, $(2): make targets and options, $(3): commands to run before kbuild
# kbuild takes about half a second even if nothing changed, and runs twice
# per build. Skip it if the command line did not change since its last
# successful run, and no file in the kernel tree or the toolchain is newer.
# The build system files at the top of the kernel tree are not inputs of
# kbuild, apart from .config.
define Kernel/Make
	{ \
		stamp=$(LINUX_DIR)/.kbuild_$(1); \
		key="$$$$(printf '%s' '$(subst ','\'',$(KERNEL_MAKE) $(2))' | $(MKHASH) md5)"; \
		[ "$$$$(cat $$$$stamp 2>/dev/null)" = "$$$$key" ] && [ -z "$$$$(find $(LINUX_DIR) $(TOOLCHAIN_DIR)/bin \
			$(KERNEL_MAKE_DEPENDS_$(1)) \
			\( -path '$(LINUX_DIR)/.*' ! -path '$(LINUX_DIR)/.config' \) -prune -o \
			! -type d -newer $$$$stamp -print -quit)" ] || { \
			rm -f $$$$stamp; \
			$(3) \
			$(KERNEL_MAKE) $(2) && echo "$$$$key" > $$$$stamp; \
		}; \
	}
endef

define Kernel/CompileModules/Default
	+$(call Kernel/Make,modules,$(if $(KERNELNAME),$(KERNELNAME),all) modules, \
		rm -f $(LINUX_DIR)/vmlinux $(LINUX_DIR)/System.map;)
	# If .config did not change, use the previous timestamp to avoid package rebuilds
	cmp -s $(LINUX_DIR)/.config $(LINUX_DIR)/.config.modules.save && \
		mv $(LINUX_DIR)/.config.modules.save $(LINUX_DIR)/.config; \
	$(CP) $(LINUX_DIR)/.config $(LINUX_DIR)/.config.modules.save
endef

OBJCOPY_STRIP = -R .reginfo -R .notes -R .note -R .comment -R .mdebug -R .note.gnu.build-id

# AMD64 shares the location with x86
ifeq ($(LINUX_KARCH),x86_64)
IMAGES_DIR:=../../x86/boot
endif

# $1: image suffix
# $2: Per Device Rootfs ID
define Kernel/CopyImage
	cmp -s $(LINUX_DIR)$(2)/vmlinux $(KERNEL_BUILD_DIR)/vmlinux$(1).debug$(2) || { \
		$(KERNEL_CROSS)objcopy -O binary $(OBJCOPY_STRIP) -S $(LINUX_DIR)$(2)/vmlinux $(LINUX_KERNEL)$(1)$(2); \
		$(KERNEL_CROSS)objcopy $(OBJCOPY_STRIP) -S $(LINUX_DIR)$(2)/vmlinux $(KERNEL_BUILD_DIR)/vmlinux$(1).elf$(2); \
		$(CP) $(LINUX_DIR)$(2)/vmlinux $(KERNEL_BUILD_DIR)/vmlinux$(1).debug$(2); \
		$(foreach k, \
			$(if $(KERNEL_IMAGES),$(KERNEL_IMAGES),$(filter-out vmlinux dtbs,$(KERNELNAME))), \
			$(CP) $(LINUX_DIR)$(2)/arch/$(LINUX_KARCH)/boot/$(IMAGES_DIR)/$(k) $(KERNEL_BUILD_DIR)/$(k)$(1)$(2); \
		) \
	}
endef

define Kernel/CompileImage/Default
	rm -f $(TARGET_DIR)/init
	+$(call Kernel/Make,image,$(KERNEL_MAKEOPTS_IMAGE) $(if $(KERNELNAME),$(KERNELNAME),all))
	$(call Kernel/CopyImage)
endef

define Kernel/PrepareConfigPerRootfs
	{ \
		[ ! -d "$(1)" ] || rm -rf $(1); \
		mkdir $(1) && $(CP) -T $(LINUX_DIR) $(1); \
		touch $(1)/.config; \
	}
endef

ifneq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),)
# $1: Per Device Rootfs ID
# $2: suffix of the compressed initrd
Kernel/CacheInitrd = $(CACHE_RUN) $(KERNEL_BUILD_DIR)/cache/initrd$(1).cpio.$(2) \
	$(KERNEL_BUILD_DIR)/initrd$(1).cpio $(KERNEL_BUILD_DIR)/initrd$(1).cpio.$(2)

# $1: Per Device Rootfs ID
# The rootfs directory is shared with the image generation, so /init is
# appended to the finished archive instead of being staged in it.
define Kernel/AppendInitrdInit
	rm -rf $(KERNEL_BUILD_DIR)/initrd-init$(1); \
	mkdir -p $(KERNEL_BUILD_DIR)/initrd-init$(1); \
	$(INSTALL_BIN) $(INITRAMFS_INIT_FILE) $(KERNEL_BUILD_DIR)/initrd-init$(1)/init; \
	$(if $(SOURCE_DATE_EPOCH), \
		touch -hcd "@$(SOURCE_DATE_EPOCH)" $(KERNEL_BUILD_DIR)/initrd-init$(1)/init;) \
	( cd $(KERNEL_BUILD_DIR)/initrd-init$(1); echo ./init | \
		$(STAGING_DIR_HOST)/bin/cpio --reproducible -o -H newc -R 0:0 \
			-A -F $(KERNEL_BUILD_DIR)/initrd$(1).cpio )
endef

# $1: Custom TARGET_DIR. If omitted TARGET_DIR is used.
# $2: If defined Generate Per Rootfs Kernel Directory and use it
# For Separate Initramfs, the regular kernel is used as is, as its config
# already has the ramdisk support (see Kernel/SetNoInitramfs)
define Kernel/CompileImage/Initramfs
	$(call locked,{ \
		$(if $(2),$(call Kernel/PrepareConfigPerRootfs,$(LINUX_DIR)$(2));) \
		$(if $(CONFIG_TARGET_ROOTFS_INITRAMFS_SEPARATE),, \
			$(call Kernel/Configure/Initramfs,$(if $(1),$(1),$(TARGET_DIR)),$(LINUX_DIR)$(2)); \
			rm -rf $(LINUX_DIR)$(2)/usr/initramfs_data.cpio*;) \
		$(if $(CONFIG_TARGET_ROOTFS_INITRAMFS_SEPARATE), \
			$(call locked,{ \
				$(if $(call qstrip,$(CONFIG_EXTERNAL_CPIO)), \
					$(CP) $(CONFIG_EXTERNAL_CPIO) $(KERNEL_BUILD_DIR)/initrd$(2).cpio;,\
					( cd $(if $(1),$(1),$(TARGET_DIR)); find . | LC_ALL=C sort | $(STAGING_DIR_HOST)/bin/cpio --reproducible -o -H newc -R 0:0 > $(KERNEL_BUILD_DIR)/initrd$(2).cpio ); \
					$(call Kernel/AppendInitrdInit,$(2));) \
				$(if $(SOURCE_DATE_EPOCH), \
					touch -hcd "@$(SOURCE_DATE_EPOCH)" $(KERNEL_BUILD_DIR)/initrd$(2).cpio;) \
				$(if $(CONFIG_TARGET_INITRAMFS_COMPRESSION_BZIP2), \
					$(STAGING_DIR_HOST)/bin/bzip2 -9 -c < $(KERNEL_BUILD_DIR)/initrd$(2).cpio > $(KERNEL_BUILD_DIR)/initrd$(2).cpio.bzip2;) \
				$(if $(CONFIG_TARGET_INITRAMFS_COMPRESSION_GZIP), \
					$(call Kernel/CacheInitrd,$(2),gzip) \
					$(STAGING_DIR_HOST)/bin/libdeflate-gzip -n -k -f -S .gzip -12 $(KERNEL_BUILD_DIR)/initrd$(2).cpio;) \
				$(if $(CONFIG_TARGET_INITRAMFS_COMPRESSION_LZ4), \
					$(call Kernel/CacheInitrd,$(2),lz4) \
					$(STAGING_DIR_HOST)/bin/lz4c -l -c1 -fz --favor-decSpeed $(KERNEL_BUILD_DIR)/initrd$(2).cpio $(KERNEL_BUILD_DIR)/initrd$(2).cpio.lz4;) \
				$(if $(CONFIG_TARGET_INITRAMFS_COMPRESSION_LZMA), \
					$(call Kernel/CacheInitrd,$(2),lzma) \
					$(STAGING_DIR_HOST)/bin/lzma e -lc1 -lp2 -pb2 $(KERNEL_BUILD_DIR)/initrd$(2).cpio $(KERNEL_BUILD_DIR)/initrd$(2).cpio.lzma;) \
				$(if $(CONFIG_TARGET_INITRAMFS_COMPRESSION_LZO), \
					$(STAGING_DIR_HOST)/bin/lzop -9 -f $(KERNEL_BUILD_DIR)/initrd$(2).cpio;) \
				$(if $(CONFIG_TARGET_INITRAMFS_COMPRESSION_XZ), \
					$(call Kernel/CacheInitrd,$(2),xz) \
					$(STAGING_DIR_HOST)/bin/xz -T$(if $(filter 1,$(NPROC)),2,0) -9 -fzk --check=crc32 $(KERNEL_BUILD_DIR)/initrd$(2).cpio;) \
				$(if $(CONFIG_TARGET_INITRAMFS_COMPRESSION_ZSTD), \
					$(call Kernel/CacheInitrd,$(2),zstd) \
					$(STAGING_DIR_HOST)/bin/zstd -T0 -f -o $(KERNEL_BUILD_DIR)/initrd$(2).cpio.zstd $(KERNEL_BUILD_DIR)/initrd$(2).cpio;) \
			}, gen-cpio$(2));,\
			$(KERNEL_MAKE) $(if $(2),-C $(LINUX_DIR)$(2)) $(KERNEL_MAKEOPTS_IMAGE) $(if $(KERNELNAME),$(KERNELNAME),all);) \
		$(call Kernel/CopyImage,-initramfs,$(2)); \
		$(if $(2),rm -rf $(LINUX_DIR)$(2);) \
	}, gen-initramfs$(2));
endef
else
define Kernel/CompileImage/Initramfs
endef
endif

define Kernel/Clean/Default
	rm -f $(KERNEL_BUILD_DIR)/linux-$(LINUX_VERSION)/.configured
	rm -f $(LINUX_KERNEL)
	$(_SINGLE)$(MAKE) -C $(KERNEL_BUILD_DIR)/linux-$(LINUX_VERSION) clean
endef
