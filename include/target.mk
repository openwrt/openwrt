#
# Copyright (C) 2007-2008 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

ifneq ($(__target_inc),1)
__target_inc=1

# default device type
DEVICE_TYPE?=router

# Default packages - the really basic set
DEFAULT_PACKAGES:=base-files libc libgcc busybox dropbear mtd uci opkg netifd
# For router targets
DEFAULT_PACKAGES.router:=dnsmasq iptables ip6tables ppp ppp-mod-pppoe kmod-ipt-nathelper firewall odhcpd odhcp6c
DEFAULT_PACKAGES.bootloader:=

ifneq ($(DUMP),)
  all: dumpinfo
endif

target_conf=$(subst .,_,$(subst -,_,$(subst /,_,$(1))))
ifeq ($(DUMP),)
  PLATFORM_DIR:=$(TOPDIR)/target/linux/$(BOARD)
  SUBTARGET:=$(strip $(foreach subdir,$(patsubst $(PLATFORM_DIR)/%/target.mk,%,$(wildcard $(PLATFORM_DIR)/*/target.mk)),$(if $(CONFIG_TARGET_$(call target_conf,$(BOARD)_$(subdir))),$(subdir))))
else
  PLATFORM_DIR:=${CURDIR}
  ifeq ($(SUBTARGETS),)
    SUBTARGETS:=$(strip $(patsubst $(PLATFORM_DIR)/%/target.mk,%,$(wildcard $(PLATFORM_DIR)/*/target.mk)))
  endif
endif

TARGETID:=$(BOARD)$(if $(SUBTARGET),/$(SUBTARGET))
PLATFORM_SUBDIR:=$(PLATFORM_DIR)$(if $(SUBTARGET),/$(SUBTARGET))

ifneq ($(TARGET_BUILD),1)
  ifndef DUMP
    include $(PLATFORM_DIR)/Makefile
    ifneq ($(PLATFORM_DIR),$(PLATFORM_SUBDIR))
      include $(PLATFORM_SUBDIR)/target.mk
    endif
  endif
else
  ifneq ($(SUBTARGET),)
    -include ./$(SUBTARGET)/target.mk
  endif
endif

# Add device specific packages (here below to allow device type set from subtarget)
DEFAULT_PACKAGES += $(DEFAULT_PACKAGES.$(DEVICE_TYPE))

define Profile/Default
  NAME:=
  PACKAGES:=
endef

ifndef Profile
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
	$(SH_FUNC) getvar "$(call shvar,Profile/$(1)/Config)"; \
	echo "@@"; \
	echo "Target-Profile-Description:"; \
	$(SH_FUNC) getvar "$(call shvar,Profile/$(1)/Description)"; \
	echo "@@"; \
	echo;
  ifeq ($(CONFIG_TARGET_$(call target_conf,$(BOARD)_$(if $(SUBTARGET),$(SUBTARGET)_))$(1)),y)
    PROFILE=$(1)
  endif
endef
endif

ifneq ($(PLATFORM_DIR),$(PLATFORM_SUBDIR))
  define IncludeProfiles
    -include $(sort $(wildcard $(PLATFORM_DIR)/profiles/*.mk))
    -include $(sort $(wildcard $(PLATFORM_SUBDIR)/profiles/*.mk))
  endef
else
  define IncludeProfiles
    -include $(sort $(wildcard $(PLATFORM_DIR)/profiles/*.mk))
  endef
endif

ifeq ($(TARGET_BUILD),1)
  $(eval $(call IncludeProfiles))
else
  ifeq ($(DUMP),)
    $(eval $(call IncludeProfiles))
  endif
endif

$(eval $(call shexport,Target/Description))

ifneq ($(TARGET_BUILD)$(if $(DUMP),,1),)
  include $(INCLUDE_DIR)/kernel-version.mk
endif

GENERIC_PLATFORM_DIR := $(TOPDIR)/target/linux/generic
GENERIC_PATCH_DIR := $(GENERIC_PLATFORM_DIR)/patches$(if $(wildcard $(GENERIC_PLATFORM_DIR)/patches-$(KERNEL_PATCHVER)),-$(KERNEL_PATCHVER))
GENERIC_FILES_DIR := $(foreach dir,$(wildcard $(GENERIC_PLATFORM_DIR)/files $(GENERIC_PLATFORM_DIR)/files-$(KERNEL_PATCHVER)),"$(dir)")

__config_name_list = $(1)/config-$(KERNEL_PATCHVER) $(1)/config-default
__config_list = $(firstword $(wildcard $(call __config_name_list,$(1))))
find_kernel_config=$(if $(__config_list),$(__config_list),$(lastword $(__config_name_list)))

GENERIC_LINUX_CONFIG = $(call find_kernel_config,$(GENERIC_PLATFORM_DIR))
LINUX_TARGET_CONFIG = $(call find_kernel_config,$(PLATFORM_DIR))
ifneq ($(PLATFORM_DIR),$(PLATFORM_SUBDIR))
  LINUX_SUBTARGET_CONFIG = $(call find_kernel_config,$(PLATFORM_SUBDIR))
endif

# config file list used for compiling
LINUX_KCONFIG_LIST = $(wildcard $(GENERIC_LINUX_CONFIG) $(LINUX_TARGET_CONFIG) $(LINUX_SUBTARGET_CONFIG) $(TOPDIR)/env/kernel-config)

# default config list for reconfiguring
# defaults to subtarget if subtarget exists and target does not
# defaults to target otherwise
USE_SUBTARGET_CONFIG = $(if $(wildcard $(LINUX_TARGET_CONFIG)),,$(if $(LINUX_SUBTARGET_CONFIG),1))

LINUX_RECONFIG_LIST = $(wildcard $(GENERIC_LINUX_CONFIG) $(LINUX_TARGET_CONFIG) $(if $(USE_SUBTARGET_CONFIG),$(LINUX_SUBTARGET_CONFIG)))
LINUX_RECONFIG_TARGET = $(if $(USE_SUBTARGET_CONFIG),$(LINUX_SUBTARGET_CONFIG),$(LINUX_TARGET_CONFIG))

# select the config file to be changed by kernel_menuconfig/kernel_oldconfig
ifeq ($(CONFIG_TARGET),platform)
  LINUX_RECONFIG_LIST = $(wildcard $(GENERIC_LINUX_CONFIG) $(LINUX_TARGET_CONFIG))
  LINUX_RECONFIG_TARGET = $(LINUX_TARGET_CONFIG)
endif
ifeq ($(CONFIG_TARGET),subtarget)
  LINUX_RECONFIG_LIST = $(wildcard $(GENERIC_LINUX_CONFIG) $(LINUX_TARGET_CONFIG) $(LINUX_SUBTARGET_CONFIG))
  LINUX_RECONFIG_TARGET = $(LINUX_SUBTARGET_CONFIG)
endif
ifeq ($(CONFIG_TARGET),subtarget_platform)
  LINUX_RECONFIG_LIST = $(wildcard $(GENERIC_LINUX_CONFIG) $(LINUX_SUBTARGET_CONFIG) $(LINUX_TARGET_CONFIG))
  LINUX_RECONFIG_TARGET = $(LINUX_TARGET_CONFIG)
endif
ifeq ($(CONFIG_TARGET),env)
  LINUX_RECONFIG_LIST = $(LINUX_KCONFIG_LIST)
  LINUX_RECONFIG_TARGET = $(TOPDIR)/env/kernel-config
endif

__linux_confcmd = $(SCRIPT_DIR)/kconfig.pl $(2) $(patsubst %,+,$(wordlist 2,9999,$(1))) $(1)

LINUX_CONF_CMD = $(call __linux_confcmd,$(LINUX_KCONFIG_LIST),)
LINUX_RECONF_CMD = $(call __linux_confcmd,$(LINUX_RECONFIG_LIST),)
LINUX_RECONF_DIFF = $(call __linux_confcmd,$(filter-out $(LINUX_RECONFIG_TARGET),$(LINUX_RECONFIG_LIST)),'>')

ifeq ($(DUMP),1)
  BuildTarget=$(BuildTargets/DumpCurrent)

  ifneq ($(BOARD),)
    TMP_CONFIG:=$(TMP_DIR)/.kconfig-$(call target_conf,$(TARGETID))
    $(TMP_CONFIG): $(LINUX_KCONFIG_LIST)
		$(LINUX_CONF_CMD) > $@ || rm -f $@
    -include $(TMP_CONFIG)
    .SILENT: $(TMP_CONFIG)
    .PRECIOUS: $(TMP_CONFIG)

    ifneq ($(CONFIG_OF),)
      FEATURES += dt
    endif
    ifneq ($(CONFIG_GENERIC_GPIO)$(CONFIG_GPIOLIB),)
      FEATURES += gpio
    endif
    ifneq ($(CONFIG_PCI),)
      FEATURES += pci
    endif
    ifneq ($(CONFIG_PCIEPORTBUS),)
      FEATURES += pcie
    endif
    ifneq ($(CONFIG_USB)$(CONFIG_USB_SUPPORT),)
      ifneq ($(CONFIG_USB_ARCH_HAS_HCD)$(CONFIG_USB_EHCI_HCD),)
        FEATURES += usb
      endif
    endif
    ifneq ($(CONFIG_PCMCIA)$(CONFIG_PCCARD),)
      FEATURES += pcmcia
    endif
    ifneq ($(CONFIG_VGA_CONSOLE)$(CONFIG_FB),)
      FEATURES += display
    endif
    ifneq ($(CONFIG_RTC_CLASS),)
      FEATURES += rtc
    endif
    FEATURES += $(foreach v,v4 v5 v6 v7,$(if $(findstring -march=arm$(v),$(CFLAGS)),arm_$(v)))

    # remove duplicates
    FEATURES:=$(sort $(FEATURES))
  endif
  CPU_CFLAGS = -Os -pipe
  ifneq ($(findstring mips,$(ARCH)),)
    ifneq ($(findstring mips64,$(ARCH)),)
      CPU_TYPE ?= mips64
    else
      CPU_TYPE ?= mips32
    endif
    CPU_CFLAGS += -mno-branch-likely
    CPU_CFLAGS_mips32 = -mips32 -mtune=mips32
    CPU_CFLAGS_mips32r2 = -mips32r2 -mtune=mips32r2
    CPU_CFLAGS_mips64 = -mips64 -mtune=mips64 -mabi=64
    CPU_CFLAGS_24kec = -mips32r2 -mtune=24kec
    CPU_CFLAGS_34kc = -mips32r2 -mtune=34kc
    CPU_CFLAGS_dsp = -mdsp
    CPU_CFLAGS_dsp2 = -mdspr2
  endif
  ifeq ($(ARCH),i386)
    CPU_TYPE ?= i486
    CPU_CFLAGS_i486 = -march=i486
    CPU_CFLAGS_geode = -march=geode -mmmx -m3dnow
  endif
  ifneq ($(findstring arm,$(ARCH)),)
    CPU_TYPE ?= xscale
    CPU_CFLAGS_arm920t = -march=armv4t -mtune=arm920t
    CPU_CFLAGS_arm926ej-s = -march=armv5te -mtune=arm926ej-s
    CPU_CFLAGS_arm1136j-s = -march=armv6 -mtune=arm1136j-s
    CPU_CFLAGS_arm1176jzf-s = -march=armv6 -mtune=arm1176jzf-s
    CPU_CFLAGS_cortex-a7 = -march=armv7-a -mtune=cortex-a7
    CPU_CFLAGS_cortex-a8 = -march=armv7-a -mtune=cortex-a8
    CPU_CFLAGS_cortex-a9 = -march=armv7-a -mtune=cortex-a9
    CPU_CFLAGS_fa526 = -march=armv4 -mtune=fa526
    CPU_CFLAGS_mpcore = -march=armv6k -mtune=mpcore
    CPU_CFLAGS_xscale = -march=armv5te -mtune=xscale
    ifneq ($(CONFIG_SOFT_FLOAT),)
      CPU_CFLAGS_vfp = -mfpu=vfp
      CPU_CFLAGS_vfpv3 = -mfpu=vfpv3-d16
    endif
  endif
  ifeq ($(ARCH),powerpc)
    CPU_CFLAGS_603e:=-mcpu=603e
    CPU_CFLAGS_8540:=-mcpu=8540
    CPU_CFLAGS_405:=-mcpu=405
    CPU_CFLAGS_440:=-mcpu=440
  endif
  ifeq ($(ARCH),sparc)
    CPU_TYPE = sparc
    CPU_CFLAGS_ultrasparc = -mcpu=ultrasparc
  endif
  DEFAULT_CFLAGS=$(strip $(CPU_CFLAGS) $(CPU_CFLAGS_$(CPU_TYPE)) $(CPU_CFLAGS_$(CPU_SUBTYPE)))
endif

define BuildTargets/DumpCurrent
  .PHONY: dumpinfo
  dumpinfo:
	@echo 'Target: $(TARGETID)'; \
	 echo 'Target-Board: $(BOARD)'; \
	 echo 'Target-Name: $(BOARDNAME)$(if $(SUBTARGETS),$(if $(SUBTARGET),))'; \
	 echo 'Target-Path: $(subst $(TOPDIR)/,,$(PWD))'; \
	 echo 'Target-Arch: $(ARCH)'; \
	 echo 'Target-Arch-Packages: $(if $(ARCH_PACKAGES),$(ARCH_PACKAGES),$(BOARD))'; \
	 echo 'Target-Features: $(FEATURES)'; \
	 echo 'Target-Depends: $(DEPENDS)'; \
	 echo 'Target-Optimization: $(if $(CFLAGS),$(CFLAGS),$(DEFAULT_CFLAGS))'; \
	 echo 'CPU-Type: $(CPU_TYPE)$(if $(CPU_SUBTYPE),+$(CPU_SUBTYPE))'; \
	 echo 'Linux-Version: $(LINUX_VERSION)'; \
	 echo 'Linux-Release: $(LINUX_RELEASE)'; \
	 echo 'Linux-Kernel-Arch: $(LINUX_KARCH)'; \
	$(if $(SUBTARGET),,$(if $(DEFAULT_SUBTARGET), echo 'Default-Subtarget: $(DEFAULT_SUBTARGET)'; ))
	 echo 'Target-Description:'; \
	 $(SH_FUNC) getvar $(call shvar,Target/Description); \
	 echo '@@'; \
	 echo 'Default-Packages: $(DEFAULT_PACKAGES)'; \
	 $(DUMPINFO)
	$(if $(SUBTARGET),,@$(foreach SUBTARGET,$(SUBTARGETS),$(SUBMAKE) -s DUMP=1 SUBTARGET=$(SUBTARGET); ))
endef

include $(INCLUDE_DIR)/kernel.mk
ifeq ($(TARGET_BUILD),1)
  include $(INCLUDE_DIR)/kernel-build.mk
  BuildTarget?=$(BuildKernel)
endif

endif #__target_inc
