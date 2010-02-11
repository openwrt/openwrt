#
# Copyright (C) 2006-2007 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(INCLUDE_DIR)/host.mk
include $(INCLUDE_DIR)/prereq.mk

ifneq ($(DUMP),1)
  all: compile
endif

export QUILT=1
STAMP_PREPARED:=$(LINUX_DIR)/.prepared
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
endef

define Kernel/Clean
	$(call Kernel/Clean/Default)
endef

define Download/kernel
  URL:=$(LINUX_SITE)
  FILE:=$(LINUX_SOURCE)
  MD5SUM:=$(LINUX_KERNEL_MD5SUM)
endef

define BuildKernel
  $(if $(QUILT),$(Build/Quilt))
  $(if $(LINUX_SITE),$(call Download,kernel))

  $(STAMP_PREPARED): $(DL_DIR)/$(LINUX_SOURCE)
	-rm -rf $(KERNEL_BUILD_DIR)
	-mkdir -p $(KERNEL_BUILD_DIR)
	$(Kernel/Prepare)
	touch $$@

  $(KERNEL_BUILD_DIR)/symtab.txt: FORCE
	find $(LINUX_DIR) $(STAGING_DIR_ROOT)/lib/modules -name \*.ko | \
		xargs $(TARGET_CROSS)nm | \
		awk '$$$$1 == "U" { print $$$$2 } ' | \
		sort -u > $$@

  $(KERNEL_BUILD_DIR)/symtab.h: $(KERNEL_BUILD_DIR)/symtab.txt
	( \
		echo '#define SYMTAB_KEEP \'; \
		cat $(KERNEL_BUILD_DIR)/symtab.txt | \
			awk '{print "*(__ksymtab." $$$$1 ") \\" }'; \
		echo; \
		echo '#define SYMTAB_KEEP_GPL \'; \
		cat $(KERNEL_BUILD_DIR)/symtab.txt | \
			awk '{print "*(__ksymtab_gpl." $$$$1 ") \\" }'; \
		echo; \
		echo '#define SYMTAB_KEEP_STR \'; \
		cat $(KERNEL_BUILD_DIR)/symtab.txt | \
			awk '{print "*(__ksymtab_strings." $$$$1 ") \\" }'; \
		echo; \
	) > $$@

  $(STAMP_CONFIGURED): $(STAMP_PREPARED) $(LINUX_CONFIG) $(GENERIC_LINUX_CONFIG) $(TOPDIR)/.config
	$(Kernel/Configure)
	touch $$@

  $(LINUX_DIR)/.modules: $(STAMP_CONFIGURED) $(LINUX_DIR)/.config FORCE
	$(Kernel/CompileModules)
	touch $$@

  $(LINUX_DIR)/.image: $(STAMP_CONFIGURED) $(if $(CONFIG_STRIP_KERNEL_EXPORTS),$(KERNEL_BUILD_DIR)/symtab.h) FORCE
	$(Kernel/CompileImage)
	touch $$@
	
  mostlyclean: FORCE
	$(Kernel/Clean)

  define BuildKernel
  endef

  download: $(DL_DIR)/$(LINUX_SOURCE)
  prepare: $(STAMP_CONFIGURED)
  compile: $(LINUX_DIR)/.modules
	$(MAKE) -C image compile TARGET_BUILD=

  oldconfig menuconfig: $(STAMP_PREPARED) $(STAMP_CHECKED) FORCE
	[ -e "$(LINUX_CONFIG)" ] || touch "$(LINUX_CONFIG)"
	$(LINUX_CONFCMD) > $(LINUX_DIR)/.config
	touch $(LINUX_CONFIG)
	$(_SINGLE)$(MAKE) -C $(LINUX_DIR) $(KERNEL_MAKEOPTS) $$@
	$(SCRIPT_DIR)/kconfig.pl '>' $(if $(LINUX_SUBCONFIG),'+' $(GENERIC_LINUX_CONFIG) $(LINUX_CONFIG),$(GENERIC_LINUX_CONFIG)) \
		$(LINUX_DIR)/.config > $(if $(LINUX_SUBCONFIG),$(LINUX_SUBCONFIG),$(LINUX_CONFIG))
	LC_ALL='' sort $(if $(LINUX_SUBCONFIG),$(LINUX_SUBCONFIG),$(LINUX_CONFIG)) -o $(if $(LINUX_SUBCONFIG),$(LINUX_SUBCONFIG),$(LINUX_CONFIG))
	$(Kernel/Configure)

  install: $(LINUX_DIR)/.image
	+$(MAKE) -C image compile install TARGET_BUILD=

  clean: FORCE
	rm -rf $(KERNEL_BUILD_DIR)

  image-prereq:
	@+$(NO_TRACE_MAKE) -s -C image prereq TARGET_BUILD=

  prereq: image-prereq

endef
