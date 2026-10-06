# SPDX-License-Identifier: GPL-2.0-only
#
# Copyright (C) 2006-2020 OpenWrt.org

ifneq ($(filter check,$(MAKECMDGOALS)),)
CHECK:=1
DUMP:=1
endif

ifneq ($(SOURCE_DATE_EPOCH),)
  ifndef DUMP
    KBUILD_BUILD_TIMESTAMP:=$(shell perl -e 'print scalar gmtime($(SOURCE_DATE_EPOCH))')
  endif
endif

ifeq ($(__target_inc),)
  ifndef CHECK
    include $(INCLUDE_DIR)/target.mk
  endif
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
    PATCH_DIR ?= $(CURDIR)/patches$(if $(wildcard ./patches-$(KERNEL_PATCHVER)),-$(KERNEL_PATCHVER))
    FILES_DIR ?= $(foreach dir,$(wildcard $(CURDIR)/files $(CURDIR)/files-$(KERNEL_PATCHVER)),"$(dir)")
  endif
  KERNEL_BUILD_DIR ?= $(BUILD_DIR)/linux-$(BOARD)_$(SUBTARGET)
  LINUX_DIR ?= $(KERNEL_BUILD_DIR)/linux-$(LINUX_VERSION)
  LINUX_UAPI_DIR=uapi/
  LINUX_VERMAGIC:=$(strip $(shell cat $(LINUX_DIR)/.vermagic 2>/dev/null))
  LINUX_VERMAGIC:=$(if $(LINUX_VERMAGIC),$(LINUX_VERMAGIC),unknown)

  LINUX_UNAME_VERSION:=$(KERNEL_BASE)
  ifneq ($(findstring -rc,$(LINUX_VERSION)),)
    LINUX_UNAME_VERSION:=$(LINUX_UNAME_VERSION)-$(strip $(lastword $(subst -, ,$(LINUX_VERSION))))
  endif

  LINUX_KERNEL:=$(KERNEL_BUILD_DIR)/vmlinux

  ifneq (,$(findstring -rc,$(LINUX_VERSION)))
      LINUX_SOURCE:=linux-$(LINUX_VERSION).tar.gz
  else
      LINUX_SOURCE:=linux-$(LINUX_VERSION).tar.xz
  endif

  ifneq (,$(findstring -rc,$(LINUX_VERSION)))
      LINUX_SITE:=https://git.kernel.org/torvalds/t
  else ifeq ($(call qstrip,$(CONFIG_EXTERNAL_KERNEL_TREE))$(call qstrip,$(CONFIG_KERNEL_GIT_CLONE_URI)),)
      LINUX_SITE:=@KERNEL/linux/kernel/v$(word 1,$(subst ., ,$(KERNEL_BASE))).x
  else
      LINUX_UNAME_VERSION:=$(strip $(shell cat $(LINUX_DIR)/include/config/kernel.release 2>/dev/null))
  endif

  MODULES_SUBDIR:=lib/modules/$(LINUX_UNAME_VERSION)
  TARGET_MODULES_DIR:=$(LINUX_TARGET_DIR)/$(MODULES_SUBDIR)

  ifneq ($(TARGET_BUILD),1)
    PKG_BUILD_DIR ?= $(KERNEL_BUILD_DIR)/$(if $(BUILD_VARIANT),$(PKG_NAME)-$(BUILD_VARIANT)/)$(PKG_NAME)$(if $(PKG_VERSION),-$(PKG_VERSION))
  endif
endif

ifneq (,$(findstring uml,$(BOARD)))
  LINUX_KARCH=um
else ifneq (,$(findstring $(ARCH) , aarch64 aarch64_be ))
  LINUX_KARCH := arm64
else ifneq (,$(findstring $(ARCH) , armeb ))
  LINUX_KARCH := arm
else ifneq (,$(findstring $(ARCH) , loongarch64 ))
  LINUX_KARCH := loongarch
else ifneq (,$(findstring $(ARCH) , mipsel mips64 mips64el ))
  LINUX_KARCH := mips
else ifneq (,$(findstring $(ARCH) , powerpc64 ))
  LINUX_KARCH := powerpc
else ifneq (,$(findstring $(ARCH) , riscv64 ))
  LINUX_KARCH := riscv
else ifneq (,$(findstring $(ARCH) , i386 x86_64 ))
  LINUX_KARCH := x86
else
  LINUX_KARCH := $(ARCH)
endif

KERNEL_MAKE = $(MAKE) $(KERNEL_MAKEOPTS)

