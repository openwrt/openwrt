# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2006-2020 OpenWrt.org

include $(INCLUDE_DIR)/prereq.mk
include $(INCLUDE_DIR)/depends.mk

ifneq ($(DUMP),1)
  all: compile
endif

KERNEL_FILE_DEPENDS=$(GENERIC_BACKPORT_DIR) $(GENERIC_PATCH_DIR) $(GENERIC_HACK_DIR) $(PATCH_DIR) $(GENERIC_FILES_DIR) $(FILES_DIR)
STAMP_PREPARED=$(LINUX_DIR)/.prepared$(if $(QUILT)$(DUMP),,_$(shell $(call $(if $(CONFIG_AUTOREMOVE),find_md5_reproducible,find_md5),$(KERNEL_FILE_DEPENDS),)))
STAMP_CONFIGURED:=$(LINUX_DIR)/.configured
include $(INCLUDE_DIR)/download.mk
include $(INCLUDE_DIR)/quilt.mk
include $(INCLUDE_DIR)/kernel-defaults.mk

define Kernel/Prepare
	$(call Kernel/Prepare/Default)
endef

define Kernel/Configure
	$(call Kernel/Configure/Default)
endef

define Kernel/CompileModules
	$(call Kernel/CompileModules/Default)
endef

define Kernel/CompileImage
	$(call Kernel/CompileImage/Default)
	$(call Kernel/CompileImage/Initramfs)
endef

define Kernel/Clean
	$(call Kernel/Clean/Default)
endef

define Download/kernel
  URL:=$(LINUX_SITE)
  FILE:=$(LINUX_SOURCE)
  HASH:=$(LINUX_KERNEL_HASH)
endef

KERNEL_GIT_OPTS:=
ifneq ($(strip $(CONFIG_KERNEL_GIT_LOCAL_REPOSITORY)),"")
  KERNEL_GIT_OPTS+=--reference $(CONFIG_KERNEL_GIT_LOCAL_REPOSITORY)
endif

define Download/git-kernel
  URL:=$(call qstrip,$(CONFIG_KERNEL_GIT_CLONE_URI))
  PROTO:=git
  SOURCE_VERSION:=$(CONFIG_KERNEL_GIT_REF)
  FILE:=$(LINUX_SOURCE)
  SUBDIR:=linux-$(LINUX_VERSION)
  OPTS:=$(KERNEL_GIT_OPTS)
endef

ifdef CONFIG_COLLECT_KERNEL_DEBUG
  define Kernel/CollectDebug
	rm -rf $(KERNEL_BUILD_DIR)/debug
	mkdir -p $(KERNEL_BUILD_DIR)/debug/modules
	$(CP) $(LINUX_DIR)/vmlinux $(KERNEL_BUILD_DIR)/debug/
	-$(CP) \
		$(STAGING_DIR_ROOT)/lib/modules/$(LINUX_VERSION)/*.ko \
		$(KERNEL_BUILD_DIR)/debug/modules/
	$(FIND) $(KERNEL_BUILD_DIR)/debug -type f | $(XARGS) $(KERNEL_CROSS)strip --only-keep-debug
	$(TAR) c -C $(KERNEL_BUILD_DIR) debug \
		$(if $(SOURCE_DATE_EPOCH),--mtime="@$(SOURCE_DATE_EPOCH)") \
		| zstd -T0 -f -o $(BIN_DIR)/kernel-debug.tar.zst
  endef
endif

ifeq ($(DUMP)$(filter prereq clean refresh update,$(MAKECMDGOALS)),)
  ifneq ($(if $(QUILT),,$(CONFIG_AUTOREBUILD)),)
    define Kernel/Autoclean
      $(PKG_BUILD_DIR)/.dep_files: $(STAMP_PREPARED)
      $(call rdep,$(KERNEL_FILE_DEPENDS),$(STAMP_PREPARED),$(PKG_BUILD_DIR)/.dep_files,-x "*/.dep_*")
    endef
  endif
endif

KERNEL_CONFIG_CHECK:=$(LINUX_DIR)/.configured-check
KERNEL_CONFIG_DEPENDS:=$(LINUX_KCONFIG_LIST) $(TOPDIR)/.config $(TMP_DIR)/.packageinfo \
	$(SCRIPT_DIR)/kconfig.pl $(SCRIPT_DIR)/package-metadata.pl $(SCRIPT_DIR)/metadata.pm \
	$(INCLUDE_DIR)/kernel-defaults.mk $(INCLUDE_DIR)/kernel-build.mk $(CURDIR)

