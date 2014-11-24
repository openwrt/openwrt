# 
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

ifeq ($(__target_inc),)
  include $(INCLUDE_DIR)/target.mk
endif

ifeq ($(DUMP),1)
  KERNEL?=<KERNEL>
  BOARD?=<BOARD>
  LINUX_VERSION?=<LINUX_VERSION>
  LINUX_VERMAGIC?=<LINUX_VERMAGIC>
else
  ifeq ($(CONFIG_EXTERNAL_TOOLCHAIN),)
    export GCC_HONOUR_COPTS=s
  endif

  LINUX_KMOD_SUFFIX=ko

  ifneq (,$(findstring uml,$(BOARD)))
    KERNEL_CC?=$(HOSTCC)
    KERNEL_CROSS?=
  else
    KERNEL_CC?=$(TARGET_CC)
    KERNEL_CROSS?=$(TARGET_CROSS)
  endif

  ifeq ($(TARGET_BUILD),1)
    PATCH_DIR ?= ./patches$(if $(wildcard ./patches-$(KERNEL_PATCHVER)),-$(KERNEL_PATCHVER))
    FILES_DIR ?= $(foreach dir,$(wildcard ./files ./files-$(KERNEL_PATCHVER)),"$(dir)")
  endif
  KERNEL_BUILD_DIR ?= $(BUILD_DIR)/linux-$(BOARD)$(if $(SUBTARGET),_$(SUBTARGET))
  LINUX_DIR ?= $(KERNEL_BUILD_DIR)/linux-$(LINUX_VERSION)
  ifeq ($(strip $(call CompareKernelPatchVer,$(KERNEL_PATCHVER),ge,3.7.0)),1)
    LINUX_UAPI_DIR=uapi/
  endif
  LINUX_VERMAGIC:=$(strip $(shell cat $(LINUX_DIR)/.vermagic 2>/dev/null))
  LINUX_VERMAGIC:=$(if $(LINUX_VERMAGIC),$(LINUX_VERMAGIC),unknown)

  LINUX_UNAME_VERSION:=$(if $(word 3,$(subst ., ,$(KERNEL_BASE))),$(KERNEL_BASE),$(KERNEL_BASE).0)
  ifneq ($(findstring -rc,$(LINUX_VERSION)),)
    LINUX_UNAME_VERSION:=$(LINUX_UNAME_VERSION)-$(strip $(lastword $(subst -, ,$(LINUX_VERSION))))
  endif

  MODULES_SUBDIR:=lib/modules/$(LINUX_UNAME_VERSION)
  TARGET_MODULES_DIR := $(LINUX_TARGET_DIR)/$(MODULES_SUBDIR)

  LINUX_KERNEL:=$(KERNEL_BUILD_DIR)/vmlinux

  LINUX_SOURCE:=linux-$(LINUX_VERSION).tar.xz
  TESTING:=$(if $(findstring -rc,$(LINUX_VERSION)),/testing,)
  ifeq ($(call qstrip,$(CONFIG_EXTERNAL_KERNEL_TREE))$(call qstrip,$(CONFIG_KERNEL_GIT_CLONE_URI)),)
      LINUX_SITE:=@KERNEL/linux/kernel/v3.x$(TESTING)
  endif

  ifneq ($(TARGET_BUILD),1)
    PKG_BUILD_DIR ?= $(KERNEL_BUILD_DIR)/$(PKG_NAME)$(if $(PKG_VERSION),-$(PKG_VERSION))
  endif
endif

ifneq (,$(findstring uml,$(BOARD)))
  LINUX_KARCH=um
else
  ifeq (,$(LINUX_KARCH))
    LINUX_KARCH=$(strip $(subst i386,x86,$(subst armeb,arm,$(subst mipsel,mips,$(subst mips64,mips,$(subst mips64el,mips,$(subst sh2,sh,$(subst sh3,sh,$(subst sh4,sh,$(subst aarch64,arm64,$(subst aarch64_be,arm64,$(ARCH))))))))))))
  endif
endif


define KernelPackage/Defaults
  FILES:=
  AUTOLOAD:=
endef

define ModuleAutoLoad
	$(SH_FUNC) \
	export modules=; \
	probe_module() { \
		mods="$$$$$$$$1"; \
		boot="$$$$$$$$2"; \
		shift 2; \
		for mod in $$$$$$$$mods; do \
			mkdir -p $(2)/etc/modules.d; \
			echo "$$$$$$$$mod" >> $(2)/etc/modules.d/$(1); \
		done; \
		if [ -e $(2)/etc/modules.d/$(1) ]; then \
			if [ "$$$$$$$$boot" = "1" ]; then \
				mkdir -p $(2)/etc/modules-boot.d; \
				ln -s ../modules.d/$(1) $(2)/etc/modules-boot.d/; \
			fi; \
			modules="$$$$$$$${modules:+$$$$$$$$modules}"; \
		fi; \
	}; \
	add_module() { \
		priority="$$$$$$$$1"; \
		mods="$$$$$$$$2"; \
		boot="$$$$$$$$3"; \
		shift 3; \
		for mod in $$$$$$$$mods; do \
			mkdir -p $(2)/etc/modules.d; \
			echo "$$$$$$$$mod" >> $(2)/etc/modules.d/$$$$$$$$priority-$(1); \
		done; \
		if [ -e $(2)/etc/modules.d/$$$$$$$$priority-$(1) ]; then \
			if [ "$$$$$$$$boot" = "1" ]; then \
				mkdir -p $(2)/etc/modules-boot.d; \
				ln -s ../modules.d/$$$$$$$$priority-$(1) $(2)/etc/modules-boot.d/; \
			fi; \
			modules="$$$$$$$${modules:+$$$$$$$$modules }$$$$$$$$priority-$(1)"; \
		fi; \
	}; \
	$(3) \
	if [ -n "$$$$$$$$modules" ]; then \
		mkdir -p $(2)/etc/modules.d; \
		mkdir -p $(2)/CONTROL; \
		echo "#!/bin/sh" > $(2)/CONTROL/postinst; \
		echo "[ -z \"\$$$$$$$$IPKG_INSTROOT\" ] || exit 0" >> $(2)/CONTROL/postinst; \
		echo ". /lib/functions.sh" >> $(2)/CONTROL/postinst; \
		echo "insert_modules $$$$$$$$modules" >> $(2)/CONTROL/postinst; \
		chmod 0755 $(2)/CONTROL/postinst; \
	fi
