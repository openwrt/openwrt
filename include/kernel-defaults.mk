#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

KERNEL_MAKEOPTS := -C $(LINUX_DIR) \
	HOSTCFLAGS="$(HOST_CFLAGS) -Wall -Wmissing-prototypes -Wstrict-prototypes" \
	CROSS_COMPILE="$(KERNEL_CROSS)" \
	ARCH="$(LINUX_KARCH)" \
	KBUILD_HAVE_NLS=no \
	CONFIG_SHELL="$(BASH)" \
	$(if $(findstring c,$(OPENWRT_VERBOSE)),V=1,V='')

ifdef CONFIG_STRIP_KERNEL_EXPORTS
  KERNEL_MAKEOPTS += \
	EXTRA_LDSFLAGS="-I$(KERNEL_BUILD_DIR) -include symtab.h"
endif

INITRAMFS_EXTRA_FILES ?= $(GENERIC_PLATFORM_DIR)/image/initramfs-base-files.txt

ifneq (,$(KERNEL_CC))
  KERNEL_MAKEOPTS += CC="$(KERNEL_CC)"
endif

export HOST_EXTRACFLAGS=-I$(STAGING_DIR_HOST)/include

# defined in quilt.mk
Kernel/Patch:=$(Kernel/Patch/Default)
ifeq ($(strip $(CONFIG_EXTERNAL_KERNEL_TREE)),"")
  ifeq ($(strip $(CONFIG_KERNEL_GIT_CLONE_URI)),"")
    define Kernel/Prepare/Default
	xzcat $(DL_DIR)/$(LINUX_SOURCE) | $(TAR) -C $(KERNEL_BUILD_DIR) $(TAR_OPTIONS)
	$(Kernel/Patch)
	touch $(LINUX_DIR)/.quilt_used
    endef
  else
    ifeq ($(strip $(CONFIG_KERNEL_GIT_LOCAL_REPOSITORY)),"")
      define Kernel/Prepare/Default
	git clone $(CONFIG_KERNEL_GIT_CLONE_URI) $(LINUX_DIR)
      endef
    else
      define Kernel/Prepare/Default
	git clone --reference $(CONFIG_KERNEL_GIT_LOCAL_REPOSITORY) $(CONFIG_KERNEL_GIT_CLONE_URI) $(LINUX_DIR)
      endef
    endif
  endif
else
  define Kernel/Prepare/Default
	mkdir -p $(KERNEL_BUILD_DIR)
	if [ -d $(LINUX_DIR) ]; then \
		rmdir $(LINUX_DIR); \
	fi
	ln -s $(CONFIG_EXTERNAL_KERNEL_TREE) $(LINUX_DIR)
  endef
endif

ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
  ifeq ($(strip $(CONFIG_EXTERNAL_CPIO)),"")
    define Kernel/SetInitramfs/PreConfigure
	grep -v -e INITRAMFS -e CONFIG_RD_ -e CONFIG_BLK_DEV_INITRD $(LINUX_DIR)/.config.old > $(LINUX_DIR)/.config
	echo 'CONFIG_BLK_DEV_INITRD=y' >> $(LINUX_DIR)/.config
	echo 'CONFIG_INITRAMFS_SOURCE="$(strip $(TARGET_DIR) $(INITRAMFS_EXTRA_FILES))"' >> $(LINUX_DIR)/.config
    endef
  else
    define Kernel/SetInitramfs/PreConfigure
	grep -v INITRAMFS $(LINUX_DIR)/.config.old > $(LINUX_DIR)/.config
	echo 'CONFIG_INITRAMFS_SOURCE="$(call qstrip,$(CONFIG_EXTERNAL_CPIO))"' >> $(LINUX_DIR)/.config
    endef
  endif

  define Kernel/SetInitramfs
	mv $(LINUX_DIR)/.config $(LINUX_DIR)/.config.old
	$(call Kernel/SetInitramfs/PreConfigure)
	echo 'CONFIG_INITRAMFS_ROOT_UID=$(shell id -u)' >> $(LINUX_DIR)/.config
	echo 'CONFIG_INITRAMFS_ROOT_GID=$(shell id -g)' >> $(LINUX_DIR)/.config
	echo "$(if $(CONFIG_TARGET_INITRAMFS_COMPRESSION_NONE),CONFIG_INITRAMFS_COMPRESSION_NONE=y,# CONFIG_INITRAMFS_COMPRESSION_NONE is not set)" >> $(LINUX_DIR)/.config
	echo -e "$(if $(CONFIG_TARGET_INITRAMFS_COMPRESSION_GZIP),CONFIG_INITRAMFS_COMPRESSION_GZIP=y\nCONFIG_RD_GZIP=y,# CONFIG_INITRAMFS_COMPRESSION_GZIP is not set\n# CONFIG_RD_GZIP is not set)" >> $(LINUX_DIR)/.config
	echo -e "$(if $(CONFIG_TARGET_INITRAMFS_COMPRESSION_BZIP2),CONFIG_INITRAMFS_COMPRESSION_BZIP2=y\nCONFIG_RD_BZIP2=y,# CONFIG_INITRAMFS_COMPRESSION_BZIP2 is not set\n# CONFIG_RD_BZIP2 is not set)" >> $(LINUX_DIR)/.config
	echo -e "$(if $(CONFIG_TARGET_INITRAMFS_COMPRESSION_LZMA),CONFIG_INITRAMFS_COMPRESSION_LZMA=y\nCONFIG_RD_LZMA=y,# CONFIG_INITRAMFS_COMPRESSION_LZMA is not set\n# CONFIG_RD_LZMA is not set)" >> $(LINUX_DIR)/.config
	echo -e "$(if $(CONFIG_TARGET_INITRAMFS_COMPRESSION_LZO),CONFIG_INITRAMFS_COMPRESSION_LZO=y\nCONFIG_RD_LZO=y,# CONFIG_INITRAMFS_COMPRESSION_LZO is not set\n# CONFIG_RD_LZO is not set)" >> $(LINUX_DIR)/.config
	echo -e "$(if $(CONFIG_TARGET_INITRAMFS_COMPRESSION_XZ),CONFIG_INITRAMFS_COMPRESSION_XZ=y\nCONFIG_RD_XZ=y,# CONFIG_INITRAMFS_COMPRESSION_XZ is not set\n# CONFIG_RD_XZ is not set)" >> $(LINUX_DIR)/.config
  endef