IMAGE_INSTALL_STAMP:=$(KERNEL_BUILD_DIR)/.image_install
IMAGE_INSTALL_INPUTS:=$(LINUX_DIR) $(STAGING_DIR)/image \
	$(STAGING_DIR_HOST)/bin $(CURDIR) $(INCLUDE_DIR) $(SCRIPT_DIR) $(TOPDIR)/.config
IMAGE_INSTALL_PRUNE:=\( -path '$(LINUX_DIR)/.*' ! -path '$(LINUX_DIR)/.config' \)
image_install_state = ( \
		cd $(TARGET_DIR) && \
		find . -printf '%P %y %m %U %G %s %l\n' | LC_ALL=C sort && \
		find . -type f -print0 | LC_ALL=C sort -z | xargs -0 -r cat; \
	) | $(MKHASH) sha256; \
	find $(BIN_DIR) -maxdepth 1 -type f \
		! -name sha256sums ! -name profiles.json ! -name '*.buildinfo' \
		-printf '%f %s\n' 2>/dev/null | LC_ALL=C sort; \
	find $(BUILD_DIR)/json_info_files -maxdepth 1 -type f \
		-printf '%f %s\n' 2>/dev/null | LC_ALL=C sort
image_install_newer = \
	find $(KERNEL_BUILD_DIR) -maxdepth 1 ! -type d \
		-newer $(IMAGE_INSTALL_STAMP) -print -quit 2>/dev/null; \
	find $(IMAGE_INSTALL_INPUTS) $(IMAGE_INSTALL_PRUNE) -prune -o \
		! -type d -newer $(IMAGE_INSTALL_STAMP) -print -quit 2>/dev/null

