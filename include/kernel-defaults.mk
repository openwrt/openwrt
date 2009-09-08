#
# Copyright (C) 2006-2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

KERNELNAME=
ifneq (,$(findstring x86,$(BOARD)))
  KERNELNAME="bzImage"
endif
ifneq (,$(findstring rdc,$(BOARD)))
  KERNELNAME="bzImage"
endif
ifneq (,$(findstring avr32,$(BOARD)))
  KERNELNAME="uImage"
endif

KERNEL_MAKEOPTS := -C $(LINUX_DIR) \
	CROSS_COMPILE="$(KERNEL_CROSS)" \
	ARCH="$(LINUX_KARCH)" \
	KBUILD_HAVE_NLS=no \
	CONFIG_SHELL="$(BASH)"

ifdef CONFIG_STRIP_KERNEL_EXPORTS
  KERNEL_MAKEOPTS += \
	EXTRA_LDSFLAGS="-I$(KERNEL_BUILD_DIR) -include symtab.h"
endif

INITRAMFS_EXTRA_FILES ?= $(GENERIC_PLATFORM_DIR)/image/initramfs-base-files.txt

ifneq (,$(KERNEL_CC))
  KERNEL_MAKEOPTS += CC="$(KERNEL_CC)"
endif

# defined in quilt.mk
Kernel/Patch:=$(Kernel/Patch/Default)
ifeq ($(strip $(CONFIG_EXTERNAL_KERNEL_TREE)),"")
  ifeq ($(strip $(CONFIG_KERNEL_GIT_CLONE_URI)),"")
    define Kernel/Prepare/Default
	bzcat $(DL_DIR)/$(LINUX_SOURCE) | $(TAR) -C $(KERNEL_BUILD_DIR) $(TAR_OPTIONS)
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

ifeq ($(KERNEL),2.6)
  ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
    define Kernel/SetInitramfs
		mv $(LINUX_DIR)/.config $(LINUX_DIR)/.config.old
		grep -v -e INITRAMFS -e CONFIG_RD_ $(LINUX_DIR)/.config.old > $(LINUX_DIR)/.config
		echo 'CONFIG_INITRAMFS_SOURCE="$(strip $(TARGET_DIR) $(INITRAMFS_EXTRA_FILES))"' >> $(LINUX_DIR)/.config
		echo 'CONFIG_INITRAMFS_ROOT_UID=$(shell id -u)' >> $(LINUX_DIR)/.config
		echo 'CONFIG_INITRAMFS_ROOT_GID=$(shell id -g)' >> $(LINUX_DIR)/.config
		echo "$(if $(CONFIG_TARGET_INITRAMFS_COMPRESSION_NONE),CONFIG_INITRAMFS_COMPRESSION_NONE=y,# CONFIG_INITRAMFS_COMPRESSION_NONE is not set)" >> $(LINUX_DIR)/.config
		echo -e "$(if $(CONFIG_TARGET_INITRAMFS_COMPRESSION_GZIP),CONFIG_INITRAMFS_COMPRESSION_GZIP=y\nCONFIG_RD_GZIP=y,# CONFIG_INITRAMFS_COMPRESSION_GZIP is not set\n# CONFIG_RD_GZIP is not set)" >> $(LINUX_DIR)/.config
		echo -e "$(if $(CONFIG_TARGET_INITRAMFS_COMPRESSION_BZIP2),CONFIG_INITRAMFS_COMPRESSION_BZIP2=y\nCONFIG_RD_BZIP2=y,# CONFIG_INITRAMFS_COMPRESSION_BZIP2 is not set\n# CONFIG_RD_BZIP2 is not set)" >> $(LINUX_DIR)/.config
		echo -e "$(if $(CONFIG_TARGET_INITRAMFS_COMPRESSION_LZMA),CONFIG_INITRAMFS_COMPRESSION_LZMA=y\nCONFIG_RD_LZMA=y,# CONFIG_INITRAMFS_COMPRESSION_LZMA is not set\n# CONFIG_RD_LZMA is not set)" >> $(LINUX_DIR)/.config
    endef
  else
    define Kernel/SetInitramfs
		mv $(LINUX_DIR)/.config $(LINUX_DIR)/.config.old
		grep -v INITRAMFS $(LINUX_DIR)/.config.old > $(LINUX_DIR)/.config
		echo 'CONFIG_INITRAMFS_SOURCE=""' >> $(LINUX_DIR)/.config
    endef
  endif