else
  define Kernel/SetInitramfs
	mv $(LINUX_DIR)/.config $(LINUX_DIR)/.config.old
	grep -v INITRAMFS $(LINUX_DIR)/.config.old > $(LINUX_DIR)/.config
	echo 'CONFIG_INITRAMFS_SOURCE=""' >> $(LINUX_DIR)/.config
  endef
endif

define Kernel/Configure/Default
	$(LINUX_CONF_CMD) > $(LINUX_DIR)/.config.target
# copy CONFIG_KERNEL_* settings over to .config.target
	awk '/^(#[[:space:]]+)?CONFIG_KERNEL/{sub("CONFIG_KERNEL_","CONFIG_");print}' $(TOPDIR)/.config >> $(LINUX_DIR)/.config.target
	echo "# CONFIG_KALLSYMS_EXTRA_PASS is not set" >> $(LINUX_DIR)/.config.target
	echo "# CONFIG_KALLSYMS_ALL is not set" >> $(LINUX_DIR)/.config.target
	echo "# CONFIG_KALLSYMS_UNCOMPRESSED is not set" >> $(LINUX_DIR)/.config.target
	echo "# CONFIG_KPROBES is not set" >> $(LINUX_DIR)/.config.target
	$(SCRIPT_DIR)/metadata.pl kconfig $(TMP_DIR)/.packageinfo $(TOPDIR)/.config > $(LINUX_DIR)/.config.override
	$(SCRIPT_DIR)/kconfig.pl 'm+' '+' $(LINUX_DIR)/.config.target /dev/null $(LINUX_DIR)/.config.override > $(LINUX_DIR)/.config
	$(call Kernel/SetInitramfs)
	rm -rf $(KERNEL_BUILD_DIR)/modules
	[ -d $(LINUX_DIR)/user_headers ] || $(MAKE) $(KERNEL_MAKEOPTS) INSTALL_HDR_PATH=$(LINUX_DIR)/user_headers headers_install
	$(SH_FUNC) grep '=[ym]' $(LINUX_DIR)/.config | LC_ALL=C sort | md5s > $(LINUX_DIR)/.vermagic
endef

define Kernel/CompileModules/Default
	rm -f $(LINUX_DIR)/vmlinux $(LINUX_DIR)/System.map
	+$(MAKE) $(KERNEL_MAKEOPTS) modules
endef

OBJCOPY_STRIP = -R .reginfo -R .notes -R .note -R .comment -R .mdebug -R .note.gnu.build-id

define Kernel/CompileImage/Default
	$(if $(CONFIG_TARGET_ROOTFS_INITRAMFS),,rm -f $(TARGET_DIR)/init)
	+$(MAKE) $(KERNEL_MAKEOPTS) $(subst ",,$(KERNELNAME))
	$(KERNEL_CROSS)objcopy -O binary $(OBJCOPY_STRIP) -S $(LINUX_DIR)/vmlinux $(LINUX_KERNEL)
	$(KERNEL_CROSS)objcopy $(OBJCOPY_STRIP) -S $(LINUX_DIR)/vmlinux $(KERNEL_BUILD_DIR)/vmlinux.elf
endef

define Kernel/Clean/Default
	rm -f $(KERNEL_BUILD_DIR)/linux-$(LINUX_VERSION)/.configured
	rm -f $(LINUX_KERNEL)
	$(_SINGLE)$(MAKE) -C $(KERNEL_BUILD_DIR)/linux-$(LINUX_VERSION) clean
endef


