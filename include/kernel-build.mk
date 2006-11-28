# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
KERNEL_BUILD:=1

include $(INCLUDE_DIR)/prereq.mk

-include ./config
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

# remove duplicates
FEATURES:=$(sort $(FEATURES))

# For target profile selection - the default set
DEFAULT_PACKAGES:=base-files libgcc uclibc bridge busybox dnsmasq dropbear iptables mtd ppp ppp-mod-pppoe mtd

ifeq ($(DUMP),1)
  all: dumpinfo
else
  all: compile
endif
KERNEL:=2.$(word 2,$(subst ., ,$(strip $(LINUX_VERSION))))

include $(INCLUDE_DIR)/host.mk
include $(INCLUDE_DIR)/kernel.mk

LINUX_CONFIG:=./config

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

KERNELNAME=
ifneq (,$(findstring x86,$(BOARD)))
  KERNELNAME="bzImage"
endif
ifneq (,$(findstring ppc,$(BOARD)))
  KERNELNAME="uImage"
endif


define Kernel/Prepare/Default
	bzcat $(DL_DIR)/$(LINUX_SOURCE) | tar -C $(KERNEL_BUILD_DIR) $(TAR_OPTIONS)
	[ -d ../generic-$(KERNEL)/patches ] && $(PATCH) $(LINUX_DIR) ../generic-$(KERNEL)/patches 
	[ -d ./patches ] && $(PATCH) $(LINUX_DIR) ./patches
endef
define Kernel/Prepare
	$(call Kernel/Prepare/Default)
endef

KERNEL_MAKEOPTS := -C $(LINUX_DIR) \
	CROSS_COMPILE="$(KERNEL_CROSS)" \
	ARCH="$(LINUX_KARCH)" \
	CONFIG_SHELL="$(BASH)"

define Kernel/Configure/2.4
	$(SED) "s,\-mcpu=,\-mtune=,g;" $(LINUX_DIR)/arch/mips/Makefile
	$(MAKE) $(KERNEL_MAKEOPTS) CC="$(KERNEL_CC)" oldconfig include/linux/compile.h include/linux/version.h
	$(MAKE) $(KERNEL_MAKEOPTS) dep
endef
define Kernel/Configure/2.6
	$(MAKE) $(KERNEL_MAKEOPTS) CC="$(KERNEL_CC)" oldconfig prepare scripts
endef
define Kernel/Configure/Default
	@$(CP) $(LINUX_CONFIG) $(LINUX_DIR)/.config
	$(call Kernel/Configure/$(KERNEL))
endef
define Kernel/Configure
	$(call Kernel/Configure/Default)
endef


define Kernel/CompileModules/Default
	$(MAKE) -j$(CONFIG_JLEVEL) $(KERNEL_MAKEOPTS) CC="$(KERNEL_CC)" modules
	$(MAKE) $(KERNEL_MAKEOPTS) CC="$(KERNEL_CC)" DEPMOD=true INSTALL_MOD_PATH=$(KERNEL_BUILD_DIR)/modules modules_install
endef
define Kernel/CompileModules
	$(call Kernel/CompileModules/Default)
endef


ifeq ($(KERNEL),2.6)
  ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
    define Kernel/SetInitramfs
		mv $(LINUX_DIR)/.config $(LINUX_DIR)/.config.old
		grep -v INITRAMFS $(LINUX_DIR)/.config.old > $(LINUX_DIR)/.config
		echo 'CONFIG_INITRAMFS_SOURCE="../../root"' >> $(LINUX_DIR)/.config
		echo 'CONFIG_INITRAMFS_ROOT_UID=0' >> $(LINUX_DIR)/.config
		echo 'CONFIG_INITRAMFS_ROOT_GID=0' >> $(LINUX_DIR)/.config
		mkdir -p $(BUILD_DIR)/root/etc/init.d
		$(CP) ../generic-2.6/files/init $(BUILD_DIR)/root/
    endef
  else
    define Kernel/SetInitramfs
		mv $(LINUX_DIR)/.config $(LINUX_DIR)/.config.old
		grep -v INITRAMFS $(LINUX_DIR)/.config.old > $(LINUX_DIR)/.config
		rm -f $(BUILD_DIR)/root/init $(BUILD_DIR)/root/etc/init.d/S00initramfs
		echo 'CONFIG_INITRAMFS_SOURCE=""' >> $(LINUX_DIR)/.config
    endef
  endif
endif
define Kernel/CompileImage/Default
	$(call Kernel/SetInitramfs)
	$(MAKE) -j$(CONFIG_JLEVEL) $(KERNEL_MAKEOPTS) CC="$(KERNEL_CC)" $(KERNELNAME)
	$(KERNEL_CROSS)objcopy -O binary -R .reginfo -R .note -R .comment -R .mdebug -S $(LINUX_DIR)/vmlinux $(LINUX_KERNEL)
endef
define Kernel/CompileImage
	$(call Kernel/CompileImage/Default)
endef

define Kernel/Clean/Default
	rm -f $(LINUX_DIR)/.linux-compile
	rm -f $(KERNEL_BUILD_DIR)/linux-$(LINUX_VERSION)/.configured
	rm -f $(LINUX_KERNEL)
	$(MAKE) -C $(KERNEL_BUILD_DIR)/linux-$(LINUX_VERSION) clean
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

define Profile
  $(eval $(call Profile/Default))
  $(eval $(call Profile/$(1)))
  DUMPINFO += \
	echo "Target-Profile: $(1)"; \
	echo "Target-Profile-Name: $(NAME)"; \
	echo "Target-Profile-Packages: $(PACKAGES)"; 
endef

$(eval $(call shexport,Target/Description))

download: $(DL_DIR)/$(LINUX_SOURCE)
prepare: $(LINUX_DIR)/.configured $(TMP_DIR)/.kernel.mk
compile: $(LINUX_DIR)/.modules
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