endif

define Kernel/Configure/2.4
	$(SED) "s,\-mcpu=,\-mtune=,g;" $(LINUX_DIR)/arch/mips/Makefile
	$(_SINGLE)$(MAKE) $(KERNEL_MAKEOPTS) oldconfig include/linux/compile.h include/linux/version.h
	$(_SINGLE)$(MAKE) $(KERNEL_MAKEOPTS) dep
endef
define Kernel/Configure/2.6
	-$(_SINGLE)$(MAKE) $(KERNEL_MAKEOPTS) oldconfig prepare scripts
endef
define Kernel/Configure/Default
	$(LINUX_CONFCMD) > $(LINUX_DIR)/.config.target
	echo "$(if $(CONFIG_KERNEL_KALLSYMS),CONFIG_KALLSYMS=y,# CONFIG_KALLSYMS is not set)" >> $(LINUX_DIR)/.config.target
	echo "$(if $(CONFIG_KERNEL_PROFILING),CONFIG_PROFILING=y,# CONFIG_PROFILING is not set)" >> $(LINUX_DIR)/.config.target
	echo "$(if $(CONFIG_KERNEL_DEBUG_FS),CONFIG_DEBUG_FS=y,# CONFIG_DEBUG_FS is not set)" >> $(LINUX_DIR)/.config.target
	echo "# CONFIG_KALLSYMS_EXTRA_PASS is not set" >> $(LINUX_DIR)/.config.target
	echo "# CONFIG_KALLSYMS_ALL is not set" >> $(LINUX_DIR)/.config.target
	echo "# CONFIG_KPROBES is not set" >> $(LINUX_DIR)/.config.target
	$(SED) 's,.*CONFIG_AEABI.*,$(if $(CONFIG_EABI_SUPPORT),CONFIG_AEABI=y,# CONFIG_AEABI is not set),' $(LINUX_DIR)/.config.target
	$(if $(CONFIG_EABI_SUPPORT),echo '# CONFIG_OABI_COMPAT is not set' >> $(LINUX_DIR)/.config.target)
	$(SCRIPT_DIR)/metadata.pl kconfig $(TMP_DIR)/.packageinfo $(TOPDIR)/.config > $(LINUX_DIR)/.config.override
	$(SCRIPT_DIR)/kconfig.pl 'm+' $(LINUX_DIR)/.config.target $(LINUX_DIR)/.config.override > $(LINUX_DIR)/.config
	$(call Kernel/SetInitramfs)
	$(call Kernel/Configure/$(KERNEL))
	rm -rf $(KERNEL_BUILD_DIR)/modules
endef

define Kernel/CompileModules/Default
	rm -f $(LINUX_DIR)/vmlinux $(LINUX_DIR)/System.map
	+$(MAKE) $(KERNEL_MAKEOPTS) modules
endef

OBJCOPY_STRIP = -R .reginfo -R .notes -R .note -R .comment -R .mdebug -R .note.gnu.build-id

define Kernel/CompileImage/Default
	$(if $(CONFIG_TARGET_ROOTFS_INITRAMFS),,rm -f $(TARGET_DIR)/init)
	+$(MAKE) $(KERNEL_MAKEOPTS) $(KERNELNAME)
	$(KERNEL_CROSS)objcopy -O binary $(OBJCOPY_STRIP) -S $(LINUX_DIR)/vmlinux $(LINUX_KERNEL)
	$(KERNEL_CROSS)objcopy $(OBJCOPY_STRIP) -S $(LINUX_DIR)/vmlinux $(KERNEL_BUILD_DIR)/vmlinux.elf
endef

define Kernel/Clean/Default
	rm -f $(KERNEL_BUILD_DIR)/linux-$(LINUX_VERSION)/.configured
	rm -f $(LINUX_KERNEL)
	$(_SINGLE)$(MAKE) -C $(KERNEL_BUILD_DIR)/linux-$(LINUX_VERSION) clean
endef