endef

ifeq ($(DUMP)$(TARGET_BUILD),)
  -include $(LINUX_DIR)/.config
endif

define KernelPackage/depends
  $(STAMP_BUILT): $(LINUX_DIR)/.config
  define KernelPackage/depends
  endef
endef

define KernelPackage
  NAME:=$(1)
  $(eval $(call Package/Default))
  $(eval $(call KernelPackage/Defaults))
  $(eval $(call KernelPackage/$(1)))
  $(eval $(call KernelPackage/$(1)/$(BOARD)))

  define Package/kmod-$(1)
    TITLE:=$(TITLE)
    SECTION:=kernel
    CATEGORY:=Kernel modules
    DESCRIPTION:=$(DESCRIPTION)
    EXTRA_DEPENDS:=kernel (=$(LINUX_VERSION)-$(LINUX_RELEASE)-$(LINUX_VERMAGIC))
    VERSION:=$(LINUX_VERSION)$(if $(PKG_VERSION),+$(PKG_VERSION))-$(if $(PKG_RELEASE),$(PKG_RELEASE),$(LINUX_RELEASE))
    $(call KernelPackage/$(1))
    $(call KernelPackage/$(1)/$(BOARD))
  endef

  ifdef KernelPackage/$(1)/description
    define Package/kmod-$(1)/description
$(call KernelPackage/$(1)/description)
    endef
  endif

  ifdef KernelPackage/$(1)/config
    define Package/kmod-$(1)/config
$(call KernelPackage/$(1)/config)
    endef
  endif

  $(call KernelPackage/depends)

  ifneq ($(if $(filter-out %=y %=n %=m,$(KCONFIG)),$(filter m y,$(foreach c,$(filter-out %=y %=n %=m,$(KCONFIG)),$($(c)))),.),)
    ifneq ($(strip $(FILES)),)
      define Package/kmod-$(1)/install
		  @for mod in $$(FILES); do \
			if [ -e $$$$$$$$mod ]; then \
				mkdir -p $$(1)/$(MODULES_SUBDIR) ; \
				$(CP) -L $$$$$$$$mod $$(1)/$(MODULES_SUBDIR)/ ; \
			elif [ -e "$(LINUX_DIR)/modules.builtin" ]; then \
				if grep -q "$$$$$$$${mod##$(LINUX_DIR)/}" "$(LINUX_DIR)/modules.builtin"; then \
					echo "NOTICE: module '$$$$$$$$mod' is built-in."; \
				else \
					echo "ERROR: module '$$$$$$$$mod' is missing." >&2; \
					exit 1; \
				fi; \
			else \
				echo "WARNING: module '$$$$$$$$mod' missing and modules.builtin not available, assuming built-in." >&2; \
			fi; \
		  done;
		  $(call ModuleAutoLoad,$(1),$$(1),$(AUTOLOAD))
		  $(call KernelPackage/$(1)/install,$$(1))
      endef
    endif
  $(if $(CONFIG_PACKAGE_kmod-$(1)),
    else
      compile: $(1)-disabled
      $(1)-disabled:
		@echo "WARNING: kmod-$(1) is not available in the kernel config - generating empty package" >&2

      define Package/kmod-$(1)/install
		true
      endef
  )
  endif
  $$(eval $$(call BuildPackage,kmod-$(1)))

  $$(IPKG_kmod-$(1)): $$(wildcard $$(FILES))
endef

define AutoLoad
  add_module "$(1)" "$(2)" "$(3)";
endef

define AutoProbe
  probe_module "$(1)" "$(2)";
endef

version_field=$(if $(word $(1),$(2)),$(word $(1),$(2)),0)
kernel_version_merge=$$(( ($(call version_field,1,$(1)) << 24) + ($(call version_field,2,$(1)) << 16) + ($(call version_field,3,$(1)) << 8) + $(call version_field,4,$(1)) ))

ifdef DUMP
  kernel_version_cmp=
else
  kernel_version_cmp=$(shell [ $(call kernel_version_merge,$(call split_version,$(2))) $(1) $(call kernel_version_merge,$(call split_version,$(3))) ] && echo 1 )
endif

CompareKernelPatchVer=$(if $(call kernel_version_cmp,-$(2),$(1),$(3)),1,0)

kernel_patchver_gt=$(call kernel_version_cmp,-gt,$(KERNEL_PATCHVER),$(1))
kernel_patchver_ge=$(call kernel_version_cmp,-ge,$(KERNEL_PATCHVER),$(1))
kernel_patchver_eq=$(call kernel_version_cmp,-eq,$(KERNEL_PATCHVER),$(1))
kernel_patchver_le=$(call kernel_version_cmp,-le,$(KERNEL_PATCHVER),$(1))
kernel_patchver_lt=$(call kernel_version_cmp,-lt,$(KERNEL_PATCHVER),$(1))