KERNEL_MAKE_FLAGS = \
	KCFLAGS="$(call iremap,$(BUILD_DIR),$(notdir $(BUILD_DIR))) $(IREMAP_STAGING_DIR) $(filter-out -fno-plt,$(call qstrip,$(CONFIG_EXTRA_OPTIMIZATION))) $(call qstrip,$(CONFIG_KERNEL_CFLAGS))" \
	KAFLAGS="$(call iremap,$(BUILD_DIR),$(notdir $(BUILD_DIR))) $(IREMAP_STAGING_DIR)" \
	HOSTCFLAGS="$(HOST_CFLAGS) -Wall -Wmissing-prototypes -Wstrict-prototypes" \
	CROSS_COMPILE="$(KERNEL_CROSS)" \
	ARCH="$(LINUX_KARCH)" \
	KBUILD_HAVE_NLS=no \
	KBUILD_BUILD_USER="$(call qstrip,$(CONFIG_KERNEL_BUILD_USER))" \
	KBUILD_BUILD_HOST="$(call qstrip,$(CONFIG_KERNEL_BUILD_DOMAIN))" \
	KBUILD_BUILD_TIMESTAMP="$(KBUILD_BUILD_TIMESTAMP)" \
	KBUILD_BUILD_VERSION="0" \
	KBUILD_HOSTLDFLAGS="-L$(STAGING_DIR_HOST)/lib" \
	CONFIG_SHELL="$(BASH)" \
	$(if $(findstring c,$(OPENWRT_VERBOSE)),V=1,V='') \
	$(if $(PKG_BUILD_ID),LDFLAGS_MODULE=--build-id=0x$(PKG_BUILD_ID)) \
	cmd_syscalls= \
	$(if $(__package_mk),KBUILD_EXTRA_SYMBOLS="$(wildcard $(PKG_SYMVERS_DIR)/*.symvers)")

ifneq (,$(KERNEL_CC))
  KERNEL_MAKE_FLAGS += CC="$(KERNEL_CC)"
endif

ifeq ($(HOST_OS),Darwin)
  KERNEL_MAKE_FLAGS += MACOSX_DEPLOYMENT_TARGET="$(shell sw_vers -productVersion)"
endif

KERNEL_NOSTDINC_FLAGS = \
	-nostdinc $(if $(DUMP),, -isystem $(shell $(TARGET_CC) -print-file-name=include))

ifeq ($(call qstrip,$(CONFIG_EXTERNAL_KERNEL_TREE))$(call qstrip,$(CONFIG_KERNEL_GIT_CLONE_URI)),)
  KERNEL_MAKE_FLAGS += \
	KERNELRELEASE=$(LINUX_VERSION)
endif

KERNEL_MAKEOPTS = -C $(LINUX_DIR) $(KERNEL_MAKE_FLAGS)

ifdef CONFIG_USE_SPARSE
  KERNEL_MAKEOPTS += C=1 CHECK=$(STAGING_DIR_HOST)/bin/sparse
endif

PKG_EXTMOD_SUBDIRS ?= .

PKG_SYMVERS_DIR = $(KERNEL_BUILD_DIR)/symvers

define collect_module_symvers
	for subdir in $(PKG_EXTMOD_SUBDIRS); do \
		realdir=$$$$(readlink -f $(PKG_BUILD_DIR)); \
		grep -F $(PKG_BUILD_DIR) $(PKG_BUILD_DIR)/$$$$subdir/Module.symvers >> $(PKG_BUILD_DIR)/Module.symvers.tmp; \
		[ "$(PKG_BUILD_DIR)" = "$$$$realdir" ] || \
			grep -F $$$$realdir $(PKG_BUILD_DIR)/$$$$subdir/Module.symvers >> $(PKG_BUILD_DIR)/Module.symvers.tmp; \
		[ -s "$(PKG_BUILD_DIR)/Module.symvers.tmp" ] || [ "$(KERNEL_PATCHVER)" = "6.18" ] && \
			sed 's/\.o$$$$//' $(PKG_BUILD_DIR)/$$$$subdir/modules.order | \
			grep -Ff - $(PKG_BUILD_DIR)/$$$$subdir/Module.symvers >> $(PKG_BUILD_DIR)/Module.symvers.tmp; \
	done; \
	sort -u $(PKG_BUILD_DIR)/Module.symvers.tmp > $(PKG_BUILD_DIR)/Module.symvers; \
	mkdir -p $(PKG_SYMVERS_DIR); \
	mv $(PKG_BUILD_DIR)/Module.symvers $(PKG_SYMVERS_DIR)/$(PKG_NAME).symvers
endef

define KernelPackage/hooks
  ifneq ($(PKG_NAME),kernel)
    Hooks/Compile/Post += collect_module_symvers
  endif
  define KernelPackage/hooks
  endef
endef

define KernelPackage/Defaults
  FILES:=
  AUTOLOAD:=
  MODPARAMS:=
  PKGFLAGS+=nonshared
endef

