# 
# Copyright (C) 2006-2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
KERNEL_BUILD:=1

include $(INCLUDE_DIR)/kernel-version.mk
include $(INCLUDE_DIR)/host.mk
include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/prereq.mk

LINUX_CONFIG ?= ./config/default

-include $(TOPDIR)/target/linux/generic-$(KERNEL)/config-template
-include $(LINUX_CONFIG)

ifneq ($(CONFIG_ATM),)
  FEATURES += atm
endif
ifneq ($(CONFIG_PCI),)
  FEATURES += pci
endif
ifneq ($(CONFIG_USB),)
  FEATURES += usb
endif
ifneq ($(CONFIG_PCMCIA),)
  FEATURES += pcmcia
endif
ifneq ($(CONFIG_VIDEO_DEV),)
  FEATURES += video
endif

# remove duplicates
FEATURES:=$(sort $(FEATURES))

ifeq ($(DUMP),1)
  all: dumpinfo
else
  all: compile
endif

ifneq (,$(findstring uml,$(BOARD)))
  LINUX_KARCH:=um
else
  LINUX_KARCH:=$(shell echo $(ARCH) | sed -e 's/i[3-9]86/i386/' \
	-e 's/mipsel/mips/' \
	-e 's/mipseb/mips/' \
	-e 's/powerpc/ppc/' \
	-e 's/sh[234]/sh/' \
	-e 's/armeb/arm/' \
  )
endif

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
endef

define Kernel/Clean
	$(call Kernel/Clean/Default)
endef

define BuildKernel
  ifneq ($(LINUX_SITE),)
    $(DL_DIR)/$(LINUX_SOURCE):
		-mkdir -p $(DL_DIR)
		$(SCRIPT_DIR)/download.pl $(DL_DIR) $(LINUX_SOURCE) $(LINUX_KERNEL_MD5SUM) $(LINUX_SITE)
  endif

  $(LINUX_DIR)/.prepared: $(DL_DIR)/$(LINUX_SOURCE)
	-rm -rf $(KERNEL_BUILD_DIR)
	-mkdir -p $(KERNEL_BUILD_DIR)
	$(call Kernel/Prepare)
	touch $$@

  $(LINUX_DIR)/.configured: $(LINUX_DIR)/.prepared $(LINUX_CONFIG)
	$(call Kernel/Configure)
	touch $$@

  $(LINUX_DIR)/.modules: $(LINUX_DIR)/.configured
	rm -rf $(KERNEL_BUILD_DIR)/modules
	@rm -f $(BUILD_DIR)/linux
	ln -sf $(KERNEL_BUILD_DIR)/linux-$(LINUX_VERSION) $(BUILD_DIR)/linux
	$(call Kernel/CompileModules)
	touch $$@

  $(LINUX_DIR)/.image: $(LINUX_DIR)/.configured FORCE
	$(call Kernel/CompileImage)
	touch $$@
	
  mostlyclean: FORCE
	$(call Kernel/Clean)

  ifeq ($(DUMP),1)
    dumpinfo:
		@echo 'Target: $(BOARD)-$(KERNEL)'
		@echo 'Target-Name: $(BOARDNAME) [$(KERNEL)]'
		@echo 'Target-Path: $(subst $(TOPDIR)/,,$(PWD))'
		@echo 'Target-Arch: $(ARCH)'
		@echo 'Target-Features: $(FEATURES)'
		@echo 'Linux-Version: $(LINUX_VERSION)'
		@echo 'Linux-Release: $(LINUX_RELEASE)'
		@echo 'Linux-Kernel-Arch: $(LINUX_KARCH)'
		@echo 'Target-Description:'
		@getvar $(call shvar,Target/Description)
		@echo '@@'
		@echo 'Default-Packages: $(DEFAULT_PACKAGES)'
    ifneq ($(DUMPINFO),)
		@$(DUMPINFO)
    endif
  endif

  define BuildKernel
  endef
endef

define Profile/Default
  NAME:=
  PACKAGES:=
endef

confname=$(subst .,_,$(subst -,_,$(1)))
define Profile
  $(eval $(call Profile/Default))
  $(eval $(call Profile/$(1)))
  $(eval $(call shexport,Profile/$(1)/Config))
  $(eval $(call shexport,Profile/$(1)/Description))
  DUMPINFO += \
	echo "Target-Profile: $(1)"; \
	echo "Target-Profile-Name: $(NAME)"; \
	echo "Target-Profile-Packages: $(PACKAGES)"; \
	if [ -f ./config/profile-$(1) ]; then \
		echo "Target-Profile-Kconfig: yes"; \
	fi; \
	echo "Target-Profile-Config: "; \
	getvar "$(call shvar,Profile/$(1)/Config)"; \
	echo "@@"; \
	echo "Target-Profile-Description:"; \
	getvar "$(call shvar,Profile/$(1)/Description)"; \
	echo "@@"; \
	echo;
  ifeq ($(CONFIG_LINUX_$(call confname,$(KERNEL)_$(1))),y)
    PROFILE=$(1)
  endif
endef

$(eval $(call shexport,Target/Description))

download: $(DL_DIR)/$(LINUX_SOURCE)
prepare: $(LINUX_DIR)/.configured $(TMP_DIR)/.kernel.mk
compile: $(LINUX_DIR)/.modules
menuconfig: $(LINUX_DIR)/.prepared FORCE
	$(call Kernel/Configure)
	$(SCRIPT_DIR)/config.pl '+' $(GENERIC_PLATFORM_DIR)/config-template $(LINUX_CONFIG) > $(LINUX_DIR)/.config
	$(MAKE) -C $(LINUX_DIR) $(KERNEL_MAKEOPTS) menuconfig
	$(SCRIPT_DIR)/config.pl '>' $(GENERIC_PLATFORM_DIR)/config-template $(LINUX_DIR)/.config > $(LINUX_CONFIG)

install: $(LINUX_DIR)/.image

clean: FORCE
	rm -f $(STAMP_DIR)/.linux-compile
	rm -rf $(KERNEL_BUILD_DIR)

rebuild: FORCE
	@$(MAKE) mostlyclean
	@if [ -f $(LINUX_KERNEL) ]; then \
		$(MAKE) clean; \
	fi
	@$(MAKE) compile