define BuildKernel
  $(if $(QUILT),$(Build/Quilt))
  $(if $(LINUX_SITE),$(call Download,kernel))
  $(if $(call qstrip,$(CONFIG_KERNEL_GIT_CLONE_URI)),$(call Download,git-kernel))

  .NOTPARALLEL:

  $(Kernel/Autoclean)
  $(STAMP_PREPARED): $(if $(LINUX_SITE),$(DL_DIR)/$(LINUX_SOURCE))
	$(call BuildTimeLog,begin,prepare)
	-rm -rf $(KERNEL_BUILD_DIR)
	-mkdir -p $(KERNEL_BUILD_DIR)
	$(Kernel/Prepare)
	touch $$@
	$(call BuildTimeLog,end,prepare)

  $(KERNEL_BUILD_DIR)/symtab.h: FORCE
	$(call BuildTimeLog,begin,compile)
	rm -f $(KERNEL_BUILD_DIR)/symtab.h
	touch $(KERNEL_BUILD_DIR)/symtab.h
	+$(KERNEL_MAKE) vmlinux
	find $(LINUX_DIR) $(STAGING_DIR_ROOT)/lib/modules -name \*.ko | \
		xargs $(TARGET_CROSS)nm | \
		awk '$$$$1 == "U" { print $$$$2 } ' | \
		sort -u > $(KERNEL_BUILD_DIR)/mod_symtab.txt
	$(TARGET_CROSS)nm -n $(LINUX_DIR)/vmlinux.o | awk '/^[0-9a-f]+ [rR] __ksymtab_/ {print substr($$$$3,11)}' > $(KERNEL_BUILD_DIR)/kernel_symtab.txt
	grep -Ff $(KERNEL_BUILD_DIR)/mod_symtab.txt $(KERNEL_BUILD_DIR)/kernel_symtab.txt > $(KERNEL_BUILD_DIR)/sym_include.txt
	grep -Fvf $(KERNEL_BUILD_DIR)/mod_symtab.txt $(KERNEL_BUILD_DIR)/kernel_symtab.txt > $(KERNEL_BUILD_DIR)/sym_exclude.txt
	( \
		echo '#define SYMTAB_KEEP \'; \
		cat $(KERNEL_BUILD_DIR)/sym_include.txt | \
			awk '{print "KEEP(*(___ksymtab+" $$$$1 ")) \\" }'; \
		echo; \
		echo '#define SYMTAB_KEEP_GPL \'; \
		cat $(KERNEL_BUILD_DIR)/sym_include.txt | \
			awk '{print "KEEP(*(___ksymtab_gpl+" $$$$1 ")) \\" }'; \
		echo; \
		echo '#define SYMTAB_DISCARD \'; \
		cat $(KERNEL_BUILD_DIR)/sym_exclude.txt | \
			awk '{print "*(___ksymtab+" $$$$1 ") \\" }'; \
		echo; \
		echo '#define SYMTAB_DISCARD_GPL \'; \
		cat $(KERNEL_BUILD_DIR)/sym_exclude.txt | \
			awk '{print "*(___ksymtab_gpl+" $$$$1 ") \\" }'; \
		echo; \
	) > $$@
	$(call BuildTimeLog,end,compile)

  $(KERNEL_CONFIG_CHECK): $(STAMP_PREPARED) $(LINUX_KCONFIG_LIST) $(TOPDIR)/.config FORCE
	@mkdir -p $(LINUX_DIR)
	@[ -f $(STAMP_CONFIGURED) ] && [ -d $(LINUX_DIR)/user_headers ] && \
		[ -z "$$$$(find $(KERNEL_CONFIG_DEPENDS) ! -type d \
			-newer $(STAMP_CONFIGURED) -print -quit 2>/dev/null)" ] || \
		touch $$@

  $(STAMP_CONFIGURED): $(KERNEL_CONFIG_CHECK)
	$(call BuildTimeLog,begin,configure)
	$(Kernel/Configure)
	touch $$@
	$(call BuildTimeLog,end,configure)

  $(LINUX_DIR)/.modules: export STAGING_PREFIX=$$(STAGING_DIR_HOST)
  $(LINUX_DIR)/.modules: export PKG_CONFIG_PATH=$$(STAGING_DIR_HOST)/lib/pkgconfig
  $(LINUX_DIR)/.modules: export PKG_CONFIG_LIBDIR=$$(STAGING_DIR_HOST)/lib/pkgconfig
  $(LINUX_DIR)/.modules: export FAIL_ON_UNCONFIGURED=1
  $(LINUX_DIR)/.modules: $(STAMP_CONFIGURED) $(LINUX_DIR)/.config FORCE
	$(call BuildTimeLog,begin,compile)
	$(Kernel/CompileModules)
	touch $$@
	$(call BuildTimeLog,end,compile)

  $(LINUX_DIR)/.image: export STAGING_PREFIX=$$(STAGING_DIR_HOST)
  $(LINUX_DIR)/.image: export PKG_CONFIG_PATH=$$(STAGING_DIR_HOST)/lib/pkgconfig
  $(LINUX_DIR)/.image: export PKG_CONFIG_LIBDIR=$$(STAGING_DIR_HOST)/lib/pkgconfig
  $(LINUX_DIR)/.image: $(STAMP_CONFIGURED) $(if $(CONFIG_STRIP_KERNEL_EXPORTS),$(KERNEL_BUILD_DIR)/symtab.h) FORCE
	$(call BuildTimeLog,begin,compile)
	$(Kernel/CompileImage)
	$(Kernel/CollectDebug)
	touch $$@
	$(call BuildTimeLog,end,compile)
	
  mostlyclean: FORCE
	$(Kernel/Clean)

  define BuildKernel
  endef

  download: $(if $(LINUX_SITE),$(DL_DIR)/$(LINUX_SOURCE))
  prepare: $(STAMP_PREPARED)
  compile: $(LINUX_DIR)/.modules
	+$(MAKE) -C image compile TARGET_BUILD=

  dtb: $(STAMP_CONFIGURED)
	$(_SINGLE)$(KERNEL_MAKE) scripts_dtc
	$(MAKE) -C image compile-dtb TARGET_BUILD=

  oldconfig menuconfig nconfig xconfig: $(STAMP_PREPARED) $(STAMP_CHECKED) FORCE
	rm -f $(LINUX_DIR)/.config.prev
	rm -f $(STAMP_CONFIGURED)
	$(LINUX_RECONF_CMD) > $(LINUX_DIR)/.config
	$(_SINGLE)$(KERNEL_MAKE) \
		$(if $(findstring Darwin,$(HOST_OS)), \
			HOSTLDLIBS_mconf="-L$(STAGING_DIR_HOST)/lib -lncurses" \
			filechk_conf_cfg="	:" \
		) \
		YACC=$(STAGING_DIR_HOST)/bin/bison \
		$$@
	$(call LINUX_RECONF_DIFF,$(LINUX_DIR)/.config) > $(LINUX_RECONFIG_TARGET)

  install: $(LINUX_DIR)/.image
	+{ \
		state="$$$$( $(image_install_state) )"; \
		[ "$$$$state" = "$$$$(cat $(IMAGE_INSTALL_STAMP) 2>/dev/null)" ] && \
		[ -z "$$$$($(image_install_newer))" ] || { \
			rm -f $(IMAGE_INSTALL_STAMP); \
			$(MAKE) -C image compile install TARGET_BUILD= && \
			{ $(image_install_state); } > $(IMAGE_INSTALL_STAMP); \
		}; \
	}

  clean: FORCE
	rm -rf $(KERNEL_BUILD_DIR)

  image-prereq:
	@+$(NO_TRACE_MAKE) -s -C image prereq TARGET_BUILD=

  prereq: image-prereq

endef