# 1: name
# 2: install prefix
# 3: module priority prefix
# 4: required for boot
# 5: module list
define ModuleAutoLoad
  $(if $(5), \
    mkdir -p $(2)/etc/modules.d; \
    ($(foreach mod,$(5), \
      echo "$(mod)$(if $(MODPARAMS.$(mod)), $(MODPARAMS.$(mod)),$(if $(MODPARAMS), $(MODPARAMS)))"; )) > $(2)/etc/modules.d/$(3)$(1); \
    $(if $(4), \
      mkdir -p $(2)/etc/modules-boot.d; \
      ln -sf ../modules.d/$(3)$(1) $(2)/etc/modules-boot.d/;))
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
  $(eval $(call KernelPackage/$(1)/$(BOARD)/$(SUBTARGET)))

  define Package/kmod-$(1)
    TITLE:=$(TITLE)
    SECTION:=kernel
    CATEGORY:=Kernel modules
    EXTRA_DEPENDS:=kernel (=$(subst -rc,_rc,$(LINUX_VERSION))~$(LINUX_VERMAGIC)-r$(LINUX_RELEASE))
    VERSION:=$(subst -rc,_rc,$(LINUX_VERSION))$(if $(PKG_VERSION),.$(PKG_VERSION))-r$(if $(PKG_RELEASE),$(PKG_RELEASE),$(LINUX_RELEASE))
    PKGFLAGS:=$(PKGFLAGS)
    $(call KernelPackage/$(1))
    $(call KernelPackage/$(1)/$(BOARD))
    $(call KernelPackage/$(1)/$(BOARD)/$(SUBTARGET))
  endef

  ifdef KernelPackage/$(1)/conffiles
    define Package/kmod-$(1)/conffiles
$(call KernelPackage/$(1)/conffiles)
    endef
  endif

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
  $(call KernelPackage/hooks)

  ifneq ($(if $(filter-out %=y %=n %=m,$(KCONFIG)),$(filter m y,$(foreach c,$(call version_filter,$(filter-out %=y %=n %=m,$(KCONFIG))),$($(c)))),.),)
    define Package/kmod-$(1)/install
		  @for mod in $$(call version_filter,$$(FILES)); do \
			if grep -q "$$$$$$$${mod##$(LINUX_DIR)/}" "$(LINUX_DIR)/modules.builtin"; then \
				echo "NOTICE: module '$$$$$$$$mod' is built-in."; \
			elif [ -e $$$$$$$$mod ]; then \
				mkdir -p $$(1)/$(MODULES_SUBDIR) ; \
				$(CP) -L $$$$$$$$mod $$(1)/$(MODULES_SUBDIR)/ ; \
			else \
				echo "ERROR: module '$$$$$$$$mod' is missing." >&2; \
				exit 1; \
			fi; \
		  done;
		  $(call ModuleAutoLoad,$(1),$$(1),$(filter-out 0-,$(word 1,$(AUTOLOAD))-),$(filter-out 0,$(word 2,$(AUTOLOAD))),$(sort $(wordlist 3,99,$(AUTOLOAD))))
		  $(call KernelPackage/$(1)/install,$$(1))
    endef
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

  $$(IPKG_kmod-$(1)): $$(wildcard $$(call version_filter,$$(FILES)))

endef

version_filter_perl=$(shell $(SCRIPT_DIR)/package-metadata.pl version_filter $(KERNEL_PATCHVER) $(1))

# Items of the form <name>@<op><version> are filtered in make, as a perl
# call per list costs seconds when parsing all kmod packages. The version
# comparison stays in package-metadata.pl, but runs only once per version.
# Any other form of item is passed to package-metadata.pl as before.

# $(1): version, returns the operators that are true for KERNEL_PATCHVER
version_ops=$(if $(filter undefined,$(origin version_ops/$(1))),$(eval \
	version_ops/$(1):=$(call version_filter_perl,$(foreach op,lt le gt ge eq ne,$(op)@$(op)$(1)))))$(version_ops/$(1))

strip_digits=$(subst 9,,$(subst 8,,$(subst 7,,$(subst 6,,$(subst 5,,$(subst 4,,$(subst 3,,$(subst 2,,$(subst 1,,$(subst 0,,$(1)))))))))))

# $(1): version, returns y if it is a list of numbers separated by dots
version_is_plain=$(if $(or $(filter .% %.,$(1)),$(findstring ..,$(1)),$(subst .,,$(call strip_digits,$(1)))),,$(if $(findstring .,$(1)),y))

# $(1): condition such as ge6.18
version_cond_op=$(firstword $(foreach op,lt le gt ge eq ne,$(if $(filter $(op)%,$(1)),$(op))))
version_cond_ver=$(patsubst $(call version_cond_op,$(1))%,%,$(1))

# $(1): item such as foo@ge6.18, $(2): name, $(3): condition
version_filter_item=$(if $(and $(2),$(call version_cond_op,$(3)),$(call version_is_plain,$(call version_cond_ver,$(3)))), \
	$(if $(filter $(call version_cond_op,$(3)),$(call version_ops,$(call version_cond_ver,$(3)))),$(2)), \
	$(call version_filter_perl,$(1)))

version_filter=$(if $(findstring @,$(1)),$(strip $(foreach item,$(1), \
	$(if $(findstring @,$(item)), \
		$(if $(filter 2,$(words $(subst @, ,$(item)))), \
			$(call version_filter_item,$(item),$(word 1,$(subst @, ,$(item))),$(word 2,$(subst @, ,$(item)))), \
			$(call version_filter_perl,$(item))), \
		$(item)))),$(1))

# 1: priority (optional)
# 2: module list
# 3: boot flag
define AutoLoad
  $(if $(1),$(1),0) $(if $(3),1,0) $(call version_filter,$(2))
endef

# 1: module list
# 2: boot flag
define AutoProbe
  $(call AutoLoad,,$(1),$(2))
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
