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
DEFAULT_PACKAGES:=base-files libgcc uclibc busybox dropbear mtd uci opkg
# For router targets
DEFAULT_PACKAGES.router:=dnsmasq iptables ppp ppp-mod-pppoe kmod-ipt-nathelper bridge firewall

# Additional packages for Linux 2.6
ifneq ($(KERNEL),2.4)
  DEFAULT_PACKAGES += udevtrigger hotplug2
endif

# Add device specific packages
DEFAULT_PACKAGES += $(DEFAULT_PACKAGES.$(DEVICE_TYPE))

ifneq ($(DUMP),)
  all: dumpinfo
endif

target_conf=$(subst .,_,$(subst -,_,$(subst /,_,$(1))))
ifeq ($(DUMP),)
  PLATFORM_DIR:=$(TOPDIR)/target/linux/$(BOARD)
  SUBTARGET:=$(strip $(foreach subdir,$(patsubst $(PLATFORM_DIR)/%/target.mk,%,$(wildcard $(PLATFORM_DIR)/*/target.mk)),$(if $(CONFIG_TARGET_$(call target_conf,$(BOARD)_$(subdir))),$(subdir))))
else
  PLATFORM_DIR:=${CURDIR}
endif

TARGETID:=$(BOARD)$(if $(SUBTARGET),/$(SUBTARGET))
PLATFORM_SUBDIR:=$(PLATFORM_DIR)$(if $(SUBTARGET),/$(SUBTARGET))

ifneq ($(TARGET_BUILD),1)
  include $(PLATFORM_DIR)/Makefile
  ifneq ($(PLATFORM_DIR),$(PLATFORM_SUBDIR))
    include $(PLATFORM_SUBDIR)/target.mk
  endif
else
  ifneq ($(SUBTARGET),)
    -include ./$(SUBTARGET)/target.mk
  endif
endif

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
	getvar "$(call shvar,Profile/$(1)/Config)"; \
	echo "@@"; \
	echo "Target-Profile-Description:"; \
	getvar "$(call shvar,Profile/$(1)/Description)"; \
	echo "@@"; \
	echo;
  ifeq ($(CONFIG_TARGET_$(call target_conf,$(BOARD)_$(if $(SUBTARGET),$(SUBTARGET)_)$(1))),y)
    PROFILE=$(1)
  endif
endef
endif

ifneq ($(PLATFORM_DIR),$(PLATFORM_SUBDIR))
  define IncludeProfiles
    -include $(PLATFORM_DIR)/profiles/*.mk
    -include $(PLATFORM_SUBDIR)/profiles/*.mk
  endef
else
  define IncludeProfiles
    -include $(PLATFORM_DIR)/profiles/*.mk
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

include $(INCLUDE_DIR)/kernel-version.mk

GENERIC_PLATFORM_DIR := $(TOPDIR)/target/linux/generic-$(KERNEL)
GENERIC_PATCH_DIR := $(GENERIC_PLATFORM_DIR)/patches$(shell [ -d "$(GENERIC_PLATFORM_DIR)/patches-$(KERNEL_PATCHVER)" ] && printf -- "-$(KERNEL_PATCHVER)" || true )
GENERIC_FILES_DIR := $(GENERIC_PLATFORM_DIR)/files$(shell [ -d "$(GENERIC_PLATFORM_DIR)/files-$(KERNEL_PATCHVER)" ] && printf -- "-$(KERNEL_PATCHVER)" || true )

GENERIC_LINUX_CONFIG?=$(firstword $(wildcard $(GENERIC_PLATFORM_DIR)/config-$(KERNEL_PATCHVER) $(GENERIC_PLATFORM_DIR)/config-default))
LINUX_CONFIG?=$(firstword $(wildcard $(foreach subdir,$(PLATFORM_DIR) $(PLATFORM_SUBDIR),$(subdir)/config-$(KERNEL_PATCHVER) $(subdir)/config-default)) $(PLATFORM_DIR)/config-$(KERNEL_PATCHVER))
LINUX_SUBCONFIG?=$(firstword $(wildcard $(PLATFORM_SUBDIR)/config-$(KERNEL_PATCHVER) $(PLATFORM_SUBDIR)/config-default))
ifeq ($(LINUX_CONFIG),$(LINUX_SUBCONFIG))
  LINUX_SUBCONFIG:=
endif
LINUX_CONFCMD=$(if $(LINUX_CONFIG),$(SCRIPT_DIR)/kconfig.pl + $(GENERIC_LINUX_CONFIG) $(if $(LINUX_SUBCONFIG),+ $(LINUX_CONFIG) $(LINUX_SUBCONFIG),$(LINUX_CONFIG)),true)

ifeq ($(DUMP),1)
  BuildTarget=$(BuildTargets/DumpCurrent)

  ifneq ($(BOARD),)
    TMP_CONFIG:=$(TMP_DIR)/.kconfig-$(call target_conf,$(TARGETID))
    $(TMP_CONFIG): $(GENERIC_LINUX_CONFIG) $(LINUX_CONFIG) $(LINUX_SUBCONFIG)
		$(LINUX_CONFCMD) > $@ || rm -f $@
    -include $(TMP_CONFIG)
    .SILENT: $(TMP_CONFIG)
    .PRECIOUS: $(TMP_CONFIG)

    ifneq ($(CONFIG_GENERIC_GPIO),)
      FEATURES += gpio
    endif
    ifneq ($(CONFIG_PCI),)
      FEATURES += pci
    endif
    ifneq ($(CONFIG_USB)$(CONFIG_USB_SUPPORT),)
      FEATURES += usb
    endif
    ifneq ($(CONFIG_PCMCIA)$(CONFIG_PCCARD),)
      FEATURES += pcmcia
    endif
    ifneq ($(CONFIG_VGA_CONSOLE)$(CONFIG_FB),)
      FEATURES += display
    endif

    # remove duplicates
    FEATURES:=$(sort $(FEATURES))
  endif
endif

define BuildTargets/DumpCurrent
  .PHONY: dumpinfo
  dumpinfo:
	@echo 'Target: $(TARGETID)'; \
	 echo 'Target-Board: $(BOARD)'; \
	 echo 'Target-Kernel: $(KERNEL)'; \
	 echo 'Target-Name: $(BOARDNAME)$(if $(SUBTARGET),, [$(KERNEL)])'; \
	 echo 'Target-Path: $(subst $(TOPDIR)/,,$(PWD))'; \
	 echo 'Target-Arch: $(ARCH)'; \
	 echo 'Target-Features: $(FEATURES)'; \
	 echo 'Target-Depends: $(DEPENDS)'; \
	 echo 'Linux-Version: $(LINUX_VERSION)'; \
	 echo 'Linux-Release: $(LINUX_RELEASE)'; \
	 echo 'Linux-Kernel-Arch: $(LINUX_KARCH)'; \
	 echo 'Target-Description:'; \
	 getvar $(call shvar,Target/Description); \
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
